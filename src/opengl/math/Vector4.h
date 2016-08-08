/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   Vector4.h
 * Author: bbyington
 *
 * Created on August 5, 2016, 9:06 PM
 */

#ifndef VECTOR4_H
#define VECTOR4_H

#include <array>
#include <smmintrin.h>

namespace ShapeShifter {
namespace Opengl {
namespace math {

class alignas(16) Vector4 {
public:
	//TODO make sure I'm storing in correct order
  Vector4(const std::array<float, 4>& data);
	Vector4(const __m128 data) {_mm_store_ps(data_.begin(), data); }
  Vector4(const Vector4& orig);
	const Vector4& operator=(const Vector4& other);
  virtual ~Vector4();

	//TODO ensure idx is valid?
	float operator[](int idx) const { data_[idx];}
  const float* begin() const { return data_.begin(); }
  const float* end() const { return data_.end(); }

	operator __m128() const { return _mm_load_ps(data_.begin()); }

private:
  std::array<float, 4> data_;
};

}}} // ShapeShifter::Opengl::math

#endif /* VECTOR4_H */

