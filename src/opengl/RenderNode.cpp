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
namespace Opengl {

void RenderNode::SetRotation(const math::Quaternion& rot) {
  this->rotation_ = rot;
}

void RenderNode::SetTranslation(const math::Vector4& trans) {
  this->translation_ = trans;
}

size_t RenderNode::SubtreeVertexCount() const {
	auto ret = ExclusiveNodeVertexCount();
	for (const auto& child : children) {
		ret += child->SubtreeVertexCount();
	}
	return ret;
}

// TODO combine with above?
size_t RenderNode::SubtreeIndexCount() const {
	auto ret = ExclusiveNodeIndexCount();
	for (const auto& child : children) {
		ret += child->SubtreeIndexCount();
	}
	return ret;
}

void RenderNode::PopulateBufferData(MixedDataMap& data) {
	for (const auto& child : children) {
		child->PopulateBufferData(data);
	}

  auto local_data = data.NextSlice(ExclusiveNodeVertexCount(), ExclusiveNodeIndexCount());
	start_vertex_ = local_data.start_vertex();
	end_vertex_ = local_data.end_vertex();
	start_index_ = local_data.start_index();
	end_index_ = local_data.end_index();

  // TODO automate this mapping somehow...
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
        FillIndexData(local_data.get<SupportedBuffers::INDICES>());
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

  // Note, this function essentially recurses the tree twice, once to figure
	// out how big the tree is, and then again to actually populate the VAO.
	// Could potentially recurse once, filling pre-allocated buffers and adding
	// more as necessary?
  // TODO: I don't think this will save much time anymore, but regardless,
  //       with the new tree construction constraints, the first recursion
  //       could be done then since things are essentially forced to be
  //       created bottom up.
	CleanupBuffer();
	auto vert_size = this->SubtreeVertexCount();
  auto idx_size = this->SubtreeIndexCount();

  std::set<SupportedBuffers> keys;
  for (const auto& kv : idx_map) {
    keys.insert(kv.first);
  }
  MixedDataMap data(keys, vert_size, idx_size);

	PopulateBufferData(data);
	assert(data.VertexDataRemaining() == 0);

  glGenVertexArrays (1, &vao);
  glBindVertexArray (vao);

  for (const auto& kv: data.FloatData()) {
    auto vbo = GLuint{0};
    const auto& buffer_dat = kv.second;
    glGenBuffers (1, &vbo);
    glBindBuffer (GL_ARRAY_BUFFER, vbo);
    glBufferData (GL_ARRAY_BUFFER, buffer_dat.size() * sizeof (float), buffer_dat.data(), GL_STATIC_DRAW);
    // TODO 3 hardcode is wrong for textures?
    glVertexAttribPointer (idx_map.at(kv.first), 3, GL_FLOAT, GL_FALSE, 0, NULL);
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
	//TODO check to make sure this is correct
	glDeleteVertexArrays(1, &vao);
  if (ibo != 0) glDeleteBuffers(1, &ibo);
	vao = 0;
  ibo = 0;
}

void RootNode::RenderTree(const Camera& camera) const {
  glBindVertexArray(vao);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
  program_->UseProgram();
  this->DrawChildren(camera, math::Quaternion(), math::Vector4({0, 0, 0, 1}), *program_);
}

void RenderNode::DrawChildren(
    const Camera& camera,
    const math::Quaternion& cumRot,
    const math::Vector4& cumTrans,
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

void RenderNode::DebugRotation(const math::Matrix4& mat) const {
  auto dataset = MixedDataMap({SupportedBuffers::VERTICES}, ExclusiveNodeVertexCount(), 0);
  auto slices = dataset.NextSlice(ExclusiveNodeVertexCount(), 0);
  auto& slice = slices.get<SupportedBuffers::VERTICES>();
  FillVertexData(slice);
  std::cerr << "Matrix: " << std::endl;
  mat.print();
  auto& data = dataset.get<SupportedBuffers::VERTICES>();
  for (size_t i = 0; i < data.size(); i += 3) {
    // TODO figure out how to make Vector4 constructor less verbose...
    auto vec = math::Vector4(std::array<float,4>{{data[i], data[i+1], data[i+2], 1}});
    std::cerr << "Before: " << std::endl;
    vec.print();
    auto result = mat * vec;
    std::cerr << "After: " << std::endl;
    result.print();
  }
}

// TODO clean up this so it's less manual
template class TypedRenderNode<SupportedBufferFlags::COLORS>;

}} // ShapeShifter::Opengl
