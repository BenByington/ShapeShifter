/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   Vector3.cpp
 * Author: bbyington
 *
 * Created on August 22, 2016, 8:25 AM
 */

#include "Vector3.h"

#include <cassert>
#include <cmath>
#include <iostream>

namespace ShapeShifter::Math {

Vector3 Vector3::cross(const Vector3& other) const {
  auto ret = Vector3 {};
  ret[0] = data_[1] * other.data_[2] - data_[2]*other.data_[1];
  ret[1] = data_[2] * other.data_[0] - data_[0]*other.data_[2];
  ret[2] = data_[0] * other.data_[1] - data_[1]*other.data_[0];
  return ret;
}

float Vector3::dot(const Vector3& other) const {
  auto ret = 0.0f;
  for (size_t i = 0; i < 3; ++i) {
    ret += data_[i] * other[i];
  }
  return ret;
}

float Vector3::Magnitude() const {
  auto mag = 0.0f;
  for (size_t i = 0; i < 3; ++i) {
    mag += data_[i]*data_[i];
  }
  mag = sqrt(mag);
  return mag;
}

void Vector3::Normalize() {
  auto mag = Magnitude();
  for (size_t i = 0; i < 3; ++i) {
    data_[i] /= mag;
  }
  //ISSUE: Create unit tests (this should be one of the checks)
  //assert(Magnitude() > .99 && Magnitude() < 1.01);
}

void Vector3::Print() const {
  std::cerr << data_[0] << " " << data_[1] << " " << data_[2] << std::endl;
}

} // ShapeShifter::Math

