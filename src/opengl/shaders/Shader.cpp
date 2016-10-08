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

#include "opengl/shaders/Shader.h"

#include <cassert>
#include <iostream>
#include <fstream>
#include <memory>
#include <sstream>
#include <stdexcept>

namespace ShapeShifter {
namespace Opengl {
namespace Shaders {

Shader::Shader(const std::string& filename, ShaderType t) {
	auto input_stream = std::ifstream(filename);
	auto data = std::string((std::istreambuf_iterator<char>(input_stream)), std::istreambuf_iterator<char>());
	assert(data.size() > 0);

  ParseLayouts(data);

	// TODO make debug check only?
	auto can_compile = GLint{GL_FALSE};
  glGetIntegerv(GL_SHADER_COMPILER, &can_compile);
	assert(can_compile == GL_TRUE);

  shader = glCreateShader(t == ShaderType::VERTEX ? GL_VERTEX_SHADER : GL_FRAGMENT_SHADER);
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

		//TODO need error handling on this as well.
		glDeleteShader(shader);

		//TODO throw specific exception and do proper logging
		std::cerr << slog;
		throw std::exception();
  }
}

Shader::~Shader() {
	//TODO this wont necessary delete if we share shader amongst different
	//rendering pipelines.  For now things will be set up to not be shared, but
	//if that changes we may need to put in reference counters to keep track of
	//how many pipelines use this particular shader.
	glDeleteShader(shader);
}

void Shader::ParseLayouts(const std::string& data) {

  auto stream = std::istringstream(data);
  auto line = std::string{};
  while (std::getline(stream, line)) {
    if (line.find("layout") != std::string::npos && line.find(" in ") != std::string::npos) {
      // TODO:
      // Horrible manual parsing...  Need to find a better way...
      auto start = line.find('(');
      auto end = line.find(')');
      assert(start != std::string::npos);
      assert(end != std::string::npos);
      std::istringstream temp(line.substr(start+1, end-start-1));
      std::string trash;
      char op;
      size_t location;
      temp >> trash >> op >> location;

      if (line.find("inPosition") != std::string::npos) {
        layout_map_[Data::SupportedBuffers::VERTICES] = location;
      } else if (line.find("inColor") != std::string::npos) {
        layout_map_[Data::SupportedBuffers::COLORS] = location;
      } else {
        throw std::runtime_error("Unsupported input buffer type:\n" + line);
      }
    }
  }
}


}}} // ShapeShifter::Opengl::Shaders
