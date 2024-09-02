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

    volumeIndicationTimeout_ = new QTimer(this);
    volumeIndicationTimeout_->setSingleShot(true);
    connect(volumeIndicationTimeout_, &QTimer::timeout, this, [&ui = ui_]() {
        ui->volumeIndicator->hide();
        ui->aaFrame->setFocus();
    });

    connect(alsa_, &autoapp::service::Alsa::onVolumeChange, this, &ProjectionPage::updateVolume);
}

ProjectionPage::~ProjectionPage()
{
    delete ui_;
}

void ProjectionPage::updateVolume(int volume)
{
    ui_->volumeIndicator->setValue(volume);
    ui_->volumeIndicator->update();
    showVolumeIndicator();
}

void ProjectionPage::keyPressEvent(QKeyEvent* event)
{
    int key = event->key();
    if (key == Qt::Key_J) {
        if (ui_->volumeIndicator->hasFocus()) {
            hideVolumeIndicator();
        } else {
            showVolumeIndicator();
        }
    } else if (key == Qt::Key_1 || key == Qt::Key_2) {
        alsa_->adjustVolumeRelative(key == Qt::Key_1 ? -5 : 5);
    } else {
        QWidget::keyPressEvent(event);
    }
}


void ProjectionPage::showVolumeIndicator()
{
    ui_->volumeIndicator->show();
    ui_->volumeIndicator->setFocus();
    volumeIndicationTimeout_->start(2000);
}


void ProjectionPage::hideVolumeIndicator()
{
    volumeIndicationTimeout_->start(0);
}


}
}
