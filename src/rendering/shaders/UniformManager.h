/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   UniformManager.h
 * Author: bbyington
 *
 * Created on December 20, 2016, 2:40 PM
 */

#ifndef RENDERING_SHADERS_UNIFORM_MANAGER_H
#define RENDERING_SHADERS_UNIFORM_MANAGER_H

#include "rendering/shaders/BaseUniformManager.h"
#include "rendering/shaders/ShaderProgramBase.h"
#include "rendering/Camera.h"

namespace ShapeShifter {
namespace Rendering {
namespace Shaders {

template <class... Uniforms>
struct UniformManager : BaseUniformManager, Uniforms::UniformManager... {
  UniformManager() {}

  void Combine(const UniformManager& other) {
    auto worker = {(Uniforms::UniformManager::Combine(dynamic_cast<const typename Uniforms::UniformManager&>(other)),1)...};
  }
};

}}} /* ShapeShifter::Rendering::Shaders */

#endif /* RENDERING_SHADERS_UNIFORM_MANAGER_H */

