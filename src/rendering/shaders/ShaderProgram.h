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
#include "rendering/shaders/ShaderProgramBase.h"
#include "rendering/shaders/Shader.h"

#include <limits>
#include <map>
#include <memory>
#include <vector>

namespace ShapeShifter {
namespace Rendering {
namespace Shaders {

template <class Interface, class Uniforms>
class ShaderProgram;

template <class... Interface, class... Uniforms>
class ShaderProgram<Pack<Interface...>, Pack<Uniforms...>> : public ShaderProgramBase {
public:
  ShaderProgram(const ShaderProgram&) = delete;
	ShaderProgram& operator()(ShaderProgram&) = delete;
  template <class Vertex, class Fragment>
  ShaderProgram(
      std::unique_ptr<VertexShader<Vertex>> vert,
      std::unique_ptr<FragmentShader<Fragment>> frag)
    : ShaderProgramBase(std::move(vert), std::move(frag)) {
    // Could do extra work allowing for re-ordering of inputs, but that
    // should never be necessary if using the proper convenience functions
    static_assert(
        std::is_same<typename Vertex::Managers_t, Pack<Interface...>>::value,
        "Input parameters for Vertex shader must match those of the"
        " entire shader program");
    // This one should allow also re-ordering.  It's more important than the
    // above, but still not necessary since the only shader currently written
    // only has a single variable bridging these two stages.
    static_assert(
        std::is_same<typename Vertex::Outputs_t, typename Fragment::Inputs_t>::value,
        "Vertex and Fragment shader do not share common IO interface");
    // TODO: Refactor to include Uniform variables as part of interface.
  }
  ShaderProgram(
      std::unique_ptr<RawShader<RawShaderType::VERTEX>> vert,
      std::unique_ptr<RawShader<RawShaderType::FRAGMENT>> frag)
    : ShaderProgramBase(std::move(vert), std::move(frag)) {}

  using Interface_t = Pack<Interface...>;
  using Uniform_t = Pack<Uniforms...>;
};

// TODO remove if unused
namespace detail {

template <typename Input>
struct get_program_type;

template <typename... Inputs>
struct get_program_type<Pack<Inputs...>> {
  using Type = Rendering::Shaders::ShaderProgram<Inputs...>;
};

}

/*
 * Helper function that does a lot of the legwork in creating a shader
 * program, requiring fewer long fully-qualified names at the call site.
 * This should be the preferred method of instantiation.
 */
template <class VertexGenerator, class FragmentGenerator>
decltype(auto) CreateShaderProgram() {
  auto vert = std::make_unique<VertexShader<VertexGenerator>>();
  auto frag = std::make_unique<FragmentShader<FragmentGenerator>>();
  using ShaderProgram = ShaderProgram<
      typename VertexGenerator::Managers_t,
      typename VertexGenerator::Uniforms_t>;
	return std::make_shared<ShaderProgram>(std::move(vert), std::move(frag));
}

}}} // ShapeShifter::Rendering::Shaders

#endif /* RENDERING_SHADERS_SHADER_PROGRAM_H */

