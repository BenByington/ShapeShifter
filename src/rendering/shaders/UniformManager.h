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
    auto worker = {(Manager<Uniforms>::Combine(dynamic_cast<const Manager<Uniforms>&>(other)),1)...};
  }

  const UniformManager& operator=(const UniformManager& other) {
    auto worker = {(
        dynamic_cast<Manager<Uniforms>&>(*this).Clone(dynamic_cast<Manager<Uniforms>&>(other))
        ,1)...};
  }
private:
  template <class Manager>
  using Manager = typename Manager::UniformManager;
};

}}} /* ShapeShifter::Rendering::Shaders */

#endif /* RENDERING_SHADERS_UNIFORM_MANAGER_H */

