/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   BufferSliceMap.h
 * Author: bbyington
 *
 * Created on October 7, 2016, 9:12 PM
 */

#ifndef DATA_BUFFER_SLICE_MAP_H
#define DATA_BUFFER_SLICE_MAP_H

#include "data/VectorSlice.h"
#include "data/BufferMapBase.h"

#include <cassert>
#include <memory>
#include <set>

namespace ShapeShifter::Data {

/*
 * Class that combines the view functionality of a VectorSlice with the
 * behaviour and interface of the MixedDataMap class.  Used when populating
 * data from individual RenderNodes into the buffers.
 */
template <BufferManager... Managers>
class BufferSliceMap final : public BufferMapBase<VectorSlice, Managers...> {
public:
  BufferSliceMap(const BufferSliceMap& other) = delete;
  BufferSliceMap(BufferSliceMap&& other) = default;

  BufferSliceMap& operator=(const BufferSliceMap& other) = delete;
  BufferSliceMap& operator=(BufferSliceMap&& other) = default;

  BufferSliceMap(
      BufferMapBase<std::vector, Managers...>& data,
      std::vector<uint32_t>& indices,
      BufferIndex start, BufferIndex end)
    : start_(start)
    , end_(end) {

    auto worker = {(
        this->template Val<Managers>() =
            VectorSlice(data.template Val<Managers>(),
                        start_.vertex_,
                        end_.vertex_,
                        data.template Key<Managers>()->ElementsPerEntry())
    ,1)...};
    (void) worker;


    this->indices_ = VectorSlice<uint32_t>(
        indices, start_.index_, end_.index_, 1);
  }


  BufferIndex start() { return start_; }
  BufferIndex end() { return end_; }

private:
  BufferIndex start_;
  BufferIndex end_;
};

} // ShapeShifter::Data

#endif /* DATA_BUFFER_SLICE_MAP_H */

