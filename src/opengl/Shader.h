/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   Shader.h
 * Author: bbyington
 *
 * Created on July 29, 2016, 8:38 AM
 */

#ifndef SHADER_H
#define SHADER_H

#include <string>
#include <opengl/gl3.h>

namespace ShapeShifter {
namespace Opengl {

class Shader {
public:
  enum ShaderType {
		VERTEX,
		FRAGMENT
	};
	
protected:
  Shader(const std::string& filename, ShaderType t);
  Shader(const Shader&) = delete;
	Shader(const Shader&&) = delete;
public:
  virtual ~Shader();
	
public:
	virtual ShaderType type() = 0;
	operator GLuint() const {return shader;}
private:
  GLuint shader = 0;
	bool valid = false;
};

class VertexShader : public Shader {
public:
	VertexShader(const std::string& filename) : Shader(filename, VERTEX) {}
	VertexShader(const VertexShader&) = delete;
	VertexShader(const VertexShader&&) = delete;
	virtual ~VertexShader() {}

	virtual ShaderType type() override { return ShaderType::VERTEX; }
};

class FragmentShader : public Shader {
public:
	FragmentShader(const std::string& filename) : Shader(filename, FRAGMENT) {}
	FragmentShader(const FragmentShader&) = delete;
	FragmentShader(const FragmentShader&&) = delete;
	virtual ~FragmentShader() {}

	virtual ShaderType type() override { return ShaderType::FRAGMENT; }
};

}} // ShapeShifter::Opengl
#endif /* SHADER_H */

