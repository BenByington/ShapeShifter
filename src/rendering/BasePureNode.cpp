/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   RenderNode.cpp
 * Author: bbyington
 *
 * Created on August 2, 2016, 7:53 AM
 */

#include "BasePureNode.h"

#include "rendering/shaders/ShaderProgramBase.h"

#include <iostream>

namespace ShapeShifter {
namespace Rendering {

Data::BufferIndex BasePureNode::SubtreeCounts() const {
  auto ret = Data::BufferIndex{};
  for (const auto& child : subtrees_) {
    ret += child.Node().SubtreeCounts();
  }
  if (leaf_) {
    ret += leaf_->ExclusiveNodeDataCount();
  }
  return ret;
}

void BasePureNode::PopulateBufferData(Data::MixedDataMap& data) {
  for (auto& child : subtrees_) {
    child.Node().PopulateBufferData(data);
  }
  if (leaf_) {
    leaf_->FillLocalBuffer(data);
  }
}

}} // ShapeShifter::Rendering
