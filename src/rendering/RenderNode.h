/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   RenderNode.h
 * Author: bbyington
 *
 * Created on August 2, 2016, 7:53 AM
 */

#ifndef RENDERING_RENDERNODE_H
#define RENDERING_RENDERNODE_H

#include "data/MixedDataMap.h"
#include "math/Quaternion.h"
#include "math/Vector4.h"
#include "rendering/Camera.h"

#include <opengl/gl3.h>

namespace ShapeShifter {
namespace Rendering {

namespace Shaders {
// forward declare
class ShaderProgramBase;
}

/**
 * Basic vertex for a tree of objects to render.  The entire tree will share
 * a vertex array object, though each node can have it's own rotation relative
 * to it's parent
 *
 * Note: Implementation classes should inherit from TypedRenderNode rather
 * than this class.
 */
class RenderNode {
public:
  virtual ~RenderNode() {}

  void SetRotation(const Math::Quaternion& rot);
  void SetTranslation(const Math::Vector4& trans);

protected:
  RenderNode() : translation_(0.0f, 0.0f, 0.0f, 1.0f) {}
  RenderNode(const RenderNode& orig) = delete;
	RenderNode& operator=(RenderNode&) = delete;

	// Functions for child classes to figure out what indices in the VAO they
	// should be modifying.
	Data::BufferIndex start() const {return start_; }
	Data::BufferIndex end() const {return end_; }
  GLvoid* StartIndexAsVP() const {
    return (GLvoid*)(start().index_*sizeof(uint32_t));
  }

	// Compute how big the VAO should be
	Data::BufferIndex SubtreeCounts() const;

	// Fill the VAO with data and push to card
  void PopulateBufferData(Data::MixedDataMap& data);

	// Renders all children in the tree.
	void DrawChildren(
      const Camera& camera,
      const Math::Quaternion& cumRot,
      const Math::Vector4& cumTrans,
      const Shaders::ShaderProgramBase& shader) const;

	std::vector<std::shared_ptr<RenderNode>> children;

private:
  // Size required in the buffers, not counting children nodes
  virtual Data::BufferIndex ExclusiveNodeDataCount() const = 0;

  // Populates data for the index vector.  Children can implement this as
  // a noop if they are drawing independent triangles that do not share
  // vertices
	virtual void FillIndexData(Data::VectorSlice<uint32_t>& data) const = 0;

  // Personal rendering function
	virtual void DrawSelf() const = 0;

  void DebugRotation(const Math::Matrix4& mat) const;

  // Positions of personal data in the buffers.  Not valid until tree is
  // finalized by creating a RootNode.
  Data::BufferIndex start_;
  Data::BufferIndex end_;

  Math::Quaternion rotation_;
  Math::Vector4 translation_;
};

}} // ShapeShifter::Rendering

#endif /* RENDERING_RENDERNODE_H */
