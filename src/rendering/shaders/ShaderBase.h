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

#include "rendering/OpenglWrapper.h"
#include <map>
#include <string>

namespace ShapeShifter::Rendering::Shaders {

/*
 * Base class that unifies both the RawShaders that parse text files, and the
 * Shaders that procedurally generate the GLSL code from c++ code.  Prefer
 * using the latter when possible.
 *
 * This class will manage the actual opengl resources
 */
class ShaderBase {
protected:
  ShaderBase(const std::string& data, GLenum shader_type);
  ShaderBase(const ShaderBase&) = delete;
  ShaderBase(ShaderBase&& other);
  ShaderBase& operator=(const ShaderBase&) = delete;
  ShaderBase& operator=(ShaderBase&& other);
public:
  virtual ~ShaderBase();

public:
  operator GLuint() const {return shader;}
  virtual const std::map<std::string, size_t>& layout_map() const = 0;
private:
  GLuint shader = 0;
};

} // ShapeShifter::Rendering::Shaders
#endif /* RENDERING_SHADERS_SHADER_BASE_H */

