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

#include "scenarios/Empty.h"
#include "scenarios/tests/IndexBuffers.h"
#include "scenarios/tests/Squares2D.h"

#include <QtCore/QDebug>
#include <QtGui/QMouseEvent>
#include <QtGui/QOpenGLContext>

#include <cmath>
#include <memory>

namespace ShapeShifter::ui {

MyQtWidget::MyQtWidget() { widget.setupUi(this); }

MyQtWidget::~MyQtWidget() {}

void MyQtWidget::initializeGL() {
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  glClearDepth(1.0f);

  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LEQUAL);

  glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

  QString openglVersionString(
      QLatin1String(reinterpret_cast<const char*>(glGetString(GL_VERSION))));
  QString shaderVersionString(
      QLatin1String(reinterpret_cast<const char*>(glGetString(GL_SHADING_LANGUAGE_VERSION))));
  qDebug() << "Driver Version String:" << openglVersionString;
  qDebug() << "Shader Version String:" << shaderVersionString;
  qDebug() << "Current Context:" << this->format();

  // world_ = scenarios::Empty().Setup();
  // world_ = scenarios::tests::Squares2D().Setup();
  world_ = scenarios::tests::IndexBuffers().Setup();
}

void MyQtWidget::resizeGL(int width, int height) {
  height = height ? height : 1;
  auto aspect = static_cast<float>(width) / static_cast<float>(height);
  world_->camera().SetAspectRatio(aspect);
}

void MyQtWidget::paintGL() { world_->Render(); }

void PixelCoordToCameraProj(int x, int y) {}

void MyQtWidget::mousePressEvent(QMouseEvent* event) {
  if (Qt::MouseButton::LeftButton == event->button()) tracking_mouse = true;
  last_mouse_coords_rel_ = PixelCoordToCameraProj(event->localPos());
}

void MyQtWidget::mouseReleaseEvent(QMouseEvent* event) { tracking_mouse = false; }

void MyQtWidget::mouseMoveEvent(QMouseEvent* event) {
  if (tracking_mouse) {
    auto next_mouse_coords_rel = PixelCoordToCameraProj(event->localPos());
    world_->camera().PivotAroundLook(last_mouse_coords_rel_, next_mouse_coords_rel);
    last_mouse_coords_rel_ = next_mouse_coords_rel;
    update();
  }
}

std::pair<float, float> MyQtWidget::PixelCoordToCameraProj(const QPointF& p) const {
  return std::make_pair(p.x() * 2 / static_cast<float>(width()) - 1.0f,
                        1.0f - p.y() * 2 / static_cast<float>(height()));
}

void MyQtWidget::EmptyScenario() { ScenarioSlotHelper<scenarios::Empty>(); }

void MyQtWidget::Squares2DScenario() { ScenarioSlotHelper<scenarios::tests::Squares2D>(); }

void MyQtWidget::IndexBufferScenario() {
  ScenarioSlotHelper<scenarios::tests::IndexBuffers>();
}

} // namespace ShapeShifter::ui
