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

#include <cassert>
#include <iostream>
#include <numeric>

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

  // ISSUE: Refactor buffer type meta programming.  Moving away from bit
  //        flags (or at least the use of '|' to combine them) would be nice,
  //        as well as the ability to avoid these switch statements scattered
  //        all over.
  for(auto& key: data.keys()) {
    switch (key) {
      case SupportedBuffers::COLORS:
      {
        FillColorData(local_data.get<SupportedBuffers::COLORS>());
        break;
      }
      case SupportedBuffers::VERTICES:
      {
        FillVertexData(local_data.get<SupportedBuffers::VERTICES>());
        break;
      }
      case SupportedBuffers::INDICES:
      {
        auto&& slice = local_data.get<SupportedBuffers::INDICES>();
        FillIndexData(slice);
        size_t offset = start().vertex_;
        for (auto& ind: slice) {
          ind += offset;
        }
        break;
      }
      case SupportedBuffers::TEXTURES:
      {
        FillTextureData(local_data.get<SupportedBuffers::TEXTURES>());
        break;
      }
    }
  }
}

void RootNode::UpdateData() {

	CleanupBuffer();
  auto size = SubtreeCounts();

  std::set<SupportedBuffers> keys;
  for (const auto& kv : idx_map) {
    keys.insert(kv.first);
  }
  Data::MixedDataMap data(keys, size);

	PopulateBufferData(data);
	assert(data.DataRemaining().vertex_ == 0);
	assert(data.DataRemaining().triangle_ == 0);

  glGenVertexArrays (1, &vao);
  glBindVertexArray (vao);

  for (const auto& kv: data.FloatData()) {
    auto vbo = GLuint{0};
    const auto& buffer_dat = kv.second;
    glGenBuffers (1, &vbo);
    glBindBuffer (GL_ARRAY_BUFFER, vbo);
    glBufferData (GL_ARRAY_BUFFER, buffer_dat.size() * sizeof (float), buffer_dat.data(), GL_STATIC_DRAW);
    glVertexAttribPointer (idx_map.at(kv.first),  buffer_dat.size()/data.Size().vertex_, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(idx_map.at(kv.first));
  }

  for (const auto& kv : data.IntegralData()) {
    assert(kv.first == SupportedBuffers::INDICES);
    ibo = GLuint{0};
    const auto& buffer_dat = kv.second;
    glGenBuffers (1, &ibo);
    glBindBuffer (GL_ELEMENT_ARRAY_BUFFER, ibo);
    glBufferData (GL_ELEMENT_ARRAY_BUFFER, buffer_dat.size() * sizeof (uint32_t), buffer_dat.data(), GL_STATIC_DRAW);
  }
}

void RootNode::CleanupBuffer() {
	glDeleteVertexArrays(1, &vao);
  if (ibo != 0) glDeleteBuffers(1, &ibo);
	vao = 0;
  ibo = 0;
}

void RootNode::RenderTree(const Camera& camera) const {
  glBindVertexArray(vao);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
  program_->UseProgram();
  this->DrawChildren(camera, Math::Quaternion(), Math::Vector4(0, 0, 0, 1), *program_);
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
  auto dataset = Data::MixedDataMap({SupportedBuffers::VERTICES}, ExclusiveNodeDataCount());
  auto slices = dataset.NextSlice(ExclusiveNodeDataCount());
  auto& slice = slices.get<SupportedBuffers::VERTICES>();
  FillVertexData(slice);
  std::cerr << "Matrix: " << std::endl;
  mat.print();
  auto& data = dataset.get<SupportedBuffers::VERTICES>();
  for (size_t i = 0; i < data.size(); i += 3) {
    auto vec = Math::Vector4(data[i], data[i+1], data[i+2], 1);
    std::cerr << "Before: " << std::endl;
    vec.print();
    auto result = mat * vec;
    std::cerr << "After: " << std::endl;
    result.print();
  }
}

// ISSUE: Set up type list to automatically instantiate explicit templates
template class TypedRenderNode<SupportedBufferFlags::COLORS>;

}} // ShapeShifter::Rendering
