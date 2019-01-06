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

#include "data/BufferMap.h"
#include "rendering/OpenglWrapper.h"
#include "rendering/shaders/Pack.h"
#include "util/MultiReferenceWrapper.h"

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

  // Personal rendering function
  virtual void DrawSelf() const = 0;

  // Populates data for the index vector.  Children may implement this as
  // a noop if they are drawing independent triangles that do not share
  // vertices
  virtual void FillIndexData(Data::VectorSlice<uint32_t>& data) const = 0;

  // ISSUE This public visibility is not desirable.  Would be preferrable to set
  //       some how as part of construction, but that isn't currently possible until
  //       things are reworked.  (Probably want to separate tree heirarchy from buffer
  //       creation, but am not yet sure.)
public:
  // Positions of personal data in the buffers.  Not valid until tree is
  // finalized by creating a RootNode.
  Data::BufferIndex start_;
  Data::BufferIndex end_;

};


template <class... Managers>
struct LeafNode : BaseLeafNode, Managers::Interface... {
  LeafNode() {}
  ~LeafNode() override {}
  using Interface_t = Pack<Managers...>;
};

template <class... Managers>
struct LeafWrapper {

  template <class Leaf>
  LeafWrapper(std::unique_ptr<Leaf> node) {
    static_assert(
        is_subset<Pack<Managers...>, typename Leaf::Interface_t>::value(),
        "Attempting to add leaf node that does not fulfill the input interface"
        " of the parent node");

    this->leaf_ = std::make_unique<Util::MultiReferenceOwner<BaseLeafNode, typename Managers::Interface...>>(std::move(node));
  }
  LeafWrapper() = default;

  LeafWrapper(const LeafWrapper&) = delete;
  LeafWrapper(LeafWrapper&&) = default;
  LeafWrapper& operator=(const LeafWrapper&) = delete;
  LeafWrapper& operator=(LeafWrapper&&) = default;

  BaseLeafNode* operator->() { return (*leaf_).operator->(); }
  const BaseLeafNode* operator->() const { return (*leaf_).operator->(); }
  BaseLeafNode& operator*() { return *leaf_; }
  operator bool() const { return static_cast<bool>(leaf_); }

  // ISSUE: implement something like client-attorney here so that *only*
  //        PureNodes can call these two functions
  template <typename... Keys>
  void FillLocalBuffer(Data::BufferMap<Keys...>& data) {

    static_assert(
        is_subset<Pack<Keys...>, Pack<Managers...>>::value(),
        "Attempting fill BufferMap containing Keys not part of our interface");

    auto size = (*leaf_)->ExclusiveNodeDataCount();

    auto local_data = data.NextSlice(size);
    (*leaf_)->start_= local_data.start();
    (*leaf_)->end_= local_data.end();

    auto worker = {(leaf_->template Convert<typename Keys::Interface>()->FillData(local_data.template Val<Keys>())
                    ,1)...
    };
    (void) worker;

    auto& indices = local_data.indices();
    (*leaf_)->FillIndexData(indices);
    size_t offset = (*leaf_)->start().vertex_;
    for (auto& ind: indices) {
      ind += offset;
    }
  }

private:
  std::unique_ptr<Util::MultiReferenceOwner<BaseLeafNode, typename Managers::Interface...>> leaf_;
};

}} // ShapeShifter::Rendering

#endif /* SHAPESHIFTER_RENDERING_LEAFNODE_H */

