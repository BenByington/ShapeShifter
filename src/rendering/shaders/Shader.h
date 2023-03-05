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
#include "rendering/shaders/InterfaceVariable.h"
#include "rendering/shaders/Pack.h"
#include "rendering/shaders/ShaderBase.h"

#include <cassert>

namespace ShapeShifter::Rendering::Shaders {

namespace detail {

// Dummy functions for the concept definition to try and call.
template <typename... T>
void valid_vertex_shader(const GLSLVertexGeneratorBase<T...>&);
template <typename... T>
void valid_fragment_shader(const GLSLFragmentGeneratorBase<T...>&);

template <typename T>
concept ValidVertexGenerator = requires(T t) {
    valid_vertex_shader(t);
};

template <typename T>
concept ValidFragmentGenerator = requires(T t) {
    valid_fragment_shader(t);
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
template <detail::ValidVertexGenerator Generator>
class VertexShader : public Shader<Generator> {
public:
  VertexShader()
    : Shader<Generator>(
        Generator(VariableFactory())
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
template <detail::ValidFragmentGenerator Generator>
class FragmentShader : public Shader<Generator> {
public:
  FragmentShader()
    : Shader<Generator>(
        Generator(VariableFactory())
      , GL_FRAGMENT_SHADER)
    {}
  FragmentShader(const FragmentShader&) = delete;
  FragmentShader(FragmentShader&&) = default;
  FragmentShader& operator=(const FragmentShader&) = delete;
  FragmentShader& operator=(FragmentShader&&) = default;
  virtual ~FragmentShader() {}
};

} // ShapeShifter::Rendering::Shaders
#endif /* RENDERING_SHADERS_SHADER_H */

