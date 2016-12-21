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
      child_uniforms.Combine(dynamic_cast<UniformManager&>(*child.first));
	  	child.second->DrawChildren(camera, child_uniforms, shader);
	  }
	  for (const auto& leaf : leaves_) {
      auto child_uniforms = cumulativeUniforms;
      child_uniforms.Combine(dynamic_cast<UniformManager&>(*leaf.first));
      shader.Upload(camera, child_uniforms);
      leaf.second->DrawSelf();
	  }
  }

  template <class T>
  using element = std::pair<std::shared_ptr<Shaders::BaseUniformManager>, std::unique_ptr<T>>;
	std::vector<element<BasePureNode>> subtrees_;
  std::vector<element<BaseLeafNode>> leaves_;

};

}} // ShapeShifter::Rendering

#endif /* RENDERING_BASE_PURE_NODE_H */
