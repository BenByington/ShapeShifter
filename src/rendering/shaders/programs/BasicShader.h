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

namespace ShapeShifter {
namespace Rendering {
namespace Shaders {
namespace Programs {

using Data::ColorManager;
using Data::VertexManager;

namespace detail {
struct ColorPass : InterfaceVariableBase<ColorPass, Vec3> {
  ColorPass() = delete;

  using Base = InterfaceVariableBase<ColorPass, Vec3>;
  using Base::InterfaceVariableBase;
  static constexpr const char* name() {
    return "theColor";
  }
  static constexpr bool smooth = true;
  Variable_T& theColor = Base::var;
};

struct Transform : InterfaceVariableBase<Transform, Mat4> {
  Transform() = delete;
  using Base = InterfaceVariableBase<Transform, Mat4>;
  using Base::InterfaceVariableBase;
  static constexpr const char* name() {
    return "transform";
  }
  static constexpr bool smooth = false;
  Variable_T& transform = Base::var;
};

}

class BasicVertexShader : public GLSLGeneratorBase<
    pack<ColorManager::Variable, VertexManager::Variable>,
    pack<detail::Transform>,
    pack<detail::ColorPass>> {
  using Base = GLSLGeneratorBase<pack<ColorManager::Variable, VertexManager::Variable>, pack<detail::Transform>, pack<detail::ColorPass>>;
public:
  BasicVertexShader(VariableFactory&& factory) : Base(std::move(factory)) {}
private:
  void DefineMain(const VariableFactory& factory) const override;
};

class BasicFragmentShader : public GLSLGeneratorBase<
    pack<detail::ColorPass>,
    pack<>,
    pack<>> {
  using Base = GLSLGeneratorBase<pack<detail::ColorPass>, pack<>, pack<>>;
public:
  BasicFragmentShader(VariableFactory&& factory) : Base(std::move(factory)) {}
private:
  void DefineMain(const VariableFactory& factory) const override;
};

}}}} // ShapeShifter::Rendering::Shaders::Programs

#endif /* RENDERING_SHADERS_PROGRAMS_BASIC_SHADER_H */

