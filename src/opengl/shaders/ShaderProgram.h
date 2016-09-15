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

#include <map>
#include <memory>
#include <vector>

#include "opengl/BufferTypes.h"
#include "opengl/math/Matrix4.h"
#include "opengl/shaders/Shader.h"

namespace ShapeShifter {
namespace Opengl {
namespace Shaders {

class ShaderProgram {
public:
  ShaderProgram(std::vector<std::unique_ptr<Shader>> shaders);
  ShaderProgram(const ShaderProgram&) = delete;
	ShaderProgram& operator()(ShaderProgram&) = delete;
  virtual ~ShaderProgram();

	void UseProgram() const { glUseProgram(program_); }
  void uploadMatrix(const math::Matrix4& mat) const;

  std::map<SupportedBuffers, size_t> BufferMapping() const;
private:
	std::vector<std::unique_ptr<Shader>> shaders_;
	GLuint program_;
};

}}} // ShapeShifter::Opengl::Shaders

#endif /* SHADERPROGRAM_H */

