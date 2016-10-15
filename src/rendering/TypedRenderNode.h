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

}

template <class... Managers>
struct TypedRenderNode : public RenderNode, Managers::Interface... {

  public:
  using Interface_t = std::tuple<Managers...>;
protected:

public:
  TypedRenderNode() {}
  virtual ~TypedRenderNode() {}

	/**
	 * Adds a child to this node.
	 * Note: These are intentionally shared, and external code may keep a
	 *       reference to do things like tweak the rotation matrix.  Tweaking the
	 *       actual vertex count or absolute position will not be supported.
	 *
   * @param child subtree to add to this node.
   */
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

}} // ShapeShifter::Rendering

#endif /* RENDERING_TYPED_RENDERNODE_H */

