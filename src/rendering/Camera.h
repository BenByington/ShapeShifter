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

#ifndef RENDERING_CAMERA_H
#define RENDERING_CAMERA_H

#include "math/Vector4.h"
#include "math/Quaternion.h"
#include "rendering/Frustum.h"

namespace ShapeShifter {
namespace Rendering {

class Camera {
public:
  Camera(const Frustum& frust, float look_depth);
  virtual ~Camera();

  Math::Matrix4 ProjectionMatrix() const;

  void ChangeRollLeft(float rads);
  void ChangePitchUp(float rads);
  void ChangeYawLeft(float rads);
  void ChangePosition(const Math::Vector4& trans);
  void PivotAroundLook(const std::pair<float, float>& start, const std::pair<float, float>& end);

  void SetAspectRatio(float aspect);

private:
  Frustum frust_;
  float look_depth_;
  Math::Vector4 translation_{{0, 0, 0, 1}};
  Math::Quaternion rotation_{0, 0, 0, 1};

};

}} // ShapeShifter::Rendering

#endif /* RENDERING_CAMERA_H */

