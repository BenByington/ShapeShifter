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

#include "ShaderProgram.h"

#include <cassert>
#include <iostream>
#include <limits>

namespace ShapeShifter {
namespace Rendering {
namespace Shaders {

using Data::SupportedBuffers;
using Data::SupportedBufferFlags;

ShaderProgram::ShaderProgram(
    std::unique_ptr<VertexShader> vert,
    std::unique_ptr<FragmentShader> frag)
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

ShaderProgram::~ShaderProgram() {
	glDeleteProgram(program_);
}

void ShaderProgram::uploadMatrix(const Math::Matrix4& mat) const {
  auto transform_location = glGetUniformLocation(program_, "transform");
  glUniformMatrix4fv(transform_location, 1, GL_FALSE, mat.data());
}

template <size_t Flags>
std::map<SupportedBuffers, size_t> ShaderProgram::BufferMapping() const {
  auto ret = vert_shader_->layout_map();

  auto validate = [] (auto in, auto flag) {
    static_assert(std::is_integral<decltype(in)>::value,"Need integral type for lambda");
    static_assert(std::is_same<
        decltype(in),
        typename std::underlying_type<decltype(flag)>::type
        >::value,"Need same type parameters for lambda");
    if (!in & flag) throw std::runtime_error("Shader requires input buffer not supplied");
    return in xor flag;
  };

  auto check = Flags;
  auto found_vertex = false;
  for (const auto& kv : ret) {
    switch (kv.first) {
      case SupportedBuffers::COLORS:
        check = validate(check, SupportedBufferFlags::COLORS);
        break;
      case SupportedBuffers::INDICES:
        check = validate(check, SupportedBufferFlags::INDICES);
        break;
      case SupportedBuffers::VERTICES:
        found_vertex = true;
        break;
      case SupportedBuffers::TEXTURES:
        check = validate(check, SupportedBufferFlags::TEXTURES);
        break;
    }
  }

  // TODO fix this hack.  Shader doesn't care about indices.
  if (check == SupportedBufferFlags::INDICES) {
    check = 0;
    ret[SupportedBuffers::INDICES] = std::numeric_limits<size_t>::max();
  }
  if (check != 0 || !found_vertex) {
    throw std::runtime_error("Shader program does not take the requested inputs");
  }
  return ret;
}

template std::map<SupportedBuffers, size_t> ShaderProgram::BufferMapping<SupportedBufferFlags::COLORS>() const;
template std::map<SupportedBuffers, size_t> ShaderProgram::BufferMapping<SupportedBufferFlags::COLORS | SupportedBufferFlags::INDICES>() const;

}}} // ShapeShifter::Rendering::Shaders

