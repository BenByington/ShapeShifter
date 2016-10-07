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
#include "opengl/BufferTypes.h"

#include <opengl/gl3.h>

#include <cassert>
#include <cstdlib>
#include <memory>
#include <set>
#include <vector>

namespace ShapeShifter {
namespace Opengl {

// TODO figure out cleaner solution
template <SupportedBuffers b>
struct traits {
  using type = float;
};
template <>
struct traits<SupportedBuffers::INDICES> {
  using type = uint32_t;
};

// Helper class to help make sure nodes don't overwrite each other's data
// TODO: Should live elsewhere?
template <typename T>
class VectorSlice final {
public:
  VectorSlice(const VectorSlice& other) = delete;
  VectorSlice(VectorSlice<T>&& other) = default;
  VectorSlice() : data_(nullptr), size_(0) {}
  VectorSlice(std::vector<T>& v, size_t start, size_t end, size_t elem_size)
    : data_(v.data() + start*elem_size)
    , size_(elem_size * (end-start)) {
    assert(v.size() > 0 || (start == 0 && end == 0));
    assert(end >= start);
    assert(start*elem_size <= v.size());
    assert(end*elem_size <= v.size());
    assert(elem_size > 0);
  }

  VectorSlice& operator=(const VectorSlice& other) = delete;
  VectorSlice& operator=(VectorSlice<T>&& other) = default;

  T& operator[](size_t idx) {
    assert(idx < size_);
    return data_[idx];
  }
  T* begin() { return  data_; }
  T* end() {return data_ + size_; }
  size_t size() const { return size_; }
private:
  T* data_;
  size_t size_;
};

// TODO move elsewhere
template <template <typename...> class Storage>
class MixedDataMapBase {
public:
  // TODO find better home?
  static const size_t floats_per_vert_ = 3;
  static const size_t floats_per_color_ = 3;
  static const size_t floats_per_text_ = 2;
  // TODO change this to be 3 and rename floats_per_tri_
  static const size_t floats_per_ind_ = 1;

  template <typename T>
  using Storage_t = Storage<T>;

  MixedDataMapBase() = default;
  MixedDataMapBase(const MixedDataMapBase&) = delete;
  MixedDataMapBase(MixedDataMapBase&&) = default;
  MixedDataMapBase& operator=(const MixedDataMapBase&) = delete;
  MixedDataMapBase& operator=(MixedDataMapBase&&) = default;

  template <SupportedBuffers buffer>
  Storage<typename traits<buffer>::type>& get() {
    keys_.insert(buffer);
    return get_dispatch<buffer>();
  }

  const std::set<SupportedBuffers>& keys() {
    return keys_;
  }

  auto FloatData() {
    std::vector<std::pair<SupportedBuffers, Storage<float>&>> ret;
    for (const auto& key : keys_) {
      switch(key) {
        case SupportedBuffers::COLORS:
          ret.emplace_back(SupportedBuffers::COLORS, colors_);
          break;
        case SupportedBuffers::TEXTURES:
          ret.emplace_back(SupportedBuffers::TEXTURES, textures_);
          break;
        case SupportedBuffers::VERTICES:
          ret.emplace_back(SupportedBuffers::VERTICES, vertices_);
          break;
        case SupportedBuffers::INDICES:
          //do nothing
          break;
      }
    }
    return ret;
  }

  auto IntegralData() {
    std::vector<std::pair<SupportedBuffers, Storage<uint32_t>&>> ret;
    for (const auto& key : keys_) {
      switch(key) {
        case SupportedBuffers::COLORS:
          //do nothing
          break;
        case SupportedBuffers::TEXTURES:
          //do nothing
          break;
        case SupportedBuffers::VERTICES:
          //do nothing
          break;
        case SupportedBuffers::INDICES:
          ret.emplace_back(SupportedBuffers::INDICES, indices_);
          //do nothing
          break;
      }
    }
    return ret;
  }
private:

  template <SupportedBuffers buffer>
  typename std::enable_if<
      std::is_same<float, typename traits<buffer>::type>::value,
      Storage<typename traits<buffer>::type>
  >::type& get_dispatch() {
    auto item = buffer;
    switch (item) {
      case SupportedBuffers::COLORS:
        return colors_;
        break;
      case SupportedBuffers::TEXTURES:
        return textures_;
        break;
      case SupportedBuffers::VERTICES:
        return vertices_;
        break;
      case SupportedBuffers::INDICES:
        assert(false);
        return vertices_;
        break;
    }
  }

