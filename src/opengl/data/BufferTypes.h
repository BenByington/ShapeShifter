/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   BufferTypes.h
 * Author: bbyington
 *
 * Created on September 14, 2016, 4:00 PM
 */

#ifndef BUFFERTYPES_H
#define BUFFERTYPES_H

namespace ShapeShifter {
namespace Opengl {
namespace Data {

// TODO can I force these two to be automatically consistent?
enum SupportedBufferFlags : size_t {
  COLORS = 1,
  TEXTURES = 2,
  INDICES = 4,
  END_VALUE = 8
};

enum class SupportedBuffers {
  COLORS,
  TEXTURES,
  INDICES,
  VERTICES
};

static constexpr size_t floats_per_vert = 3;
static constexpr size_t floats_per_color = 3;
static constexpr size_t floats_per_text = 2;
static constexpr size_t floats_per_triangle = 3;

}}} // ShapeShifter::Opengl::Data

#endif /* BUFFERTYPES_H */

