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

#ifndef DATA_MIXED_SLICE_MAP_H
#define DATA_MIXED_SLICE_MAP_H

#include "data/VectorSlice.h"
#include "data/MixedDataMapBase.h"

#include <cassert>
#include <memory>
#include <set>

namespace ShapeShifter {
namespace Data {

/*
 * Class that combines the view functionality of a VectorSlice with the
 * behaviour and interface of the MixedDataMap class.  Used when populating
 * data from individual RenderNodes into the buffers.
 */
class MixedSliceMap final : public MixedDataMapBase<VectorSlice> {
public:
  MixedSliceMap(const MixedSliceMap& other) = delete;
  MixedSliceMap(MixedSliceMap&& other) = default;

  MixedSliceMap& operator=(const MixedSliceMap& other) = delete;
  MixedSliceMap& operator=(MixedSliceMap&& other) = default;

  MixedSliceMap(
      std::map<std::shared_ptr<AbstractManager>, std::vector<float>>& float_data,
      std::map<std::shared_ptr<AbstractManager>, std::vector<uint32_t>>& int_data,
      BufferIndex start, BufferIndex end);

  BufferIndex start() { return start_; }
  BufferIndex end() { return end_; }

private:
  BufferIndex start_;
  BufferIndex end_;
};

}} // ShapeShifter::Data

#endif /* DATA_MIXED_SLICE_MAP_H */

