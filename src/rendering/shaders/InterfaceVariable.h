/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   InterfaceVariableBase.h
 * Author: bbyington
 *
 * Created on October 24, 2016, 8:18 AM
 */

#ifndef RENDERING_SHADERS_INTERFACE_VARIABLE_H
#define RENDERING_SHADERS_INTERFACE_VARIABLE_H

#include "rendering/shaders/VariableFactory.h"
#include "rendering/shaders/language/Types.h"
#include "rendering/shaders/language/Variable.h"

#include <concepts>

namespace ShapeShifter::Rendering::Shaders {

/*
 * Base class used in the framework defining the interface for shader
 * stages.  Classes that satisfy this conecpt will be used to define
 * what Variables the different shader stages use for input and
 * output, allowing compile time constraints for keeping everything
 * consistent.
 */
template <typename Var>
concept InterfaceVariable =
    requires(Var* t) {
      // Defines which GLSL type wrapper will be used in C++ code
      // (e.g. vec3).  Var::Type is actually supplied by the Variable
      // base class we must be derived from
      typename Var::Type;
      requires Language::InterfaceType<typename Var::Type>;
      requires std::derived_from<Var, Language::Variable<typename Var::Type>>;

      // Provides the string name to use in the generated GLSL program
      { Var::name() } -> std::same_as<const char*>;
      // Defines if the variable is smoothed or not
      { Var::smooth } -> std::convertible_to<bool>;
    };

} // namespace ShapeShifter::Rendering::Shaders

#endif /* RENDERING_SHADERS_INTERFACE_VARIABLE_H */
