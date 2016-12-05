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
#include "rendering/shaders/ShaderBase.h"

namespace ShapeShifter {
namespace Rendering {
namespace Shaders {

/*
 * Abstract base class for all shader programs.  (Children classes will be
 * typed in various ways depending on what Interface they support)
 */
class ShaderProgramBase {
public:
  ShaderProgramBase(const ShaderProgramBase&) = delete;
	ShaderProgramBase& operator()(ShaderProgramBase&) = delete;
  ShaderProgramBase(
      std::unique_ptr<ShaderBase> vert,
      std::unique_ptr<ShaderBase> frag);

  virtual ~ShaderProgramBase();

	void UseProgram() const { glUseProgram(program_); }
  void uploadMatrix(const Math::Matrix4& mat) const;

  // TODO think about if this should remain public
  const auto& layout_map() const {
    return vert_shader_->layout_map();
  }

private:
	std::unique_ptr<ShaderBase> vert_shader_;
	std::unique_ptr<ShaderBase> frag_shader_;
	GLuint program_;
};

}}} // ShapeShifter::Rendering::Shaders

#endif /* RENDERING_SHADERS_SHADER_PROGRAM_BASE_H */

