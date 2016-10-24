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

#include "rendering/shaders/ShaderProgramBase.h"

#include "data/AbstractBufferManager.h"
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
      managers.emplace_back(std::make_shared<Head>(map.at(Head::Variable::name())));
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
  ShaderProgram(
      std::unique_ptr<VertexShader> vert,
      std::unique_ptr<FragmentShader> frag) : ShaderProgramBase(std::move(vert), std::move(frag)) {}

  virtual std::vector<std::shared_ptr<Data::AbstractManager>> BufferMapping() const override {
    auto map = vert_shader().layout_map();
    return detail::instantiate_managers<Interface...>::foo(map);
  }

  using Interface_t = std::tuple<Interface...>;
};

}}} // ShapeShifter::Rendering::Shaders

#endif /* RENDERING_SHADERS_SHADER_PROGRAM_H */

