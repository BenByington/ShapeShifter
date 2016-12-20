/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   LeafNode.h
 * Author: bbyington
 *
 * Created on December 20, 2016, 3:57 PM
 */

#ifndef SHAPESHIFTER_RENDERING_LEAFNODE_H
#define SHAPESHIFTER_RENDERING_LEAFNODE_H

#include "data/MixedDataMap.h"
#include "rendering/OpenglWrapper.h"
#include "rendering/shaders/Pack.h"

namespace ShapeShifter {
namespace Rendering {

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


template <class... Managers>
struct LeafNode : BaseLeafNode, Managers::Interface... {
  LeafNode() {}
  virtual ~LeafNode() {}

  // TODO rename?
  using Interface_t = Pack<Managers...>;
};
}} // ShapeShifter::Rendering

#endif /* SHAPESHIFTER_RENDERING_LEAFNODE_H */

