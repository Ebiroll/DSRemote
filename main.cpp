

#include <QApplication>

#include "mainwindow.h"


int main(int argc, char *argv[])
{
  QApplication app(argc, argv);

  app.setAttribute(Qt::AA_DontUseNativeMenuBar);
  app.setAttribute(Qt::AA_ImmediateWidgetCreation);

  class UI_Mainwindow MainWindow;

  return app.exec();
}






