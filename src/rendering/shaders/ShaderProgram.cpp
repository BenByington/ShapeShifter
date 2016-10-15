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

}}} // ShapeShifter::Rendering::Shaders

