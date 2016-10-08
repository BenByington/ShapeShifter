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
    size_t start_idx,
    size_t end_idx)
  : start_vertex_(start_vert)
  , end_vertex_(end_vert)
  , start_index_(start_idx)
  , end_index_(end_idx) {

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
        get<SupportedBuffers::INDICES>() = VectorSlice<uint32_t>(kv.second, start_idx, end_idx, floats_per_ind_);
        break;
    }
  }
}

MixedDataMap::MixedDataMap(std::set<SupportedBuffers> keys, size_t vertex_count, size_t idx_count) {
  for (const auto& key: keys) {
    switch (key) {
      case SupportedBuffers::COLORS:
        get<SupportedBuffers::COLORS>().resize(vertex_count*floats_per_color_);
        break;
      case SupportedBuffers::INDICES:
        get<SupportedBuffers::INDICES>().resize(idx_count*floats_per_ind_);
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
  total_indices_ = idx_count;
  next_free_vertex_ = 0;
  next_free_index_ = 0;
}

MixedSliceMap MixedDataMap::NextSlice(size_t vertex_count, size_t index_count) {
  // TODO unify vertex and index into single structure?
  auto vertex = next_free_vertex_;
  auto idx = next_free_index_;
  next_free_vertex_ += vertex_count;
  next_free_index_ += index_count;
  return MixedSliceMap(
      FloatData(),
      IntegralData(),
      vertex,
      next_free_vertex_,
      idx,
      next_free_index_
      );
}

}}} // ShapeShifter::Opengl::Data