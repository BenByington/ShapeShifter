/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   BufferData.h
 * Author: bbyington
 *
 * Created on October 7, 2016, 9:12 PM
 */

#ifndef BUFFERDATA_H
#define BUFFERDATA_H

#include <cassert>
#include <set>

#include "opengl/data/VectorSlice.h"

namespace ShapeShifter {
namespace Opengl {
namespace Data {

namespace detail {
// TODO figure out cleaner solution
template <SupportedBuffers b>
struct traits {
  using type = float;
};
template <>
struct traits<SupportedBuffers::INDICES> {
  using type = uint32_t;
};
}

template <template <typename...> class Storage>
class MixedDataMapBase {
public:
  // TODO find better home?
  static const size_t floats_per_vert_ = 3;
  static const size_t floats_per_color_ = 3;
  static const size_t floats_per_text_ = 2;
  // TODO change this to be 3 and rename floats_per_tri_
  static const size_t floats_per_ind_ = 1;

  template <typename T>
  using Storage_t = Storage<T>;

  MixedDataMapBase() = default;
  MixedDataMapBase(const MixedDataMapBase&) = delete;
  MixedDataMapBase(MixedDataMapBase&&) = default;
  MixedDataMapBase& operator=(const MixedDataMapBase&) = delete;
  MixedDataMapBase& operator=(MixedDataMapBase&&) = default;

  template <SupportedBuffers buffer>
  Storage<typename detail::traits<buffer>::type>& get() {
    keys_.insert(buffer);
    return get_dispatch<buffer>();
  }

  const std::set<SupportedBuffers>& keys() {
    return keys_;
  }

  auto FloatData() {
    std::vector<std::pair<SupportedBuffers, Storage<float>&>> ret;
    for (const auto& key : keys_) {
      switch(key) {
        case SupportedBuffers::COLORS:
          ret.emplace_back(SupportedBuffers::COLORS, colors_);
          break;
        case SupportedBuffers::TEXTURES:
          ret.emplace_back(SupportedBuffers::TEXTURES, textures_);
          break;
        case SupportedBuffers::VERTICES:
          ret.emplace_back(SupportedBuffers::VERTICES, vertices_);
          break;
        case SupportedBuffers::INDICES:
          //do nothing
          break;
      }
    }
    return ret;
  }

  auto IntegralData() {
    std::vector<std::pair<SupportedBuffers, Storage<uint32_t>&>> ret;
    for (const auto& key : keys_) {
      switch(key) {
        case SupportedBuffers::COLORS:
          //do nothing
          break;
        case SupportedBuffers::TEXTURES:
          //do nothing
          break;
        case SupportedBuffers::VERTICES:
          //do nothing
          break;
        case SupportedBuffers::INDICES:
          ret.emplace_back(SupportedBuffers::INDICES, indices_);
          //do nothing
          break;
      }
    }
    return ret;
  }
private:

  template <SupportedBuffers buffer>
  typename std::enable_if<
      std::is_same<float, typename detail::traits<buffer>::type>::value,
      Storage<typename detail::traits<buffer>::type>
  >::type& get_dispatch() {
    auto item = buffer;
    switch (item) {
      case SupportedBuffers::COLORS:
        return colors_;
        break;
      case SupportedBuffers::TEXTURES:
        return textures_;
        break;
      case SupportedBuffers::VERTICES:
        return vertices_;
        break;
      case SupportedBuffers::INDICES:
        assert(false);
        return vertices_;
        break;
    }
  }

  template <SupportedBuffers buffer>
  typename std::enable_if<
      std::is_same<uint32_t, typename detail::traits<buffer>::type>::value,
      Storage<typename detail::traits<buffer>::type>
  >::type& get_dispatch() {
    auto item = buffer;
    switch (item) {
      case SupportedBuffers::COLORS:
        assert(false);
      case SupportedBuffers::TEXTURES:
        assert(false);
      case SupportedBuffers::VERTICES:
        assert(false);
      case SupportedBuffers::INDICES:
        return indices_;
        break;
    }
  }

  std::set<SupportedBuffers> keys_;
  Storage<float> colors_;
  Storage<float> textures_;
  Storage<float> vertices_;
  Storage<uint32_t> indices_;
};

class MixedSliceMap final : public MixedDataMapBase<VectorSlice> {
public:
  MixedSliceMap(const MixedSliceMap& other) = delete;
  MixedSliceMap(MixedSliceMap&& other) = default;

  MixedSliceMap& operator=(const MixedSliceMap& other) = delete;
  MixedSliceMap& operator=(MixedSliceMap&& other) = default;

  MixedSliceMap(
      const std::vector<std::pair<SupportedBuffers, std::vector<float>&>>& float_data,
      const std::vector<std::pair<SupportedBuffers, std::vector<uint32_t>&>>& int_data,
      size_t start_vert, size_t end_vert, size_t start_idx, size_t end_idx);

  size_t start_vertex() { return start_vertex_; }
  size_t end_vertex() { return end_vertex_; }

  size_t start_index() { return start_index_; }
  size_t end_index() { return end_index_; }
private:
  size_t start_vertex_;
  size_t start_index_;
  size_t end_vertex_;
  size_t end_index_;
};

class MixedDataMap final : public MixedDataMapBase<std::vector> {
public:
  MixedDataMap(const MixedDataMap&) = delete;
  MixedDataMap(MixedDataMap&&) = default;
  MixedDataMap(
      std::set<SupportedBuffers> keys,
      size_t vertex_count, size_t idx_count);

  MixedDataMap& operator=(const MixedDataMap&) = delete;
  MixedDataMap& operator=(MixedDataMap&&) = default;

  MixedSliceMap NextSlice(size_t vertex_count, size_t index_count);

  size_t VertexDataRemaining() { return total_vertices_ - next_free_vertex_; }
  size_t IndexDataRemaining() { return total_indices_ - next_free_index_; }

private:
  size_t next_free_vertex_ = 0;
  size_t next_free_index_ = 0;
  size_t total_vertices_ = 0;
  size_t total_indices_ = 0;
};

}}} // ShapeShifter::Opengl::Data

#endif /* BUFFERDATA_H */

