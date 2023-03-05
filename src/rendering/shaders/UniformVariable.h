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

#ifndef RENDERING_SHADERS_UNIFORM_VARIABLE_H
#define RENDERING_SHADERS_UNIFORM_VARIABLE_H

#include "rendering/shaders/InterfaceVariable.h"
#include "rendering/Camera.h"

namespace ShapeShifter::Rendering::Shaders {

// ISSUE: Clean up camera/transform/uniform.  They are all conflated.
//        The positions and relative transforms should be pulled out
//        into the tree, since everything fundamentally has its own
//        position, and the tree adds relative paths/orientations.
//        The `transform` uniform should just be the way of feeding
//        that into the program.  Maybe it doesn't make sense, but
//        maybe some nodes will want to ignore the positions to do
//        something like a HUD, and maybe others will transform the
//        position to give something like a "stretch/zoom" effect.
//
//        Anyway, the combine/clone/etc functions probably should be
//        pulled out of the Uniform concept, and let the tree handle
//        the heirarchies
template <typename Manager>
concept UniformVarManager = requires(Manager& uniform,
                                     const Manager& cuniform,
                                     const Rendering::Camera& c){
        typename Manager::StorageType;
        requires std::default_initializable<Manager>;
        { cuniform.Data(c) } -> std::same_as<typename Manager::StorageType>;
        { uniform.Clone(cuniform) } -> std::same_as<void>;
        { uniform.Combine(cuniform) } -> std::same_as<void>;
        { uniform.CombineInverse(cuniform) } -> std::same_as<void>;
};

// GLSL program inputs such as the transform matrix to use.
template <typename Var>
concept UniformVariable = requires(Var* var) {
    requires InterfaceVariable<Var>;

    typename Var::UniformManager;
    requires UniformVarManager<typename Var::UniformManager>;

    typename Var::UniformInitializer;
    requires requires(const typename Var::UniformInitializer& initializer) {
        { initializer.InitUniform() } -> std::same_as<typename Var::UniformManager>;
    };
};

} // ShapeShifter::Rendering::Shaders

#endif /* RENDERING_SHADERS_INTERFACE_VARIABLE_H */
