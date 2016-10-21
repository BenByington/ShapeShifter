/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   RootNode.cpp
 * Author: bbyington
 *
 * Created on October 13, 2016, 4:02 PM
 */

#include "RootNode.h"

namespace ShapeShifter {
namespace Rendering {

void RootNode::UpdateData() {

	CleanupBuffer();
  auto size = SubtreeCounts();

  Data::MixedDataMap data(managers_, size);

	PopulateBufferData(data);
	assert(data.DataRemaining().vertex_ == 0);
	assert(data.DataRemaining().index_ == 0);

  glGenVertexArrays (1, &vao);
  glBindVertexArray (vao);

  for (const auto& kv: data.FloatData()) {
    auto vbo = GLuint{0};
    const auto& buffer_dat = kv.second;
    glGenBuffers (1, &vbo);
    glBindBuffer (GL_ARRAY_BUFFER, vbo);
    glBufferData (GL_ARRAY_BUFFER, buffer_dat.size() * sizeof (float), buffer_dat.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(kv.first->idx(),  buffer_dat.size()/data.Size().vertex_, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(kv.first->idx());
  }

  for (const auto& kv: data.IntegralData()) {
    auto vbo = GLuint{0};
    const auto& buffer_dat = kv.second;
    glGenBuffers (1, &vbo);
    glBindBuffer (GL_ARRAY_BUFFER, vbo);
    glBufferData (GL_ARRAY_BUFFER, buffer_dat.size() * sizeof (uint32_t), buffer_dat.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(kv.first->idx(),  buffer_dat.size()/data.Size().vertex_, GL_INT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(kv.first->idx());
  }

  const auto& indices = data.indices();
  ibo = GLuint{0};
  glGenBuffers (1, &ibo);
  glBindBuffer (GL_ELEMENT_ARRAY_BUFFER, ibo);
  glBufferData (GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof (uint32_t), indices.data(), GL_STATIC_DRAW);
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

}} // namespace Shapeshifter::Rendering