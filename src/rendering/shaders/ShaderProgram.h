/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   ShaderProgram.h
 * Author: bbyington
 *
 * Created on July 31, 2016, 3:08 PM
 */

#ifndef RENDERING_SHADERS_SHADER_PROGRAM_H
#define RENDERING_SHADERS_SHADER_PROGRAM_H

#include "data/AbstractBufferManager.h"
#include "rendering/shaders/RawShader.h"
#include "rendering/shaders/Shader.h"
#include "rendering/shaders/ShaderProgramBase.h"
#include "rendering/shaders/UniformManager.h"

#include <limits>
#include <map>
#include <memory>
#include <vector>

namespace ShapeShifter::Rendering::Shaders {

template <class Interface, class Uniforms>
class ShaderProgram;

template <class... Interface, class... Uniforms>
class ShaderProgram<Pack<Interface...>, Pack<Uniforms...>> : public ShaderProgramBase {
public:
  ShaderProgram(const ShaderProgram&) = delete;
  ShaderProgram& operator()(ShaderProgram&) = delete;
  template <class Vertex, class Fragment>
  // Could do extra work allowing for re-ordering of inputs,
  // but that should never be necessary if using the proper
  // convenience functions
    requires std::same_as<typename Vertex::Managers_t, Pack<Interface...>> &&
             // This one should allow also re-ordering.  It's more
             // important than the above, but still not necessary since
             // the only shader currently written only has a single
             // variable bridging these two stages.
             std::same_as<typename Vertex::Outputs_t, typename Fragment::Inputs_t>
  ShaderProgram(std::unique_ptr<VertexShader<Vertex>> vert,
                std::unique_ptr<FragmentShader<Fragment>> frag)
      : ShaderProgramBase(std::move(vert), std::move(frag)) {}
  ShaderProgram(std::unique_ptr<RawShader<RawShaderType::VERTEX>> vert,
                std::unique_ptr<RawShader<RawShaderType::FRAGMENT>> frag)
      : ShaderProgramBase(std::move(vert), std::move(frag)) {}

  void Upload(const Camera& camera, const UniformManager<Uniforms...>& uniforms) const {
    auto worker = {
        (UploadValue(
             static_cast<const typename Uniforms::UniformManager&>(uniforms).Data(camera),
             Uniforms::name()),
         1)...};
    (void)worker;
  }

  using Interface_t = Pack<Interface...>;
  using Uniform_t = Pack<Uniforms...>;
};

/*
 * Helper function that does a lot of the legwork in creating a shader
 * program, requiring fewer long fully-qualified names at the call site.
 * This should be the preferred method of instantiation.
 */
template <class VertexGenerator, class FragmentGenerator>
decltype(auto) CreateShaderProgram() {
  auto vert = std::make_unique<VertexShader<VertexGenerator>>();
  auto frag = std::make_unique<FragmentShader<FragmentGenerator>>();
  using ShaderProgram = ShaderProgram<typename VertexGenerator::Managers_t,
                                      typename VertexGenerator::Uniforms_t>;
  return std::make_shared<ShaderProgram>(std::move(vert), std::move(frag));
}

} // namespace ShapeShifter::Rendering::Shaders

#endif /* RENDERING_SHADERS_SHADER_PROGRAM_H */
