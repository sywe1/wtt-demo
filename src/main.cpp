#include "mainwindow.hpp"

#include <QApplication>
#include <QSurfaceFormat>
int main(int argc, char** argv)
{
  QSurfaceFormat format;
  format.setVersion(3, 3);
  format.setProfile(QSurfaceFormat::CoreProfile);
  QSurfaceFormat::setDefaultFormat(format);
  QApplication app(argc, argv);

  MainWindow window;

  window.show();
  return app.exec();
}