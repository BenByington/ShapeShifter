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

#include <QtCore/QDebug>
#include <QtGui/QOpenGLContext>

#include <memory>
#include <cmath>

namespace ShapeShifter {
namespace ui {

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

  QString openglVersionString(QLatin1String(reinterpret_cast<const char*>(glGetString(GL_VERSION))));
  QString shaderVersionString(QLatin1String(reinterpret_cast<const char*>(glGetString(GL_SHADING_LANGUAGE_VERSION))));
  qDebug() << "Driver Version String:" << openglVersionString;
  qDebug() << "Shader Version String:" << shaderVersionString;
  qDebug() << "Current Context:" << this->format();

	root_.reset(new Opengl::SquareTest2D());
  root_->SetTranslation(Opengl::math::Vector4({-.5, -.5, 2.5, 1}));

  float pi = 4*std::atan(1.0f);

  std::shared_ptr<Opengl::RenderNode> second(new Opengl::SquareTest2D());
  second->SetRotation({pi/2, 0 , 1, 0});
  root_->AddChild(second);

  std::shared_ptr<Opengl::RenderNode> third(new Opengl::SquareTest2D());
  third->SetTranslation(Opengl::math::Vector4({0, 0 , 1.0, 1.0}));
  second->AddChild(third);

  std::shared_ptr<Opengl::RenderNode> fourth(new Opengl::SquareTest2D());
  fourth->SetRotation({pi/2, 0, 0, 1});
  third->AddChild(fourth);

  std::shared_ptr<Opengl::RenderNode> fifth(new Opengl::SquareTest2D());
  fifth->SetTranslation(Opengl::math::Vector4({0, 0 , 1.0, 1.0}));
  fourth->AddChild(fifth);

  std::shared_ptr<Opengl::RenderNode> sixth(new Opengl::SquareTest2D());
  sixth->SetRotation({pi/2, 1, 0, 0});
  sixth->SetTranslation(Opengl::math::Vector4({-1, 0 , -1.0, 1.0}));
  //fifth->AddChild(sixth);

	root_->UpdateData();

	std::vector<std::unique_ptr<Opengl::Shader>> shaders;
	shaders.emplace_back(
	    new Opengl::VertexShader("/Users/bbyington/ShapeShifter/shaders/vertex/BasicVertexShader.vert"));
	shaders.emplace_back(
	    new Opengl::FragmentShader("/Users/bbyington/ShapeShifter/shaders/fragment/BasicFragmentShader.frag"));
	program_.reset(new Opengl::ShaderProgram(std::move(shaders)));
  auto frust = Opengl::Frustum::Build()->aspect(1)->fov(.5)->far(3)->near(1.5);
  camera_.reset(new Opengl::Camera(frust));
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

		root_->RenderTree(*camera_, *program_);
}

}} //ShapeShifter::ui
