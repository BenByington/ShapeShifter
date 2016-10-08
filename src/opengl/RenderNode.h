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
#include "opengl/data/BufferTypes.h"
#include "opengl/data/MixedDataMap.h"

#include <opengl/gl3.h>

#include <cassert>
#include <cstdlib>
#include <memory>
#include <set>
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
  virtual ~RenderNode() {}

  void SetRotation(const math::Quaternion& rot);
  void SetTranslation(const math::Vector4& trans);

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

	// Compute how big the VAO should be
	Data::BufferIndex SubtreeCounts() const;
	// Fill the VAO with data and push to card
  void PopulateBufferData(Data::MixedDataMap& data);
	// Renders all children in the tree.
	void DrawChildren(const Camera& camera, const math::Quaternion& cumRot, const math::Vector4& cumTrans, const Shaders::ShaderProgram& shader) const;

	std::vector<std::shared_ptr<RenderNode>> children;
private:
  /**
	 * Functions that must be implemented by any concrete child implementations.
	 * These are used to figure out how much space each child needs in the VAO
	 * (3x number of vertices) and to actually populate the data and render
	 * your own vertices.
   */
  virtual Data::BufferIndex ExclusiveNodeDataCount() const = 0;
  virtual void FillIndexData(Data::VectorSlice<uint32_t>& data) const = 0;
  virtual void FillTextureData(Data::VectorSlice<float>& data) const = 0;
	virtual void FillColorData(Data::VectorSlice<float>& data) const = 0;
	virtual void FillVertexData(Data::VectorSlice<float>& data) const = 0;
	virtual void DrawSelf() const = 0;

  void DebugRotation(const math::Matrix4& mat) const;

  Data::BufferIndex start_;
  Data::BufferIndex end_;

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

using Opengl::Data::SupportedBufferFlags;

template <bool Enabled> struct TextureInterface : public RenderNode {};
template <>
class TextureInterface<false> : public RenderNode {
  virtual void FillTextureData(Data::VectorSlice<float>& data) const override {}
};
template <size_t Flags> using TextureNode = TextureInterface<Flags & SupportedBufferFlags::TEXTURES>;

template <size_t Flags, bool enabled> struct ColorInterface : public TextureNode<Flags> {};
template <size_t Flags>
class ColorInterface<Flags, false> : public TextureNode<Flags> {
	virtual void FillColorData(Data::VectorSlice<float>& data) const override {}
};
template <size_t Flags> using ColorNode = ColorInterface<Flags, Flags & SupportedBufferFlags::COLORS>;

template <size_t Flags, bool enabled> struct IndexInterface : public ColorNode<Flags> {};
template <size_t Flags>
class IndexInterface<Flags, false> : public ColorNode<Flags> {
	virtual void FillIndexData(Data::VectorSlice<uint32_t>& data) const override {}
};
template <size_t Flags> using IndexNode = IndexInterface<Flags, Flags & SupportedBufferFlags::INDICES>;

}

template <size_t Flags>
class TypedRenderNode : public detail::IndexNode<Flags> {

protected:
  using SupportedBufferFlags = Data::SupportedBufferFlags;
  using SupportedBuffers = Data::SupportedBuffers;
  static_assert(Flags < SupportedBufferFlags::END_VALUE, "Invalid flags for buffer support");

public:
  constexpr static const size_t Flags_t = Flags;
  TypedRenderNode() {}
  virtual ~TypedRenderNode() {}

	/**
	 * Adds a child to this node.
	 * Note: These are intentionally shared, and external code may keep a
	 *       reference to do things like tweak the rotation matrix.  Tweaking the
	 *       actual vertex count or absolute position will not be supported.
	 *
   * @param child subtree to add to this node.
   */
  template <typename Other>
  std::shared_ptr<RenderNode> AddChild(
      std::unique_ptr<Other> child,
	    typename std::enable_if<
          //TODO cleanup and hide away this logic
          (Other::Flags_t & Flags) == Flags
          && std::is_base_of<TypedRenderNode<Other::Flags_t>, Other>::value
          && (Other::Flags_t & SupportedBufferFlags::INDICES) == (Flags & SupportedBufferFlags::INDICES)
      >::type* dummy = 0
      ) {
    this->children.emplace_back(child.release());
    return this->children.back();

  }
};

/**
 * Basic implementation for nodes that only hold other nodes
 */
template <size_t Flags>
class PureNode : public TypedRenderNode<Flags> {
public:
	PureNode() = default;
	virtual ~PureNode() {}
private:
  // TODO fix this.  Had to remove 'override' keyword because we can't tell
  // up front which functions need to be supported.
	virtual Data::BufferIndex ExclusiveNodeDataCount() const { return Data::BufferIndex(); }
	virtual void FillVertexData(Data::VectorSlice<float>& data) const {};
	virtual void FillColorData(Data::VectorSlice<float>& data) const {};
	virtual void FillIndexData(Data::VectorSlice<uint32_t>& data) const {};
  virtual void DrawSelf() const {}
};

class RootNode : private TypedRenderNode<0> {
public:
  template <typename Other, typename dummy =
	    typename std::enable_if<
          std::is_base_of<TypedRenderNode<Other::Flags_t>, Other>::value
      >::type
  >
  RootNode(
      std::unique_ptr<Other> tree,
      std::shared_ptr<Shaders::ShaderProgram> program)
    : program_(program) {
    children.emplace_back(tree.release());
    idx_map = program->BufferMapping<Other::Flags_t>();
    UpdateData();
  }

  void SetRotation(const math::Quaternion& rot) {
    children.back()->SetRotation(rot);
  }
  void SetTranslation(const math::Vector4& trans) {
    children.back()->SetTranslation(trans);
  }

  virtual ~RootNode() { CleanupBuffer(); }

	/**
	 * Walks down the tree, applies rotation matrices, and calls opengl to render
   */
	void RenderTree(const Camera& camera) const;

private:
	virtual void FillVertexData(Data::VectorSlice<float>& data) const override {}
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
  std::map<SupportedBuffers, size_t> idx_map;
};

}} // ShapeShifter::Opengl

#endif /* RENDERNODE_H */
