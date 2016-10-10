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

#ifndef DATA_BUFFERDATA_H
#define DATA_BUFFERDATA_H

#include <cassert>
#include <set>

#include "data/VectorSlice.h"

namespace ShapeShifter {
namespace Data {

namespace detail {
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
        case SupportedBuffers::INDICES:
          ret.emplace_back(SupportedBuffers::INDICES, indices_);
          break;
        case SupportedBuffers::COLORS:
        case SupportedBuffers::TEXTURES:
        case SupportedBuffers::VERTICES:
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

struct BufferIndex {
  BufferIndex() : vertex_(0), triangle_(0) {}
  BufferIndex(const BufferIndex& o)
  : vertex_(o.vertex_), triangle_(o.triangle_) {}

  size_t vertex_ = 0;
  size_t triangle_ = 0;

  BufferIndex& operator+=(const BufferIndex& o) {
    vertex_ += o.vertex_;
    triangle_ += o.triangle_;
    return *this;
  }
  BufferIndex operator-(const BufferIndex& o) {
    BufferIndex ret;
    ret.vertex_ = vertex_ - o.vertex_;
    ret.triangle_ = triangle_ - o.triangle_;
    return ret;
  }
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
      BufferIndex start, BufferIndex end);

  BufferIndex start() { return start_; }
  BufferIndex end() { return end_; }

private:
  BufferIndex start_;
  BufferIndex end_;
};

class MixedDataMap final : public MixedDataMapBase<std::vector> {
public:
  MixedDataMap(const MixedDataMap&) = delete;
  MixedDataMap(MixedDataMap&&) = default;
  MixedDataMap(
      std::set<SupportedBuffers> keys,
      BufferIndex count);

  MixedDataMap& operator=(const MixedDataMap&) = delete;
  MixedDataMap& operator=(MixedDataMap&&) = default;

  MixedSliceMap NextSlice(BufferIndex count);

  BufferIndex DataRemaining();
  BufferIndex Size();
private:
  BufferIndex next_free_;
  BufferIndex total_;
};

}} // ShapeShifter::Data

#endif /* DATA_BUFFERDATA_H */

