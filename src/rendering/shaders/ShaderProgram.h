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

#ifndef RENDERING_SHADERS_SHADERPROGRAM_H
#define RENDERING_SHADERS_SHADERPROGRAM_H

#include "data/AbstractBufferTypes.h"
#include "math/Matrix4.h"
#include "rendering/shaders/Shader.h"

#include <limits>
#include <map>
#include <memory>
#include <iostream>
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
      // TODO ugly hack
      if (std::string(Head::key) == "pass") {
        map["pass"] = std::numeric_limits<size_t>::max();
      }
      if (map.count(Head::key) == 0) {
        throw std::runtime_error("Shader does not support required buffer");
      }
      managers.emplace_back(std::make_shared<Head>(map.at(Head::key)));
      map.erase(Head::key);
      if (sizeof...(Args) > 0) {
        instantiate_managers_helper<Args...>::foo(managers, map);
      }
    }
  };

  template <class > struct instantiate_managers;
  template <class... Args>
  struct instantiate_managers<std::tuple<Args...>> {
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

class ShaderProgram {
public:
  ShaderProgram(const ShaderProgram&) = delete;
	ShaderProgram& operator()(ShaderProgram&) = delete;
  ShaderProgram(
      std::unique_ptr<VertexShader> vert,
      std::unique_ptr<FragmentShader> frag);

  virtual ~ShaderProgram();

	void UseProgram() const { glUseProgram(program_); }
  void uploadMatrix(const Math::Matrix4& mat) const;

  template <class Interface>
  std::vector<std::shared_ptr<Data::AbstractManager>> BufferMapping() const {
    auto map = vert_shader_->layout_map();
    return detail::instantiate_managers<Interface>::foo(map);
  }

private:
	std::unique_ptr<VertexShader> vert_shader_;
	std::unique_ptr<FragmentShader> frag_shader_;
	GLuint program_;
};

}}} // ShapeShifter::Rendering::Shaders

#endif /* RENDERING_SHADERS_SHADERPROGRAM_H */

