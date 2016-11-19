/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   PureNode.h
 * Author: bbyington
 *
 * Created on October 13, 2016, 4:09 PM
 */

#ifndef RENDERING_PURE_NODE_H
#define RENDERING_PURE_NODE_H

#include "rendering/TypedRenderNode.h"

namespace ShapeShifter {
namespace Rendering {

/**
 * Basic implementation for nodes that only hold other nodes
 */
template <class... Interface>
class PureNode final : public PureTypedRenderNode<Interface...> {
public:
	PureNode() = default;
	virtual ~PureNode() {}
private:
	virtual Data::BufferIndex ExclusiveNodeDataCount() const override {
    return Data::BufferIndex();
  }
  virtual void FillIndexData(Data::VectorSlice<uint32_t>&) const override {}
  virtual void DrawSelf() const override {}
};

namespace detail {
template <typename Interface>
struct pure_type;
template <typename... Interface>
struct pure_type<Pack<Interface...>> {
  using Type = PureNode<Interface...>;
};
}

/*
 * Helper function, to avoid typing out explicit long type names. As
 * long as you have a ShaderProgram laying around, you can easily retrieve
 * a pure node that will be compatible with it.
 */
template <class ShaderProgram>
decltype(auto) CompatiblePureNode(const ShaderProgram&) {
  using Type = typename detail::pure_type<typename ShaderProgram::Interface_t>::Type;
  return std::make_unique<Type>();
}

}} //namespace Shapeshifter::Rendering

#endif /* RENDERING_PURE_NODE_H */
