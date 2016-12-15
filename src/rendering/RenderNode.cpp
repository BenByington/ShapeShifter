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

void BaseNode::SetRotation(const Math::Quaternion& rot) {
  rotation_ = rot;
}

void BaseNode::SetTranslation(const Math::Vector4& trans) {
  translation_ = trans;
}

Data::BufferIndex BasePureNode::SubtreeCounts() const {
	auto ret = Data::BufferIndex{};
	for (const auto& child : subtrees_) {
		ret += child->SubtreeCounts();
	}
  for (const auto& child : leaves_) {
    ret += child->ExclusiveNodeDataCount();
  }
	return ret;
}

void BasePureNode::PopulateBufferData(Data::MixedDataMap& data) {
	for (const auto& child : subtrees_) {
		child->PopulateBufferData(data);
	}
	for (const auto& child : leaves_) {
		child->FillLocalBuffer(data);
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
    const Math::Quaternion& cumRot,
    const Math::Vector4& cumTrans,
    const Shaders::ShaderProgramBase& shader) const {

  auto localQuat = cumRot*rotation_;
  auto rot = localQuat.RotationMatrix();
  auto localTrans = rot * translation_ + cumTrans;
	for (const auto& child : subtrees_) {
		child->DrawChildren(camera, localQuat, localTrans, shader);
	}
	for (const auto& child : leaves_) {
		child->DrawLeaf(camera, localQuat, localTrans, shader);
	}
}

// TODO I don't like this duplication.  Unify with PureNode version somehow.
void BaseLeafNode::DrawLeaf(
    const Camera& camera,
    const Math::Quaternion& cumRot,
    const Math::Vector4& cumTrans,
    const Shaders::ShaderProgramBase& shader) const {
  auto localQuat = cumRot*rotation_;
  auto rot = localQuat.RotationMatrix();
  auto localTrans = rot * translation_ + cumTrans;
  rot.WriteColumn(3, localTrans);
  shader.uploadMatrix(camera.ProjectionMatrix()*rot);

	this->DrawSelf();
}

}} // ShapeShifter::Rendering
