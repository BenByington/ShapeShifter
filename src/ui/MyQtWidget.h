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

#include <QtWidgets/QOpenGLWidget>

#include "ui_MyQtWidget.h"

#include "rendering/World.h"

namespace ShapeShifter::ui {

class MyQtWidget : public QOpenGLWidget {
  Q_OBJECT
public:
  MyQtWidget();
  virtual ~MyQtWidget();

private:
  Ui::MyQtWidget widget;

protected:
  void initializeGL() override;
  void resizeGL(int width, int height) override;
  void paintGL() override;

  void mousePressEvent(QMouseEvent* event) override;
  void mouseReleaseEvent(QMouseEvent* event) override;
  void mouseMoveEvent(QMouseEvent* event) override;

  // ISSUE: see if we can find a more elegant way to unify slots via templates.
  // Still need hand coded linkage functions below.
  template <class Scenario>
  void ScenarioSlotHelper() {
    this->makeCurrent();
    world_ = Scenario().Setup();
    update();
  }

  // Can we make this not public?
public slots:
  void EmptyScenario();
  void Squares2DScenario();
  void IndexBufferScenario();

private:
  std::pair<float, float> PixelCoordToCameraProj(const QPointF& p) const;

  std::unique_ptr<Rendering::World> world_;

  bool tracking_mouse = false;
  std::pair<float, float> last_mouse_coords_rel_;
};

} // namespace ShapeShifter::ui
#endif /* _MYQTWIDGET_H */
