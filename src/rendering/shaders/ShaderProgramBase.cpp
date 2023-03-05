/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 *
 *
 * ISSUE: Find license and update headers
 */

/*
 * File:   ShaderProgram.cpp
 * Author: bbyington
 *
 * Created on July 31, 2016, 3:08 PM
 */

#include "ShaderProgramBase.h"

#include <stdexcept>

namespace ShapeShifter::Rendering::Shaders {

ShaderProgramBase::ShaderProgramBase(
    std::unique_ptr<ShaderBase> vert,
    std::unique_ptr<ShaderBase> frag)
  : vert_shader_(std::move(vert))
  , frag_shader_(std::move(frag)) {

  program_ = glCreateProgram();
  assert(program_ != 0);

  glAttachShader(program_, *vert_shader_);
  glAttachShader(program_, *frag_shader_);

  glLinkProgram(program_);

  auto linked = GL_FALSE;
  glGetProgramiv(program_, GL_LINK_STATUS, &linked);
  if (linked == GL_FALSE) {
    auto logLength = GLint {0};
    glGetProgramiv(program_, GL_INFO_LOG_LENGTH, &logLength);

    auto log = std::unique_ptr<char[]>(new char[logLength+1]);
    glGetProgramInfoLog(program_, logLength, nullptr, log.get());

    auto slog = std::string(log.get());

    glDeleteProgram(program_);

    throw std::runtime_error(slog);
  }
}

ShaderProgramBase::~ShaderProgramBase() {
  glDeleteProgram(program_);
}

// Will need overloads for the different allowed types
void ShaderProgramBase::UploadValue(const Math::Matrix4& mat, const char* name) const {
  auto transform_location = glGetUniformLocation(program_, name);
  glUniformMatrix4fv(transform_location, 1, GL_FALSE, mat.data());
}

} // ShapeShifter::Rendering::Shaders

