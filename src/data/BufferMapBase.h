/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   BufferMapBase.h
 * Author: bbyington
 *
 * Created on October 7, 2016, 9:12 PM
 */

#ifndef DATA_BUFFER_MAP_BASE_H
#define DATA_BUFFER_MAP_BASE_H

#include "data/AbstractBufferManager.h"
#include "data/MultiTypeMap.h"
#include "data/VectorSlice.h"
#include "rendering/shaders/Pack.h"

#include <cassert>
#include <map>
#include <memory>
#include <set>

namespace ShapeShifter::Data {

/*
 * Generic base class for both BufferMap and BufferSliceMap, giving them the
 * same interface while allowing different storage types.  Both are used to contain
 * and iterate over the different data buffers used for rendering, despite having
 * different Manager classes (which are used as the keys)
 */
template <template <typename...> class Storage, BufferManager... Managers>
class BufferMapBase {
public:
  template <typename T>
  using Storage_t = Storage<T>;

  BufferMapBase() = default;
  BufferMapBase(const BufferMapBase&) = delete;
  BufferMapBase(BufferMapBase&&) = default;
  BufferMapBase& operator=(const BufferMapBase&) = delete;
  BufferMapBase& operator=(BufferMapBase&&) = default;

  Storage<uint32_t>& indices() {
    return indices_;
  }

  template <BufferManager Manager>
  auto& Key() {
    return data_.template Key<std::shared_ptr<Manager>, Storage<typename Manager::Type>>();
  }
  template <BufferManager Manager>
  const auto& Key() const {
    return data_.template Key<std::shared_ptr<Manager>, Storage<typename Manager::Type>>();
  }
  template <BufferManager Manager>
  auto& Val() {
    return data_.template Val<std::shared_ptr<Manager>, Storage<typename Manager::Type>>();
  }
  template <BufferManager Manager>
  const auto& Val() const {
    return data_.template Val<std::shared_ptr<Manager>, Storage<typename Manager::Type>>();
  }

  template <typename Functor>
  void ForEachVal(Functor&& func) {
    data_.ForEach(std::forward<Functor>(func));
  }
  template <typename Functor>
  void ForEachKeyVal(Functor&& func) {
    data_.ForEachKeyVal(std::forward<Functor>(func));
  }
protected:

  using MapData = MultiTypeMap<Pack<std::shared_ptr<Managers>...>,
                               Pack<Storage<typename Managers::Type>...>>;
  MapData data_;
  Storage<uint32_t> indices_;
};

/*
 * Wraps together vertex_ and triangle_ counts to help simplify code, as they
 * are almost always used in conjunction with each other.
 */
struct BufferIndex {
  BufferIndex() : vertex_(0), index_(0) {}
  BufferIndex(const BufferIndex& o)
  : vertex_(o.vertex_), index_(o.index_) {}

  size_t vertex_ = 0;
  size_t index_ = 0;

  BufferIndex& operator+=(const BufferIndex& o) {
    vertex_ += o.vertex_;
    index_ += o.index_;
    return *this;
  }

  BufferIndex operator-(const BufferIndex& o) {
    BufferIndex ret;
    ret.vertex_ = vertex_ - o.vertex_;
    ret.index_ = index_ - o.index_;
    return ret;
  }

  BufferIndex operator+(const BufferIndex& o) {
    BufferIndex ret;
    ret.vertex_ = vertex_ + o.vertex_;
    ret.index_ = index_ + o.index_;
    return ret;
  }
};

} // ShapeShifter::Data

#endif /* DATA_BUFFER_MAP_BASE_H */

