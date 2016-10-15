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
// TODO clean up cycle?
class ShaderProgram;
}

/**
 * Basic vertex for a tree of objects to render.  The entire tree will share
 * a vertex array object, though each node can have it's own rotation relative
 * to it's parent
 */
class RenderNode {
public:
  virtual ~RenderNode() {}

  void SetRotation(const Math::Quaternion& rot);
  void SetTranslation(const Math::Vector4& trans);

protected:
	// Prevent any duplication so we can easier avoid conflicts over opengl
	// resources.
  RenderNode() : translation_(0.0f, 0.0f, 0.0f, 1.0f) {}
  RenderNode(const RenderNode& orig) = delete;
	RenderNode& operator=(RenderNode&) = delete;

	// Functions for child classes to figure out what indices in the VAO they
	// should be modifying.  Only guaranteed to be valid during calls to
	// FillVertexData and FillColorData.
	Data::BufferIndex start() const {return start_; }
	Data::BufferIndex end() const {return end_; }
  GLvoid* StartIndexAsVP() const {
    //TODO fix hardcode
    return (GLvoid*)(start().triangle_*3*sizeof(uint32_t));
  }

	// Compute how big the VAO should be
	Data::BufferIndex SubtreeCounts() const;
	// Fill the VAO with data and push to card
  void PopulateBufferData(Data::MixedDataMap& data);
	// Renders all children in the tree.
	void DrawChildren(const Camera& camera, const Math::Quaternion& cumRot, const Math::Vector4& cumTrans, const Shaders::ShaderProgram& shader) const;

	std::vector<std::shared_ptr<RenderNode>> children;
private:
  /**
	 * Functions that must be implemented by any concrete child implementations.
	 * These are used to figure out how much space each child needs in the VAO
	 * (3x number of vertices) and to actually populate the data and render
	 * your own vertices.
   */
  virtual Data::BufferIndex ExclusiveNodeDataCount() const = 0;
	virtual void DrawSelf() const = 0;

  void DebugRotation(const Math::Matrix4& mat) const;

  Data::BufferIndex start_;
  Data::BufferIndex end_;

  Math::Quaternion rotation_;
  Math::Vector4 translation_;
  size_t type_;
};

}} // ShapeShifter::Rendering

#endif /* RENDERING_RENDERNODE_H */
