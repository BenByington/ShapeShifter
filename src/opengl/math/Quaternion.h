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
#include "opengl/math/Vector3.h"

namespace ShapeShifter {
namespace Opengl {
namespace math {

class Quaternion {
public:
  Quaternion(float s, float x, float y, float z);
  Quaternion(float s, const Vector3& dir);
  Quaternion() : Quaternion(0, 0, 0, 0) {}
  Quaternion(const Quaternion& orig) = default;
	Quaternion& operator=(const Quaternion& other) = default;
  virtual ~Quaternion() {}

  Quaternion operator*(const Quaternion& other) const;
  void Normalize();

  Matrix4 RotationMatrix() const;

private:
  float qw;
  float qx;
  float qy;
  float qz;
};

}}} // ShapeShifter::Opengl::math

#endif /* QUATERNION_H */

