/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   TypedRenderNode.h
 * Author: bbyington
 *
 * Created on October 13, 2016, 3:52 PM
 */

#ifndef RENDERING_TYPED_RENDERNODE_H
#define RENDERING_TYPED_RENDERNODE_H

#include "data/MixedDataMap.h"
#include "rendering/RenderNode.h"

namespace ShapeShifter {
namespace Rendering {

/*
 * Sets up a configurable interface.  Depending on what value is handed in for
 * the Flags template parameter, extending from the BaseNode class below will
 * require children to implement different combinations of abstract virtual
 * functions.  Valid values of 'Flags' are determined by using the above
 * definitions in namespace SupportedBuffers as bitflags.
 */
namespace detail {

using Data::SupportedBufferFlags;

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

template <class... Interface>
struct extract_flags {
  static size_t constexpr impl() {
    const std::array<size_t, sizeof...(Interface)> dat { Interface::Flag... };
    size_t ret = 0;
    for (size_t i = 0; i < sizeof...(Interface); ++i) ret |= dat[i];
    return ret;
  }

  static size_t constexpr Flags = impl();
};

}


template <class... Interface>
class TypedRenderNode : public detail::IndexNode<detail::extract_flags<Interface...>::Flags> {

  public:
  static constexpr size_t Flags = detail::extract_flags<Interface...>::Flags;
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
          (Other::Flags_t & Flags) == Flags
          && std::is_base_of<RenderNode, Other>::value
          && (Other::Flags_t & SupportedBufferFlags::INDICES) == (Flags & SupportedBufferFlags::INDICES)
      >::type* dummy = 0
      ) {
    this->children.emplace_back(child.release());
    return this->children.back();

  }
};

}} // ShapeShifter::Rendering

#endif /* RENDERING_TYPED_RENDERNODE_H */

