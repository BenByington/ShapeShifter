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
// TODO see if we can eliminate this...
#include "rendering/shaders/ShaderProgram.h"

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
// ISSUE: Allow multiple VBO's in a tree, so that a tree can have wildly
//        different shaders used in some parts (making a single set of VBO's
//        unsuitable), yet everything contained in a unified tree so that
//        hierarchical uniforms (like position) can be used across the shader
//        boundaries.  It should be constrained so that children support a
//        subset (?) of their parents uniform variables
// TODO clean up this hack of an inheritance
class RootNode : public PureNode<Pack<>, Pack<>> {
protected:
  template <typename TreePack, typename UniformPack>
  RootNode(std::unique_ptr<PureNode<TreePack, UniformPack>> tree)
  // TODO the Interface_t extracted here is just TreePack?
    : managers_(detail::manage<typename PureNode<TreePack, UniformPack>::Interface_t>::instantiate()) {

    using Manipulator_t = typename PureNode<TreePack, UniformPack>::Manipulator_t;
    subtrees_.emplace_back(std::make_shared<Manipulator_t>(), tree.release());
    UpdateData();
  }

public:

  virtual ~RootNode();

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

template <class TreePack, class UniformPack>
class TypedRootNode final : public RootNode {
public:
  TypedRootNode(std::unique_ptr<PureNode<TreePack, UniformPack>> tree) : RootNode(std::move(tree)) {}

	/**
	 * Walks down the tree, applies rotation matrices, and calls opengl to render
   */
  // TODO make sure this template is valid compared to our TreePack and UniformPack
  template <class IPack, class... Uniforms>
	void RenderTree(
      const Camera& camera,
      const Shaders::ShaderProgram<IPack, Pack<Uniforms...>>& program) const {
    Shaders::UniformManager<Uniforms...> uniforms;
    DrawChildren(camera, uniforms, program);
  }
};

template <class TreePack, class UniformPack>
auto CreateRootPtr(std::unique_ptr<PureNode<TreePack, UniformPack>> tree) {
  return std::make_shared<TypedRootNode<TreePack, UniformPack>>(std::move(tree));
}

}} // ShapeShifter::Rendering

#endif /* RENDERING_ROOT_NODE_H */

