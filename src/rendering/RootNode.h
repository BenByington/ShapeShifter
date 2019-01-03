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

namespace detail {

class GLDataLoader {
public:
  template <typename... Keys>
  GLDataLoader(Data::BufferMap<Keys...>& data) {

    assert(data.DataRemaining().vertex_ == 0);
    assert(data.DataRemaining().index_ == 0);

    auto func = [&](std::shared_ptr<Data::AbstractManager> key, auto& data){
        auto vbo = GLuint{0};
        glGenBuffers (1, &vbo);
        buffers_[key] = vbo;
        glBindBuffer (GL_ARRAY_BUFFER, vbo);
        glBufferData (GL_ARRAY_BUFFER, data, GL_STATIC_DRAW);
    };
    data.ForEachKeyVal(func);

    const auto& indices = data.indices();
    ibo_ = GLuint{0};
    glGenBuffers (1, &ibo_);
    glBindBuffer (GL_ARRAY_BUFFER, ibo_);
    glBufferData (GL_ARRAY_BUFFER, indices, GL_STATIC_DRAW);
  
  }

  ~GLDataLoader() {
    for (auto& kv : buffers_) {
      glDeleteBuffers(1, &kv.second);
    }
    if (ibo_ != 0) glDeleteBuffers(1, &ibo_);
    ibo_ = 0;
    buffers_.clear();
  }

  GLuint ibo() const { return ibo_; }

  const std::map<std::shared_ptr<Data::AbstractManager>, GLuint>&
  buffers() const { return buffers_; }
private:

  GLuint ibo_ = 0;
  std::map<std::shared_ptr<Data::AbstractManager>, GLuint> buffers_;
};

}

template <class TreePack, class UniformPack>
struct RootNode;
template <class... Tree, class... Uniforms>
struct RootNode<Pack<Tree...>, Pack<Uniforms...>> final
  : PureNode<Pack<Tree...>, Pack<Uniforms...>>
  , Shaders::UniformInitializer<Uniforms...> {
public:
  using TreePack = Pack<Tree...>;
  using UniformPack = Pack<Uniforms...>;

  RootNode(std::unique_ptr<PureNode<TreePack, UniformPack>> tree) {

    this->AddChild(std::move(tree));
    this->FinalizeTree();

    auto size = this->SubtreeCounts();
    Data::BufferMap<Tree...> data(size);

    this->PopulateBufferData(data);
    loader_ = std::make_unique<detail::GLDataLoader>(data);
  }

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
    this->DrawChildren(camera, uniforms, program);
  }

  const std::map<std::shared_ptr<Data::AbstractManager>, GLuint>&
  buffers() const { return loader_->buffers(); }

  GLuint ibo() const { return loader_->ibo(); }

private:
  std::unique_ptr<detail::GLDataLoader> loader_;
};

template <class TreePack, class UniformPack>
auto CreateRootPtr(std::unique_ptr<PureNode<TreePack, UniformPack>> tree) {
  return std::make_shared<RootNode<TreePack, UniformPack>>(std::move(tree));
}

}} // ShapeShifter::Rendering

#endif /* RENDERING_ROOT_NODE_H */

