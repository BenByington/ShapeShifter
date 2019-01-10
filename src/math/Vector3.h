/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   Vector3.h
 * Author: bbyington
 *
 * Created on August 22, 2016, 8:25 AM
 */

#ifndef MATH_VECTOR3_H
#define MATH_VECTOR3_H

#include <array>

namespace ShapeShifter {
namespace Math {

class Vector3 {
public:
  Vector3() : data_{0, 0, 0,} {}
  Vector3(float x, float y, float z) : data_{x, y, z} {}
  Vector3(const Vector3& orig) = default;
  virtual ~Vector3() {}

  Vector3 cross(const Vector3& other) const;
  float dot(const Vector3& other) const;
  float Magnitude() const;
  void Normalize();

  const float& operator[](size_t idx) const { return data_[idx]; }
  float& operator[](size_t idx) { return data_[idx]; }
  const float* data() const { return &data_[0]; }

  void Print() const;
private:
  std::array<float, 3> data_;

};

}} // ShapeShifter::Math

#endif /* MATH_VECTOR3_H */

