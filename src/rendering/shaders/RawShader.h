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

#ifndef RENDERING_SHADERS_RAW_SHADER_H
#define RENDERING_SHADERS_RAW_SHADER_H

#include "rendering/shaders/ShaderBase.h"

#include <iostream>
#include <fstream>

namespace ShapeShifter {
namespace Rendering {
namespace Shaders {

enum class RawShaderType {
  VERTEX,
  FRAGMENT
};

template <RawShaderType type>
class RawShader : public ShaderBase {
public:
  RawShader(const std::string& filename)
    : ShaderBase(ReadFile(filename)
    , RawEnum()) {}

  RawShader(const RawShader&) = delete;
  RawShader(RawShader&&) = default;
	RawShader& operator=(const RawShader&) = delete;
	RawShader& operator=(RawShader&&) = default;
public:
  virtual ~RawShader() {}

private:
  static std::string ReadFile(const std::string& filename) {
  	auto input_stream = std::ifstream(filename);
  	return std::string((std::istreambuf_iterator<char>(input_stream)), std::istreambuf_iterator<char>());
  }

  static GLenum RawEnum() {
    switch (type) {
      case RawShaderType::VERTEX:
        return GL_VERTEX_SHADER;
      case RawShaderType::FRAGMENT:
        return GL_FRAGMENT_SHADER;
    }
  }
};

}}} // ShapeShifter::Rendering::Shaders
#endif /* RENDERING_SHADERS_RAW_SHADER_H */

