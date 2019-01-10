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

/*
 * Every valid variable type in GLSL needs a specialization for this class,
 * defining both what string to use for the variable type, and if the
 * variable is allowed to be used as an InterfaceVariable (between c++ and opengl)
 * Common types will have a specialization supplied here, but any custom types
 * will need to do this themselves
 */
template <class T>
class VariableTraits;

struct Vec3 {};
class Vec4 {
public:
  static Vec4 Create(Vec3, float) {}

};
class Mat4 {
public:
  Vec4 operator*(const Vec4& other) {}
};

class Float {
public:
  Vec4 operator*(const Vec4&) {}
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

template <>
struct VariableTraits<Float> {
  static constexpr const char* name() {
    return "float";
  }
  static constexpr bool InterfaceAllowed = true;
};

}}}} // ShapeShifter::Rendering::Shaders::Language

#endif /* RENDERING_SHADERS_LANGUAGE_TYPES_H */

