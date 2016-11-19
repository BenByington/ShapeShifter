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

namespace detail {

template <class... Args> struct instantiate_managers_helper;
template <> struct instantiate_managers_helper<> {
  static void foo(
      std::vector<std::shared_ptr<Data::AbstractManager>>& managers,
      std::map<std::string, size_t>& map) {}
};

template <class Head, class... Args>
struct instantiate_managers_helper<Head, Args...> {
  static void foo(
      std::vector<std::shared_ptr<Data::AbstractManager>>& managers,
      std::map<std::string, size_t>& map) {
    if (map.count(Head::Variable::name()) == 0) {
      throw std::runtime_error("Shader does not support required buffer");
    }
    managers.emplace_back(
        std::make_shared<Head>(map.at(Head::Variable::name())));
    map.erase(Head::Variable::name());
    if (sizeof...(Args) > 0) {
      instantiate_managers_helper<Args...>::foo(managers, map);
    }
  }
};

template <class... Args>
struct instantiate_managers {
  static auto foo(std::map<std::string, size_t>& mapper) {
    std::vector<std::shared_ptr<Data::AbstractManager>> ret;
    instantiate_managers_helper<Args...>::foo(ret, mapper);
    if (!mapper.empty()) {
      throw std::runtime_error("Shader requires buffers not present\n");
    }
    return ret;
  }
};

}

template <class... Interface>
class ShaderProgram : public ShaderProgramBase {
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
        std::is_same<typename Vertex::Inputs_t, Pack<Interface...>>::value,
        "Input parameters for Vertex shader must match those of the"
        " entire shader program");
    // This one should allow also re-ordering.  It's more important than the
    // above, but still not necessary since the only shader currently written
    // only has a single variable bridging these two stagess.
    static_assert(
        std::is_same<typename Vertex::Outputs_t, typename Fragment::Inputs_t>::value,
        "Vertex and Fragment shader do not share common IO interface");
    // ISSUE: Refactor to include Uniform variables as part of interface.
  }
  ShaderProgram(
      std::unique_ptr<RawShader<RawShaderType::VERTEX>> vert,
      std::unique_ptr<RawShader<RawShaderType::FRAGMENT>> frag)
    : ShaderProgramBase(std::move(vert), std::move(frag)) {}

  virtual std::vector<std::shared_ptr<Data::AbstractManager>>
  BufferMapping() const override {
    auto map = layout_map();
    return detail::instantiate_managers<Interface...>::foo(map);
  }

  using Interface_t = std::tuple<Interface...>;
};

namespace detail {

template <typename Input>
struct get_program_type;

template <typename... Inputs>
struct get_program_type<Pack<Inputs...>> {
  using Type = Rendering::Shaders::ShaderProgram<Inputs...>;
};

}

template <class VertexGenerator, class FragmentGenerator>
decltype(auto) CreateShaderProgram() {
  auto vert = std::make_unique<VertexShader<VertexGenerator>>();
  auto frag = std::make_unique<FragmentShader<FragmentGenerator>>();
  using ShaderProgram = typename detail::get_program_type<typename VertexGenerator::Inputs_t>::Type;
	return std::make_shared<ShaderProgram>(std::move(vert), std::move(frag));
}

}}} // ShapeShifter::Rendering::Shaders

#endif /* RENDERING_SHADERS_SHADER_PROGRAM_H */

