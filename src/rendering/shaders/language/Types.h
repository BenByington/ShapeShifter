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

#include "rendering/shaders/language/Variable.h"
#include "rendering/shaders/Pack.h"

#include <string>

namespace ShapeShifter {
namespace Rendering {
namespace Shaders {
namespace Language {

struct Vec3Policy {
  static constexpr bool InterfaceAllowed = true;
  static constexpr bool Subtract = true;
  using CtorArgs = Pack<Pack<>>;
  static constexpr const char* TypeNameImpl() { return "vec3"; }
};
using Vec3 = Variable<Vec3Policy>;

struct Vec4Policy {
  static constexpr bool InterfaceAllowed = true;
  static constexpr bool Subtract = true;
  using CtorArgs = Pack<Pack<>,
                        Pack<Vec3, float>>;
  static constexpr const char* TypeNameImpl() { return "vec4"; }
};
using Vec4 = Variable<Vec4Policy>;

struct Mat4Policy {
  static constexpr bool InterfaceAllowed = true;
  static constexpr bool Subtract = true;
  using CtorArgs = Pack<Pack<>>;
  static constexpr const char* TypeNameImpl() { return "mat4"; }
};
using Mat4 = Variable<Mat4Policy>;

struct FloatPolicy {
  static constexpr bool InterfaceAllowed = true;
  static constexpr bool Subtract = true;
  using CtorArgs = Pack<Pack<>>;
  static constexpr const char* TypeNameImpl() { return "float"; }
};
using Float = Variable<FloatPolicy>;

inline Expr<Vec4> operator*(const Expr<Mat4>&, const Expr<Vec4>& v) {}
inline Expr<Vec3> operator-(const Expr<Vec3>&, const Expr<Vec3>&) {}

}}}} // ShapeShifter::Rendering::Shaders::Language

#endif /* RENDERING_SHADERS_LANGUAGE_TYPES_H */

