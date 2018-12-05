/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   LeafNode.cpp
 * Author: bbyington
 *
 * Created on December 20, 2016, 3:57 PM
 */

#include "rendering/LeafNode.h"
#include "data/AbstractBufferManager.h"

namespace ShapeShifter {
namespace Rendering {

void BaseLeafNode::FillLocalBuffer(Data::MixedDataMap& data) {
  auto size = ExclusiveNodeDataCount();
  // WARNING: This is very brittle, but this statement is required to prevent
  // nullptr exception, since neither the root node nor pure nodes will inherit
  // from any of the interface classes, and the FillData function below will
  // involve dynamic casts to those classes.
  if (size.vertex_ == 0 && size.index_ == 0) return;

  auto local_data = data.NextSlice(size);
  start_= local_data.start();
  end_= local_data.end();

  for(auto& key: local_data.FloatData()) {
    key.first->FillData(key.second, this);
  }

  for(auto& key: local_data.IntegralData()) {
    key.first->FillData(key.second, this);
  }

  auto& indices = local_data.indices();
  FillIndexData(indices);
  size_t offset = start().vertex_;
  for (auto& ind: indices) {
    ind += offset;
  }
}

}} // ShapeShifter::Rendering


