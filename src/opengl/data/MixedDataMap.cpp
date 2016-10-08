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
    size_t start_vert,
    size_t end_vert,
    size_t start_tri,
    size_t end_tri)
  : start_vertex_(start_vert)
  , end_vertex_(end_vert)
  , start_tri_(start_tri)
  , end_tri_(end_tri) {

  for (const auto& kv : float_data) {
    switch (kv.first) {
      case SupportedBuffers::COLORS:
        get<SupportedBuffers::COLORS>() = VectorSlice<float>(kv.second, start_vert, end_vert, floats_per_color_);
        break;
      case SupportedBuffers::TEXTURES:
        get<SupportedBuffers::TEXTURES>() = VectorSlice<float>(kv.second, start_vert, end_vert, floats_per_text_);
        break;
      case SupportedBuffers::VERTICES:
        get<SupportedBuffers::VERTICES>() = VectorSlice<float>(kv.second, start_vert, end_vert, floats_per_vert_);
        break;
      case SupportedBuffers::INDICES:
        assert(false);
        break;
    }
  }

  for (const auto& kv : int_data) {
    switch (kv.first) {
      case SupportedBuffers::COLORS:
        assert(false);
        break;
      case SupportedBuffers::TEXTURES:
        assert(false);
        break;
      case SupportedBuffers::VERTICES:
        assert(false);
        break;
      case SupportedBuffers::INDICES:
        get<SupportedBuffers::INDICES>() = VectorSlice<uint32_t>(kv.second, start_tri_, end_tri_, floats_per_tri_);
        break;
    }
  }
}

MixedDataMap::MixedDataMap(std::set<SupportedBuffers> keys, size_t vertex_count, size_t triangle_count) {
  for (const auto& key: keys) {
    switch (key) {
      case SupportedBuffers::COLORS:
        get<SupportedBuffers::COLORS>().resize(vertex_count*floats_per_color_);
        break;
      case SupportedBuffers::INDICES:
        get<SupportedBuffers::INDICES>().resize(triangle_count*floats_per_tri_);
        break;
      case SupportedBuffers::TEXTURES:
        get<SupportedBuffers::TEXTURES>().resize(vertex_count*floats_per_text_);
        break;
      case SupportedBuffers::VERTICES:
        get<SupportedBuffers::VERTICES>().resize(vertex_count*floats_per_vert_);
        break;
    }
  }
  total_vertices_ = vertex_count;
  total_tri_ = triangle_count;
  next_free_vertex_ = 0;
  next_free_tri_ = 0;
}

MixedSliceMap MixedDataMap::NextSlice(size_t vertex_count, size_t triangle_count) {
  // TODO unify vertex and index into single structure?
  auto vertex = next_free_vertex_;
  auto triangle = next_free_tri_;
  next_free_vertex_ += vertex_count;
  next_free_tri_ += triangle_count;
  return MixedSliceMap(
      FloatData(),
      IntegralData(),
      vertex,
      next_free_vertex_,
      triangle,
      next_free_tri_
      );
}

}}} // ShapeShifter::Opengl::Data