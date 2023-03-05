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

#include <concepts>

namespace ShapeShifter::Rendering::Shaders::Language {

class Vec3 {};
class Vec4 {
public:
  static Vec4 Create(Vec3, float) { return Vec4{}; }
};
class Mat4 {
public:
  Vec4 operator*(const Vec4& other) { return Vec4{}; }
};

/*
 * Every valid variable type in GLSL needs a specialization for this
 * class, defining what string to use for the variable type Common
 * types will have a specialization supplied here, but any custom
 * types will need to do this themselves
 */
template <class T>
class VariableTraits;

template <>
struct VariableTraits<Mat4> {
  static constexpr const char* name() { return "mat4"; }
};
template <>
struct VariableTraits<Vec4> {
  static constexpr const char* name() { return "vec4"; }
};

template <>
struct VariableTraits<Vec3> {
  static constexpr const char* name() { return "vec3"; }
};

template <typename T>
concept InterfaceType =
    std::same_as<T, Mat4> || std::same_as<T, Vec4> || std::same_as<T, Vec3>;

} // namespace ShapeShifter::Rendering::Shaders::Language

#endif /* RENDERING_SHADERS_LANGUAGE_TYPES_H */
