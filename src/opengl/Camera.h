/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   Camera.h
 * Author: bbyington
 *
 * Created on August 11, 2016, 6:36 PM
 */

#ifndef CAMERA_H
#define CAMERA_H

#include "opengl/math/Vector4.h"
#include "opengl/Frustum.h"
#include "opengl/math/Vector4.h"
#include "opengl/math/Quaternion.h"

namespace ShapeShifter {
namespace Opengl {

class Camera {
public:
  Camera(const Frustum& frust);
  virtual ~Camera();

  math::Matrix4 ProjectionMatrix() const;

  void ChangeRollLeft(float rads);
  void ChangePitchUp(float rads);
  void ChangeYawLeft(float rads);
  void ChangePosition(const math::Vector4& trans);

  void SetAspectRatio(float aspect);

private:
  Frustum frust_;
  math::Vector4 translation_{{0, 0, 0, 1}};
  math::Quaternion rotation_{0, 0, 0, 0};

};

}} // ShapeShifter::Opengl

#endif /* CAMERA_H */

