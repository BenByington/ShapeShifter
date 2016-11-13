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

#include <cassert>
#include <iostream>
#include <sstream>

// remove
#include <vector>

namespace ShapeShifter {
namespace Rendering {
namespace Shaders {

template <class ... Ts>
using pack = std::tuple<Ts...>;

namespace detail {
struct check_inputs {

  template <class Input>
  static constexpr bool is_child() {
    return std::is_base_of<InterfaceVariableBase<Input, typename Input::Type>, Input>::value;
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

template <class Input, class Uniform, class Output>
class GLSLGeneratorBase;
template <class Input, class Uniform, class Output>
class GLSLVertexGeneratorBase;
template <class Input, class Uniform, class Output>
class GLSLFragmentGeneratorBase;

template <class... Inputs, class... Uniforms, class... Outputs>
struct GLSLGeneratorBase<pack<Inputs...>, pack<Uniforms...>, pack<Outputs...>>
  : Inputs... , Uniforms..., Outputs... {
private:
  static_assert(detail::check_inputs::valid<Inputs...>(),
      "Shader template parameters must be InterfaceVariable types");

public:
  using InputTypes = pack<Inputs...>;
  using UniformTypes = pack<Uniforms...>;
  using OutputTypes = pack<Outputs...>;

  GLSLGeneratorBase(const GLSLGeneratorBase&) = delete;
  GLSLGeneratorBase(GLSLGeneratorBase&&) = delete;
  GLSLGeneratorBase& operator=(const GLSLGeneratorBase&) = delete;
  GLSLGeneratorBase& operator=(GLSLGeneratorBase&&) = delete;

  GLSLGeneratorBase(VariableFactory&& factory) : factory_(std::move(factory)), Inputs(factory_)..., Uniforms(factory_)..., Outputs(factory_)... {}

  std::string program(bool vertex) {
    // TODO wrap stream to handle own indentation levels
    // TODO have stream automatically handle newlines
    factory_.stream() << "#version 410\n\n";
    if (vertex) {
      size_t idx = 0;
      auto temp = {(static_cast<Inputs&>(*this).LayoutDeclaration(factory_, idx++), 0)...};
    } else {
      auto temp = {(static_cast<Inputs&>(*this).InputDeclaration(factory_), 0)...};
    }
    //factory_.stream() << "\n";
    // TODO figure out why auto doesn't work here, but does above
    std::initializer_list<int> temp = {(static_cast<Uniforms&>(*this).UniformDeclaration(factory_), 0)...};
    //factory_.stream() << "\n";
    temp = {(static_cast<Outputs&>(*this).OutputDeclaration(factory_), 0)...};

    factory_.stream() << "\nvoid main() {\n\n";
    factory_.stream().incIndent();
    DefineMain(factory_);
    factory_.stream().decIndent();
    factory_.stream() << "\n}\n\n";
    return factory_.stream().str();
  }

protected:
  virtual void DefineMain(const VariableFactory& factory) = 0;

  VariableFactory factory_;
  std::stringstream stream;
};


template <class... Inputs, class... Uniforms, class... Outputs>
class GLSLVertexGeneratorBase<pack<Inputs...>, pack<Uniforms...>, pack<Outputs...>>
  : public GLSLGeneratorBase<pack<Inputs...>, pack<Uniforms...>, pack<Outputs...>> {

using  Base = GLSLGeneratorBase<pack<Inputs...>, pack<Uniforms...>, pack<Outputs...>>;

public:

  GLSLVertexGeneratorBase(VariableFactory&& factory)
    : Base(std::move(factory))
    , gl_Position(this->factory_.template create<Vec4>("gl_Position")){}

  GLSLVertexGeneratorBase(const GLSLVertexGeneratorBase&) = delete;
  GLSLVertexGeneratorBase(GLSLVertexGeneratorBase&&) = delete;
  GLSLVertexGeneratorBase& operator=(const GLSLVertexGeneratorBase&) = delete;
  GLSLVertexGeneratorBase& operator=(GLSLVertexGeneratorBase&&) = delete;

protected:
  Variable<Vec4>  gl_Position;
};

template <class... Inputs, class... Uniforms, class... Outputs>
class GLSLFragmentGeneratorBase<pack<Inputs...>, pack<Uniforms...>, pack<Outputs...>>
  : public GLSLGeneratorBase<pack<Inputs...>, pack<Uniforms...>, pack<Outputs...>> {

using  Base = GLSLGeneratorBase<pack<Inputs...>, pack<Uniforms...>, pack<Outputs...>>;

public:

  GLSLFragmentGeneratorBase(VariableFactory&& factory) : Base(std::move(factory)) {}

  GLSLFragmentGeneratorBase(const GLSLFragmentGeneratorBase&) = delete;
  GLSLFragmentGeneratorBase(GLSLFragmentGeneratorBase&&) = delete;
  GLSLFragmentGeneratorBase& operator=(const GLSLFragmentGeneratorBase&) = delete;
  GLSLFragmentGeneratorBase& operator=(GLSLFragmentGeneratorBase&&) = delete;

};

namespace detail {

struct generator_traits {
  template <class... Types>
  static constexpr bool valid_vertex_shader(GLSLVertexGeneratorBase<Types...>*) {
    return true;
  }
  static constexpr bool valid_vertex_shader(...) {
    return false;
  }

  template <class... Types>
  static constexpr bool valid_fragment_shader(GLSLFragmentGeneratorBase<Types...>*) {
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
  Shader(const std::string& data, GLenum shader_type) : ShaderBase(data, shader_type) {}
  Shader(const Shader&) = delete;
  Shader(Shader&&) = default;
	Shader& operator=(const Shader&) = delete;
	Shader& operator=(Shader&&) = default;
public:
  virtual ~Shader() {}
};

template <class Generator>
class VertexShader : public Shader<Generator> {
    static_assert(detail::generator_traits::valid_vertex_shader((Generator*)nullptr),
        "Template to Shader class must be a child of an GLSLGeneratorBase type");
public:
	VertexShader()
    : Shader<Generator>(Generator(VariableFactory()).program(true), GL_VERTEX_SHADER) {}

	VertexShader(const VertexShader&) = delete;
	VertexShader(VertexShader&&) = default;
	VertexShader& operator=(const VertexShader&) = delete;
	VertexShader& operator=(VertexShader&&) = default;
	virtual ~VertexShader() {}
};

template <class Generator>
class FragmentShader : public Shader<Generator> {
    static_assert(detail::generator_traits::valid_fragment_shader((Generator*)nullptr),
        "Template to Shader class must be a child of an GLSLGeneratorBase type");
public:
	FragmentShader()
    : Shader<Generator>(Generator(VariableFactory()).program(false), GL_FRAGMENT_SHADER) {}
	FragmentShader(const FragmentShader&) = delete;
	FragmentShader(FragmentShader&&) = default;
	FragmentShader& operator=(const FragmentShader&) = delete;
	FragmentShader& operator=(FragmentShader&&) = default;
	virtual ~FragmentShader() {}
};

}}} // ShapeShifter::Rendering::Shaders
#endif /* RENDERING_SHADERS_SHADER_H */

