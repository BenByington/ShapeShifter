/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   BaseUniformManager.h
 * Author: bbyington
 *
 * Created on December 20, 2016, 2:50 PM
 */

#ifndef BASE_RENDERING_SHADERS_UNIFORM_MANAGER_H
#define BASE_RENDERING_SHADERS_UNIFORM_MANAGER_H

#include "rendering/shaders/BaseUniformManager.h"

namespace ShapeShifter::Rendering::Shaders {

struct BaseUniformManager {
  virtual ~BaseUniformManager() {}
};

// Move elsewhere?
struct BaseUniformInitializer {
  virtual ~BaseUniformInitializer() {}
};

} // namespace ShapeShifter::Rendering::Shaders

#endif /* BASE_RENDERING_SHADERS_UNIFORM_MANAGER_H */
