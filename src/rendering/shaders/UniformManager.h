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

#include "rendering/shaders/ShaderProgramBase.h"
#include "rendering/Camera.h"

namespace ShapeShifter {
namespace Rendering {
namespace Shaders {

namespace detail {
template <class Self, class Required>
struct required_wrapper;
template <class Self, class... Required>
struct required_wrapper<Self, Pack<Required...>> {
  template <typename T>
  static auto make(T& t) {
    return Util::MultiReferenceWrapper<Self, typename Required::UniformAccumulator...>(t);
  }
};
template <class Self>
struct required_wrapper<Self, Pack<>> {
  template <typename T>
  // TODO ugly sneaky use of int
  static int make(T& t) {return 0;}
};

}

// TODO cleanup the Required nonsense, or make it a check in UniformVariableBase
template <class... Uniforms>
struct UniformManager : Uniforms::UniformManager... {
};

template <class... Uniforms>
struct UniformAccumulator : Uniforms::UniformAccumulator... {
  template <typename T> void SiblingHelper(int) {};
  template <typename T, typename... Us>
  void SiblingHelper(const Util::MultiReferenceWrapper<Us...>& m) {
    T::UniformAccumulator::SetSiblings(m);
  }
  UniformAccumulator() {
    SetSiblings();
  }

  void Combine(const UniformManager<Uniforms...>& other) {
    auto worker = {(Accumulator<Uniforms>::Combine(other),1)...};
    (void) worker;
  }

  const UniformAccumulator& operator=(const UniformAccumulator& other) {
    auto worker = {(Accumulator<Uniforms>::Clone(other),1)...};
    (void) worker;
    SetSiblings();
  }
  UniformAccumulator(const UniformAccumulator& other) {
    auto worker = {(Accumulator<Uniforms>::Clone(other),1)...};
    (void) worker;
    SetSiblings();
  }
private:
  void SetSiblings() {
    auto worker = {(SiblingHelper<Uniforms>(
        detail::required_wrapper<typename Uniforms::UniformAccumulator,
                                 typename Uniforms::Required>::make(*this))
                    ,1)...};
    (void) worker;
  }

  template <class Manage>
  using Manager = typename Manage::UniformManager;
  template <class Manage>
  using Accumulator = typename Manage::UniformAccumulator;
};

// Move elsewhere?
template <class... Uniforms>
struct UniformInitializer : Uniforms::UniformInitializer... {
private:
  template <class T>
  using Initializer = typename T::UniformInitializer;
  template <class T>
  using Manager = typename T::UniformManager;

public:
  template <class... Uniforms_>
  static UniformAccumulator<Uniforms_...>
  InitializeUniforms(const UniformInitializer<Uniforms_...>& initializer) {
    // TODO make sure we've got proper subsets
    // rename these damned _ differences
    UniformAccumulator<Uniforms...> ret;
    auto worker = {(
        static_cast<const Initializer<Uniforms>&>(initializer).InitUniform(ret)
        ,1)...};
    (void) worker;
    return ret;
  }

};

}}} /* ShapeShifter::Rendering::Shaders */

#endif /* RENDERING_SHADERS_UNIFORM_MANAGER_H */

