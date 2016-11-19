/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   Types.h
 * Author: bbyington
 *
 * Created on November 15, 2016, 8:40 AM
 */

#ifndef RENDERING_SHADERS_LANGUAGE_TYPES_H
#define RENDERING_SHADERS_LANGUAGE_TYPES_H

namespace ShapeShifter {
namespace Rendering {
namespace Shaders {
namespace Language {

template <class T>
class VariableTraits;

class Vec3 {};
class Vec4 {
public:
  static Vec4 Create(Vec3, float) {}

};
class Mat4 {
public:
  Vec4 operator*(const Vec4& other) {}
};


template <>
struct VariableTraits<Mat4> {
  static constexpr const char* name() {
    return "mat4";
  }
  static constexpr bool InterfaceAllowed = true;
};
template <>
struct VariableTraits<Vec4> {
  static constexpr const char* name() {
    return "vec4";
  }
  static constexpr bool InterfaceAllowed = true;
};

template <>
struct VariableTraits<Vec3> {
  static constexpr const char* name() {
    return "vec3";
  }
  static constexpr bool InterfaceAllowed = true;
};

}}}} // ShapeShifter::Rendering::Shaders::Language

#endif /* RENDERING_SHADERS_LANGUAGE_TYPES_H */

