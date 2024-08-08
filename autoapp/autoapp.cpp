#include <thread>
#include <QApplication>
#include <QWindow>
#include <QMainWindow>
#include <QSplashScreen>
#include <QStackedWidget>
#include <QFile>
#include <QThread>
#include <QMetaType>
#include "aasdk/USB/USBHub.hpp"
#include "aasdk/USB/ConnectedAccessoriesEnumerator.hpp"
#include "aasdk/USB/AccessoryModeQueryChain.hpp"
#include "aasdk/USB/AccessoryModeQueryChainFactory.hpp"
#include "aasdk/USB/AccessoryModeQueryFactory.hpp"
#include "aasdk/TCP/TCPWrapper.hpp"
#include "openauto/App.hpp"
#include "openauto/Configuration/IConfiguration.hpp"
#include "openauto/Configuration/RecentAddressesList.hpp"
#include "openauto/Service/AndroidAutoEntityFactory.hpp"
#include "openauto/Service/ServiceFactory.hpp"
#include "openauto/Configuration/Configuration.hpp"
#include "./Pages/HomePage.hpp"
#include "./Pages/ProjectionPage.hpp"
#include "./Service/OpenautoEventFilter.hpp"
#include "./Service/Alsa.hpp"
#include "./Service/SteeringWheelControl.hpp"
#include "OpenautoLog.hpp"

using namespace openauto;
using ThreadPool = std::vector<std::thread>;

const int SCREEN_WIDTH = 710;
const int SCREEN_HEIGHT = 480;

void startUSBWorkers(boost::asio::io_service& ioService, libusb_context* usbContext, ThreadPool& threadPool)
{
    auto usbWorker = [&ioService, usbContext]() {
        timeval libusbEventTimeout{180, 0};

        while(!ioService.stopped())
        {
            libusb_handle_events_timeout_completed(usbContext, &libusbEventTimeout, nullptr);
        }
    };

    threadPool.emplace_back(usbWorker);
    threadPool.emplace_back(usbWorker);
    threadPool.emplace_back(usbWorker);
    threadPool.emplace_back(usbWorker);
}

void startIOServiceWorkers(boost::asio::io_service& ioService, ThreadPool& threadPool)
{
    auto ioServiceWorker = [&ioService]() {
        ioService.run();
    };

    threadPool.emplace_back(ioServiceWorker);
    threadPool.emplace_back(ioServiceWorker);
    threadPool.emplace_back(ioServiceWorker);
    threadPool.emplace_back(ioServiceWorker);
}



