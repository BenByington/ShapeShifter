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

#ifndef DATA_MIXED_DATA_MAP_H
#define DATA_MIXED_DATA_MAP_H

#include "data/MixedSliceMap.h"

#include <cassert>
#include <memory>
#include <set>

namespace ShapeShifter {
namespace Data {

class MixedDataMap final : public MixedDataMapBase<std::vector> {
public:
  MixedDataMap(const MixedDataMap&) = delete;
  MixedDataMap(MixedDataMap&&) = default;
  MixedDataMap(
      const std::vector<std::shared_ptr<Data::AbstractManager>>& keys,
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

#endif /* DATA_MIXED_DATA_MAP_H */

