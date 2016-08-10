/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   Quaternion.cpp
 * Author: bbyington
 * 
 * Created on August 7, 2016, 1:09 PM
 */

#include "Quaternion.h"

#include "opengl/math/Vector4.h"

namespace ShapeShifter {
namespace Opengl {
namespace math {

Quaternion::Quaternion(float s, float x, float y, float z) 
  : qw(s), qx(x), qy(y), qz(z) {}

Matrix4 Quaternion::RotationMatrix() const {
  // TODO This is row major returned as column major?
  return {{1 - 2*qy*qy - 2*qz*qz, 2*qx*qy + 2*qz*qw, 2*qx*qz - 2*qy*qw, 0
         ,2*qx*qy - 2*qz*qw, 1 - 2*qx*qx - 2*qz*qz,	2*qy*qz + 2*qx*qw, 0
         ,2*qx*qz + 2*qy*qw, 2*qy*qz - 2*qx*qw, 1 - 2*qx*qx - 2*qy*qy, 0
         ,0, 0, 0, 1}};

}

}}} // ShapeShifter::Opengl::math