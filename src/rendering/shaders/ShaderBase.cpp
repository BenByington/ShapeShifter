/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   Shader.cpp
 * Author: bbyington
 *
 * Created on July 29, 2016, 8:38 AM
 */

#include "rendering/shaders/ShaderBase.h"

#include <cassert>
#include <memory>

namespace ShapeShifter {
namespace Rendering {
namespace Shaders {

ShaderBase::ShaderBase(const std::string& data, GLenum shader_type) {
  assert(data.size() > 0);

  auto can_compile = GLint{GL_FALSE};
  glGetIntegerv(GL_SHADER_COMPILER, &can_compile);
  assert(can_compile == GL_TRUE);

  assert(shader_type == GL_VERTEX_SHADER || shader_type == GL_FRAGMENT_SHADER);
  shader = glCreateShader(shader_type);
  auto source = data.c_str();
  auto source_len = static_cast<GLint>(data.length());
  glShaderSource(shader, 1, &source, &source_len);
  glCompileShader(shader);

  auto compiled = GL_FALSE;

  glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
  if (!compiled) {
    auto logLength = GLint {0};
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLength);

    auto log = std::unique_ptr<char[]>(new char[logLength+1]);
    glGetShaderInfoLog(shader, logLength, nullptr, log.get());

    auto slog = std::string(log.get());

    // ISSUE: wrap all gl functions and add error checking.
    glDeleteShader(shader);

    throw std::runtime_error(slog);
  }
}

ShaderBase::~ShaderBase() {
  if (shader != 0) {
    glDeleteShader(shader);
  }
}

ShaderBase::ShaderBase(ShaderBase&& other) {
  shader = other.shader;

  other.shader = 0;
}

ShaderBase& ShaderBase::operator =(ShaderBase&& other) {
  shader = other.shader;

  other.shader = 0;

  return *this;
}

}}} // ShapeShifter::Rendering::Shaders
