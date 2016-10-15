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

#include "rendering/shaders/ShaderProgram.h"

#include <iostream>

namespace ShapeShifter {
namespace Rendering {

using Data::SupportedBuffers;
using Data::SupportedBufferFlags;

void RenderNode::SetRotation(const Math::Quaternion& rot) {
  this->rotation_ = rot;
}

void RenderNode::SetTranslation(const Math::Vector4& trans) {
  this->translation_ = trans;
}

Data::BufferIndex RenderNode::SubtreeCounts() const {
	auto ret = ExclusiveNodeDataCount();
	for (const auto& child : children) {
		ret += child->SubtreeCounts();
	}
	return ret;
}

void RenderNode::PopulateBufferData(Data::MixedDataMap& data) {
	for (const auto& child : children) {
		child->PopulateBufferData(data);
	}

  auto local_data = data.NextSlice(ExclusiveNodeDataCount());
	start_= local_data.start();
	end_= local_data.end();

  for(auto& key: local_data.FloatData()) {
    switch (key.first->buffer()) {
      case SupportedBuffers::COLORS:
      {
        FillColorData(key.second);
        break;
      }
      case SupportedBuffers::VERTICES:
      {
        FillVertexData(key.second);
        break;
      }
      case SupportedBuffers::INDICES:
      {
        assert(false);
        break;
      }
      case SupportedBuffers::TEXTURES:
      {
        FillTextureData(key.second);
        break;
      }
    }
  }

  for(auto& key: local_data.IntegralData()) {
    switch (key.first->buffer()) {
      case SupportedBuffers::INDICES:
      {
        FillIndexData(key.second);
        size_t offset = start().vertex_;
        for (auto& ind: key.second) {
          ind += offset;
        }
        break;
      }
      case SupportedBuffers::COLORS:
      case SupportedBuffers::VERTICES:
      case SupportedBuffers::TEXTURES:
      {
        assert(false);
        break;
      }
    }
  }
}

void RenderNode::DrawChildren(
    const Camera& camera,
    const Math::Quaternion& cumRot,
    const Math::Vector4& cumTrans,
    const Shaders::ShaderProgram& shader) const {

  auto localQuat = cumRot*rotation_;
  auto rot = localQuat.RotationMatrix();
  auto localTrans = rot * translation_ + cumTrans;
	for (const auto& child : children) {
		child->DrawChildren(camera, localQuat, localTrans, shader);
	}
  rot.WriteColumn(3, localTrans);
  shader.uploadMatrix(camera.ProjectionMatrix()*rot);

	this->DrawSelf();
}

void RenderNode::DebugRotation(const Math::Matrix4& mat) const {
  assert(false);
  //auto dataset = Data::MixedDataMap({SupportedBuffers::VERTICES}, ExclusiveNodeDataCount());
  //auto slices = dataset.NextSlice(ExclusiveNodeDataCount());
  //auto& slice = slices.get<SupportedBuffers::VERTICES>();
  //FillVertexData(slice);
  //std::cerr << "Matrix: " << std::endl;
  //mat.print();
  //auto& data = dataset.get<SupportedBuffers::VERTICES>();
  //for (size_t i = 0; i < data.size(); i += 3) {
  //  auto vec = Math::Vector4(data[i], data[i+1], data[i+2], 1);
  //  std::cerr << "Before: " << std::endl;
  //  vec.print();
  //  auto result = mat * vec;
  //  std::cerr << "After: " << std::endl;
  //  result.print();
  //}
}

}} // ShapeShifter::Rendering
