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

  void Combine(const BaseUniformManager& other) {
    auto worker = {(Manager<Uniforms>::Combine(dynamic_cast<const Manager<Uniforms>&>(other)),1)...};
    (void) worker;
  }

  const UniformManager& operator=(const BaseUniformManager& other) {
    auto worker = {(
        dynamic_cast<Manager<Uniforms>&>(*this).Clone(dynamic_cast<Manager<Uniforms>&>(other))
        ,1)...};
    (void) worker;
  }
private:
  template <class Manager>
  using Manager = typename Manager::UniformManager;
};

// Move elsewhere?
template <class... Uniforms>
class UniformInitializer : BaseUniformInitializer, Uniforms::UniformInitializer... {
private:
  template <class T>
  using Initializer = typename T::UniformInitializer;
  template <class T>
  using Manager = typename T::UniformManager;

public:
  template <class... Uniforms_>
  static UniformManager<Uniforms_...>
  InitializeUniforms(const UniformInitializer<Uniforms_...>& initializer) {
    // make sure we've got proper subsets
    // rename these damned _ differences
    UniformManager<Uniforms...> ret;
    auto worker = {(
        dynamic_cast<Manager<Uniforms>&>(ret) = dynamic_cast<const Initializer<Uniforms>&>(initializer).InitUniform()
        ,1)...};
    (void) worker;
    return ret;
  }

};

}}} /* ShapeShifter::Rendering::Shaders */

#endif /* RENDERING_SHADERS_UNIFORM_MANAGER_H */

