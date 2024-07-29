#include <iostream>
#include <QFile>
#include <QChar>
#include <QTextStream>
#include <QKeyEvent>
#include <QFileInfo>
#include "OpenautoLog.hpp"
#include "KeyReceiver.hpp"
#include "OpenautoLog.hpp"


namespace autoapp
{
namespace service
{


void KeyReceiver::run()
{
    QString pipeName = "inject_keypress.fifo";
    QFile pipeFile(pipeName);

    if (!pipeFile.open(QIODevice::ReadOnly)) {
        OPENAUTO_LOG(error) << "[KeyReceiver] Failed to open keyreceiver pipe";
        return;
    }

    QTextStream in(&pipeFile);
    while (true) {
        QString line = in.readLine();
        if (line.isEmpty()) {
            QThread::msleep(10);
            continue;
        }
        OPENAUTO_LOG(info) << "[KeyReceiver] Got line: " << line.toStdString();

        if (line == "ENTER") {
            emit onKeyPress(Qt::Key_Enter);
        } else if (line == "LEFT") {
            emit onKeyPress(Qt::Key_Left);
        } else if (line == "RIGHT") {
            emit onKeyPress(Qt::Key_Right);
        } else if (line == "UP") {
            emit onKeyPress(Qt::Key_Up);
        } else if (line == "DOWN") {
            emit onKeyPress(Qt::Key_Down);
        } else if (line == "BACK") {
            emit onKeyPress(Qt::Key_Escape);
        } else if (line == "HOME") {
            emit onKeyPress(Qt::Key_H);
        } else if (line == "PHONE") {
            emit onKeyPress(Qt::Key_P);
        } else if (line == "CALL_END") {
            emit onKeyPress(Qt::Key_O);
        } else if (line == "PLAY") {
            emit onKeyPress(Qt::Key_X);
        } else if (line == "PAUSE") {
            emit onKeyPress(Qt::Key_C);
        } else if (line == "PREV") {
            emit onKeyPress(Qt::Key_MediaPrevious);
        } else if (line == "TOGGLE_PLAY") {
            emit onKeyPress(Qt::Key_MediaPlay);
        } else if (line == "NEXT") {
            emit onKeyPress(Qt::Key_MediaNext);
        } else if (line == "MICROPHONE") {
            emit onKeyPress(Qt::Key_M);
        } else if (line == "VOLUME_UP") {
            emit onKeyPress(Qt::Key_VolumeUp);
        } else if (line == "VOLUME_DOWN") {
            emit onKeyPress(Qt::Key_VolumeDown);
        } else if (line == "VOLUME_MUTE") {
            emit onKeyPress(Qt::Key_VolumeMute);
        } else {
            QKeySequence seq = QKeySequence(line);
            emit onKeyPress(seq[0]);
        }
    }
}

}
}
