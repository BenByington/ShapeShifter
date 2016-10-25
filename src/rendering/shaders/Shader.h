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

#ifndef RENDERING_SHADERS_SHADER_H
#define RENDERING_SHADERS_SHADER_H

#include "rendering/shaders/InterfaceVariableBase.h"
#include "rendering/shaders/ShaderBase.h"

namespace ShapeShifter {
namespace Rendering {
namespace Shaders {

namespace detail {
struct check_inputs {

  template <class Input>
  static constexpr bool is_child() {
    return std::is_base_of<InterfaceVariableBase<Input>, Input>::value;
  }
  template <class... Inputs>
  static constexpr bool valid() {
    constexpr bool checks[] = { is_child<Inputs>()... };
    bool ret = true;
    for (auto b : checks) ret &= b;
    return ret;
  }
  static constexpr bool valid(...) { return false; }
};
}

template <class... Inputs>
class Shader : public ShaderBase {
  static_assert(detail::check_inputs::valid<Inputs...>(),
      "Shader template parameters must be InterfaceVariable types");
protected:
  Shader(const std::string& data, GLenum shader_type) : ShaderBase(data, shader_type) {}
  Shader(const Shader&) = delete;
	Shader& operator=(const Shader&) = delete;
public:
  virtual ~Shader() {}
};

template <class... Inputs>
class VertexShader : public Shader<Inputs...> {
  using Base = Shader<Inputs...>;
public:
	VertexShader(const std::string& data) : Base(data, GL_VERTEX_SHADER) {}
	VertexShader(const VertexShader&) = delete;
	VertexShader& operator()(const VertexShader&) = delete;
	virtual ~VertexShader() {}
};

template <class... Inputs>
class FragmentShader : public Shader<Inputs...> {
  using Base = Shader<Inputs...>;
public:
	FragmentShader(const std::string& data) : Base(data, GL_FRAGMENT_SHADER) {}
	FragmentShader(const FragmentShader&) = delete;
	FragmentShader& operator()(const FragmentShader&) = delete;
	virtual ~FragmentShader() {}
};

}}} // ShapeShifter::Rendering::Shaders
#endif /* RENDERING_SHADERS_SHADER_H */

