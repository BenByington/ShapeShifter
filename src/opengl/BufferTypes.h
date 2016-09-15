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

}} // ShapeShifter::Opengl

#endif /* BUFFERTYPES_H */