  template <SupportedBuffers buffer>
  typename std::enable_if<
      std::is_same<uint32_t, typename traits<buffer>::type>::value,
      Storage<typename traits<buffer>::type>
  >::type& get_dispatch() {
    auto item = buffer;
    switch (item) {
      case SupportedBuffers::COLORS:
        assert(false);
      case SupportedBuffers::TEXTURES:
        assert(false);
      case SupportedBuffers::VERTICES:
        assert(false);
      case SupportedBuffers::INDICES:
        return indices_;
        break;
    }
  }

  std::set<SupportedBuffers> keys_;
  Storage<float> colors_;
  Storage<float> textures_;
  Storage<float> vertices_;
  Storage<uint32_t> indices_;
};

class MixedSliceMap final : public MixedDataMapBase<VectorSlice> {
public:
  MixedSliceMap(const MixedSliceMap& other) = delete;
  MixedSliceMap(MixedSliceMap&& other) = default;

  MixedSliceMap& operator=(const MixedSliceMap& other) = delete;
  MixedSliceMap& operator=(MixedSliceMap&& other) = default;

  MixedSliceMap(
      const std::vector<std::pair<SupportedBuffers, std::vector<float>&>>& float_data,
      const std::vector<std::pair<SupportedBuffers, std::vector<uint32_t>&>>& int_data,
      size_t start_vert,
      size_t end_vert,
      size_t start_idx,
      size_t end_idx)
    : start_vertex_(start_vert)
    , end_vertex_(end_vert)
    , start_index_(start_idx)
    , end_index_(end_idx) {

    for (const auto& kv : float_data) {
      switch (kv.first) {
        case SupportedBuffers::COLORS:
          get<SupportedBuffers::COLORS>() = VectorSlice<float>(kv.second, start_vert, end_vert, floats_per_color_);
          break;
        case SupportedBuffers::TEXTURES:
          get<SupportedBuffers::TEXTURES>() = VectorSlice<float>(kv.second, start_vert, end_vert, floats_per_text_);
          break;
        case SupportedBuffers::VERTICES:
          get<SupportedBuffers::VERTICES>() = VectorSlice<float>(kv.second, start_vert, end_vert, floats_per_vert_);
          break;
        case SupportedBuffers::INDICES:
          assert(false);
          break;
      }
    }

    for (const auto& kv : int_data) {
      switch (kv.first) {
        case SupportedBuffers::COLORS:
          assert(false);
          break;
        case SupportedBuffers::TEXTURES:
          assert(false);
          break;
        case SupportedBuffers::VERTICES:
          assert(false);
          break;
        case SupportedBuffers::INDICES:
          get<SupportedBuffers::INDICES>() = VectorSlice<uint32_t>(kv.second, start_idx, end_idx, floats_per_ind_);
          break;
      }
    }

  }

  size_t start_vertex() { return start_vertex_; }
  size_t end_vertex() { return end_vertex_; }

  size_t start_index() { return start_index_; }
  size_t end_index() { return end_index_; }
private:
  size_t start_vertex_;
  size_t start_index_;
  size_t end_vertex_;
  size_t end_index_;
};

class MixedDataMap final : public MixedDataMapBase<std::vector> {
public:
  MixedDataMap(std::set<SupportedBuffers> keys, size_t vertex_count, size_t idx_count) {
    for (const auto& key: keys) {
      switch (key) {
        case SupportedBuffers::COLORS:
          get<SupportedBuffers::COLORS>().resize(vertex_count*floats_per_color_);
          break;
        case SupportedBuffers::INDICES:
          get<SupportedBuffers::INDICES>().resize(idx_count*floats_per_ind_);
          break;
        case SupportedBuffers::TEXTURES:
          get<SupportedBuffers::TEXTURES>().resize(vertex_count*floats_per_text_);
          break;
        case SupportedBuffers::VERTICES:
          get<SupportedBuffers::VERTICES>().resize(vertex_count*floats_per_vert_);
          break;
      }
    }
    total_vertices_ = vertex_count;
    total_indices_ = idx_count;
    next_free_vertex_ = 0;
    next_free_index_ = 0;
  }

  MixedDataMap(const MixedDataMap&) = delete;
  MixedDataMap(MixedDataMap&&) = default;

  MixedDataMap& operator=(const MixedDataMap&) = delete;
  MixedDataMap& operator=(MixedDataMap&&) = default;

