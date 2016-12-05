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

#include "rendering/RenderNode.h"
#include "rendering/RootNode.h"

#include <type_traits>

namespace ShapeShifter {
namespace Rendering {

class RenderingTree final{
public:
  template <typename TreeNode,
            typename ShaderProgram,
            typename dummy =
	    typename std::enable_if<
          std::is_base_of<RenderNode, TreeNode>::value &&
          std::is_base_of<Shaders::ShaderProgramBase, ShaderProgram>::value
      >::type
  >
  RenderingTree(
      std::shared_ptr<TypedRootNode<TreeNode>> root,
      std::shared_ptr<ShaderProgram> program)
  : root_(root)
  , program_(program) {

    using T1 = typename TreeNode::Interface_t;
    using T2 = typename ShaderProgram::Interface_t;
    constexpr bool sub1 = detail::is_subset<T1, T2>::value();
    constexpr bool sub2 = detail::is_subset<T2, T1>::value();
    static_assert(sub1 && sub2,
                  "Incompatible shader handed in with rendering tree\n");

    glGenVertexArrays (1, &vao);
    glBindVertexArray (vao);

    auto map = program_->layout_map();
    for (const auto& kv: root_->buffers()) {
      auto idx = map[kv.first->name()];
      auto type = kv.first->isFloating() ? GL_FLOAT : GL_INT;
      glBindBuffer(GL_ARRAY_BUFFER, kv.second);
      glVertexAttribPointer(idx,  kv.first->ElementsPerEntry(), type, GL_FALSE, 0, NULL);
      glEnableVertexAttribArray(idx);
    }

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, root_->ibo());
  }

  ~RenderingTree() {
	  glDeleteVertexArrays(1, &vao);
  }

  void Render(const Camera& camera) const {
    glBindVertexArray(vao);
    program_->UseProgram();
    root_->RenderTree(camera, *program_);
  }

private:
  std::shared_ptr<RootNode> root_;
  std::shared_ptr<Shaders::ShaderProgramBase> program_;
  GLuint vao = 0;
};

}} // ShapeShifter::Rendering

#endif /* RENDERING_RENDERING_TREE_H */

