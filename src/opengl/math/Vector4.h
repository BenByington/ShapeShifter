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

class alignas(16) Vector4 final {
public:
	//TODO make sure I'm storing in correct order
  explicit Vector4(const std::array<float, 4>& data);
  // TODO see why explicit breaks this...
	Vector4(const __m128 data) {_mm_store_ps(data_.begin(), data); }
  explicit Vector4(const Vector4& orig);
	const Vector4& operator=(const Vector4& other);
  ~Vector4();

  Vector4 operator+(const Vector4& other) const;
  Vector4 operator-(const Vector4& other) const;

	//TODO ensure idx is valid?
	float& operator[](int idx) { return data_[idx];}
	const float& operator[](int idx) const { return data_[idx];}
  const float* begin() const { return data_.begin(); }
  const float* end() const { return data_.end(); }

	operator __m128() const { return _mm_load_ps(data_.begin()); }

  void print() const;

private:
  std::array<float, 4> data_;
};

}}} // ShapeShifter::Opengl::math

#endif /* VECTOR4_H */

