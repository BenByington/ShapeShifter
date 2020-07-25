/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   GLSLGeneratorBase.h
 * Author: bbyington
 *
 * Created on November 16, 2016, 8:58 PM
 */

#ifndef RENDERING_SHADERS_LANGUAGE_GLSLGENERATORBASE_H
#define RENDERING_SHADERS_LANGUAGE_GLSLGENERATORBASE_H

#include "rendering/shaders/InterfaceVariableBase.h"
#include "rendering/shaders/UniformVariableBase.h"
#include "rendering/shaders/Pack.h"
#include "data/ConcreteBufferManager.h"

#include <type_traits>

namespace ShapeShifter {
namespace Rendering {
namespace Shaders {
namespace Language {

namespace detail {

template <template <class, class> class Parent>
struct check_inputs {
  template <class Input>
  static constexpr bool is_child() {
    return std::is_base_of<
        Parent<Input, typename Input::Type>,
        Input
    >::value;
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

template <class T>
struct is_manager {
  static constexpr bool value = std::is_base_of<Data::BaseManager<T>,T>::value;
};

template <class... Ts>
struct are_managers {
  static constexpr const bool check() {
    const std::array<bool, sizeof...(Ts)> check { is_manager<Ts>::value... };
    bool ret = true;
    for (size_t i = 0; i < sizeof...(Ts); ++i) ret = ret && check[i];
    return ret;
  }
};


}

template <class Input, class Uniform, class Output>
struct GLSLGeneratorBase;
template <class Input, class Uniform, class Output>
class GLSLVertexGeneratorBase;
template <class Input, class Uniform, class Output>
class GLSLFragmentGeneratorBase;

/*
 * Base class for procedurally generating GLSL shader code.  Inputs...
 * Uniforms... and Outputs... define interface variables for the shader
 * stage.  They must be of InterfaceVariable type, and will automatically
 * provide Variable<T> variables for the implementation class to use.
 *
 * This class should not be extended directly, but rather the Vertex and
 * Fragment children below.
 */
template <class... Inputs, class... Uniforms, class... Outputs>
struct GLSLGeneratorBase<Pack<Inputs...>, Pack<Uniforms...>, Pack<Outputs...>>
  : Inputs... , Uniforms..., Outputs... {
private:
  static_assert(
      detail::check_inputs<InterfaceVariableBase>::valid<Inputs..., Outputs...>(),
      "Shader template parameters must be InterfaceVariable types");
  static_assert(
      detail::check_inputs<UniformVariableBase>::valid<Uniforms...>(),
      "Shader template parameters must be UniformVariable types");

public:

  GLSLGeneratorBase(const GLSLGeneratorBase&) = delete;
  GLSLGeneratorBase(GLSLGeneratorBase&&) = delete;
  GLSLGeneratorBase& operator=(const GLSLGeneratorBase&) = delete;
  GLSLGeneratorBase& operator=(GLSLGeneratorBase&&) = delete;

  GLSLGeneratorBase()
    : Inputs()...
    , Uniforms()...
    , Outputs()... {}

  std::string program(bool vertex) {
    Language::Stream() << "#version 410\n\n";
    if (vertex) {
      size_t idx = 0;
      layout_map_ = {static_cast<Inputs&>(*this).LayoutDeclaration(idx++)...};
    } else {
      auto worker = {(static_cast<Inputs&>(*this).InputDeclaration(), 0)...};
      (void) worker;
    }

    std::initializer_list<int> temp = {(static_cast<Uniforms&>(*this).UniformDeclaration(), 0)...};
    temp = {(static_cast<Outputs&>(*this).OutputDeclaration(), 0)...};

    Language::Stream() << "\nvoid main() {\n\n";
    Language::Stream().incIndent();
    DefineMain();
    Language::Stream().decIndent();
    Language::Stream() << "\n}\n\n";
    return Language::Stream().str();
  }

  decltype(auto) layout_map() { return layout_map_; }

protected:
  virtual void DefineMain() = 0;

  std::stringstream stream;

private:
  std::map<std::string, size_t> layout_map_;
};

template <class... Inputs, class... Uniforms, class... Outputs>
class GLSLVertexGeneratorBase<Pack<Inputs...>, Pack<Uniforms...>, Pack<Outputs...>>
  : public GLSLGeneratorBase<Pack<typename Inputs::Variable...>, Pack<Uniforms...>, Pack<Outputs...>> {

static_assert(detail::are_managers<Inputs...>::check(),
    "GLSLVertexGeneratorBase only accepts BufferManagers within the Inputs pack.");
using  Base = GLSLGeneratorBase<Pack<typename Inputs::Variable...>, Pack<Uniforms...>, Pack<Outputs...>>;

public:

  using Managers_t = Pack<Inputs...>;
  using Uniforms_t = Pack<Uniforms...>;
  using Outputs_t = Pack<Outputs...>;

  GLSLVertexGeneratorBase()
    : Base()
    , gl_Position("gl_Position") {}

  GLSLVertexGeneratorBase(const GLSLVertexGeneratorBase&) = delete;
  GLSLVertexGeneratorBase(GLSLVertexGeneratorBase&&) = delete;
  GLSLVertexGeneratorBase& operator=(const GLSLVertexGeneratorBase&) = delete;
  GLSLVertexGeneratorBase& operator=(GLSLVertexGeneratorBase&&) = delete;

  std::string program() { return Base::program(true); }
protected:
  Language::Vec4 gl_Position;
};

template <class... Inputs, class... Uniforms, class... Outputs>
class GLSLFragmentGeneratorBase<Pack<Inputs...>, Pack<Uniforms...>, Pack<Outputs...>>
  : public GLSLGeneratorBase<Pack<Inputs...>, Pack<Uniforms...>, Pack<Outputs...>> {
using  Base = GLSLGeneratorBase<Pack<Inputs...>, Pack<Uniforms...>, Pack<Outputs...>>;

public:
  GLSLFragmentGeneratorBase() : Base() {}

  GLSLFragmentGeneratorBase(const GLSLFragmentGeneratorBase&) = delete;
  GLSLFragmentGeneratorBase(GLSLFragmentGeneratorBase&&) = delete;
  GLSLFragmentGeneratorBase& operator=(const GLSLFragmentGeneratorBase&) = delete;
  GLSLFragmentGeneratorBase& operator=(GLSLFragmentGeneratorBase&&) = delete;

  std::string program() { return Base::program(false); }

  using Inputs_t = Pack<Inputs...>;
  using Uniforms_t = Pack<Uniforms...>;
};

}}}} // ShapeShifter::Rendering::Shaders::Language

#endif /* RENDERING_SHADERS_LANGUAGE_GLSLGENERATORBASE_H */

