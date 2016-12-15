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
#include "rendering/OpenglWrapper.h"

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
// TODO salvage above comment

struct Manipulator {
  Manipulator() : translation_(0,0,0,1) {}

  void SetRotation(const Math::Quaternion& rot) {
    rotation_ = rot;
  }
  void SetTranslation(const Math::Vector4& trans) {
    translation_ = trans;
  }

  Manipulator Combine(const Manipulator& cumulative) const {
    Manipulator ret;
    ret.SetRotation(cumulative.rotation_*rotation_);
    // TODO define multiple of vector with quaternion directly.
    ret.SetTranslation(cumulative.translation_ + ret.rotation_.RotationMatrix() * translation_);
    return ret;

  }

  void Upload(const Camera& camera, const Shaders::ShaderProgramBase& program) const;

  Math::Quaternion rotation_;
  Math::Vector4 translation_;
};

//  TODO Move to own files, once the refactoring has calmed down.
class BaseLeafNode {
protected:
  BaseLeafNode() = default;
public:
  BaseLeafNode(const BaseLeafNode& orig) = delete;
  BaseLeafNode(BaseLeafNode&& orig) = delete;
	BaseLeafNode& operator=(const BaseLeafNode&) = delete;
	BaseLeafNode& operator=(BaseLeafNode&&) = delete;
  virtual ~BaseLeafNode() {}

  // Size required in the buffers, not counting children nodes
  virtual Data::BufferIndex ExclusiveNodeDataCount() const = 0;

	// Functions for child classes to figure out what indices in the VAO they
	// should be modifying.
	Data::BufferIndex start() const {return start_; }
	Data::BufferIndex end() const {return end_; }
  GLvoid* StartIndexAsVP() const {
    return (GLvoid*)(start().index_*sizeof(uint32_t));
  }

  // TODO These two functions should not be public, the first because it could
  // cause this object to be partially associated with two VBO's, and the second
  // because it is only valid to call when the correct VBO is loaded.  Once this
  // refactor settles down, try to find a way that *only* the internal tree
  // nodes can call them.
  void FillLocalBuffer(Data::MixedDataMap& data);
  // Personal rendering function
	virtual void DrawSelf() const = 0;

private:
  // Populates data for the index vector.  Children may implement this as
  // a noop if they are drawing independent triangles that do not share
  // vertices
	virtual void FillIndexData(Data::VectorSlice<uint32_t>& data) const = 0;

  // Positions of personal data in the buffers.  Not valid until tree is
  // finalized by creating a RootNode.
  Data::BufferIndex start_;
  Data::BufferIndex end_;

};

class BasePureNode {
protected:
  BasePureNode() = default;
  BasePureNode(const BasePureNode& orig) = delete;
  BasePureNode(BasePureNode&& orig) = delete;
	BasePureNode& operator=(const BasePureNode&) = delete;
	BasePureNode& operator=(BasePureNode&&) = delete;
  virtual ~BasePureNode() {}

	// Compute how big the VAO should be
	Data::BufferIndex SubtreeCounts() const;

	// Fill the VAO with data and push to card
  void PopulateBufferData(Data::MixedDataMap& data);

	// Renders all children in the tree.
	void DrawChildren(
      const Camera& camera,
      const Manipulator& cumUniforms,
      const Shaders::ShaderProgramBase& shader) const;

  // TODO make unique pointers, after we start returning an id instead
  // of a shared_ptr
  template <class T>
  using element = std::pair<std::shared_ptr<Manipulator>, std::shared_ptr<T>>;
	std::vector<element<BasePureNode>> subtrees_;
  std::vector<element<BaseLeafNode>> leaves_;

};

}} // ShapeShifter::Rendering

#endif /* RENDERING_RENDERNODE_H */
