#pragma once

#include <QWidget>
#include <QTimer>
#include "../Service/Alsa.hpp"

namespace Ui {
class ProjectionPage;
}

namespace autoapp
{
namespace pages
{

class ProjectionPage : public QWidget
{
    Q_OBJECT


public:
    QWidget *aaFrame;
    explicit ProjectionPage(autoapp::service::Alsa *alsaWorker, QWidget *parent = nullptr);
    ~ProjectionPage() override;
    void keyPressEvent(QKeyEvent* event) override;

private:
    autoapp::service::Alsa* alsa_;
    Ui::ProjectionPage* ui_;
    QTimer* volumeIndicationTimeout_;
    void updateVolume(int);
    void showVolumeIndicator();
    void hideVolumeIndicator();

};

}
}

