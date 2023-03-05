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

#include "rendering/shaders/InterfaceVariable.h"
#include "rendering/shaders/UniformVariable.h"
#include "rendering/shaders/Pack.h"
#include "data/ConcreteBufferManager.h"

#include <type_traits>

namespace ShapeShifter {
namespace Rendering {
namespace Shaders {
namespace Language {

template <typename T>
struct AllInterface
{ static constexpr bool val = false; };
template <InterfaceVariable... Is>
struct AllInterface<Pack<Is...>>
{ static constexpr bool val  = true; };
template <typename T>
concept InterfacePack = requires {
    requires AllInterface<T>::val;
};

template <typename T>
struct AllUniform
{ static constexpr bool val = false; };
template <UniformVariable... Is>
struct AllUniform<Pack<Is...>>
{ static constexpr bool val  = true; };
template <typename T>
concept UniformPack = requires {
    requires AllUniform<T>::val;
};

template <typename T>
struct AllManager
{ static constexpr bool val = false; };
template <Data::BufferManager... Is>
struct AllManager<Pack<Is...>>
{ static constexpr bool val  = true; };
template <typename T>
concept ManagerPack = requires {
    requires AllManager<T>::val;
};

template <InterfacePack Input, UniformPack Uniform, InterfacePack Output>
class GLSLGeneratorBase;
template <ManagerPack Input, UniformPack Uniform, InterfacePack Output>
class GLSLVertexGeneratorBase;
template <InterfacePack Input, UniformPack Uniform, InterfacePack Output>
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
template <InterfaceVariable... Inputs, UniformVariable... Uniforms, InterfaceVariable... Outputs>
struct GLSLGeneratorBase<Pack<Inputs...>, Pack<Uniforms...>, Pack<Outputs...>>
  : Inputs... , Uniforms..., Outputs... {

  GLSLGeneratorBase(const GLSLGeneratorBase&) = delete;
  GLSLGeneratorBase(GLSLGeneratorBase&&) = delete;
  GLSLGeneratorBase& operator=(const GLSLGeneratorBase&) = delete;
  GLSLGeneratorBase& operator=(GLSLGeneratorBase&&) = delete;

  GLSLGeneratorBase(VariableFactory&& factory)
    : factory_(std::move(factory))
    , Inputs(factory_)...
    , Uniforms(factory_)...
    , Outputs(factory_)... {}

  std::string program(bool vertex) {
    factory_.stream() << "#version 410\n\n";
    if (vertex) {
      size_t idx = 0;
      layout_map_ = {LayoutDeclaration<Inputs>(factory_, idx++)...};
    } else {
      auto temp = {(InputDeclaration<Inputs>(factory_), 0)...};
    }

    std::initializer_list<int> temp = {(UniformDeclaration<Uniforms>(factory_), 0)...};
    auto temp2 = {(OutputDeclaration<Outputs>(factory_), 0)...};

    factory_.stream() << "\nvoid main() {\n\n";
    factory_.stream().incIndent();
    DefineMain(factory_);
    factory_.stream().decIndent();
    factory_.stream() << "\n}\n\n";
    return factory_.stream().str();
  }

  decltype(auto) layout_map() { return layout_map_; }

protected:
  virtual void DefineMain(const VariableFactory& factory) = 0;

  VariableFactory factory_;
  std::stringstream stream;

private:

  template <InterfaceVariable Var>
  std::pair<const std::string, size_t>
  static LayoutDeclaration(VariableFactory& factory, size_t idx) {
    factory.stream()
        << "layout (location = "
        << idx << ") in "
        << Language::Variable<typename Var::Type>::TypeName()
        << " " << Var::name()
        << ";\n";
    return {Var::name(), idx};
  }

  template <InterfaceVariable Var>
  static void OutputDeclaration(VariableFactory& factory) {
    if (Var::smooth) factory.stream() << "smooth ";
    factory.stream()
        << "out "
        << Language::Variable<typename Var::Type>::TypeName()
        << " " << Var::name()
        << ";\n";
  }

  template <InterfaceVariable Var>
  static void InputDeclaration(VariableFactory& factory) {
    if (Var::smooth) factory.stream() << "smooth ";
    factory.stream()
        << "in "
        << Language::Variable<typename Var::Type>::TypeName()
        << " " << Var::name()
        << ";\n";
  }

  template <UniformVariable Var>
  static void UniformDeclaration(VariableFactory& factory) {
    factory.stream()
        << "uniform "
        << Language::Variable<typename Var::Type>::TypeName()
        << " " << Var::name()
        << ";\n";
  }


  std::map<std::string, size_t> layout_map_;
};

template <Data::BufferManager... Inputs, UniformVariable... Uniforms, InterfaceVariable... Outputs>
class GLSLVertexGeneratorBase<Pack<Inputs...>, Pack<Uniforms...>, Pack<Outputs...>>
  : public GLSLGeneratorBase<Pack<typename Inputs::Variable...>, Pack<Uniforms...>, Pack<Outputs...>> {

using  Base = GLSLGeneratorBase<Pack<typename Inputs::Variable...>, Pack<Uniforms...>, Pack<Outputs...>>;

public:

  using Managers_t = Pack<Inputs...>;
  using Uniforms_t = Pack<Uniforms...>;
  using Outputs_t = Pack<Outputs...>;

  GLSLVertexGeneratorBase(VariableFactory&& factory)
    : Base(std::move(factory))
    , gl_Position(this->factory_.template create<Language::Vec4>("gl_Position")){}

  GLSLVertexGeneratorBase(const GLSLVertexGeneratorBase&) = delete;
  GLSLVertexGeneratorBase(GLSLVertexGeneratorBase&&) = delete;
  GLSLVertexGeneratorBase& operator=(const GLSLVertexGeneratorBase&) = delete;
  GLSLVertexGeneratorBase& operator=(GLSLVertexGeneratorBase&&) = delete;

  std::string program() { return Base::program(true); }
protected:
  Language::Variable<Language::Vec4>  gl_Position;
};

template <InterfaceVariable... Inputs, UniformVariable... Uniforms, InterfaceVariable... Outputs>
class GLSLFragmentGeneratorBase<Pack<Inputs...>, Pack<Uniforms...>, Pack<Outputs...>>
  : public GLSLGeneratorBase<Pack<Inputs...>, Pack<Uniforms...>, Pack<Outputs...>> {
using  Base = GLSLGeneratorBase<Pack<Inputs...>, Pack<Uniforms...>, Pack<Outputs...>>;

public:
  GLSLFragmentGeneratorBase(VariableFactory&& factory) : Base(std::move(factory)) {}

  GLSLFragmentGeneratorBase(const GLSLFragmentGeneratorBase&) = delete;
  GLSLFragmentGeneratorBase(GLSLFragmentGeneratorBase&&) = delete;
  GLSLFragmentGeneratorBase& operator=(const GLSLFragmentGeneratorBase&) = delete;
  GLSLFragmentGeneratorBase& operator=(GLSLFragmentGeneratorBase&&) = delete;

  std::string program() { return Base::program(false); }

  using Inputs_t = Pack<Inputs...>;
};

}}}} // ShapeShifter::Rendering::Shaders::Language

#endif /* RENDERING_SHADERS_LANGUAGE_GLSLGENERATORBASE_H */

