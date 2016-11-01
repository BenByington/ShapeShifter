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

template <class Input, class Output>
class GLSLGeneratorBase;

template <class... Inputs, class... Outputs>
struct GLSLGeneratorBase<pack<Inputs...>, pack<Outputs...>>
  : Inputs... , Outputs... {
private:
  static_assert(detail::check_inputs::valid<Inputs...>(),
      "Shader template parameters must be InterfaceVariable types");

public:
  using InputTypes = std::tuple<Inputs...>;
  using OutputTypes = std::tuple<Outputs...>;

  GLSLGeneratorBase(const GLSLGeneratorBase&) = delete;
  GLSLGeneratorBase(GLSLGeneratorBase&&) = delete;
  GLSLGeneratorBase& operator=(const GLSLGeneratorBase&) = delete;
  GLSLGeneratorBase& operator=(GLSLGeneratorBase&&) = delete;

  GLSLGeneratorBase(VariableFactory&& factory) : factory_(std::move(factory)) {}

  std::string program() {
    // TODO wrap stream to handle own indentation levels
    // TODO have stream automatically handle newlines
    factory_.stream() << "#version 410\n\n";
    std::tuple<Inputs&...> in_parents { static_cast<Inputs&>(*this)... };
    size_t idx = 0;
    auto temp = {(static_cast<Inputs&>(*this).LayoutDeclaration(factory_, idx++), 0)...};
    factory_.stream() << "\n";
    temp = {(static_cast<Outputs&>(*this).OutputDeclaration(factory_), 0)...};

    factory_.stream() << "\nvoid main() {\n";
    factory_.stream() << "}\n";
    std::cerr << factory_.stream().str();
    exit(0);
    return " ";
  }

private:
  VariableFactory factory_;
  std::stringstream stream;
};

namespace detail {

struct generator_traits {
  template <class... Types>
  static constexpr bool valid(GLSLGeneratorBase<Types...>*) {
    return true;
  }
  static constexpr bool valid(...) {
    return false;
  }
};

}

template <class Generator>
class Shader : public ShaderBase {
    static_assert(detail::generator_traits::valid((Generator*)nullptr),
        "Template to Shader class must be a child of an GLSLGeneratorBase type");
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
public:
	VertexShader()
    : Shader<Generator>(Generator(VariableFactory()).program(), GL_VERTEX_SHADER) {}

	VertexShader(const VertexShader&) = delete;
	VertexShader(VertexShader&&) = default;
	VertexShader& operator=(const VertexShader&) = delete;
	VertexShader& operator=(VertexShader&&) = default;
	virtual ~VertexShader() {}
};

template <class Generator>
class FragmentShader : public Shader<Generator> {
public:
	FragmentShader()
    : Shader<Generator>(Generator().program(), GL_FRAGMENT_SHADER) {}
	FragmentShader(const FragmentShader&) = delete;
	FragmentShader(FragmentShader&&) = default;
	FragmentShader& operator=(const FragmentShader&) = delete;
	FragmentShader& operator=(FragmentShader&&) = default;
	virtual ~FragmentShader() {}
};

}}} // ShapeShifter::Rendering::Shaders
#endif /* RENDERING_SHADERS_SHADER_H */

