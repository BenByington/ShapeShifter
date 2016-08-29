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

	std::unique_ptr<Opengl::RenderNode> root(new Opengl::SquareTest2D());
  root->SetTranslation(Opengl::math::Vector4({-.5, -.5, -2.5, 1}));

  float pi = 4*std::atan(1.0f);

  std::shared_ptr<Opengl::RenderNode> second(new Opengl::SquareTest2D());
  second->SetRotation({-pi/2, 0 , 1, 0});
  root->AddChild(second);

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

  //TODO ownership is very weird here.  World needs a complete object.  Can
  //     provide mechanism to rotate/enable/disable nodes, but not add new
  //     ones or change point locations.
	root->UpdateData();

	std::vector<std::unique_ptr<Opengl::Shader>> shaders;
	shaders.emplace_back(
	    new Opengl::VertexShader("/Users/bbyington/ShapeShifter/shaders/vertex/BasicVertexShader.vert"));
	shaders.emplace_back(
	    new Opengl::FragmentShader("/Users/bbyington/ShapeShifter/shaders/fragment/BasicFragmentShader.frag"));
	std::unique_ptr<Opengl::ShaderProgram> program(new Opengl::ShaderProgram(std::move(shaders)));
  auto frust = Opengl::Frustum::Build()->aspect(1)->fov(.5)->far(300)->near(0.5);
  std::unique_ptr<Opengl::Camera> camera(new Opengl::Camera(frust, 2.5));
  camera->ChangePosition(Opengl::math::Vector4({0, 0, 0, 1.0f}));
  world_.reset(new Opengl::World(std::move(program), std::move(camera)));
  world_->SetRenderTree(std::move(root));
}

void MyQtWidget::resizeGL(int width, int height) {
    height = height?height:1;
    float aspect = width / static_cast<float>(height);
    world_->camera().SetAspectRatio(aspect);

}

void MyQtWidget::paintGL() {
  // TODO remove or put into world objet
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glLoadIdentity();

  world_->Render();
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
    world_->camera().PivotAroundLook(last_mouse_coords_rel_, next_mouse_coords_rel);
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
