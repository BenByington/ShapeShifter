/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   MyQtWidget.h
 * Author: bbyington
 *
 * Created on July 24, 2016, 8:59 AM
 */

#ifndef _MYQTWIDGET_H
#define _MYQTWIDGET_H

#include <cmath>
#include <iostream>

#include <QGLWidget>

#include "ui_MyQtWidget.h"

class MyQtWidget : public QGLWidget {
    Q_OBJECT
public:
    MyQtWidget();
    virtual ~MyQtWidget();
private:
    Ui::MyQtWidget widget;
protected:
    void initializeGL() override {

        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        glClearDepth(1.0f);

        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);

        glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
    }

    void gluPerspective(double fovy,double aspect, double zNear, double zFar)
    {
        // Start in projection mode.
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        double xmin, xmax, ymin, ymax;
        ymax = zNear * tan(fovy * M_PI / 360.0);
        ymin = -ymax;
        xmin = ymin * aspect;
        xmax = ymax * aspect;
        glFrustum(xmin, xmax, ymin, ymax, zNear, zFar);
    }

    void resizeGL(int width, int height) override {
        height = height?height:1;

        glViewport( 0, 0, (GLint)width, (GLint)height );

        gluPerspective(45.0f,(GLfloat)width/(GLfloat)height,0.1f,100.0f);

        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

    }

    void paintGL() override {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glLoadIdentity();

        glTranslatef(-1.5f,0.0f,-6.0f);

        glBegin(GL_TRIANGLES);
            glVertex3f( 0.0f, 1.0f, 0.0f);
            glVertex3f(-1.0f,-1.0f, 0.0f);
            glVertex3f( 1.0f,-1.0f, 0.0f);
        glEnd(); 

        glTranslatef(3.0f,0.0f,0.0f);

        glBegin(GL_QUADS);
            glVertex3f(-1.0f, 1.0f, 0.0f);
            glVertex3f( 1.0f, 1.0f, 0.0f);
            glVertex3f( 1.0f,-1.0f, 0.0f);
            glVertex3f(-1.0f,-1.0f, 0.0f);
        glEnd(); 
    }

};

#endif /* _MYQTWIDGET_H */
