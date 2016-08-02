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

#include <QOpenGLWidget>

#include "ui_MyQtWidget.h"

#include "opengl/RenderNode.h"

class MyQtWidget : public QOpenGLWidget {
    Q_OBJECT
public:
    MyQtWidget();
    virtual ~MyQtWidget();
private:
    Ui::MyQtWidget widget;
protected:
    void initializeGL() override;
    void gluPerspective(double fovy,double aspect, double zNear, double zFar);
    void resizeGL(int width, int height) override;
    void paintGL() override;

private:
	GLuint vao = 0;
	std::unique_ptr<ShapeShifter::Opengl::RenderNode> root_;

};

#endif /* _MYQTWIDGET_H */
