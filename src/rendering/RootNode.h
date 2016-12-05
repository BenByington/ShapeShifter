/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   RootNode.h
 * Author: bbyington
 *
 * Created on October 13, 2016, 4:02 PM
 */

#ifndef RENDERING_ROOT_NODE_H
#define RENDERING_ROOT_NODE_H

#include "rendering/TypedRenderNode.h"
#include "rendering/PureNode.h"
#include "rendering/shaders/ShaderProgramBase.h"

#include <map>

namespace ShapeShifter {
namespace Rendering {

// TODO remove this
namespace detail {

  template <typename T>
  struct manage;

  template <typename... Ts>
  struct manage<Pack<Ts...>> {
    static std::vector<std::shared_ptr<Data::AbstractManager>> instantiate() {
      return {std::make_shared<Ts>()...};
    }
  };

}

/*
 * This node forms the root of a tree, and a tree is not valid until it is
 * finalized by the addition of a root node.  It type erases the internal tree
 * so that external code can hold this object without knowing what buffer types
 * the tree requires.  Upon creation of this object it will allocate and fill
 * all the opengl data buffers, and will manage the life cycle of the vbo's
 * and ibo.
 */
class RootNode : private PureNode<> {
protected:
  template <typename TreeNode, typename dummy =
	          typename std::enable_if<
                std::is_base_of<RenderNode, TreeNode>::value
            >::type>
  RootNode(std::unique_ptr<TreeNode> tree)
    : managers_(detail::manage<typename TreeNode::Interface_t>::instantiate()) {

    children.emplace_back(tree.release());
    UpdateData();
  }

public:

  virtual ~RootNode();

	/**
	 * Walks down the tree, applies rotation matrices, and calls opengl to render
   */
	void RenderTree(
      const Camera& camera,
      const Shaders::ShaderProgramBase& program) const;

  // TODO clean this up?  I don't know if these accessors are required.
  GLuint ibo() { return ibo_; }

  const std::map<std::shared_ptr<Data::AbstractManager>, GLuint>&
  buffers() { return buffers_; }

private:

	/**
	 * Generates the actual VBO's for this tree.  This function is called once
   * upon construction of the object.
   */
	void UpdateData();

  GLuint ibo_ = 0;
  std::map<std::shared_ptr<Data::AbstractManager>, GLuint> buffers_;
  std::vector<std::shared_ptr<Data::AbstractManager>> managers_;
};

template <class TreeNode>
class TypedRootNode final : public RootNode {
  static_assert(std::is_base_of<Rendering::RenderNode, TreeNode>::value,
      "TypedRootNode must be templated on a type of TypedRenderNode");
public:
  TypedRootNode(std::unique_ptr<TreeNode> tree) : RootNode(std::move(tree)) {}
};

template <class TreeNode>
auto CreateRootPtr(std::unique_ptr<TreeNode> tree) {
  return std::make_shared<TypedRootNode<TreeNode>>(std::move(tree));
}

}} // ShapeShifter::Rendering

#endif /* RENDERING_ROOT_NODE_H */

