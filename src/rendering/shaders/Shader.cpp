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

#include "rendering/shaders/Shader.h"

#include <cassert>
#include <iostream>
#include <fstream>
#include <memory>
#include <sstream>
#include <stdexcept>

namespace ShapeShifter {
namespace Rendering {
namespace Shaders {

Shader::Shader(const std::string& filename, ShaderType t) {
	auto input_stream = std::ifstream(filename);
	auto data = std::string((std::istreambuf_iterator<char>(input_stream)), std::istreambuf_iterator<char>());
	assert(data.size() > 0);

  ParseLayouts(data);

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

		// ISSUE: wrap all gl functions and add error checking.
		glDeleteShader(shader);

		throw std::runtime_error(slog);
  }
}

Shader::~Shader() {
	glDeleteShader(shader);
}

void Shader::ParseLayouts(const std::string& data) {
  using SupportedBuffers = Data::SupportedBuffers;

  auto stream = std::istringstream(data);
  auto line = std::string{};
  while (std::getline(stream, line)) {
    if (line.find("layout") != std::string::npos && line.find(" in ") != std::string::npos) {
      // Issue: Create classes for writing opengl code within C++
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
        layout_map_[SupportedBuffers::VERTICES] = location;
      } else if (line.find("inColor") != std::string::npos) {
        layout_map_[SupportedBuffers::COLORS] = location;
      } else {
        throw std::runtime_error("Unsupported input buffer type:\n" + line);
      }
    }
  }
}


}}} // ShapeShifter::Rendering::Shaders
