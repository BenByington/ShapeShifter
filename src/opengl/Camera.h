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

namespace ShapeShifter {
namespace Opengl {

class Camera {
public:
  Camera(const Frustum& frust);
  virtual ~Camera();

  math::Matrix4 ProjectionMatrix() const { return frust_.FrustTransform(); }

private:
  Frustum frust_;

};

}} // ShapeShifter::Opengl

#endif /* CAMERA_H */

