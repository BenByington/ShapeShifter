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

#include "data/MixedDataMapBase.h"
#include "rendering/shaders/Pack.h"
#include "rendering/shaders/ShaderProgram.h"
#include "rendering/shaders/UniformManager.h"


namespace ShapeShifter {
namespace Rendering {

// TODO put this elsewhere...
/*
 * Used to create a non-owning reference_wrapper that can be called like a smart
 * pointer.  It has a main type T with normal semantics, but in the event of
 * type erasure with multiple inheritance, it can optionally provide access
 * to other parent types as well.
 */
template <class T, class...Other>
class CallableReferenceWrapper {
  template <typename U>
  static constexpr bool validate_aux_types() {
    const std::array<bool, sizeof...(Other)> is_child = {{std::is_base_of<Other, U>::value...}};
    int sum = 0;
    for (int i = 0; i < sizeof...(Other); ++i) {
      if (is_child[i]) sum++;
    }
    return sum == sizeof...(Other);
  }
  template <typename U>
  static constexpr bool validate_conversion() {
    const std::array<bool, sizeof...(Other)> is_child = {{std::is_base_of<Other, U>::value...}};
    int sum = 0;
    for (int i = 0; i < sizeof...(Other); ++i) {
      if (is_child[i]) sum++;
    }
    return sum == 1;
  }
public:
  template <typename U>
  explicit CallableReferenceWrapper(U& u) : w_(u) {
    static_assert(std::is_base_of<T,U>::value, "Is not a child of main type");
    static_assert(validate_aux_types<U>(), "Is not a child of one or more auxiliary types");
  }
  CallableReferenceWrapper(const CallableReferenceWrapper&) = delete;
  CallableReferenceWrapper(CallableReferenceWrapper&&) = default;
  CallableReferenceWrapper& operator=(const CallableReferenceWrapper&) = delete;
  CallableReferenceWrapper& operator=(CallableReferenceWrapper&&) = default;

  T* operator->() { return &(w_.get()); }
  T& operator*() { return w_.get(); }
  operator T&() { return w_; }
  operator const T&() const { return w_; }

  template<typename U>
  CallableReferenceWrapper<U> Convert() {
    static_assert(validate_conversion<U>(), "is not valid auxiliary type");
    return CallableReferenceWrapper<U>(dynamic_cast<U&>(w_.get()));
  }
  template<typename U>
  CallableReferenceWrapper<const U> Convert() const {
    static_assert(validate_conversion<U>(), "is not valid auxiliary type");
    return CallableReferenceWrapper<const U>(dynamic_cast<const U&>(w_.get()));
  }
private:
  std::reference_wrapper<T> w_;
};

/*
 * This is the main base class for all custom concrete RenderNode
 * implementations, though it should not be instantiated directly but rather
 * through the convenience aliases provided outside the detail namespace.
 *
 * Both Types and Managers are variadic packs describing what buffers (e.g.
 * vertices/colores/textures/etc) are to be supported by this node.  'Types'
 * are used to determine what combinations of nodes are allowed in a valid tree
 * (children node must support at least as many types as parent nodes.  They can
 * support more, but the additional ones will never be utilized).  'Managers'
 * are used to determine what virtual interface child classes will have to
 * implement.  In normal use cases 'Types' will be the same as 'Managers', except
 * in the special case of a PureNode which has an empty variadic pack for
 * Managers.  This complication is why this class is tucked away in a 'detail'
 * namespace, and external code should use the TypedRenderNode alias defined
 * below.
 */
template <class Interface, class Uniforms>
struct PureNode;
template <class... Interface, class... Uniforms>
struct PureNode<Pack<Interface...>,Pack<Uniforms...>> : Shaders::UniformManager<Uniforms...> {
  PureNode() {}
  virtual ~PureNode() {}

  /**
   * Adds a child to this node.
   * Note: These are intentionally shared, and external code may keep a
   *       reference to do things like tweak the rotation matrix.  Tweaking the
   *       actual vertex count or absolute position will not be supported.
   *
   * @param child subtree to add to this node.
   */
  using Interface_t = Pack<Interface...>;
  using Uniform_t = Pack<Uniforms...>;
  using Manipulator_t = Shaders::UniformManager<Uniforms...>;

