#pragma once

#include <memory>
#include <QWidget>
#include <QEvent>
#include <QKeyEvent>

namespace Ui {
class HomePage;
}

namespace autoapp
{
namespace pages
{

class HomePage : public QWidget
{
    Q_OBJECT

public:
    explicit HomePage(QWidget *parent = nullptr);
    ~HomePage() override;
    void keyPressEvent(QKeyEvent* event) override;

signals:
    void testConnect();
    void bluetoothConnect();
    void exit();

private:
    Ui::HomePage* ui_;
};

}
}
