/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   Shader.cpp
 * Author: bbyington
 *
 * Created on July 29, 2016, 8:38 AM
 */

#include "rendering/shaders/Shader.h"
#include "data/ConcreteBufferManager.h"

namespace ShapeShifter {
namespace Rendering {
namespace Shaders {

using Data::ColorManager;
using Data::VertexManager;

class test : public GLSLGeneratorBase<
    std::tuple<ColorManager::Variable, VertexManager::Variable>,
    std::tuple<>> {
  using Base = GLSLGeneratorBase<pack<ColorManager::Variable, VertexManager::Variable>, pack<>>;
public:
  test(VariableFactory&& factory) : Base(std::move(factory)) {}
};

static auto instance = VertexShader<test>();

}}} // ShapeShifter::Rendering::Shaders
