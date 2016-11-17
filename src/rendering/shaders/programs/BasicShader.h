/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   BasicShader.h
 * Author: bbyington
 *
 * Created on November 1, 2016, 4:26 PM
 */

#ifndef RENDERING_SHADERS_PROGRAMS_BASIC_SHADER_H
#define RENDERING_SHADERS_PROGRAMS_BASIC_SHADER_H

#include "data/ConcreteBufferManager.h"
#include "rendering/shaders/Shader.h"
#include "rendering/shaders/language/GLSLGeneratorBase.h"

namespace ShapeShifter {
namespace Rendering {
namespace Shaders {
namespace Programs {

using Data::ColorManager;
using Data::VertexManager;

namespace detail {
struct ColorPass : InterfaceVariableBase<ColorPass, Language::Vec3> {
  ColorPass() = delete;

  using Base = InterfaceVariableBase<ColorPass, Language::Vec3>;
  using Base::InterfaceVariableBase;
  static constexpr const char* name() {
    return "theColor";
  }
  static constexpr bool smooth = true;
  Variable_T& theColor = Base::var;
};

struct OutputColor : InterfaceVariableBase<OutputColor, Language::Vec4> {
  OutputColor() = delete;

  using Base = InterfaceVariableBase<OutputColor, Language::Vec4>;
  using Base::InterfaceVariableBase;
  static constexpr const char* name() {
    return "outputColor";
  }
  static constexpr bool smooth = false;
  Variable_T& outputColor = Base::var;
};

struct Transform : InterfaceVariableBase<Transform, Language::Mat4> {
  Transform() = delete;
  using Base = InterfaceVariableBase<Transform, Language::Mat4>;
  using Base::InterfaceVariableBase;
  static constexpr const char* name() {
    return "transform";
  }
  static constexpr bool smooth = false;
  Variable_T& transform = Base::var;
};

}

class BasicVertexShader : public Language::GLSLVertexGeneratorBase<
    Pack<ColorManager, VertexManager>,
    Pack<detail::Transform>,
    Pack<detail::ColorPass>> {
  using Base = Language::GLSLVertexGeneratorBase<
      Pack<ColorManager, VertexManager>,
      Pack<detail::Transform>,
      Pack<detail::ColorPass>>;
public:
  BasicVertexShader(VariableFactory&& factory) : Base(std::move(factory)) {}
private:
  void DefineMain(const VariableFactory& factory) override;
};

class BasicFragmentShader : public Language::GLSLFragmentGeneratorBase<
    Pack<detail::ColorPass>,
    Pack<>,
    Pack<detail::OutputColor>> {
  using Base = Language::GLSLFragmentGeneratorBase<
      Pack<detail::ColorPass>,
      Pack<>,
      Pack<detail::OutputColor>>;
public:
  BasicFragmentShader(VariableFactory&& factory) : Base(std::move(factory)) {}
private:
  void DefineMain(const VariableFactory& factory) override;
};

}}}} // ShapeShifter::Rendering::Shaders::Programs

#endif /* RENDERING_SHADERS_PROGRAMS_BASIC_SHADER_H */

