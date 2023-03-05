/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   IndentedStringStream.h
 * Author: bbyington
 *
 * Created on November 15, 2016, 8:27 AM
 */

#ifndef RENDERING_SHADERS_LANGUAGE_INDENTED_STRINGSTREAM_H
#define RENDERING_SHADERS_LANGUAGE_INDENTED_STRINGSTREAM_H

#include <cassert>
#include <memory>
#include <sstream>

namespace ShapeShifter::Rendering::Shaders::Language {

/*
 * Convenience class for managing indententaion levels in generated
 * GLSL shader code.  Not meant for standalone use.
 */
class IndentedStringStream {
public:
  IndentedStringStream() = default;
  IndentedStringStream(const IndentedStringStream&) = delete;
  IndentedStringStream(IndentedStringStream&&) = default;
  IndentedStringStream& operator=(const IndentedStringStream&) = delete;
  IndentedStringStream& operator=(IndentedStringStream&&) = default;

  template <class T>
  std::stringstream& operator<<(T&& t) {
    for (size_t i = 0; i < indentation_; ++i) stream_ << "  ";
    stream_ << t;
    return stream_;
  }

  void incIndent() { ++indentation_; }
  void decIndent() { assert(indentation_ >= 0); --indentation_; }
  std::string str() { return stream_.str(); }

private:
  std::stringstream stream_;
  size_t indentation_ = 0;
};


} // ShapeShifter::Rendering::Shaders::Language

#endif /* RENDERING_SHADERS_LANGUAGE_INDENTED_STRINGSTREAM_H */

