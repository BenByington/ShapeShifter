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

#include "rendering/PureNode.h"
#include "rendering/PureNode.h"
#include "rendering/shaders/ShaderProgramBase.h"

#include <map>

namespace ShapeShifter {
namespace Rendering {

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
//        Note: When doing this, try and clean up this odd inheritance.  If
//        RootNode needs access to some of PureNodes methods, but not the
//        template parameters, maybe the class needs to be split
class RootNode : public PureNode<Pack<>, Pack<>> {
protected:
  template <typename TreePack, typename UniformPack>
  RootNode(std::unique_ptr<PureNode<TreePack, UniformPack>> tree)
    : managers_(detail::manage<TreePack>::instantiate()) {

    subtrees_.emplace_back(std::move(tree));
    FinalizeTree();
    UpdateData();
  }

public:

  virtual ~RootNode();

  GLuint ibo() const { return ibo_; }

  const std::map<std::shared_ptr<Data::AbstractManager>, GLuint>&
  buffers() const { return buffers_; }

  bool Contains(const BasePureNode& node) const {
    return IsChild(node);
  }
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
struct TypedRootNode;
template <class... Tree, class... Uniforms>
struct TypedRootNode<Pack<Tree...>, Pack<Uniforms...>> final : public RootNode, Shaders::UniformInitializer<Uniforms...> {
public:
  using TreePack = Pack<Tree...>;
  using UniformPack = Pack<Uniforms...>;

  TypedRootNode(std::unique_ptr<PureNode<TreePack, UniformPack>> tree) : RootNode(std::move(tree)) {}

  /**
   * Walks down the tree, applies rotation matrices, and calls opengl to render
   */
  template <class IPack, class... Uniforms_>
  void RenderTree(
      const Camera& camera,
      const Shaders::ShaderProgram<IPack, Pack<Uniforms_...>>& program) const {
    static_assert(is_subset<IPack, TreePack>::value(),
        "Invalid shader requiring unsupported buffers");
    static_assert(is_subset<Pack<Uniforms_...>, UniformPack>::value(),
        "Invalid shader requiring unsupported uniforms");
    const auto& uniforms = Shaders::UniformInitializer<Uniforms_...>::InitializeUniforms(*this);
    DrawChildren(camera, uniforms, program);
  }
};

template <class TreePack, class UniformPack>
auto CreateRootPtr(std::unique_ptr<PureNode<TreePack, UniformPack>> tree) {
  return std::make_shared<TypedRootNode<TreePack, UniformPack>>(std::move(tree));
}

}} // ShapeShifter::Rendering

#endif /* RENDERING_ROOT_NODE_H */

