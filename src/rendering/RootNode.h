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
#include "rendering/shaders/ShaderProgram.h"

#include <map>

namespace ShapeShifter {
namespace Rendering {

/*
 * This node forms the root of a tree, and a tree is not valid until it is
 * finalized by the addition of a root node.  It type erases the internal tree
 * so that external code can hold this object without knowing what buffer types
 * the tree requires.  Upon creation of this object it will allocate and fill
 * all the buffers required by the shader program, and upload them to the gpu.
 */
class RootNode : public TypedRenderNode<> {
public:
  template <typename Other, typename dummy =
	    typename std::enable_if<
          std::is_base_of<RenderNode, Other>::value
      >::type
  >
  RootNode(
      std::unique_ptr<Other> tree,
      std::shared_ptr<Shaders::ShaderProgram> program)
    : program_(program) {
    children.emplace_back(tree.release());
    managers_ = program->BufferMapping<typename Other::Interface_t>();
    UpdateData();
  }

  virtual ~RootNode() { CleanupBuffer(); }

	/**
	 * Walks down the tree, applies rotation matrices, and calls opengl to render
   */
	void RenderTree(const Camera& camera) const;

private:
	virtual Data::BufferIndex ExclusiveNodeDataCount() const { return Data::BufferIndex(); }
  virtual void DrawSelf() const override {}
  void CleanupBuffer();


	/**
	 * Generates the actual VAO for this tree.  This function is called once
   * upon construction of the object.
   */
	void UpdateData();

  GLuint vao = 0;
  GLuint ibo = 0;
  std::shared_ptr<Shaders::ShaderProgram> program_;
  std::vector<std::shared_ptr<Data::AbstractManager>> managers_;
};

}} // ShapeShifter::Rendering

#endif /* RENDERING_ROOT_NODE_H */

