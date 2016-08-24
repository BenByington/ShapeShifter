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
#include <QtGui/QMouseEvent>
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
  root_->SetTranslation(Opengl::math::Vector4({-.5, -.5, -2.5, 1}));

  float pi = 4*std::atan(1.0f);

  std::shared_ptr<Opengl::RenderNode> second(new Opengl::SquareTest2D());
  second->SetRotation({-pi/2, 0 , 1, 0});
  root_->AddChild(second);

  std::shared_ptr<Opengl::RenderNode> third(new Opengl::SquareTest2D());
  third->SetTranslation(Opengl::math::Vector4({0, 0 , -1.0, 1.0}));
  second->AddChild(third);

  std::shared_ptr<Opengl::RenderNode> fourth(new Opengl::SquareTest2D());
  fourth->SetRotation({pi/2, 1, 0, 0});
  third->AddChild(fourth);

  std::shared_ptr<Opengl::RenderNode> fifth(new Opengl::SquareTest2D());
  fifth->SetTranslation(Opengl::math::Vector4({0, 0 , -1.0, 1.0}));
  fourth->AddChild(fifth);

  std::shared_ptr<Opengl::RenderNode> sixth(new Opengl::SquareTest2D());
  sixth->SetRotation({pi/2, 0, 1, 0});
  sixth->SetTranslation(Opengl::math::Vector4({-1, 0 , 1, 1.0}));
  //fifth->AddChild(sixth);

	root_->UpdateData();

	std::vector<std::unique_ptr<Opengl::Shader>> shaders;
	shaders.emplace_back(
	    new Opengl::VertexShader("/Users/bbyington/ShapeShifter/shaders/vertex/BasicVertexShader.vert"));
	shaders.emplace_back(
	    new Opengl::FragmentShader("/Users/bbyington/ShapeShifter/shaders/fragment/BasicFragmentShader.frag"));
	program_.reset(new Opengl::ShaderProgram(std::move(shaders)));
  auto frust = Opengl::Frustum::Build()->aspect(1)->fov(.5)->far(300)->near(0.5);
  camera_.reset(new Opengl::Camera(frust, 2.5));
  camera_->ChangePosition(Opengl::math::Vector4({0, 0, 0, 1.0f}));
}

void MyQtWidget::resizeGL(int width, int height) {
    height = height?height:1;
    float aspect = width / static_cast<float>(height);
    camera_->SetAspectRatio(aspect);

}

void MyQtWidget::paintGL() {
  // TODO not sure if this glClear is needed anymore
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

		root_->RenderTree(*camera_, *program_);
}

void PixelCoordToCameraProj(int x, int y) {

}

void MyQtWidget::mousePressEvent(QMouseEvent* event) {
  if (Qt::MouseButton::LeftButton == event->button())
  tracking_mouse = true;
  last_mouse_coords_rel_ = PixelCoordToCameraProj(event->localPos());
}

void MyQtWidget::mouseReleaseEvent(QMouseEvent* event) {
  tracking_mouse = false;
}

void MyQtWidget::mouseMoveEvent(QMouseEvent* event) {
  if (tracking_mouse) {
    auto next_mouse_coords_rel = PixelCoordToCameraProj(event->localPos());
    camera_->PivotAroundLook(last_mouse_coords_rel_, next_mouse_coords_rel);
    last_mouse_coords_rel_ = next_mouse_coords_rel;
    update();
  }
}

std::pair<float, float> MyQtWidget::PixelCoordToCameraProj(const QPointF& p) const {
  return std::make_pair(
        p.x()*2/static_cast<float>(width()) - 1.0f,
        1.0f - p.y()*2/static_cast<float>(height()));
}

}} //ShapeShifter::ui
