/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   Matrix4.cpp
 * Author: bbyington
 *
 * Created on August 5, 2016, 8:21 AM
 */

#include "Matrix4.h"

#include <cassert>
#include <immintrin.h>
#include <iostream>

namespace ShapeShifter {
namespace Math {

Matrix4::Matrix4(const std::array<float, 16>& orig) : data_(orig) {}

Matrix4::Matrix4(const Matrix4& orig) : data_(orig.data_) {}

const Matrix4& Matrix4::operator =(const Matrix4& other) {
	data_ = other.data_;
	return *this;
}

Matrix4::~Matrix4() {
}

float Matrix4::operator()(size_t row, size_t col) const {
	return data_[row + col*DIM];
}

void Matrix4::WriteColumn(size_t col, const Vector4& v) {
  std::copy(v.begin(), v.end(), data_.begin() + DIM*col);
}

Matrix4::Matrix4() : data_({0}) {}

Matrix4 Matrix4::Identity() {
	return {{1, 0, 0, 0,
			     0, 1, 0, 0,
			     0, 0, 1, 0,
			     0, 0, 0, 1}};
}

Matrix4 Matrix4::Scale(float x, float y, float z) {
	return {{x, 0, 0, 0,
			     0, y, 0, 0,
			     0, 0, z, 0,
			     0, 0, 0, 1}};
}

Matrix4 Matrix4::Translate(float x, float y, float z) {
	return {{1, 0, 0, x,
			     0, 1, 0, y,
			     0, 0, 1, z,
			     0, 0, 0, 1}};
}

Vector4 Matrix4::operator*(const Vector4& right) const {
	auto ret = _mm_setzero_ps();

	for (size_t i = 0; i < 4; ++i) {
    auto temp2 = right[i];
	  auto temp = _mm_set1_ps(temp2);
    auto col = _mm_load_ps(data_.begin() + i*DIM);
	  temp = _mm_mul_ps(temp, col);
		ret = _mm_add_ps(ret, temp);
	}
  return Vector4(ret);
}

Matrix4 Matrix4::operator *(const Matrix4& right) const {
	auto ret = Matrix4{};
  assert(alignof(data_) == 4);

  for (size_t i = 0; i < DIM; ++i) {
    auto col = _mm_load_ps(right.data_.begin() + i*DIM);
    auto vec = (*this)*col;
    ret.WriteColumn(i, vec);
  }

	return ret;

}

void Matrix4::print() const {
  std::cerr << data_[0*DIM+0] << ", " << data_[1*DIM+0] << " " << data_[2*DIM+0] << " " << data_[3*DIM+0] << std::endl;
  std::cerr << data_[0*DIM+1] << ", " << data_[1*DIM+1] << " " << data_[2*DIM+1] << " " << data_[3*DIM+1] << std::endl;
  std::cerr << data_[0*DIM+2] << ", " << data_[1*DIM+2] << " " << data_[2*DIM+2] << " " << data_[3*DIM+2] << std::endl;
  std::cerr << data_[0*DIM+3] << ", " << data_[1*DIM+3] << " " << data_[2*DIM+3] << " " << data_[3*DIM+3] << std::endl;
}

}} // ShapeShifter::Math
