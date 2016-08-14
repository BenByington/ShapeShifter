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

void RenderNode::AddChild(std::shared_ptr<RenderNode> child) {
	children.push_back(child);
}

void RenderNode::SetRotation(const math::Quaternion& rot) {
  this->rotation_ = rot;
}

void RenderNode::SetTranslation(const math::Vector4& trans) {
  this->translation_ = trans;
}

size_t RenderNode::BufferSizeRequired() const {
	size_t ret = ExclusiveBufferSizeRequired();
	for (const auto& child : children) {
		ret += child->BufferSizeRequired();
	}
	return ret;
}

//TODO let things reallocate the buffer

size_t RenderNode::PopulateBufferData(
    std::vector<float>& vert,
		std::vector<float>& color,
		const size_t start) {
	size_t idx = start;
	for (const auto& child : children) {
		idx += child->PopulateBufferData(vert, color, idx);
	}

	start_vertex_ = idx;

	FillVertexData(vert, idx);
	FillColorData(color, idx);
	idx += this->ExclusiveBufferSizeRequired();

	end_vertex_ = idx;
	return end_vertex_;
}

void RenderNode::UpdateData() {

  // Note, this function essentially recurses the tree twice, once to figure
	// out how big the tree is, and then again to actually populate the VAO.
	// Could potentially recurse once, filling pre-allocated buffers and adding
	// more as necessary?
	CleanupBuffer();
	size_t size = this->BufferSizeRequired();

  std::vector<float> tri_vert(size, 0);
	std::vector<float> tri_col(size, 0);

	size_t end = PopulateBufferData(tri_vert, tri_col, 0);
	assert(end == tri_vert.size());

  GLuint points_vbo = 0;
  glGenBuffers (1, &points_vbo);
  glBindBuffer (GL_ARRAY_BUFFER, points_vbo);
  glBufferData (GL_ARRAY_BUFFER, tri_vert.size() * sizeof (float), &tri_vert[0], GL_STATIC_DRAW);

	GLuint colours_vbo = 0;
  glGenBuffers (1, &colours_vbo);
  glBindBuffer (GL_ARRAY_BUFFER, colours_vbo);
  glBufferData (GL_ARRAY_BUFFER, tri_vert.size() * sizeof (float), &tri_col[0], GL_STATIC_DRAW);

  glGenVertexArrays (1, &vao);
  glBindVertexArray (vao);
  glBindBuffer (GL_ARRAY_BUFFER, points_vbo);
  glVertexAttribPointer (0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
  glBindBuffer (GL_ARRAY_BUFFER, colours_vbo);
  glVertexAttribPointer (1, 3, GL_FLOAT, GL_FALSE, 0, NULL);

	glEnableVertexAttribArray (0);
  glEnableVertexAttribArray (1);
}

void RenderNode::CleanupBuffer() {
	//TODO check to make sure this is correct
	glDeleteVertexArrays(1, &vao);
	vao = 0;
}

void RenderNode::RenderTree(const Camera& camera, const ShaderProgram& shader) const {
  glBindVertexArray(vao);
  shader.UseProgram();
  DrawChildren(camera, math::Quaternion(), math::Vector4({0, 0, 0, 1}), shader);
}

void RenderNode::DrawChildren(
    const Camera& camera,
    const math::Quaternion& cumRot,
    const math::Vector4& cumTrans,
    const ShaderProgram& shader) const {

  auto localQuat = rotation_*cumRot;
  auto rot = localQuat.RotationMatrix();
  auto localTrans = rot * translation_ + cumTrans;
	for (const auto& child : children) {
		child->DrawChildren(camera, localQuat, localTrans, shader);
	}
  rot.WriteColumn(3, localTrans);
  shader.uploadMatrix(camera.ProjectionMatrix()*rot);

	this->DrawSelf();
}

size_t SquareTest2D::ExclusiveBufferSizeRequired() const { return 12; }

void SquareTest2D::FillColorData(std::vector<float>& data, size_t start) const {
	data[start+0] = 1.0;
	data[start+1] = 0.0;
	data[start+2] = 0.0;

	data[start+3] = 1.0;
	data[start+4] = 1.0;
	data[start+5] = 0.0;

	data[start+6] = 0.0;
	data[start+7] = 0.0;
	data[start+8] = 1.0;

	data[start+9] = 0.0;
	data[start+10] = 1.0;
	data[start+11] = 0.0;
}

void SquareTest2D::FillVertexData(std::vector<float>& data, size_t start) const {
	data[start+0] = 0;
	data[start+1] = 0;
	data[start+2] = 0;

	data[start+3] = 0;
	data[start+4] = 1;
	data[start+5] = 0;

	data[start+6]  =  1;
	data[start+7] = 0;
	data[start+8] = 0;

	data[start+9] =  1;
	data[start+10] =  1;
	data[start+11] = 0;
}

size_t TriangleTest2D::ExclusiveBufferSizeRequired() const { return 12; }

void TriangleTest2D::FillColorData(std::vector<float>& data, size_t start) const {
	data[start+0] = 1.0;
	data[start+1] = 0.0;
	data[start+2] = 0.0;

	data[start+3] = 1.0;
	data[start+4] = 1.0;
	data[start+5] = 0.0;

	data[start+6] = 0.0;
	data[start+7] = 0.0;
	data[start+8] = 1.0;

	data[start+9] = 1.0;
	data[start+10] = 0.0;
	data[start+11] = 0.0;
}

void TriangleTest2D::FillVertexData(std::vector<float>& data, size_t start) const {
	data[start+0] = -0.6;
	data[start+1] = -0.6;
	data[start+2] = 0.0;

	data[start+3] = -0.6;
	data[start+4] =  0.6;
	data[start+5] = 0.0;

	data[start+6]  =  0.6;
	data[start+7] = -0.6;
	data[start+8] = 0.0;

	data[start+9] = -0.6;
	data[start+10] = -0.6;
	data[start+11] = 0.0;
}

void TriangleTest2D::DrawSelf() const {
	assert(start_vertex() % 3 == 0);
	assert(this->ExclusiveBufferSizeRequired() % 3 == 0);
  glDrawArrays (GL_LINE_STRIP, start_vertex()/3, this->ExclusiveBufferSizeRequired()/3);
}

void SquareTest2D::DrawSelf() const {
	assert(start_vertex() % 3 == 0);
	assert(this->ExclusiveBufferSizeRequired() % 3 == 0);
  glDrawArrays (GL_TRIANGLE_STRIP, start_vertex()/3, this->ExclusiveBufferSizeRequired()/3);
}

void RenderNode::DebugRotation(const math::Matrix4& mat) const {
  std::vector<float> data(this->ExclusiveBufferSizeRequired());
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

}} // ShapeShifter::Opengl
