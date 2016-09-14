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
#include "opengl/shaders/ShaderProgram.h"
#include "opengl/Camera.h"

#include <opengl/gl3.h>

#include <cstdlib>
#include <memory>
#include <set>
#include <vector>

namespace ShapeShifter {
namespace Opengl {

namespace SupportedBuffers {

static const size_t COLORS = 1;
static const size_t TEXTURES = 2;
static const size_t INDICES = 4;
static const size_t END_VALUE = 8;

}

/**
 * Basic vertex for a tree of objects to render.  The entire tree will share
 * a vertex array object, though each node can have it's own rotation relative
 * to it's parent
 */
class RenderNode {
public:
  virtual ~RenderNode() {}

  void SetRotation(const math::Quaternion& rot);
  void SetTranslation(const math::Vector4& trans);

  virtual void FillTextureData(std::vector<float>& rawData, size_t start) const = 0;
	virtual void FillColorData(std::vector<float>& rawData, size_t start) const = 0;
	virtual void FillVertexData(std::vector<float>& rawData, size_t start) const = 0;
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

protected:
	// Compute how big the VAO should be
	size_t BufferSizeRequired() const;
	// Fill the VAO with data and push to card
  size_t PopulateBufferData(std::vector<float>& vert, std::vector<float>& color, size_t start) ;
	// Renders all children in the tree.
	// TODO see how framerate is affected by the number/size of each child
	void DrawChildren(const Camera& camera, const math::Quaternion& cumRot, const math::Vector4& cumTrans, const Shaders::ShaderProgram& shader) const;

	std::vector<std::shared_ptr<RenderNode>> children;

private:
  /**
	 * Functions that must be implemented by any concrete child implementations.
	 * These are used to figure out how much space each child needs in the VAO
	 * (3x number of vertices) and to actually populate the data and render
	 * your own vertices.
   */
	virtual size_t ExclusiveBufferSizeRequired() const = 0;
	virtual void DrawSelf() const = 0;

  void DebugRotation(const math::Matrix4& mat) const;

	// TODO this is currently inconsistent.  I'd prefer the code to think in terms
	//      of vertices, but right now these actually record number of floats in
	//      the buffer (3x vertices);
  size_t start_vertex_ = 0;
	size_t end_vertex_ = 0;

  math::Quaternion rotation_;
  math::Vector4 translation_;
  size_t type_;
};
/*
 * Sets up a configurable interface.  Depending on what value is handed in for
 * the Flags template parameter, extending from the BaseNode class below will
 * require children to implement different combinations of abstract virtual
 * functions.  Valid values of 'Flags' are determined by using the above
 * definitions in namespace SupportedBuffers as bitflags.
 */
namespace detail {

template <bool Enabled> struct TextureInterface : public RenderNode {};
template <>
struct TextureInterface<false> : public RenderNode {
  virtual void FillTextureData(std::vector<float>& rawData, size_t start) const override {}
};
template <size_t Flags> using TextureNode = TextureInterface<Flags & SupportedBuffers::TEXTURES>;

template <size_t Flags, bool enabled> struct ColorInterface : public TextureNode<Flags> {};
template <size_t Flags>
struct ColorInterface<Flags, false> : public TextureNode<Flags> {
public:
	virtual void FillColorData(std::vector<float>& rawData, size_t start) const override {}
};
template <size_t Flags> using ColorNode = ColorInterface<Flags, Flags & SupportedBuffers::COLORS>;

template <size_t Flags> struct SubsetInterface : public ColorNode<Flags> {};
template <>
struct SubsetInterface<0> : public ColorNode<0> {
	virtual void FillVertexData(std::vector<float>& rawData, size_t start) const override {}
private:
	virtual size_t ExclusiveBufferSizeRequired() const override { return 0; }
  virtual void DrawSelf() const override {}
};

}

template <size_t Flags>
class TypedRenderNode : public detail::SubsetInterface<Flags> {
public:
  TypedRenderNode() = default;
  virtual ~TypedRenderNode() {}

	/**
	 * Adds a child to this node.
	 * TODO actually make that an error
	 * Note: These are intentionally shared, and external code may keep a
	 *       reference to do things like tweak the rotation matrix.  Tweaking the
	 *       actual vertex count or absolute position will not be supported.
	 *
   * @param child subtree to add to this node.
   */
	void AddChild(std::shared_ptr<RenderNode> child);
};

/**
 * Basic implementation for nodes that only hold other nodes
 */
template <size_t Flags>
class PureNode : public TypedRenderNode<Flags> {
public:
	PureNode() = default;
	virtual ~PureNode() {}
protected:
  // TODO fix this.  Had to remove 'override' keyword because we can't tell
  // up front which functions need to be supported.
	virtual size_t ExclusiveBufferSizeRequired() const { return 0; }
	virtual void FillVertexData(std::vector<float>& rawData, size_t start) const {};
	virtual void FillColorData(std::vector<float>& rawData, size_t start) const {};
  virtual void DrawSelf() const {}
};

class RootNode : public TypedRenderNode<0> {
public:
  RootNode() = default;
  virtual ~RootNode() { CleanupBuffer(); }

	/**
	 * Generates the actual VAO for this tree.  This function can be called
	 * multiple times (perhaps you've added more children to the tree since the
	 * last call)
   */
	void UpdateData();

	/**
	 * Walks down the tree, applies rotation matrices, and calls opengl to render
   */
	void RenderTree(const Camera& camera, const Shaders::ShaderProgram& shader) const;

  template <size_t Flags>
  void AddChild(std::shared_ptr<TypedRenderNode<Flags>> tree) {
    this->children.resize(0);
    this->children.push_back(tree);
  }

private:
  void CleanupBuffer();

  GLuint vao = 0;
  size_t RenderFlags = 0;
};

}} // ShapeShifter::Opengl

#endif /* RENDERNODE_H */
