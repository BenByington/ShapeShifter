/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   MainWindow.h
 * Author: bbyington
 *
 * Created on July 19, 2016, 7:25 PM
 */

#ifndef _MAINWINDOW_H
#define _MAINWINDOW_H

#include <memory>

#include "ui_MainWindow.h"

#include "ui/MyQtWidget.h"

namespace ShapeShifter {
namespace ui {

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    MainWindow();
    virtual ~MainWindow();
private:
    Ui::MainWindow widget;
    std::unique_ptr<MyQtWidget> qtw_;
};

}} //ShapeShifter::ui

#endif /* _MAINWINDOW_H */
