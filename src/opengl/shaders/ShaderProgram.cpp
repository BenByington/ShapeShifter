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
namespace Shaders {

ShaderProgram::ShaderProgram(std::unique_ptr<VertexShader> vert, std::unique_ptr<FragmentShader> frag)
  : vert_shader_(std::move(vert))
  , frag_shader_(std::move(frag)) {

	program_ = glCreateProgram();
	assert(program_ != 0);

	//TODO check on various error conditions that can be caused.
	glAttachShader(program_, *vert_shader_);
	glAttachShader(program_, *frag_shader_);

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

template <size_t Flags>
std::map<SupportedBuffers, size_t> ShaderProgram::BufferMapping() const {
  auto ret = vert_shader_->layout_map();

  auto validate = [] (size_t in, SupportedBufferFlags flag) {
    if (!in & flag) throw std::runtime_error("Shader requires input buffer not supplied");
    return in xor flag;
  };
  // TODO make this less hard coded...
  size_t check = Flags;
  bool found_vertex = false;
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
  if (check != 0 || !found_vertex) {
    throw std::runtime_error("Shader program does not take the requested inputs");
  }
  return ret;
}

// TODO cleanup
template std::map<SupportedBuffers, size_t> ShaderProgram::BufferMapping<SupportedBufferFlags::COLORS>() const;

}}} // ShapeShifter::Opengl::Shaders

