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

#ifndef DATA_VECTORSLICE_H
#define DATA_VECTORSLICE_H

#include <cassert>
#include <cstddef>
#include <vector>

namespace ShapeShifter::Data {

template <typename T>
class SliceFiller;

/*
 * Simple class that behaves similarly to std::vector, but does not own it's
 * own data, rather provides a 'view' into an existing std::vector.
 * Very little error checking, debug builds should assert out if you do something
 * funny, but release builds will happily let you overrun array bounds if
 * you're not careful.
 */
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

  // Returns a helper object that lets you fill data without manually keeping
  // track of current index.
  SliceFiller<T> Filler();
private:
  T* data_;
  size_t size_;
};

template <typename T> VectorSlice(std::vector<T>, size_t, size_t, size_t) -> VectorSlice<T>;

/*
 * Convenience class for populating a VectorSlice.
 * It automatically keeps track of the next available index, so you can
 * just call operator() instead of operator[].  It also has overloads allowing
 * you to enter 1, 2 or 3 elements at a time, depending on what is convenient
 * for the application.
 */
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

} // ShapeShifter::Data

#endif /* DATA_VECTORSLICE_H */