int main(int argc, char* argv[])
{
    QApplication app(argc, argv);
    QPixmap pixmap(":/ico_androidauto.png");
    QSize pixmapSize = pixmap.size();
    QSplashScreen splash(pixmap);
    splash.move(SCREEN_WIDTH / 2 - pixmapSize.width() / 2, SCREEN_HEIGHT / 2 - pixmapSize.height() / 2);
    splash.show();

    app.processEvents();

    // ===================================== SERVICE STUFF
    libusb_context* usbContext;
    if(libusb_init(&usbContext) != 0)
    {
        OPENAUTO_LOG(error) << "[OpenAuto] libusb init failed.";
        return 1;
    }

    boost::asio::io_service ioService;
    boost::asio::io_service::work work(ioService);
    std::vector<std::thread> threadPool;
    startUSBWorkers(ioService, usbContext, threadPool);
    startIOServiceWorkers(ioService, threadPool);


    autoapp::service::Alsa alsaWorker;

    // ===================================== UI STUFF

    QMainWindow window;

    QStackedWidget *stackedWidget = new QStackedWidget;
    stackedWidget->setStyleSheet("background-color: #333333;color: #eeeeec;");


    autoapp::pages::HomePage homePage;
    QObject::connect(&homePage, &autoapp::pages::HomePage::exit, []() { std::exit(0); });
    app.setOverrideCursor(Qt::BlankCursor);
    stackedWidget->addWidget(&homePage);

    autoapp::pages::ProjectionPage projectionPage(&alsaWorker);
    projectionPage.hide();
    stackedWidget->addWidget(&projectionPage);

    stackedWidget->setCurrentIndex(0);


    window.setCentralWidget(stackedWidget);
    window.resize(SCREEN_WIDTH, SCREEN_HEIGHT);
    projectionPage.aaFrame->resize(SCREEN_WIDTH, SCREEN_HEIGHT);


    // ===================================== SERVICE STUFF
    auto configuration = std::make_shared<openauto::configuration::Configuration>();

    aasdk::tcp::TCPWrapper tcpWrapper;
    aasdk::usb::USBWrapper usbWrapper(usbContext);
    aasdk::usb::AccessoryModeQueryFactory queryFactory(usbWrapper, ioService);
    aasdk::usb::AccessoryModeQueryChainFactory queryChainFactory(usbWrapper, ioService, queryFactory);
    openauto::service::ServiceFactory serviceFactory(ioService, configuration, projectionPage.aaFrame);
    openauto::service::AndroidAutoEntityFactory androidAutoEntityFactory(ioService, configuration, serviceFactory);

    auto usbHub(std::make_shared<aasdk::usb::USBHub>(usbWrapper, ioService, queryChainFactory));
    auto connectedAccessoriesEnumerator(std::make_shared<aasdk::usb::ConnectedAccessoriesEnumerator>(usbWrapper, ioService, queryChainFactory));
    auto openautoApp = std::make_shared<openauto::App>(ioService, usbWrapper, tcpWrapper, androidAutoEntityFactory, std::move(usbHub), std::move(connectedAccessoriesEnumerator));

    serviceFactory.startBluetoothAdvertising();

    autoapp::service::OpenautoEventFilter filter;
    app.installEventFilter(&filter);
    QObject::connect(&filter, &autoapp::service::OpenautoEventFilter::onAppEvent, [&projectionPage, &stackedWidget, &openautoApp](openauto::service::AppEventType value) {
        switch (value) {
            case openauto::service::AppEventType::ProjectionShow:
                stackedWidget->setCurrentIndex(1);
                projectionPage.show();
                break;
            case openauto::service::AppEventType::ProjectionEnd:
                projectionPage.hide();
                stackedWidget->setCurrentIndex(0);
                break;
        }
    });


    alsaWorker.start();


    // ===================================== KEYPRESS FORWARDER
    qRegisterMetaType<Qt::Key>();

    autoapp::service::SteeringWheelControl swcWorker;
    swcWorker.start();
    QObject::connect(&swcWorker, &autoapp::service::SteeringWheelControl::onKeyPress, stackedWidget, [&app, &alsaWorker](Qt::Key key) {
        if (key == Qt::Key_VolumeDown || key == Qt::Key_VolumeUp) {
            alsaWorker.adjustVolumeRelative(key == Qt::Key_VolumeDown ? -10 : 10);
        } else if (key == Qt::Key_VolumeMute) {
            alsaWorker.toggleMute();
        } else {
            app.postEvent(QApplication::focusWidget(), new QKeyEvent(QEvent::KeyRelease, key, Qt::NoModifier));
        }
    });


    // ===================================== DEVELOPMENT TEST CONNECT

    QObject::connect(&homePage, &autoapp::pages::HomePage::testConnect, [&openautoApp, &tcpWrapper, &ioService]() {
        OPENAUTO_LOG(info) << "Test connection";
        aasdk::tcp::ITCPEndpoint::SocketPointer socket = std::make_shared<boost::asio::ip::tcp::socket>(ioService);
        try {
            if (!tcpWrapper.connect(*socket, "192.168.1.8", 5277)) {
                openautoApp->start(std::move(socket));
            } else {
                OPENAUTO_LOG(info) << "TCP connection failed";
            }
        } catch (const boost::system::system_error& se) {
            OPENAUTO_LOG(error) << "Failed to open socket";
        }
    });

    QObject::connect(&homePage, &autoapp::pages::HomePage::bluetoothConnect, [&serviceFactory]() {
        serviceFactory.connectToLastBluetoothDevice();
    });
    QTimer::singleShot(100, &app, [&serviceFactory]() {
        OPENAUTO_LOG(info) << "Triggering bluetooth connect";
        serviceFactory.connectToLastBluetoothDevice();
    });

    openautoApp->waitForDevice(true);

    window.show();
    window.activateWindow();
    splash.finish(&window);

    auto result = app.exec();
    std::for_each(threadPool.begin(), threadPool.end(), std::bind(&std::thread::join, std::placeholders::_1));

    libusb_exit(usbContext);

    swcWorker.quit();
    swcWorker.wait();

    alsaWorker.quit();
    alsaWorker.wait();

    return result;
}

