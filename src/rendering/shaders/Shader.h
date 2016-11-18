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

#include "rendering/shaders/language/GLSLGeneratorBase.h"
#include "rendering/shaders/InterfaceVariableBase.h"
#include "rendering/shaders/Pack.h"
#include "rendering/shaders/ShaderBase.h"

#include <cassert>

namespace ShapeShifter {
namespace Rendering {
namespace Shaders {

namespace detail {

struct generator_traits {
  template <class... Types>
  static constexpr bool valid_vertex_shader(
      Language::GLSLVertexGeneratorBase<Types...>*) {
    return true;
  }
  static constexpr bool valid_vertex_shader(...) {
    return false;
  }

  template <class... Types>
  static constexpr bool valid_fragment_shader(
      Language::GLSLFragmentGeneratorBase<Types...>*) {
    return true;
  }
  static constexpr bool valid_fragment_shader(...) {
    return false;
  }
};

}

template <class Generator>
class Shader : public ShaderBase {
protected:
  Shader(const std::string& data, GLenum shader_type)
    : ShaderBase(data, shader_type) {}
  Shader(const Shader&) = delete;
  Shader(Shader&&) = default;
	Shader& operator=(const Shader&) = delete;
	Shader& operator=(Shader&&) = default;
public:
  virtual ~Shader() {}
};

template <class Generator>
class VertexShader : public Shader<Generator> {
    static_assert(
        detail::generator_traits::valid_vertex_shader((Generator*)nullptr),
        "Template to Shader class must be a child of an GLSLGeneratorBase type");
public:
	VertexShader()
    : Shader<Generator>(
        Generator(VariableFactory()).program(true)
      , GL_VERTEX_SHADER)
    {}

	VertexShader(const VertexShader&) = delete;
	VertexShader(VertexShader&&) = default;
	VertexShader& operator=(const VertexShader&) = delete;
	VertexShader& operator=(VertexShader&&) = default;
	virtual ~VertexShader() {}
};

template <class Generator>
class FragmentShader : public Shader<Generator> {
    static_assert(
        detail::generator_traits::valid_fragment_shader((Generator*)nullptr),
        "Template to Shader class must be a child of an GLSLGeneratorBase type");
public:
	FragmentShader()
    : Shader<Generator>(
        Generator(VariableFactory()).program(false)
      , GL_FRAGMENT_SHADER)
    {}
	FragmentShader(const FragmentShader&) = delete;
	FragmentShader(FragmentShader&&) = default;
	FragmentShader& operator=(const FragmentShader&) = delete;
	FragmentShader& operator=(FragmentShader&&) = default;
	virtual ~FragmentShader() {}
};

}}} // ShapeShifter::Rendering::Shaders
#endif /* RENDERING_SHADERS_SHADER_H */

