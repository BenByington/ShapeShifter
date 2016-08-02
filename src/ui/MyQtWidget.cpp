/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   MyQtWidget.cpp
 * Author: bbyington
 *
 * Created on July 24, 2016, 8:59 AM
 */

#include "ui/MyQtWidget.h"
#include "opengl/Shader.h"
#include "opengl/ShaderProgram.h"

#include <QDebug>
#include <QOpenGLContext>

MyQtWidget::MyQtWidget() {
  widget.setupUi(this);

}

MyQtWidget::~MyQtWidget() {
}

void MyQtWidget::initializeGL() {

  glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
  glClearDepth(1.0f);

  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LEQUAL);

  glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

  std::cerr << "valid: " << this->context()->isValid() << std::endl;
  QString openglVersionString(QLatin1String(reinterpret_cast<const char*>(glGetString(GL_VERSION))));
  QString shaderVersionString(QLatin1String(reinterpret_cast<const char*>(glGetString(GL_SHADING_LANGUAGE_VERSION))));
  qDebug() << "Driver Version String:" << openglVersionString;
  qDebug() << "Shader Version String:" << shaderVersionString;
  qDebug() << "Current Context:" << this->format();

	root_.reset(new ShapeShifter::Opengl::SquareTest2D());
	root_->UpdateData();

	std::vector<std::unique_ptr<ShapeShifter::Opengl::Shader>> shaders;
	shaders.emplace_back(
	    new ShapeShifter::Opengl::VertexShader("/Users/bbyington/ShapeShifter/shaders/vertex/BasicVertexShader.vert"));
	shaders.emplace_back(
	    new ShapeShifter::Opengl::FragmentShader("/Users/bbyington/ShapeShifter/shaders/fragment/BasicFragmentShader.frag"));
	ShapeShifter::Opengl::ShaderProgram program(std::move(shaders));
	program.UseProgram();

}

void MyQtWidget::gluPerspective(double fovy,double aspect, double zNear, double zFar) {
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

void MyQtWidget::resizeGL(int width, int height) {
    height = height?height:1;

    glViewport( 0, 0, (GLint)width, (GLint)height );

    gluPerspective(45.0f,(GLfloat)width/(GLfloat)height,0.1f,100.0f);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

}

void MyQtWidget::paintGL() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
		
		root_->RenderTree();
}
