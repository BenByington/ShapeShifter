/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   TypedRenderNode.h
 * Author: bbyington
 *
 * Created on October 13, 2016, 3:52 PM
 */

#ifndef RENDERING_TYPED_RENDERNODE_H
#define RENDERING_TYPED_RENDERNODE_H

#include "data/MixedDataMapBase.h"
#include "rendering/RenderNode.h"
#include "rendering/shaders/Pack.h"

namespace ShapeShifter {
namespace Rendering {

namespace detail {

// Sees if class A exists in the variadic pack B.
template <class A, class B> struct subset_helper;
template <class A, class... B>
struct subset_helper<A, Pack<B...>> {
  static constexpr bool value() {
    const std::array<bool, sizeof...(B)> check = { std::is_same<A,B>::value... };
    auto ret = false;
    for (size_t i = 0; i < sizeof...(B); ++i) {
      ret |= check[i];
    }
    return ret;
  }
};

// Sees if all of the classes in the pack A exist in the pack B
template <class A, class B> struct is_subset;
template <class... A, class... B>
struct is_subset<Pack<A...>, Pack<B...>> {
  static constexpr bool value() {
    using other = Pack<B...>;
    const std::array<bool, sizeof...(A)> check = {subset_helper<A, other>::value()... };
    auto ret = true;
    for (size_t i = 0; i < sizeof...(A); ++i) {
      ret &= check[i];
    }
    return ret;
  }
};

}

template <class... Managers>
struct LeafNode : BaseLeafNode, Managers::Interface... {
  LeafNode() {}
  virtual ~LeafNode() {}

  // TODO rename?
  using Interface_t = Pack<Managers...>;
};

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
// TODO move to proper file.
template <class Interface, class Uniforms>
struct PureNode;
template <class... Interface, class... Uniforms>
struct PureNode<Pack<Interface...>,Pack<Uniforms...>> : BasePureNode {
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
  template <typename Other>
  std::shared_ptr<Manipulator> AddChild(
      std::unique_ptr<Other> child) {
    // TODO allow permuted types?
    static_assert(
        std::is_same<Interface_t, typename Other::Interface_t>::value,
        "Internal nodes must all have the same interface");
    auto manipulator = std::make_shared<Manipulator>();
    this->subtrees_.emplace_back(manipulator, child.release());
    return manipulator;

  }

  template <class Child, typename... Args>
  std::shared_ptr<Manipulator> AddLeaf(Args&&... args) {
    static_assert(
        detail::is_subset<Interface_t, typename Child::Interface_t>::value(),
        "Attempting to add leaf node that does not fulfill the input interface"
        " of the parent node");
    auto child = std::make_shared<Child>(std::forward<Args>(args)...);
    auto manipulator = std::make_shared<Manipulator>();
    this->leaves_.emplace_back(manipulator, child);
    return manipulator;
  }
};

}} // ShapeShifter::Rendering

#endif /* RENDERING_TYPED_RENDERNODE_H */

