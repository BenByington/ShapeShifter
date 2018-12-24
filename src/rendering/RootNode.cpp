/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   GLDataLoader.cpp
 * Author: bbyington
 *
 * Created on October 13, 2016, 4:02 PM
 */

// TODO this file named poorly?
#include "RootNode.h"

namespace ShapeShifter {
namespace Rendering {

GLDataLoader::GLDataLoader(Data::MixedDataMap& data) {

  assert(data.DataRemaining().vertex_ == 0);
  assert(data.DataRemaining().index_ == 0);

  for (const auto& kv: data.FloatData()) {
    auto vbo = GLuint{0};
    const auto& buffer_dat = kv.second;
    glGenBuffers (1, &vbo);
    buffers_[kv.first] = vbo;
    glBindBuffer (GL_ARRAY_BUFFER, vbo);
    glBufferData (GL_ARRAY_BUFFER, buffer_dat, GL_STATIC_DRAW);
  }

  for (const auto& kv: data.IntegralData()) {
    auto vbo = GLuint{0};
    const auto& buffer_dat = kv.second;
    glGenBuffers (1, &vbo);
    buffers_[kv.first] = vbo;
    glBindBuffer (GL_ARRAY_BUFFER, vbo);
    glBufferData (GL_ARRAY_BUFFER, buffer_dat, GL_STATIC_DRAW);
  }

  const auto& indices = data.indices();
  ibo_ = GLuint{0};
  glGenBuffers (1, &ibo_);
  glBindBuffer (GL_ARRAY_BUFFER, ibo_);
  glBufferData (GL_ARRAY_BUFFER, indices, GL_STATIC_DRAW);

}

GLDataLoader::~GLDataLoader() {
  for (auto& kv : buffers_) {
    glDeleteBuffers(1, &kv.second);
  }
  if (ibo_ != 0) glDeleteBuffers(1, &ibo_);
  ibo_ = 0;
  buffers_.clear();
}

}} // namespace Shapeshifter::Rendering
