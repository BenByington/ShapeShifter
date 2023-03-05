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

#ifndef MATH_QUATERNION_H
#define MATH_QUATERNION_H

#include "math/Matrix4.h"
#include "math/Vector3.h"

namespace ShapeShifter::Math {

class Quaternion {
public:
  Quaternion(float s, float x, float y, float z);
  Quaternion(float s, const Vector3& dir);
  Quaternion()
      : Quaternion(0, 0, 0, 0) {}
  Quaternion(const Quaternion& orig) = default;
  Quaternion& operator=(const Quaternion& other) = default;
  virtual ~Quaternion() {}

  Quaternion operator*(const Quaternion& other) const;
  // TODO leave inline?
  Quaternion Inverse() const {
    Quaternion ret = *this;
    ret.qx *= -1;
    ret.qy *= -1;
    ret.qz *= -1;
    return ret;
  }
  void Normalize();

  Matrix4 RotationMatrix() const;

private:
  float qw;
  float qx;
  float qy;
  float qz;
};

} // namespace ShapeShifter::Math

#endif /* MATH_QUATERNION_H */
