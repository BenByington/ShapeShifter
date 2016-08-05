/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   ShaderProgram.h
 * Author: bbyington
 *
 * Created on July 31, 2016, 3:08 PM
 */

#ifndef SHADERPROGRAM_H
#define SHADERPROGRAM_H

#include <vector>
#include <memory>

#include "opengl/Shader.h"

namespace ShapeShifter {
namespace Opengl {

class ShaderProgram {
public:
  ShaderProgram(std::vector<std::unique_ptr<Shader>> shaders);
  ShaderProgram(const ShaderProgram&) = delete;
	ShaderProgram& operator()(ShaderProgram&) = delete;
  virtual ~ShaderProgram();

	void UseProgram() { glUseProgram(program_); }
private:
	std::vector<std::unique_ptr<Shader>> shaders_;
	GLuint program_;
};

}} // ShapeShifter::Opengl

#endif /* SHADERPROGRAM_H */

