/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   main.cpp
 * Author: bbyington
 *
 * Created on July 19, 2016, 7:20 PM
 */

#include <QtWidgets/QApplication>
#include <QtWidgets/qwidget.h>

#include "ui/MainWindow.h"

int main(int argc, char *argv[]) {
  auto format = QSurfaceFormat{};
  format.setVersion(4,1);
  format.setProfile(QSurfaceFormat::CoreProfile);
  QSurfaceFormat::setDefaultFormat(format);

  auto&& app = QApplication(argc, argv);

  // create and show your widgets here
  ShapeShifter::ui::MainWindow window;
  window.show();

  return app.exec();
}
