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

#ifndef RENDERING_SHADERS_SHADER_PROGRAM_BASE_H
#define RENDERING_SHADERS_SHADER_PROGRAM_BASE_H

#include "data/AbstractBufferManager.h"
#include "math/Matrix4.h"
#include "rendering/shaders/Shader.h"

namespace ShapeShifter {
namespace Rendering {
namespace Shaders {

class ShaderProgramBase {
public:
  ShaderProgramBase(const ShaderProgramBase&) = delete;
	ShaderProgramBase& operator()(ShaderProgramBase&) = delete;
  ShaderProgramBase(
      std::unique_ptr<VertexShader> vert,
      std::unique_ptr<FragmentShader> frag);

  virtual ~ShaderProgramBase();

	void UseProgram() const { glUseProgram(program_); }
  void uploadMatrix(const Math::Matrix4& mat) const;

  virtual std::vector<std::shared_ptr<Data::AbstractManager>> BufferMapping() const = 0;

protected:

  const VertexShader& vert_shader() const {
    return *vert_shader_;
  }

  const FragmentShader& frag_shader() const {
    return *frag_shader_;
  }

private:
	std::unique_ptr<VertexShader> vert_shader_;
	std::unique_ptr<FragmentShader> frag_shader_;
	GLuint program_;
};

}}} // ShapeShifter::Rendering::Shaders

#endif /* RENDERING_SHADERS_SHADER_PROGRAM_BASE_H */

