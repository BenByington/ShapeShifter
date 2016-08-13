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

}}} // ShapeShifter::Opengl::math
