/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   RenderingTree.h
 * Author: bbyington
 *
 * Created on December 6, 2016, 8:22 PM
 */

#ifndef RENDERING_RENDERING_TREE_H
#define RENDERING_RENDERING_TREE_H

#include "rendering/BasePureNode.h"
#include "rendering/RootNode.h"

#include <type_traits>

namespace ShapeShifter {
namespace Rendering {

class RenderingTree final{
public:
  template <typename TreePack,
            typename UniformPack,
            typename ShaderProgram,
            typename dummy =
	    typename std::enable_if<
          std::is_base_of<Shaders::ShaderProgramBase, ShaderProgram>::value
      >::type
  >
  RenderingTree(
      // TODO clean this up
      std::shared_ptr<TypedRootNode<TreePack, UniformPack>> root,
      std::shared_ptr<ShaderProgram> program)
  : data_(std::make_unique<TypedStorage<TypedRootNode<TreePack, UniformPack>, ShaderProgram>>(root, program)) {

    // TODO Interface_t is just TreePack?
    using T1 = typename PureNode<TreePack, UniformPack>::Interface_t;
    using T2 = typename ShaderProgram::Interface_t;
    constexpr bool sub1 = detail::is_subset<T1, T2>::value();
    constexpr bool sub2 = detail::is_subset<T2, T1>::value();
    static_assert(sub1 && sub2,
                  "Incompatible shader handed in with rendering tree\n");

    glGenVertexArrays (1, &vao);
    glBindVertexArray (vao);

    auto map = program->layout_map();
    for (const auto& kv: root->buffers()) {
      auto idx = map[kv.first->name()];
      auto type = kv.first->isFloating() ? GL_FLOAT : GL_INT;
      glBindBuffer(GL_ARRAY_BUFFER, kv.second);
      glVertexAttribPointer(idx,  kv.first->ElementsPerEntry(), type, GL_FALSE, 0, NULL);
      glEnableVertexAttribArray(idx);
    }

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, root->ibo());
  }

  ~RenderingTree() {
	  glDeleteVertexArrays(1, &vao);
  }

  void Render(const Camera& camera) const {
    glBindVertexArray(vao);
    data_->Render(camera);
  }

private:
  // Private storage class hierarchy to avoid double dispatch problem that
  // occurs if we type erase both the root and shader program independently.
  struct BaseStorage {
    virtual void Render(const Camera& camera) = 0;
  };

  // Not bothering to make these classes very clamped down since they are
  // private.  Don't remove without clarifying copy/move/etc semantics
  template <class TypedRoot, class TypedShader>
  struct TypedStorage : BaseStorage {
    TypedStorage(
        std::shared_ptr<TypedRoot> root,
        std::shared_ptr<TypedShader> program)
      : root_(root), program_(program) {}

    virtual void Render(const Camera& camera) override {
      program_->UseProgram();
      // TODO extract Manipulator type from Camera
      root_->RenderTree(camera, *program_);
    }

    std::shared_ptr<TypedRoot> root_;
    std::shared_ptr<TypedShader> program_;
  };

  std::unique_ptr<BaseStorage> data_;
  GLuint vao = 0;
};

}} // ShapeShifter::Rendering

#endif /* RENDERING_RENDERING_TREE_H */