  template <typename Other>
  CallableReferenceWrapper<Manipulator_t, PureNode> AddChild(
      std::unique_ptr<Other> child) {

    static_assert(
        is_permutation<Interface_t, typename Other::Interface_t>::value,
        "Internal nodes must all have the same interface");
    static_assert(
        is_permutation<Uniform_t, typename Other::Uniform_t>::value,
        "Internal nodes must all have the same uniforms");

    CallableReferenceWrapper<Manipulator_t, PureNode> ret(*child);
    this->subtrees_.emplace_back(std::move(child));
    return ret;

  }

  template <class Leaf, typename... Args>
  CallableReferenceWrapper<Manipulator_t, PureNode> AddLeaf(Args&&... args) {
    static_assert(
        is_subset<Interface_t, typename Leaf::Interface_t>::value(),
        "Attempting to add leaf node that does not fulfill the input interface"
        " of the parent node");

    auto child = std::make_unique<PureNode>();
    auto leaf = std::make_unique<Leaf>(std::forward<Args>(args)...);
    child->leaf_ = std::move(leaf);
    CallableReferenceWrapper<Manipulator_t, PureNode> ret(*child);
    this->subtrees_.emplace_back(std::move(child));
    return ret;
  }

  const PureNode* Parent() const { return parent_; }

protected:

  // Compute how big the VAO should be
  Data::BufferIndex SubtreeCounts() const {
    auto ret = Data::BufferIndex{};
    for (const auto& child : subtrees_) {
      ret += child->SubtreeCounts();
    }
    if (leaf_) {
      ret += leaf_->ExclusiveNodeDataCount();
    }
    return ret;
  }

  // Fill the VAO with data and push to card
  void PopulateBufferData(Data::MixedDataMap& data) {
    for (auto& child : subtrees_) {
      child->PopulateBufferData(data);
    }
    if (leaf_) {
      leaf_->FillLocalBuffer(data);
    }
  }

  // Renders all children in the tree.
  template <class IPack, class... Uniforms_>
  void DrawChildren(
      const Camera& camera,
      const Shaders::UniformManager<Uniforms_...>& cumulativeUniforms,
      const Shaders::ShaderProgram<IPack, Pack<Uniforms_...>>& shader) const {

    // ISSUE see about only doing dynamic casts in debug mode or something.
    for (const auto& child : subtrees_) {
      auto child_uniforms = cumulativeUniforms;
      child_uniforms.Combine(*child);
      child->DrawChildren(camera, child_uniforms, shader);
    }
    if (leaf_) {
      shader.Upload(camera, cumulativeUniforms);
      leaf_->DrawSelf();
    }
  }

  // TODO some of these should be private?
protected:
  void FinalizeTree() {
    for (auto& child : subtrees_) {
      child->parent_ = this;
      child->FinalizeTree();
    }
  }

  bool IsAncestor(const PureNode& node) const {
    if (parent_ == nullptr) return &node == this;
    else return parent_->IsAncestor(node);
  }

  bool IsChild(const PureNode& node) const {
    return node.IsAncestor(*this);
  }

private:
  PureNode* parent_ = nullptr;
  // TODO does this need to be ref/pointer?
  std::vector<std::unique_ptr<PureNode>> subtrees_;
  std::unique_ptr<BaseLeafNode> leaf_;

};

/*
 * Helper function, to avoid typing out explicit long type names. As
 * long as you have a ShaderProgram laying around, you can easily retrieve
 * a pure node that will be compatible with it.
 */
template <class... ShaderProgram>
decltype(auto) CompatiblePureNode(const ShaderProgram&...) {
  using IType = typename set_union<typename ShaderProgram::Interface_t...>::type;
  using UType = typename set_union<typename ShaderProgram::Uniform_t...>::type;
  using Type = PureNode<IType, UType>;
  return std::make_unique<Type>();
}

}} //namespace Shapeshifter::Rendering

#endif /* RENDERING_PURE_NODE_H */
