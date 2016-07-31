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

#include <QApplication>
#include <qwidget.h>

#include "ui/MainWindow.h"

int main(int argc, char *argv[]) {
    // initialize resources, if needed
    // Q_INIT_RESOURCE(resfile);
		

    QSurfaceFormat format;
		format.setVersion(4,1);
		format.setProfile(QSurfaceFormat::CoreProfile);
		QSurfaceFormat::setDefaultFormat(format);
		
    QApplication app(argc, argv);

    // create and show your widgets here
    MainWindow window;
    window.show();

    return app.exec();
}
