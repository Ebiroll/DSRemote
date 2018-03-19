

#include <QApplication>
#include <QStyle>
#include <QStyleFactory>

#include "mainwindow.h"


int main(int argc, char *argv[])
{
  QApplication app(argc, argv);

//  app.setAttribute(Qt::AA_DontUseNativeMenuBar);

#if QT_VERSION >= 0x050000
  qApp->setStyle(QStyleFactory::create("Fusion"));
#endif
  qApp->setStyleSheet("QLabel, QMessageBox { messagebox-text-interaction-flags: 5; }");

  class UI_Mainwindow MainWindow;

  return app.exec();
}






