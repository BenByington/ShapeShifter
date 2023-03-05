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

#ifndef MATH_VECTOR4_H
#define MATH_VECTOR4_H

#include <array>
#include <cassert>
#include <smmintrin.h>

namespace ShapeShifter::Math {

class alignas(16) Vector4 final {
public:
  Vector4(const std::array<float, 4>& data);
  Vector4(float f1, float f2, float f3, float f4);
  Vector4(const __m128 data) { _mm_store_ps(data_.begin(), data); }
  Vector4(const Vector4& orig);
  Vector4& operator=(const Vector4& other);
  ~Vector4();

  Vector4 operator+(const Vector4& other) const;
  Vector4 operator-(const Vector4& other) const;

  float& operator[](size_t idx) {
    assert(idx < 4);
    return data_[idx];
  }
  const float& operator[](size_t idx) const {
    assert(idx < 4);
    return data_[idx];
  }
  const float* begin() const { return data_.begin(); }
  const float* end() const { return data_.end(); }

  operator __m128() const { return _mm_load_ps(data_.begin()); }

  void print() const;

private:
  std::array<float, 4> data_;
};

} // namespace ShapeShifter::Math

#endif /* MATH_VECTOR4_H */
