#include "ui_ProjectionPage.h"
#include "ProjectionPage.hpp"
#include "OpenautoLog.hpp"
#include "../Service/Alsa.hpp"
#include <QKeyEvent>

namespace autoapp
{
namespace pages
{

ProjectionPage::ProjectionPage(autoapp::service::Alsa *alsa, QWidget *parent)
    : QWidget(parent)
    , alsa_(alsa)
    , ui_(new Ui::ProjectionPage)
{
    ui_->setupUi(this);
    aaFrame = ui_->aaFrame;


    QProgressBar* volumeIndicator = ui_->volumeIndicator;
    volumeIndicationTimeout_ = new QTimer(this);
    volumeIndicationTimeout_->setSingleShot(true);
    connect(volumeIndicationTimeout_, &QTimer::timeout, this, [volumeIndicator]() {
        volumeIndicator->hide();
    });

    connect(alsa_, &autoapp::service::Alsa::onVolumeChange, this, &ProjectionPage::updateVolume);
}

ProjectionPage::~ProjectionPage()
{
    delete ui_;
}

void ProjectionPage::updateVolume(int volume)
{
    volumeIndicationTimeout_->stop();
    ui_->volumeIndicator->setValue(volume);
    ui_->volumeIndicator->show();
    volumeIndicationTimeout_->start(2000);
}

void ProjectionPage::keyPressEvent(QKeyEvent* event)
{
    int key = event->key();
    if (key == Qt::Key_J) {
        QProgressBar* volumeIndicator = ui_->volumeIndicator;
        if (volumeIndicator->hasFocus()) {
            volumeIndicator->hide();
        } else {
            volumeIndicationTimeout_->stop();
            volumeIndicator->show();
            volumeIndicator->setFocus();
            volumeIndicationTimeout_->start();
        }
    } else if (key == Qt::Key_1 || key == Qt::Key_2) {
        alsa_->adjustVolumeRelative(key == Qt::Key_1 ? -5 : 5);
    } else {
        QWidget::keyPressEvent(event);
    }
}


}
}
