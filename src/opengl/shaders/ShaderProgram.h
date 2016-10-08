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

#include "opengl/data/BufferTypes.h"
#include "opengl/math/Matrix4.h"
#include "opengl/shaders/Shader.h"

namespace ShapeShifter {
namespace Opengl {
namespace Shaders {

class ShaderProgram {
public:
  ShaderProgram(const ShaderProgram&) = delete;
	ShaderProgram& operator()(ShaderProgram&) = delete;
  ShaderProgram(
      std::unique_ptr<VertexShader> vert,
      std::unique_ptr<FragmentShader> frag);

  virtual ~ShaderProgram();

	void UseProgram() const { glUseProgram(program_); }
  void uploadMatrix(const math::Matrix4& mat) const;

  template <size_t Flags>
  std::map<Data::SupportedBuffers, size_t> BufferMapping() const;
private:
	std::unique_ptr<VertexShader> vert_shader_;
	std::unique_ptr<FragmentShader> frag_shader_;
	GLuint program_;
};

}}} // ShapeShifter::Opengl::Shaders

#endif /* SHADERPROGRAM_H */

