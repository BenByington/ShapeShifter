/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include "data/MixedDataMapBase.h"

#include "data/AbstractBufferManager.h"

namespace ShapeShifter {
namespace Data {

MixedDataMap::MixedDataMap(
    const std::vector<std::shared_ptr<Data::AbstractManager>>& managers,
    BufferIndex count)
  : total_(count) {

  for (const auto& manager: managers) {
    if (manager->isFloating()) {
      // TODO write operator== for manager.  This should work, but that will make
      // it explicit and guard against duplicate buffer types.
      // Alternatively, prove there can be no duplicates and move to a vector
      // instead of a map
      float_data_[manager].resize(count.vertex_*manager->ElementsPerEntry());
    } else {
      integral_data_[manager].resize(count.vertex_*manager->ElementsPerEntry());
    }
  }

  indices_.resize(count.index_);
}

BufferIndex MixedDataMap::DataRemaining() {
  return total_ - next_free_;
}

BufferIndex MixedDataMap::Size() {
  return total_;
}

MixedSliceMap MixedDataMap::NextSlice(BufferIndex count) {
  auto start = next_free_;
  next_free_ += count;
  return MixedSliceMap(
      float_data_,
      integral_data_,
      indices_,
      start,
      next_free_
  );
}

}} // ShapeShifter::Data