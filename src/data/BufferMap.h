/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   BufferMap.h
 * Author: bbyington
 *
 * Created on October 7, 2016, 9:12 PM
 */

#ifndef DATA_BUFFER_MAP_H
#define DATA_BUFFER_MAP_H

#include "data/BufferSliceMap.h"

#include <cassert>
#include <memory>
#include <set>
#include <vector>

namespace ShapeShifter::Data {

/*
 * Convenience class, allowing the buffers to all be stored together even though
 * they will not all have the same storage type.  This class will do bookkeeping
 * allowing it to dole out new slices of data to be populated, keeping all the
 * buffers in sync while it does so.
 */
template <BufferManager... Managers>
class BufferMap final : public BufferMapBase<std::vector, Managers...> {
public:
  // Construction requires a full set of BufferManagers already created as well
  // as a BufferIndex variable dictating how large to make the buffers
  BufferMap(BufferIndex count)
      : total_(count) {
    // ISSUE Rethink these shared_ptr being used as keys.  They live in both this data
    //       structure and another, so their lifecycle is confusing.
    auto worker = {
        (this->data_
             .template Key<std::shared_ptr<Managers>, std::vector<typename Managers::Type>>() =
             std::make_shared<Managers>(),
         1)...};
    (void)worker;
    this->ForEachKeyVal(
        [&](auto& key, auto& vec) { vec.resize(count.vertex_ * key->ElementsPerEntry()); });

    this->indices_.resize(count.index_);
  }

  BufferMap(const BufferMap&) = delete;
  BufferMap(BufferMap&&) = default;

  BufferMap& operator=(const BufferMap&) = delete;
  BufferMap& operator=(BufferMap&&) = default;

  /*
   * Asks for the next 'count' elements as a MixedSliceMap, so they can
   * be populated with data.
   * WARNING: If you ask for more data when none is available you will overrun
   * array bounds.
   */
  BufferSliceMap<Managers...> NextSlice(BufferIndex count) {
    auto start = next_free_;
    next_free_ += count;
    return BufferSliceMap<Managers...>(*this, this->indices_, start, next_free_);
  }

  // Amount of data not yet returned in a MixedSliceMap for population
  BufferIndex DataRemaining() { return total_ - next_free_; }

  // Size of the map, both in vertices and triangles.
  BufferIndex Size() { return total_; }

private:
  BufferIndex next_free_;
  BufferIndex total_;
};

} // namespace ShapeShifter::Data

#endif /* DATA_BUFFER_MAP_H */
