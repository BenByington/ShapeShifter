/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   MainWindow.cpp
 * Author: bbyington
 *
 * Created on July 19, 2016, 7:25 PM
 */

#include "ui/MainWindow.h"

namespace ShapeShifter {
namespace ui {

MainWindow::MainWindow() {
    widget.setupUi(this);
    qtw_.reset(new MyQtWidget());
    widget.DisplayFrame->layout()->addWidget(qtw_.get());
    //QMainWindow::showMaximized();
}

MainWindow::~MainWindow() {
}

}} //ShapeShifter::ui