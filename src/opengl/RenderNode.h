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

#ifndef RENDERNODE_H
#define RENDERNODE_H

//TODO change my own opengl directory to not be the same as system
#include "opengl/math/Quaternion.h"
#include "opengl/ShaderProgram.h"
#include "opengl/Camera.h"

#include <opengl/gl3.h>

#include <cstdlib>
#include <memory>
#include <vector>

namespace ShapeShifter {
namespace Opengl {

/**
 * Basic vertex for a tree of objects to render.  The entire tree will share
 * a vertex array object, though each node can have it's own rotation relative
 * to it's parent
 */
class RenderNode {
public:
  virtual ~RenderNode() { CleanupBuffer(); }

	/**
	 * Adds a child to this node.
	 * TODO actually make that an error
	 * Note: It is an error to add a child that is already established as the
	 *       root of another tree (by calling UpdateData)
	 * Note: These are intentionally shared, and external code may keep a
	 *       reference to do things like tweak the rotation matrix.  Tweaking the
	 *       actual vertex count or absolute position will not be supported.
	 *
   * @param child subtree to add to this node.
   */
	void AddChild(std::shared_ptr<RenderNode> child);

	/**
	 * Generates the actual VAO for this tree.  This should only be called on the
	 * root of the tree, and after you call this function on a node it can no
	 * longer be added as a subtree to another node.  This function can be called
	 * multiple times (perhaps you've added more children to the tree since the
	 * last call)
	 * TODO fixup so that only root can be rendered
   */
	void UpdateData();

	/**
	 * Walks down the tree, applies rotation matrices, and calls opengl to render
   */
	void RenderTree(const Camera& camera, const ShaderProgram& shader) const;

  void SetRotation(const math::Quaternion& rot);
  void SetTranslation(const math::Vector4& trans);

protected:
	// Prevent any duplication so we can easier avoid conflicts over opengl
	// resources.
  RenderNode() : translation_({0.0f, 0.0f, 0.0f, 1.0f}) {}
  RenderNode(const RenderNode& orig) = delete;
	RenderNode& operator=(RenderNode&) = delete;

	// Functions for child classes to figure out what indices in the VAO they
	// should be modifying.  Only guaranteed to be valid during calls to
	// FillVertexData and FillColorData.
	size_t start_vertex() const {return start_vertex_; }
	size_t end_vertex() const {return end_vertex_; }

private:
	// Compute how big the VAO should be
	size_t BufferSizeRequired() const;
	// Fill the VAO with data and push to card
  size_t PopulateBufferData(std::vector<float>& vert, std::vector<float>& color, size_t start) ;
	// Renders all children in the tree.
	// TODO see how framerate is affected by the number/size of each child
	void DrawChildren(const Camera& camera, const math::Quaternion& cumRot, const math::Vector4& cumTrans, const ShaderProgram& shader) const;

  void CleanupBuffer();

  /**
	 * Functions that must be implemented by any concrete child implementations.
	 * These are used to figure out how much space each child needs in the VAO
	 * (3x number of vertices) and to actually populate the data and render
	 * your own vertices.
   */
	virtual size_t ExclusiveBufferSizeRequired() const = 0;
	virtual void FillVertexData(std::vector<float>& rawData, size_t start) const = 0;
	virtual void FillColorData(std::vector<float>& rawData, size_t start) const = 0;
	virtual void DrawSelf() const = 0;

  void DebugRotation(const math::Matrix4& mat) const;

	// TODO this is currently inconsistent.  I'd prefer the code to think in terms
	//      of vertices, but right now these actually record number of floats in
	//      the buffer (3x vertices);
  size_t start_vertex_ = 0;
	size_t end_vertex_ = 0;

	// TODO need resource cleanup function
	GLuint vao = 0;
	std::vector<std::shared_ptr<RenderNode>> children;

  math::Quaternion rotation_;
  math::Vector4 translation_;
};

/**
 * Basic implementation for nodes that only hold other nodes
 */
class PureNode : public RenderNode {
public:
	PureNode() {}
	virtual ~PureNode() {}
protected:
	virtual size_t ExclusiveBufferSizeRequired() const override { return 0; }
};

/**
 * Simple test class that will draw a shaded square in the center of the screen
 */
class SquareTest2D : public RenderNode {
public:
	SquareTest2D() {}
	virtual ~SquareTest2D() {}
protected:
	virtual size_t ExclusiveBufferSizeRequired() const override;
	virtual void FillVertexData(std::vector<float>& rawData, size_t start) const override;
	virtual void FillColorData(std::vector<float>& rawData, size_t start) const override;
	virtual void DrawSelf() const override;
};

/**
 * Simple test class that will draw a triangle (lines only) in the center of
 * the screen
 */
class TriangleTest2D : public RenderNode {
public:
	TriangleTest2D() {}
	virtual ~TriangleTest2D() {}
protected:
	virtual size_t ExclusiveBufferSizeRequired() const override;
	virtual void FillVertexData(std::vector<float>& rawData, size_t start) const override;
	virtual void FillColorData(std::vector<float>& rawData, size_t start) const override;
	virtual void DrawSelf() const override;
};

}} // ShapeShifter::Opengl

#endif /* RENDERNODE_H */
