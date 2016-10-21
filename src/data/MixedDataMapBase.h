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

/*
 * Generic base class for both MixedDataMap and MixedSliceMap, giving them the
 * same interface while allowing different storage types.
 */
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

  /*
   * Returns all the buffers using floating point data.  Repacks things to return
   * references to the actual data to avoid coppies, so beware dangling references!
   */
  auto FloatData() {
    std::vector<std::pair<std::shared_ptr<AbstractManager>, Storage<float>&>> ret;
    for (auto& kv : float_data_) {
      ret.emplace_back(kv.first, kv.second);
    }
    return ret;
  }

  /*
   * Returns all the buffers using integral data.  Repacks things to return
   * references to the actual data to avoid coppies, so beware dangling references!
   */
  auto IntegralData() {
    std::vector<std::pair<std::shared_ptr<AbstractManager>, Storage<uint32_t>&>> ret;
    for (auto& kv : integral_data_) {
      ret.emplace_back(kv.first, kv.second);
    }
    return ret;
  }

  Storage<uint32_t>& indices() {
    return indices_;
  }
protected:

  std::map<std::shared_ptr<AbstractManager>, Storage<float>> float_data_;
  std::map<std::shared_ptr<AbstractManager>, Storage<uint32_t>> integral_data_;
  Storage<uint32_t> indices_;
};

/*
 * Wraps together vertex_ and triangle_ counts to help simplify code, as they
 * are almost always used in conjunction with each other.
 */
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

