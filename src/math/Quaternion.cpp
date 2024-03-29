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

#include "math/Vector4.h"

#include <cmath>

namespace ShapeShifter::Math {

Quaternion::Quaternion(float x, float y, float z, float cosv, float sinv)
    : qw(cosv)
    , qx(sinv * x)
    , qy(sinv * y)
    , qz(sinv * z) {
  Normalize();
}

Quaternion::Quaternion(float s, float x, float y, float z)
    : Quaternion(x, y, z, std::cos(s / 2.0f), std::sin(s / 2.0f)) {}

Quaternion::Quaternion(float s, const Vector3& dir)
    : Quaternion(s, dir[0], dir[1], dir[2]) {}

Matrix4 Quaternion::RotationMatrix() const {
  return {{1 - 2 * qy * qy - 2 * qz * qz,
           2 * qx * qy + 2 * qz * qw,
           2 * qx * qz - 2 * qy * qw,
           0,
           2 * qx * qy - 2 * qz * qw,
           1 - 2 * qx * qx - 2 * qz * qz,
           2 * qy * qz + 2 * qx * qw,
           0,
           2 * qx * qz + 2 * qy * qw,
           2 * qy * qz - 2 * qx * qw,
           1 - 2 * qx * qx - 2 * qy * qy,
           0,
           0,
           0,
           0,
           1}};
}

Quaternion Quaternion::operator*(const Quaternion& other) const {
  auto ret = Quaternion{};

  ret.qw = qw * other.qw - qx * other.qx - qy * other.qy - qz * other.qz;
  ret.qx = qw * other.qx + other.qw * qx + qy * other.qz - qz * other.qy;
  ret.qy = qw * other.qy + other.qw * qy + qz * other.qx - qx * other.qz;
  ret.qz = qw * other.qz + other.qw * qz + qx * other.qy - qy * other.qx;
  ret.Normalize();
  return ret;
}

void Quaternion::Normalize() {
  auto mag = std::sqrt(qw * qw + qx * qx + qy * qy + qz * qz);
  qw /= mag;
  qx /= mag;
  qy /= mag;
  qz /= mag;
}

} // namespace ShapeShifter::Math
