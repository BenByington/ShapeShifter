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
	size_t ret = ExclusiveNodeVertexCount();
	for (const auto& child : children) {
		ret += child->SubtreeVertexCount();
	}
	return ret;
}

size_t RenderNode::PopulateBufferData(
    std::map<SupportedBuffers, std::vector<float>>& data,
		const size_t start) {
	size_t idx = start;
	for (const auto& child : children) {
		idx += child->PopulateBufferData(data, idx);
	}

	start_vertex_ = idx;

  for(auto& kv: data) {
    switch (kv.first) {
      case SupportedBuffers::COLORS:
        assert(Flags & SupportedBufferFlags::COLORS || Flags == 0);
        this->FillColorData(kv.second, idx);
        break;
      case SupportedBuffers::VERTICES:
        this->FillVertexData(kv.second, idx);
        break;
      case SupportedBuffers::INDICES:
        assert(Flags & SupportedBufferFlags::INDICES || Flags == 0);
        assert(false);
        break;
      case SupportedBuffers::TEXTURES:
        assert(Flags & SupportedBufferFlags::TEXTURES || Flags == 0);
        this->FillTextureData(kv.second, idx);
        break;
    }
  }

	idx += this->ExclusiveNodeVertexCount();

	end_vertex_ = idx;
	return end_vertex_ - start;
}

void RootNode::UpdateData(const std::map<SupportedBuffers, size_t>& idx_map) {

  // Note, this function essentially recurses the tree twice, once to figure
	// out how big the tree is, and then again to actually populate the VAO.
	// Could potentially recurse once, filling pre-allocated buffers and adding
	// more as necessary?
	CleanupBuffer();
	size_t size = this->SubtreeVertexCount();

  std::map<SupportedBuffers, std::vector<float>> data;
  for (const auto& kv: idx_map) {
    switch (kv.first) {
      case SupportedBuffers::COLORS:
        data[kv.first].resize(size*floats_per_color);
        break;
      case SupportedBuffers::INDICES:
        data[kv.first].resize(size*floats_per_ind);
        break;
      case SupportedBuffers::TEXTURES:
        data[kv.first].resize(size*floats_per_text);
        break;
      case SupportedBuffers::VERTICES:
        data[kv.first].resize(size*floats_per_vert_);
        break;
    }
  }

	size_t end = this->PopulateBufferData(data, 0);
	assert(end == size);

  glGenVertexArrays (1, &vao);
  glBindVertexArray (vao);

  for (const auto& kv: data) {
    GLuint vbo = 0;
    const std::vector<float>& buffer_dat = kv.second;
    glGenBuffers (1, &vbo);
    glBindBuffer (GL_ARRAY_BUFFER, vbo);
    glBufferData (GL_ARRAY_BUFFER, buffer_dat.size() * sizeof (float), buffer_dat.data(), GL_STATIC_DRAW);
    glVertexAttribPointer (idx_map.at(kv.first), 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(idx_map.at(kv.first));
  }
}

void RootNode::CleanupBuffer() {
	//TODO check to make sure this is correct
	glDeleteVertexArrays(1, &vao);
	vao = 0;
}

void RootNode::RenderTree(const Camera& camera, const Shaders::ShaderProgram& shader) const {
  glBindVertexArray(vao);
  shader.UseProgram();
  this->DrawChildren(camera, math::Quaternion(), math::Vector4({0, 0, 0, 1}), shader);
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
  std::vector<float> data(this->ExclusiveNodeVertexCount()*floats_per_vert_);
  this->FillVertexData(data, 0);
  std::cerr << "Matrix: " << std::endl;
  mat.print();
  for (size_t i = 0; i < data.size(); i += 3) {
    // TODO figure out how to make Vector4 constructor less verbose...
    math::Vector4 vec(std::array<float,4>{{data[i], data[i+1], data[i+2], 1}});
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
