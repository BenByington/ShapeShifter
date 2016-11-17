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

class Vec3 {};
class Vec4 {
public:
  static Vec4 Create(Vec3, float) {}

};
class Mat4 {
public:
  Vec4 operator*(const Vec4& other) {}
};

}}}} // ShapeShifter::Rendering::Shaders::Language

#endif /* RENDERING_SHADERS_LANGUAGE_TYPES_H */

