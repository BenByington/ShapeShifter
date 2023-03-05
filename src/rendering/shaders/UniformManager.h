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

#include "rendering/Camera.h"
#include "rendering/shaders/BaseUniformManager.h"
#include "rendering/shaders/ShaderProgramBase.h"

namespace ShapeShifter::Rendering::Shaders {

template <class... Uniforms>
struct UniformManager : BaseUniformManager, Uniforms::UniformManager... {
  UniformManager() {}

  void Combine(const UniformManager& other) {
    auto worker = {(Manager<Uniforms>::Combine(other), 1)...};
    (void)worker;
  }

  const UniformManager& operator=(const UniformManager& other) {
    auto worker = {(Manager<Uniforms>::Clone(other), 1)...};
    (void)worker;
  }

private:
  template <class Manage>
  using Manager = typename Manage::UniformManager;
};

// Move elsewhere?
template <class... Uniforms>
struct UniformInitializer : BaseUniformInitializer, Uniforms::UniformInitializer... {
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
    auto worker = {(static_cast<Manager<Uniforms>&>(ret) =
                        static_cast<const Initializer<Uniforms>&>(initializer).InitUniform(),
                    1)...};
    (void)worker;
    return ret;
  }
};

} // namespace ShapeShifter::Rendering::Shaders

#endif /* RENDERING_SHADERS_UNIFORM_MANAGER_H */
