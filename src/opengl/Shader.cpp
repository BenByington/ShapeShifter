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

#include "opengl/Shader.h"

#include <cassert>
#include <iostream>
#include <fstream>
#include <sstream>
#include <stdexcept>

namespace ShapeShifter {
namespace Opengl {

Shader::Shader(const std::string& filename, ShaderType t) {
	std::ifstream f;
  f.open(filename);
	std::string data((std::istreambuf_iterator<char>(f)), std::istreambuf_iterator<char>());
	assert(data.size() > 0);

	// TODO make debug check only?
	GLint can_compile = GL_FALSE;
  glGetIntegerv(GL_SHADER_COMPILER, &can_compile);
	assert(can_compile == GL_TRUE);
	
  shader = glCreateShader(t == VERTEX ? GL_VERTEX_SHADER : GL_FRAGMENT_SHADER);
	auto source = data.c_str();
	int source_len = data.length();
	glShaderSource(shader, 1, &source, &source_len);
	glCompileShader(shader);

  GLint compiled = false;
  
  glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
  if (!compiled) {
		GLint logLength = 0;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLength);
		char * log = new char[logLength+1];
		glGetShaderInfoLog(shader, logLength, nullptr, log);
		std::string slog(log);
		delete [] log;

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

}} // ShapeShifter::Opengl
