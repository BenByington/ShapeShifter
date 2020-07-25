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

/*
 * Generic shader class.  Use the Specific Vertex and Fragment
 * versions below.
 */
template <class Generator>
class Shader : public ShaderBase {
protected:
  Shader(Generator&& generator, GLenum shader_type)
    : ShaderBase(generator.program(), shader_type)
    , layout_map_(generator.layout_map()) {}
  Shader(const Shader&) = delete;
  Shader(Shader&&) = default;
  Shader& operator=(const Shader&) = delete;
  Shader& operator=(Shader&&) = default;
public:
  virtual ~Shader() {}

  const std::map<std::string, size_t>& layout_map() const override {
    return layout_map_;
  }

private:
  std::map<std::string, size_t> layout_map_;
};

/*
 * Vertex shader class, that mostly just acts as a plugin layer between
 * the actual Shader class interfacing with opengl, and the
 * VertexGenerator that actually specifies the shader program to be
 * compiled
 */
template <class Generator>
class VertexShader : public Shader<Generator> {
    static_assert(
        detail::generator_traits::valid_vertex_shader((Generator*)nullptr),
        "Template to Shader class must be a child of an GLSLGeneratorBase type");
public:
  VertexShader()
    : Shader<Generator>(
        Generator()
      , GL_VERTEX_SHADER)
    {}

  VertexShader(const VertexShader&) = delete;
  VertexShader(VertexShader&&) = default;
  VertexShader& operator=(const VertexShader&) = delete;
  VertexShader& operator=(VertexShader&&) = default;
  virtual ~VertexShader() {}
};

/*
 * Fragment shader class, that mostly just acts as a plugin layer between
 * the actual Shader class interfacting with opengl, and the
 * FragmentGenerator that actually specifies the shader program to be
 * compiled
 */
template <class Generator>
class FragmentShader : public Shader<Generator> {
    static_assert(
        detail::generator_traits::valid_fragment_shader((Generator*)nullptr),
        "Template to Shader class must be a child of an GLSLGeneratorBase type");
public:
  FragmentShader()
    : Shader<Generator>(
        Generator()
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

