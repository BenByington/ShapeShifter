/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   Quaternion.h
 * Author: bbyington
 *
 * Created on August 7, 2016, 1:09 PM
 */

#ifndef QUATERNION_H
#define QUATERNION_H

#include "opengl/math/Matrix4.h"

namespace ShapeShifter {
namespace Opengl {
namespace math {

class Quaternion {
public:
  Quaternion(float s, float x, float y, float z);
  Quaternion() : Quaternion(1, 0, 0, 0) {} // TODO make matrix4 default to I as well?
  Quaternion(const Quaternion& orig) = default;
	Quaternion& operator=(const Quaternion& other) = default;
  virtual ~Quaternion() {}
	
  Matrix4 RotationMatrix() const;
  
  //TODO encapsulate?
  //TODO add normalization function
  float qw;
  float qx;
  float qy;
  float qz;
};

}}} // ShapeShifter::Opengl::math

#endif /* QUATERNION_H */

