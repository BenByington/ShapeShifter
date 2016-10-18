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

namespace ShapeShifter {
namespace Rendering {

namespace detail {

// Sees if class A exists in the variadic pack B.
template <class A, class B> struct subset_helper;
template <class A, class... B>
struct subset_helper<A, std::tuple<B...>> {
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
struct is_subset<std::tuple<A...>, std::tuple<B...>> {
  static constexpr bool value() {
    using other = std::tuple<B...>;
    const std::array<bool, sizeof...(A)> check = {subset_helper<A, other>::value()... };
    auto ret = true;
    for (size_t i = 0; i < sizeof...(A); ++i) {
      ret &= check[i];
    }
    return ret;
  }
};

// Forward declare so we can do some template specialization magic.
template <class...>
struct TypedRenderNode_;

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
// TODO: Should put in a check...  Either types==Managers or Managers is empty
template <class... Types, class... Managers>
struct TypedRenderNode_<std::tuple<Types...>, Managers...> : RenderNode, Managers::Interface... {
  TypedRenderNode_() {}
  virtual ~TypedRenderNode_() {}

	/**
	 * Adds a child to this node.
	 * Note: These are intentionally shared, and external code may keep a
	 *       reference to do things like tweak the rotation matrix.  Tweaking the
	 *       actual vertex count or absolute position will not be supported.
	 *
   * @param child subtree to add to this node.
   */
  using Interface_t = std::tuple<Types...>;
  template <typename Other>
  std::shared_ptr<RenderNode> AddChild(
      std::unique_ptr<Other> child) {
    static_assert(
        detail::is_subset<Interface_t, typename Other::Interface_t>::value(),
        "Attempting to add child node that does not fulfill the full interface"
        " of the parent node");
    this->children.emplace_back(child.release());
    return this->children.back();

  }
};

}

/*
 * This is the main alias classes should inherit when creating a custom concrete
 * child of RenderNode
 */
template <class... Types>
using TypedRenderNode = detail::TypedRenderNode_<std::tuple<Types...>, Types...>;

/*
 * Used to create a 'pure' node that has the right types to be part of a
 * specific tree, but does not inherit any virtual functions to actually
 * implement the interface.  Note there already exists a PureNode class extending
 * from this, so it is unlikely any other child class need exist.
 */
template <class... Types>
using PureTypedRenderNode = detail::TypedRenderNode_<std::tuple<Types...>>;

}} // ShapeShifter::Rendering

#endif /* RENDERING_TYPED_RENDERNODE_H */

