#include "HomePage.hpp"
#include "ui_HomePage.h"
#include "OpenautoLog.hpp"

namespace autoapp
{
namespace pages
{

HomePage::HomePage(QWidget *parent)
  : QWidget(parent)
  , ui_(new Ui::HomePage)
{
  ui_->setupUi(this);
  connect(ui_->pushButtonTestConnect, &QPushButton::clicked, this, &HomePage::testConnect);
  connect(ui_->pushButtonExit, &QPushButton::clicked, this, &HomePage::exit);
}

HomePage::~HomePage()
{
    delete ui_;
}


void HomePage::keyPressEvent(QKeyEvent* event)
{
  int key = event->key();
  if (key == Qt::Key_1) {
    QKeyEvent *newEvent = new QKeyEvent(QEvent::KeyPress, Qt::Key_Tab, Qt::ShiftModifier, "");
    QCoreApplication::postEvent(this, newEvent);
  } else if (key == Qt::Key_2) {
    QKeyEvent *newEvent = new QKeyEvent(QEvent::KeyPress, Qt::Key_Tab, Qt::NoModifier, "");
    QCoreApplication::postEvent(this, newEvent);
  } else if (key == Qt::Key_Return) {
    QWidget *focusedWidget = QApplication::focusWidget();
    if (focusedWidget) {
      QPushButton *button = qobject_cast<QPushButton*>(focusedWidget);
      button->click();
    }
  } else {
    QWidget::keyPressEvent(event);
  }
}


}
}
