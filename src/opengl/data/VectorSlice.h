/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   VectorSlice.h
 * Author: bbyington
 *
 * Created on October 7, 2016, 9:17 PM
 */

#ifndef VECTORSLICE_H
#define VECTORSLICE_H

#include <cassert>
#include <vector>

#include "opengl/data/BufferTypes.h"

namespace ShapeShifter {
namespace Opengl {
namespace Data {

//Forward.  TODO, clean this up when we split the file.
template <typename T>
class SliceFiller;

// Helper class to help make sure nodes don't overwrite each other's data
template <typename T>
class VectorSlice final {
public:
  VectorSlice(const VectorSlice& other) = default;
  VectorSlice(VectorSlice<T>&& other) = default;
  VectorSlice() : data_(nullptr), size_(0) {}
  VectorSlice(std::vector<T>& v, size_t start, size_t end, size_t elem_size)
    : data_(v.data() + start*elem_size)
    , size_(elem_size * (end-start)) {
    assert(v.size() > 0 || (start == 0 && end == 0));
    assert(end >= start);
    assert(start*elem_size <= v.size());
    assert(end*elem_size <= v.size());
    assert(elem_size > 0);
  }

  VectorSlice& operator=(const VectorSlice& other) = default;
  VectorSlice& operator=(VectorSlice<T>&& other) = default;

  T& operator[](size_t idx) {
    assert(idx < size_);
    return data_[idx];
  }
  T* begin() { return  data_; }
  T* end() {return data_ + size_; }
  size_t size() const { return size_; }

  SliceFiller<T> Filler();
private:
  T* data_;
  size_t size_;
};

template <typename T>
class SliceFiller final {
public:
  explicit SliceFiller(const VectorSlice<T>& slice) : slice_(slice), curr_idx(0) {}

  void operator()(T val) {
    slice_[curr_idx] = val;
    curr_idx += 1;
  }
  void operator()(T val1, T val2) {
    slice_[curr_idx] = val1;
    slice_[curr_idx+1] = val2;
    curr_idx += 2;
  }
  void operator()(T val1, T val2, T val3) {
    slice_[curr_idx] = val1;
    slice_[curr_idx+1] = val2;
    slice_[curr_idx+2] = val3;
    curr_idx += 3;
  }
private:
  VectorSlice<T> slice_;
  size_t curr_idx;
};

template <typename T>
SliceFiller<T> VectorSlice<T>::Filler() {
  return SliceFiller<T>(*this);
}

}}} // ShapeShifter::Opengl::Data

#endif /* VECTORSLICE_H */

