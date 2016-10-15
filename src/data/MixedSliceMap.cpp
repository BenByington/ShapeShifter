/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include "data/MixedDataMapBase.h"

#include "data/AbstractBufferManager.h"

namespace ShapeShifter {
namespace Data {

MixedSliceMap::MixedSliceMap(
    std::map<std::shared_ptr<AbstractManager>, std::vector<float>>& float_data,
    std::map<std::shared_ptr<AbstractManager>, std::vector<uint32_t>>& int_data,
    BufferIndex start, BufferIndex end)
  : start_(start)
  , end_(end) {

  for (auto& kv : float_data) {
    float_data_[kv.first] = VectorSlice<float>(
        kv.second, start_.vertex_, end_.vertex_, kv.first->ElementsPerEntry());
  }

  for (auto& kv : int_data) {
    integral_data_[kv.first] = VectorSlice<uint32_t>(
        kv.second, start_.triangle_, end_.triangle_, kv.first->ElementsPerEntry());
  }
}

}} // ShapeShifter::Data