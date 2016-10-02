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

namespace ShapeShifter {
namespace Opengl {
namespace math {

Vector4::Vector4(const std::array<float, 4>& other) :
    data_(other) {}

Vector4::Vector4(const Vector4& orig) : data_(orig.data_) {}

Vector4::~Vector4() {
}

const Vector4& Vector4::operator =(const Vector4& other) {
	data_ = other.data_;
	return *this;
}

void Vector4::print() const {
  for (size_t i = 0; i < 4; ++i) {
    std::cerr << data_[i] << std::endl;
  }
}

Vector4 Vector4::operator +(const Vector4& other) const {
  auto left = _mm_load_ps(data_.begin());
  auto right = _mm_load_ps(other.data_.begin());
  auto sum = _mm_add_ps(left, right);
  auto ret = Vector4(sum);
  //TODO clean this up... Shouldn't have to force to 1.
  ret[3] = 1;
  return ret;
}

Vector4 Vector4::operator -(const Vector4& other) const {
  auto left = _mm_load_ps(data_.begin());
  auto right = _mm_load_ps(other.data_.begin());
  auto sub = _mm_sub_ps(left, right);
  auto ret = Vector4{sub};
  ret[3] = 1;
  return ret;
}
}}} // ShapeShifter::Opengl::math
