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

#ifndef RENDERING_BASE_PURE_NODE_H
#define RENDERING_BASE_PURE_NODE_H

#include "data/MixedDataMap.h"
#include "math/Quaternion.h"
#include "math/Vector4.h"
#include "rendering/Camera.h"
#include "rendering/OpenglWrapper.h"
#include "rendering/shaders/BaseUniformManager.h"
#include "rendering/LeafNode.h"
#include "rendering/shaders/ShaderProgram.h"
#include "rendering/shaders/UniformManager.h"

namespace ShapeShifter {
namespace Rendering {

// Maybe put this elsewhere...
template <class T>
class CallableReferenceWrapper {
public:
  explicit CallableReferenceWrapper(T& t) : w_(t) {}
  CallableReferenceWrapper(const CallableReferenceWrapper&) = delete;
  CallableReferenceWrapper(CallableReferenceWrapper&&) = default;
  CallableReferenceWrapper& operator=(const CallableReferenceWrapper&) = delete;
  CallableReferenceWrapper& operator=(CallableReferenceWrapper&&) = default;

  T* operator->() { return &(w_.get()); }
  operator T&() { return w_; }
  operator const T&() const { return w_; }
private:
  std::reference_wrapper<T> w_;
};

class BasePureNode {
public:
  virtual ~BasePureNode() {}
protected:
  BasePureNode() = default;
  BasePureNode(const BasePureNode& orig) = delete;
  BasePureNode(BasePureNode&& orig) = delete;
	BasePureNode& operator=(const BasePureNode&) = delete;
	BasePureNode& operator=(BasePureNode&&) = delete;

	// Compute how big the VAO should be
	Data::BufferIndex SubtreeCounts() const;

	// Fill the VAO with data and push to card
  void PopulateBufferData(Data::MixedDataMap& data);

	// Renders all children in the tree.
  template <class IPack, class... Uniforms>
	void DrawChildren(
      const Camera& camera,
      const Shaders::UniformManager<Uniforms...>& cumulativeUniforms,
      const Shaders::ShaderProgram<IPack, Pack<Uniforms...>>& shader) const {
    using UniformManager = Shaders::UniformManager<Uniforms...>;

    // ISSUE see about only doing dynamic casts in debug mode or something.
	  for (const auto& child : subtrees_) {
      auto child_uniforms = cumulativeUniforms;
      child_uniforms.Combine(child.Manager());
	  	child.Node().DrawChildren(camera, child_uniforms, shader);
	  }
	  if (leaf_) {
      shader.Upload(camera, cumulativeUniforms);
      leaf_->DrawSelf();
	  }
  }

  class TypeErasedNode {
  public:
    using Ref = CallableReferenceWrapper<Shaders::BaseUniformManager>;
    template <typename Node>
    TypeErasedNode(std::unique_ptr<Node> node)
      : data_(Ref(*node), nullptr) {
      static_assert(std::is_base_of<BasePureNode,Node>::value,
          "Must be a child of BasePureNode");
      static_assert(std::is_base_of<Shaders::BaseUniformManager, Node>::value,
          "Mulst be a child of BaseUniformManager");
      data_.second = std::move(node);
    }

    const Ref& Manager() const { return data_.first; }
    const BasePureNode& Node() const { return *data_.second; }
    BasePureNode& Node() { return *data_.second; }
  private:
    std::pair<
        CallableReferenceWrapper<Shaders::BaseUniformManager>,
        std::unique_ptr<BasePureNode>> data_;
  };
	std::vector<TypeErasedNode> subtrees_;
  std::unique_ptr<BaseLeafNode> leaf_;

};

}} // ShapeShifter::Rendering

#endif /* RENDERING_BASE_PURE_NODE_H */
