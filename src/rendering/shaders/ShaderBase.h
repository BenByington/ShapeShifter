/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   Shader.h
 * Author: bbyington
 *
 * Created on July 29, 2016, 8:38 AM
 */

#ifndef RENDERING_SHADERS_SHADER_BASE_H
#define RENDERING_SHADERS_SHADER_BASE_H

#include "rendering/shaders/InterfaceVariableBase.h"

#include <map>
#include <string>

#include <opengl/gl3.h>

namespace ShapeShifter {
namespace Rendering {
namespace Shaders {

/*
 * Class that wraps an opengl shader resource.  It expects the shader to
 * program to live in a separate text file, which it will load and parse
 * and compile.  Must use one of the children classes to actually instantiate.
 */
class ShaderBase {
protected:
  ShaderBase(const std::string& data, GLenum shader_type);
  ShaderBase(const ShaderBase&) = delete;
	ShaderBase& operator=(const ShaderBase&) = delete;
public:
  virtual ~ShaderBase();

public:
	operator GLuint() const {return shader;}
  const std::map<std::string, size_t>& layout_map() const {
    return layout_map_;
  }
private:
  GLuint shader = 0;

  void ParseLayouts(const std::string& data);

  std::map<std::string, size_t> layout_map_;
};

}}} // ShapeShifter::Rendering::Shaders
#endif /* RENDERING_SHADERS_SHADER_BASE_H */

