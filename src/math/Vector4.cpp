/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   Vector4.cpp
 * Author: bbyington
 *
 * Created on August 5, 2016, 9:06 PM
 */

#include "Vector4.h"

#include <iostream>

namespace ShapeShifter::Math {

Vector4::Vector4(const std::array<float, 4>& other)
    : data_(other) {}

Vector4::Vector4(float f1, float f2, float f3, float f4) {
  data_[0] = f1;
  data_[1] = f2;
  data_[2] = f3;
  data_[3] = f4;
}

Vector4::Vector4(const Vector4& orig)
    : data_(orig.data_) {}

Vector4::~Vector4() {}

const Vector4& Vector4::operator=(const Vector4& other) {
  data_ = other.data_;
  return *this;
}

void Vector4::print() const {
  for (size_t i = 0; i < 4; ++i) {
    std::cerr << data_[i] << std::endl;
  }
}

Vector4 Vector4::operator+(const Vector4& other) const {
  auto left = _mm_load_ps(data_.begin());
  auto right = _mm_load_ps(other.data_.begin());
  auto sum = _mm_add_ps(left, right);
  auto ret = Vector4{sum};
  // ISSUE: Should be able to add a position + direction without this tweak.
  ret[3] = 1;
  return ret;
}

Vector4 Vector4::operator-(const Vector4& other) const {
  auto left = _mm_load_ps(data_.begin());
  auto right = _mm_load_ps(other.data_.begin());
  auto sub = _mm_sub_ps(left, right);
  auto ret = Vector4{sub};
  ret[3] = 1;
  return ret;
}
} // namespace ShapeShifter::Math
