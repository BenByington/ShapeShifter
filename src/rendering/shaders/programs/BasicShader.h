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
  static constexpr const char* name() {
    return "theColor";
  }
  static constexpr bool smooth = true;
};
}

class BasicVertexShader : public GLSLGeneratorBase<
    pack<ColorManager::Variable, VertexManager::Variable>,
    pack<detail::ColorPass>> {
  using Base = GLSLGeneratorBase<pack<ColorManager::Variable, VertexManager::Variable>, pack<detail::ColorPass>>;
public:
  BasicVertexShader(VariableFactory&& factory) : Base(std::move(factory)) {}
};

class BasicFragmentShader : public GLSLGeneratorBase<
    pack<detail::ColorPass>,
    pack<>> {
  using Base = GLSLGeneratorBase<pack<detail::ColorPass>, pack<>>;
public:
  BasicFragmentShader(VariableFactory&& factory) : Base(std::move(factory)) {}

};

}}}} // ShapeShifter::Rendering::Shaders::Programs

#endif /* RENDERING_SHADERS_PROGRAMS_BASIC_SHADER_H */

