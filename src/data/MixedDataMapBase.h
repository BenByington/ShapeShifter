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

#ifndef DATA_MIXED_DATA_MAP_BASE_H
#define DATA_MIXED_DATA_MAP_BASE_H

#include "data/VectorSlice.h"

#include <cassert>
#include <map>
#include <memory>
#include <set>

namespace ShapeShifter {
namespace Data {

class AbstractManager;

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

  auto FloatData() {
    std::vector<std::pair<std::shared_ptr<AbstractManager>, Storage<float>&>> ret;
    for (auto& kv : float_data_) {
      ret.emplace_back(kv.first, kv.second);
    }
    return ret;
  }

  auto IntegralData() {
    std::vector<std::pair<std::shared_ptr<AbstractManager>, Storage<uint32_t>&>> ret;
    for (auto& kv : integral_data_) {
      ret.emplace_back(kv.first, kv.second);
    }
    return ret;
  }
protected:

  std::map<std::shared_ptr<AbstractManager>, Storage<float>> float_data_;
  std::map<std::shared_ptr<AbstractManager>, Storage<uint32_t>> integral_data_;
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

}} // ShapeShifter::Data

#endif /* DATA_MIXED_DATA_MAP_BASE_H */

