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

#include <immintrin.h>
#include <iostream>

namespace ShapeShifter {
namespace Opengl {
namespace math {

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

//TODO check all assembly generated in this file for good SIMD use
Vector4 Matrix4::operator*(const Vector4& right) const {
  const __m128& r = right;
	__m128 ret = _mm_setzero_ps();

	for (size_t i = 0; i < 4; ++i) {
	  __m128 temp = _mm_set1_ps(right[i]);
    __m128 col = _mm_load_ps(data_.begin() + i*DIM);
	  temp = _mm_mul_ps(temp, col);
		ret = _mm_add_ps(ret, temp);
		// TODO look into xsave error about fused multiply add operations
		//_mm_fmadd_ps(temp, ret, ret);
	}
  // TODO look to see if it matters that we're saving as Vector4 instead of __m128
  return Vector4(ret);
}

Matrix4 Matrix4::operator *(const Matrix4& right) const {
  __m128 t0 = _mm_load_ps(data_.begin() + 0*DIM);
  __m128 t1 = _mm_load_ps(data_.begin() + 1*DIM);
  __m128 t2 = _mm_load_ps(data_.begin() + 2*DIM);
  __m128 t3 = _mm_load_ps(data_.begin() + 3*DIM);
	_MM_TRANSPOSE4_PS(t0, t1, t2, t3);

	Matrix4 ret;
  __m128 r;

  r = _mm_mul_ps(_mm_load_ps(right.data_.begin() + 0*DIM), t0);
  _mm_store_ps(ret.data_.begin() + 0*DIM, r);
  r = _mm_mul_ps(_mm_load_ps(right.data_.begin() + 1*DIM), t1);
  _mm_store_ps(ret.data_.begin() + 1*DIM, r);
  r = _mm_mul_ps(_mm_load_ps(right.data_.begin() + 2*DIM), t2);
  _mm_store_ps(ret.data_.begin() + 2*DIM, r);
  r = _mm_mul_ps(_mm_load_ps(right.data_.begin() + 3*DIM), t3);
  _mm_store_ps(ret.data_.begin() + 3*DIM, r);
	return ret;

}

void Matrix4::print() const {
  std::cerr << data_[0*DIM+0] << ", " << data_[1*DIM+0] << " " << data_[2*DIM+0] << " " << data_[3*DIM+0] << std::endl;
  std::cerr << data_[0*DIM+1] << ", " << data_[1*DIM+1] << " " << data_[2*DIM+1] << " " << data_[3*DIM+1] << std::endl;
  std::cerr << data_[0*DIM+2] << ", " << data_[1*DIM+2] << " " << data_[2*DIM+2] << " " << data_[3*DIM+2] << std::endl;
  std::cerr << data_[0*DIM+3] << ", " << data_[1*DIM+3] << " " << data_[2*DIM+3] << " " << data_[3*DIM+3] << std::endl;
}

}}} // ShapeShifter::Opengl::math