  MixedSliceMap NextSlice(size_t vertex_count, size_t index_count) {
    // TODO unify vertex and index into single structure?
    auto vertex = next_free_vertex_;
    auto idx = next_free_index_;
    next_free_vertex_ += vertex_count;
    next_free_index_ += index_count;
    return MixedSliceMap(
        FloatData(),
        IntegralData(),
        vertex,
        next_free_vertex_,
        idx,
        next_free_index_
        );
  }

  size_t VertexDataRemaining() {
    return total_vertices_ - next_free_vertex_;
  }

  size_t IndexDataRemaining() {
    return total_indices_ - next_free_index_;
  }

private:
  size_t next_free_vertex_ = 0;
  size_t next_free_index_ = 0;
  size_t total_vertices_ = 0;
  size_t total_indices_ = 0;
};

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
  RenderNode() : translation_({0.0f, 0.0f, 0.0f, 1.0f}) {}
  RenderNode(const RenderNode& orig) = delete;
	RenderNode& operator=(RenderNode&) = delete;

	// Functions for child classes to figure out what indices in the VAO they
	// should be modifying.  Only guaranteed to be valid during calls to
	// FillVertexData and FillColorData.
	size_t start_vertex() const {return start_vertex_; }
	size_t end_vertex() const {return end_vertex_; }
	size_t start_index() const {return start_index_; }
	size_t end_index() const {return end_index_; }

	// Compute how big the VAO should be
	size_t SubtreeVertexCount() const;
	size_t SubtreeIndexCount() const;
	// Fill the VAO with data and push to card
  void PopulateBufferData(MixedDataMap& data);
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
	virtual size_t ExclusiveNodeVertexCount() const = 0;
  virtual size_t ExclusiveNodeIndexCount() const = 0;
  virtual void FillIndexData(VectorSlice<uint32_t>& data) const = 0;
  virtual void FillTextureData(VectorSlice<float>& data) const = 0;
	virtual void FillColorData(VectorSlice<float>& data) const = 0;
	virtual void FillVertexData(VectorSlice<float>& data) const = 0;
	virtual void DrawSelf() const = 0;

  void DebugRotation(const math::Matrix4& mat) const;

  size_t start_vertex_ = 0;
  size_t start_index_ = 0;
  size_t end_vertex_= 0;
	size_t end_index_ = 0;

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
class TextureInterface<false> : public RenderNode {
  virtual void FillTextureData(VectorSlice<float>& data) const override {}
};
template <size_t Flags> using TextureNode = TextureInterface<Flags & SupportedBufferFlags::TEXTURES>;

template <size_t Flags, bool enabled> struct ColorInterface : public TextureNode<Flags> {};
template <size_t Flags>
class ColorInterface<Flags, false> : public TextureNode<Flags> {
	virtual void FillColorData(VectorSlice<float>& data) const override {}
};
template <size_t Flags> using ColorNode = ColorInterface<Flags, Flags & SupportedBufferFlags::COLORS>;

template <size_t Flags, bool enabled> struct IndexInterface : public ColorNode<Flags> {};
template <size_t Flags>
class IndexInterface<Flags, false> : public ColorNode<Flags> {
  virtual size_t ExclusiveNodeIndexCount() const override { return 0; }
	virtual void FillIndexData(VectorSlice<uint32_t>& data) const override {}
};
template <size_t Flags> using IndexNode = IndexInterface<Flags, Flags & SupportedBufferFlags::INDICES>;

}

template <size_t Flags>
class TypedRenderNode : public detail::IndexNode<Flags> {
  static_assert(Flags < SupportedBufferFlags::END_VALUE, "Invalid flags for buffer support");
public:
  constexpr static const size_t Flags_t = Flags;
  TypedRenderNode() {}
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
	virtual size_t ExclusiveNodeVertexCount() const { return 0; }
	virtual size_t ExclusiveNodeIndexCount() const { return 0; }
	virtual void FillVertexData(VectorSlice<float>& data) const {};
	virtual void FillColorData(VectorSlice<float>& data) const {};
	virtual void FillIndexData(VectorSlice<uint32_t>& data) const {};
  virtual void DrawSelf() const {}
};

class RootNode : private TypedRenderNode<0> {
public:
  template <typename Other>
  RootNode(
      std::unique_ptr<Other> tree,
      std::shared_ptr<Shaders::ShaderProgram> program,
      // TODO move this to template parameter with default argument
	    typename std::enable_if<
          std::is_base_of<TypedRenderNode<Other::Flags_t>, Other>::value
      >::type* dummy = 0 // Only here for SFINAE
      )
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
	virtual void FillVertexData(VectorSlice<float>& data) const override {}
	virtual size_t ExclusiveNodeVertexCount() const override { return 0; }
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
