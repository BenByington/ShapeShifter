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

/*
 * Class that wraps an opengl shader resource.  It expects the shader to
 * program to live in a separate text file, which it will load and parse
 * and compile.  Must use one of the children classes to actually instantiate.
 */
class Shader {
protected:
  enum class ShaderType {
		VERTEX,
		FRAGMENT
	};
	
  Shader(const std::string& filename, ShaderType t);
  Shader(const Shader&) = delete;
	Shader& operator()(const Shader&) = delete;
public:
  virtual ~Shader();
	
public:
	operator GLuint() const {return shader;}
private:
  GLuint shader = 0;
	bool valid = false;
};

class VertexShader : public Shader {
public:
	VertexShader(const std::string& filename) : Shader(filename, ShaderType::VERTEX) {}
	VertexShader(const VertexShader&) = delete;
	VertexShader& operator()(const VertexShader&) = delete;
	virtual ~VertexShader() {}
};

class FragmentShader : public Shader {
public:
	FragmentShader(const std::string& filename) : Shader(filename, ShaderType::FRAGMENT) {}
	FragmentShader(const FragmentShader&) = delete;
	FragmentShader& operator()(const FragmentShader&) = delete;
	virtual ~FragmentShader() {}
};

}} // ShapeShifter::Opengl
#endif /* SHADER_H */

