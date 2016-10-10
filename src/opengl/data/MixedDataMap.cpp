/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include "opengl/data/MixedDataMap.h"

namespace ShapeShifter {
namespace Opengl {
namespace Data {

MixedSliceMap::MixedSliceMap(
    const std::vector<std::pair<SupportedBuffers, std::vector<float>&>>& float_data,
    const std::vector<std::pair<SupportedBuffers, std::vector<uint32_t>&>>& int_data,
    BufferIndex start, BufferIndex end)
  : start_(start)
  , end_(end) {

  for (const auto& kv : float_data) {
    switch (kv.first) {
      case SupportedBuffers::COLORS:
        get<SupportedBuffers::COLORS>() = VectorSlice<float>(
            kv.second, start_.vertex_, end_.vertex_, floats_per_color);
        break;
      case SupportedBuffers::TEXTURES:
        get<SupportedBuffers::TEXTURES>() = VectorSlice<float>(
            kv.second, start_.vertex_, end_.vertex_, floats_per_text);
        break;
      case SupportedBuffers::VERTICES:
        get<SupportedBuffers::VERTICES>() = VectorSlice<float>(
            kv.second, start_.vertex_, end_.vertex_, floats_per_vert);
        break;
      case SupportedBuffers::INDICES:
        assert(false);
        break;
    }
  }

  for (const auto& kv : int_data) {
    switch (kv.first) {
      case SupportedBuffers::INDICES:
        get<SupportedBuffers::INDICES>() = VectorSlice<uint32_t>(
            kv.second, start_.triangle_, end_.triangle_, floats_per_triangle);
        break;
      case SupportedBuffers::COLORS:
      case SupportedBuffers::TEXTURES:
      case SupportedBuffers::VERTICES:
        assert(false);
        break;
    }
  }
}

MixedDataMap::MixedDataMap(
    std::set<SupportedBuffers> keys, BufferIndex count)
  : total_(count) {

  for (const auto& key: keys) {
    switch (key) {
      case SupportedBuffers::COLORS:
        get<SupportedBuffers::COLORS>().resize(count.vertex_*floats_per_color);
        break;
      case SupportedBuffers::INDICES:
        get<SupportedBuffers::INDICES>().resize(count.triangle_*floats_per_triangle);
        break;
      case SupportedBuffers::TEXTURES:
        get<SupportedBuffers::TEXTURES>().resize(count.vertex_*floats_per_text);
        break;
      case SupportedBuffers::VERTICES:
        get<SupportedBuffers::VERTICES>().resize(count.vertex_*floats_per_vert);
        break;
    }
  }
}

BufferIndex MixedDataMap::DataRemaining() {
  return total_ - next_free_;
}

MixedSliceMap MixedDataMap::NextSlice(BufferIndex count) {
  auto start = next_free_;
  next_free_ += count;
  return MixedSliceMap(
      FloatData(),
      IntegralData(),
      start,
      next_free_
  );
}

}}} // ShapeShifter::Opengl::Data