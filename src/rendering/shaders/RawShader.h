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

namespace ShapeShifter::Rendering::Shaders {

namespace detail {

std::map<std::string, size_t> ParseLayouts(const std::string& data);

}
enum class RawShaderType {
  VERTEX,
  FRAGMENT
};

/*
 * Basic class that supports reading in GLSL shader programs from text files.
 * This is fully supported, but the Shader class should be preferred, as it
 * is procedurally generated and has compile-time constraints matching those
 * provided for the RenderNodes
 */
template <RawShaderType type>
class RawShader : public ShaderBase {
  // Hidden constructor so we don't have to read the file data a second time
  // to parse for the the layout_map
  RawShader(const std::string& data, bool dummy)
    : ShaderBase(data, RawEnum()) {
    layout_map_ = ParseLayouts(data);
  }
public:
  RawShader(const std::string& filename) : RawShader(ReadFile(filename), true) {}

  RawShader(const RawShader&) = delete;
  RawShader(RawShader&&) = default;
  RawShader& operator=(const RawShader&) = delete;
  RawShader& operator=(RawShader&&) = default;

  virtual ~RawShader() {}

  const std::map<std::string, size_t>& layout_map() const override {
    return layout_map_;
  }
private:
  void ParseLayouts(const std::string& data) {
    layout_map_ = detail::ParseLayouts(data);
  }

  std::map<std::string, size_t> layout_map_;

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

} // ShapeShifter::Rendering::Shaders
#endif /* RENDERING_SHADERS_RAW_SHADER_H */

