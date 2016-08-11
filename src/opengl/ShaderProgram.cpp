/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 *
 *
 * TODO change license netbeans includes
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

namespace ShapeShifter {
namespace Opengl {

ShaderProgram::ShaderProgram(std::vector<std::unique_ptr<Shader>> shaders)
  : shaders_(std::move(shaders)) {

	program_ = glCreateProgram();
	assert(program_ != 0);

	//TODO check on various error conditions that can be caused.
	//TODO understand what happens when two of the same shader type are added.
	//TODO understand if the call ordering here affects things.
	for (const auto& shader : shaders_) {
		//TODO Should be impossible to get uncompiled program, but maybe at least
		//add debug build routine to check?
		glAttachShader(program_, *shader);
	}

	//TODO Again check for errors
	//TODO can link programs already in use.  Current RAII implementation will
	//prevent that, don't know if I need any asserts to keep things sane if
	//something evolves.
	glLinkProgram(program_);

	//TODO this is mostly copy/paste.  Factor out?
	GLint linked = GL_FALSE;
	glGetProgramiv(program_, GL_LINK_STATUS, &linked);
	if (linked == GL_FALSE) {
		GLint logLength = 0;
		glGetProgramiv(program_, GL_INFO_LOG_LENGTH, &logLength);
		char * log = new char[logLength+1];
		glGetProgramInfoLog(program_, logLength, nullptr, log);
		std::string slog(log);
		delete [] log;

		//TODO need error handling on this as well.
		glDeleteProgram(program_);

		//TODO throw specific exception and do proper logging
		std::cerr << slog;
		throw std::exception();
	}
}

ShaderProgram::~ShaderProgram() {
	//TODO check for errors?
	glDeleteProgram(program_);
}

void ShaderProgram::uploadMatrix(const math::Matrix4& mat) const {
  GLint transform_location = glGetUniformLocation(program_, "transform");
  // TODO decipher this
  glUniformMatrix4fv(transform_location, 1, GL_FALSE, mat.data());
}

}} // ShapeShifter::Opengl

