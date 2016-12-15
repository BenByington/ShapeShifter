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

#include "RenderNode.h"

#include "rendering/shaders/ShaderProgramBase.h"

#include <iostream>

namespace ShapeShifter {
namespace Rendering {

Data::BufferIndex BasePureNode::SubtreeCounts() const {
	auto ret = Data::BufferIndex{};
	for (const auto& child : subtrees_) {
    // TODO rename child all over
		ret += child.second->SubtreeCounts();
	}
  for (const auto& child : leaves_) {
    ret += child.second->ExclusiveNodeDataCount();
  }
	return ret;
}

void BasePureNode::PopulateBufferData(Data::MixedDataMap& data) {
	for (const auto& child : subtrees_) {
		child.second->PopulateBufferData(data);
	}
	for (const auto& child : leaves_) {
		child.second->FillLocalBuffer(data);
	}
}

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

void BasePureNode::DrawChildren(
    const Camera& camera,
    const Manipulator& uniforms,
    const Shaders::ShaderProgramBase& shader) const {

	for (const auto& child : subtrees_) {
    auto child_uniforms = child.first->Combine(uniforms);
    //auto child_uniforms = uniforms.Combine(*child.first);
		child.second->DrawChildren(camera, child_uniforms, shader);
	}
	for (const auto& child : leaves_) {
    auto child_uniforms = child.first->Combine(uniforms);
    //auto child_uniforms = uniforms.Combine(*child.first);
    child_uniforms.Upload(camera, shader);
    child.second->DrawSelf();
	}
}

void Manipulator::Upload(const Camera& camera, const Shaders::ShaderProgramBase& program) const {
  auto mat = rotation_.RotationMatrix();
  mat.WriteColumn(3, translation_);
  program.uploadMatrix(camera.ProjectionMatrix() * mat);
}

}} // ShapeShifter::Rendering
