/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   Frustum.cpp
 * Author: bbyington
 *
 * Created on August 11, 2016, 6:41 PM
 */

#include "Frustum.h"

#include <cmath>

namespace ShapeShifter {
namespace Opengl {

math::Matrix4 Frustum::FrustTransform() const {
  float itana = 1.0 / std::tan(fov);

  return math::Matrix4 {{itana, 0, 0, 0,
                         0, aspect*itana, 0, 0,
                         0, 0, -(far + near) / (far - near), -1,
                         0, 0, -2*far*near/(far-near), 0}};
}

namespace detail {

  FrustumFov* FrustumBuilder::aspect(float a) { aspect_ = a; return this; }
  FrustumFar* FrustumBuilder::fov(float f) { fov_ = f; return this; }
  FrustumNear* FrustumBuilder::far(float f) { far_ = f; return this; }
  Frustum FrustumBuilder::near(float near) { return Frustum{near, far_, fov_, aspect_}; }
}

}} // ShapeShifter::Opengl
