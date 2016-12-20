/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   PureNode.h
 * Author: bbyington
 *
 * Created on October 13, 2016, 4:09 PM
 */

#ifndef RENDERING_PURE_NODE_H
#define RENDERING_PURE_NODE_H

#include "rendering/TypedRenderNode.h"

namespace ShapeShifter {
namespace Rendering {

/*
 * Helper function, to avoid typing out explicit long type names. As
 * long as you have a ShaderProgram laying around, you can easily retrieve
 * a pure node that will be compatible with it.
 */
// TODO rename so that pure nodes are the only nodes.
// ISSUE: Make it so that we can use multiple shader programs to define the
//        interface (as a union)
template <class ShaderProgram>
decltype(auto) CompatiblePureNode(const ShaderProgram&) {
  using Type = PureNode<typename ShaderProgram::Interface_t, typename ShaderProgram::Uniform_t>;
  return std::make_unique<Type>();
}

}} //namespace Shapeshifter::Rendering

#endif /* RENDERING_PURE_NODE_H */
