/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   BufferTypes.h
 * Author: bbyington
 *
 * Created on September 14, 2016, 4:00 PM
 */

#ifndef DATA_ABSTRACT_BUFFERTYPES_H
#define DATA_ABSTRACT_BUFFERTYPES_H

#include "data/MixedDataMapBase.h"
#include "rendering/RenderNode.h"

#include <cstdlib>
#include <type_traits>

namespace ShapeShifter {
namespace Data {

class AbstractManager {
public:
  //TODO rule of 5, this class and children
  AbstractManager(size_t idx) : idx_(idx) {}
  virtual ~AbstractManager(){}

  virtual void FillData(VectorSlice<float>& data, Rendering::RenderNode* node) = 0;
  virtual void FillData(VectorSlice<uint32_t>& data, Rendering::RenderNode* node) = 0;

  virtual size_t ElementsPerEntry() = 0;
  virtual bool isFloating() = 0;

  size_t idx() { return idx_; }
private:
  size_t idx_;
};

}} // ShapeShifter::Data

#endif /* DATA_ABSTRACT_BUFFERTYPES_H */

