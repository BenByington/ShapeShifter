/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   Matrix4.h
 * Author: bbyington
 *
 * Created on August 5, 2016, 8:21 AM
 */

#ifndef OPENGL_MATH_MATRIX4_H
#define OPENGL_MATH_MATRIX4_H

#include <array>

#include "opengl/math/Vector4.h"

#include <smmintrin.h>

namespace ShapeShifter {
namespace Opengl {
namespace math {

// column major for now
class alignas(16) Matrix4 final {
public:

  static const size_t DIM = 4;

  Matrix4(const std::array<float, 16>& cols);
  Matrix4(const Matrix4& orig);
	const Matrix4& operator=(const Matrix4& other);
  ~Matrix4();

	// Don't know which of these will stick, just throwing out functions to try
	// for now
	float operator()(size_t row, size_t col) const;
	void WriteColumn(size_t col, const Vector4& v);

  static Matrix4 Identity();
	static Matrix4 Scale(float sx, float sy, float sz);
	static Matrix4 Translate(float tx, float ty, float tz);

	Vector4 operator*(const Vector4& right) const;
	Matrix4 operator*(const Matrix4& right) const;

  const float* data() const { return data_.begin(); }

  void print() const;

private:
	Matrix4();
	std::array<float,16> data_;
};

}}} // ShapeShifter::Opengl::math

#endif /* OPENGL_MATH_MATRIX4_H */

