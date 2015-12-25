

#include <QApplication>

#include "mainwindow.h"


int main(int argc, char *argv[])
{
  QApplication app(argc, argv);

  app.setAttribute(Qt::AA_DontUseNativeMenuBar);

  class UI_Mainwindow MainWindow;

  return app.exec();
}






