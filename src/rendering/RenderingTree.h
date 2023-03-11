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

#include "rendering/PureNode.h"
#include "rendering/RootNode.h"

#include <concepts>
#include <type_traits>

namespace ShapeShifter::Rendering {

class RenderingTree final {
public:
  template <typename TreePack,
            typename UniformPack,
            std::derived_from<Shaders::ShaderProgramBase> ShaderProgram>
    requires PackSubset<typename ShaderProgram::Interface_t, TreePack> &&
             PackSubset<typename ShaderProgram::Uniform_t, UniformPack>
  RenderingTree(std::shared_ptr<RootNode<TreePack, UniformPack>> root,
                std::shared_ptr<ShaderProgram> program)
      : data_(std::make_unique<TypedStorage<RootNode<TreePack, UniformPack>, ShaderProgram>>(
            root, program)) {
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    auto map = program->layout_map();
    for (const auto& kv : root->buffers()) {
      auto idx = map[kv.first->name()];
      auto type = kv.first->isFloating() ? GL_FLOAT : GL_INT;
      glBindBuffer(GL_ARRAY_BUFFER, kv.second);
      glVertexAttribPointer(idx, kv.first->ElementsPerEntry(), type, GL_FALSE, 0, NULL);
      glEnableVertexAttribArray(idx);
    }

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, root->ibo());
  }

  ~RenderingTree() { glDeleteVertexArrays(1, &vao); }

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
    TypedStorage(std::shared_ptr<TypedRoot> root, std::shared_ptr<TypedShader> program)
        : root_(root)
        , program_(program) {}

    virtual void Render(const Camera& camera) override {
      program_->UseProgram();
      root_->RenderTree(camera, *program_);
    }

    std::shared_ptr<TypedRoot> root_;
    std::shared_ptr<TypedShader> program_;
  };

  std::unique_ptr<BaseStorage> data_;
  GLuint vao = 0;
};

} // namespace ShapeShifter::Rendering

#endif /* RENDERING_RENDERING_TREE_H */
