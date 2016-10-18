/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   MixedDataMap.h
 * Author: bbyington
 *
 * Created on October 7, 2016, 9:12 PM
 */

#ifndef DATA_MIXED_DATA_MAP_H
#define DATA_MIXED_DATA_MAP_H

#include "data/MixedSliceMap.h"

#include <cassert>
#include <memory>
#include <set>

namespace ShapeShifter {
namespace Data {

/*
 * Convenience class, allowing the buffers to all be stored together even though
 * they will not all have the same storage type.  This class will do bookkeeping
 * allowing it to dole out new slices of data to be populated, keeping all the
 * buffers in sync while it does so.
 */
class MixedDataMap final : public MixedDataMapBase<std::vector> {
public:
  // Construction requires a full set of BufferManagers already created as well
  // as a BufferIndex variable dictating how large to make the buffers
  MixedDataMap(
      const std::vector<std::shared_ptr<Data::AbstractManager>>& keys,
      BufferIndex count);

  MixedDataMap(const MixedDataMap&) = delete;
  MixedDataMap(MixedDataMap&&) = default;

  MixedDataMap& operator=(const MixedDataMap&) = delete;
  MixedDataMap& operator=(MixedDataMap&&) = default;

  /*
   * Asks for the next 'count' elements as a MixedSliceMap, so they can
   * be populated with data.
   * WARNING: If you ask for more data when none is available you will overrun
   * array bounds.
   */
  MixedSliceMap NextSlice(BufferIndex count);

  // Amount of data not yet returned in a MixedSliceMap for population
  BufferIndex DataRemaining();

  // Size of the map, both in vertices and triangles.
  BufferIndex Size();
private:
  BufferIndex next_free_;
  BufferIndex total_;
};

}} // ShapeShifter::Data

#endif /* DATA_MIXED_DATA_MAP_H */

