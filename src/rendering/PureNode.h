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

#include "rendering/PureNode.h"
#include "data/MixedDataMapBase.h"
#include "rendering/BasePureNode.h"
#include "rendering/shaders/Pack.h"
#include "rendering/shaders/UniformManager.h"


namespace ShapeShifter {
namespace Rendering {

/*
 * This is the main base class for all custom concrete RenderNode
 * implementations, though it should not be instantiated directly but rather
 * through the convenience aliases provided outside the detail namespace.
 *
 * Both Types and Managers are variadic packs describing what buffers (e.g.
 * vertices/colores/textures/etc) are to be supported by this node.  'Types'
 * are used to determine what combinations of nodes are allowed in a valid tree
 * (children node must support at least as many types as parent nodes.  They can
 * support more, but the additional ones will never be utilized).  'Managers'
 * are used to determine what virtual interface child classes will have to
 * implement.  In normal use cases 'Types' will be the same as 'Managers', except
 * in the special case of a PureNode which has an empty variadic pack for
 * Managers.  This complication is why this class is tucked away in a 'detail'
 * namespace, and external code should use the TypedRenderNode alias defined
 * below.
 */
template <class Interface, class Uniforms>
struct PureNode;
template <class... Interface, class... Uniforms>
struct PureNode<Pack<Interface...>,Pack<Uniforms...>> : BasePureNode, Shaders::UniformManager<Uniforms...> {
  PureNode() {}
  virtual ~PureNode() {}

	/**
	 * Adds a child to this node.
	 * Note: These are intentionally shared, and external code may keep a
	 *       reference to do things like tweak the rotation matrix.  Tweaking the
	 *       actual vertex count or absolute position will not be supported.
	 *
   * @param child subtree to add to this node.
   */
  using Interface_t = Pack<Interface...>;
  using Uniform_t = Pack<Uniforms...>;
  using Manipulator_t = Shaders::UniformManager<Uniforms...>;
  template <typename Other>
  CallableReferenceWrapper<Manipulator_t> AddChild(
      std::unique_ptr<Other> child) {

    static_assert(
        is_permutation<Interface_t, typename Other::Interface_t>::value,
        "Internal nodes must all have the same interface");
    static_assert(
        is_permutation<Uniform_t, typename Other::Uniform_t>::value,
        "Internal nodes must all have the same uniforms");

    CallableReferenceWrapper<Manipulator_t> ret(*child);
    this->subtrees_.emplace_back(std::move(child));
    return ret;

  }

  template <class Leaf, typename... Args>
  CallableReferenceWrapper<Manipulator_t> AddLeaf(Args&&... args) {
    static_assert(
        is_subset<Interface_t, typename Leaf::Interface_t>::value(),
        "Attempting to add leaf node that does not fulfill the input interface"
        " of the parent node");

    auto child = std::make_unique<PureNode>();
    auto leaf = std::make_unique<Leaf>(std::forward<Args>(args)...);
    child->leaf_ = std::move(leaf);
    CallableReferenceWrapper<Manipulator_t> ret(*child);
    this->subtrees_.emplace_back(std::move(child));
    return ret;
  }
};

/*
 * Helper function, to avoid typing out explicit long type names. As
 * long as you have a ShaderProgram laying around, you can easily retrieve
 * a pure node that will be compatible with it.
 */
template <class... ShaderProgram>
decltype(auto) CompatiblePureNode(const ShaderProgram&...) {
  using IType = typename set_union<typename ShaderProgram::Interface_t...>::type;
  using UType = typename set_union<typename ShaderProgram::Uniform_t...>::type;
  using Type = PureNode<IType, UType>;
  return std::make_unique<Type>();
}

}} //namespace Shapeshifter::Rendering

#endif /* RENDERING_PURE_NODE_H */
