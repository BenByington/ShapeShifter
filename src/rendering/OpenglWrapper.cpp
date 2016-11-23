/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   OpenglWrapper.cpp
 * Author: bbyington
 *
 * Created on November 20, 2016, 5:09 PM
 */

#include <gl3.h>

#include <algorithm>
#include <cassert>
#include <iostream>
#include <sstream>
#include <map>
#include <vector>

#define DONT_SIPHON_TYPES
#include "rendering/OpenglWrapper.h"

static constexpr size_t TYPE_IDX = std::numeric_limits<size_t>::max();

template <class T>
std::string type();
template <> std::string type<long>() { return "long";}
template <> std::string type<unsigned long>() { return "unsigned long";}
template <> std::string type<uint32_t>() { return "uint32_t";}
template <> std::string type<int32_t>() { return "int32_t";}
template <> std::string type<char const* const*>() { return "char**";}
template <> std::string type<char const*>() { return "char*";}
template <> std::string type<char*>() { return "char*";}
template <> std::string type<uint8_t>() { return "uint8_t";}
template <> std::string type<float const*>() { return "float*";}
template <> std::string type<int const*>() { return "int*";}
template <> std::string type<int*>() { return "int*";}
template <> std::string type<uint32_t const*>() { return "uint32_t*";}
template <> std::string type<uint32_t *>() { return "uint32_t*";}
template <> std::string type<void const*>() { return "void*";}

template <class T>
typename std::enable_if<!std::is_pointer<T>::value, std::string>::type
value(size_t count, T v) { return std::to_string(v); }

template <class T>
std::string value(size_t count, const T* v) {
  if (count == 0) return "{}";
  std::stringstream ss;
  ss << "{ ";
  for (size_t i = 0; i < count-1; ++i) {
    ss << v[i] << ", ";
  }
  ss << v[count-1] << " }";
  return ss.str();

}

template <>
std::string value<char>(size_t count, const char* v) {
  assert(count == 1);
  return "\"" + std::string(v) + "\"";
}


std::string value(size_t count, const char* const * v) {
  if (count == 0) return "{}";
  std::stringstream ss;
  ss << "{ ";
  for (size_t i = 0; i < count-1; ++i) {
    ss << v[i] << ", ";
  }
  ss << v[count-1] << " }";
  return ss.str();
}

template<> std::string value<void>(size_t count, const void* v) {
  switch(count) {
    case GL_FLOAT:
      assert(size_t(v) / sizeof(float) == 0);
      return "(float)" + std::to_string((size_t(v)) / sizeof(float));
    case GL_UNSIGNED_INT:
      assert(size_t(v) / sizeof(uint32_t) == 0);
      return "(uint32_t)" + std::to_string((size_t(v)) / sizeof(uint32_t));
    default:
      std::cerr << "unhandled type in void printing!\n";
      assert(false);
      return "";
  }
}

template <size_t... Indexes, class... Args>
std::vector<std::pair<std::string, std::string>>
types_values_helper(
    std::map<size_t, size_t> count_map,
    std::index_sequence<Indexes...> idxs,
    Args&&... args) {
  auto count = [&](size_t index) -> size_t{
    if (count_map.count(index)) {
      return count_map[index];
    } else {
      return 1;
    }
  };
  return {
        {
            type<typename std::decay<typename std::remove_pointer<Args>::type>::type>()
          , value(count(Indexes), args)
        }...
    };
}

template <class...Args>
std::vector<std::pair<std::string, std::string>>
types_values(std::map<size_t, size_t> count_map, Args&&... args) {
  constexpr auto idxs = std::make_index_sequence<sizeof...(args)>();
  return types_values_helper(count_map, idxs, args...);
}

std::vector<std::string> param_list(std::string raw) {
  std::vector<std::string> ret;

  std::replace(raw.begin(), raw.end(), ',', ' ');
  std::stringstream stream(raw);
  std::string token;
  while (stream >> token) ret.push_back(token);
  return ret;
}

#define PRINT_PARAMS(...) \
  auto param_names_ = param_list(#__VA_ARGS__); \
  auto tvs_ = types_values(count_map, ##__VA_ARGS__); \
  for (size_t i = 0; i < param_names_.size(); ++i)  \
    std::cerr << tvs_[i].first << " " \
        << param_names_[i]  << " = " \
        << tvs_[i].second << std::endl;

#define PRINT_CALL(function, ...) \
  std::cerr << #function << "(" << #__VA_ARGS__ << ");\n";

#define FUNC_BODY(function, ...)  \
  std::cerr << std::endl; \
  PRINT_PARAMS(__VA_ARGS__); \
  PRINT_CALL(function, __VA_ARGS__); \
  return ::function(__VA_ARGS__);

namespace ShapeShifter {
GLAPI void APIENTRY glGetIntegerv (GLenum pname, GLint *params) {
  std::map<size_t, size_t> count_map;
  FUNC_BODY(glGetIntegerv, pname, params);
}
GLAPI GLuint APIENTRY glCreateProgram (void) {
  std::map<size_t, size_t> count_map;
  FUNC_BODY(glCreateProgram);
}
GLAPI GLuint APIENTRY glCreateShader (GLenum type) {
  std::map<size_t, size_t> count_map;
  FUNC_BODY(glCreateShader, type);
}
GLAPI void APIENTRY glShaderSource (GLuint shader, GLsizei count, const GLchar* const *string, const GLint *length) {
  std::map<size_t, size_t> count_map {{3, count}};
  FUNC_BODY(glShaderSource, shader, count, string, length);
}
GLAPI void APIENTRY glCompileShader (GLuint shader) {
  std::map<size_t, size_t> count_map;
  FUNC_BODY(glCompileShader, shader);
}
GLAPI void APIENTRY glGetShaderiv (GLuint shader, GLenum pname, GLint *params) {
  std::map<size_t, size_t> count_map;
  FUNC_BODY(glGetShaderiv, shader, pname, params);
}
GLAPI void APIENTRY glGetShaderInfoLog (GLuint shader, GLsizei bufSize, GLsizei *length, GLchar *infoLog) {
  std::map<size_t, size_t> count_map;
  FUNC_BODY(glGetShaderInfoLog, shader, bufSize, length, infoLog);
}
GLAPI void APIENTRY glDeleteShader (GLuint shader) {
  std::map<size_t, size_t> count_map;
  FUNC_BODY(glDeleteShader, shader);
}
GLAPI void APIENTRY glUseProgram (GLuint program) {
  std::map<size_t, size_t> count_map;
  FUNC_BODY(glUseProgram, program);
}
GLAPI void APIENTRY glGenVertexArrays (GLsizei n, GLuint *arrays) {
  std::map<size_t, size_t> count_map {{1,n}};
  FUNC_BODY(glGenVertexArrays, n, arrays);
}
GLAPI void APIENTRY glBindVertexArray (GLuint array) {
  std::map<size_t, size_t> count_map;
  FUNC_BODY(glBindVertexArray, array);
}
GLAPI void APIENTRY glGenBuffers (GLsizei n, GLuint *buffers) {
  std::map<size_t, size_t> count_map {{1,n}};
  FUNC_BODY(glGenBuffers, n, buffers);
}
GLAPI void APIENTRY glBindBuffer (GLenum target, GLuint buffer) {
  std::map<size_t, size_t> count_map;
  FUNC_BODY(glBindBuffer, target, buffer);
}
GLAPI void APIENTRY glBufferData (GLenum target, const std::vector<float>& data, GLenum usage) {
  std::map<size_t, size_t> count_map {{2, data.size()}};
  FUNC_BODY(glBufferData, target, data.size()*sizeof(float), data.data(), usage);
}
GLAPI void APIENTRY glBufferData (GLenum target, const std::vector<uint32_t>& data, GLenum usage) {
  std::map<size_t, size_t> count_map {{2, data.size()}};
  FUNC_BODY(glBufferData, target, data.size()*sizeof(uint32_t), data.data(), usage);
}
GLAPI void APIENTRY glAttachShader (GLuint program, GLuint shader) {
  std::map<size_t, size_t> count_map;
  FUNC_BODY(glAttachShader, program, shader);
}
GLAPI void APIENTRY glVertexAttribPointer (GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const GLvoid *pointer) {
  std::map<size_t, size_t> count_map {{5,type}};
  FUNC_BODY(glVertexAttribPointer, index, size, type, normalized, stride, pointer);
}
GLAPI void APIENTRY glEnableVertexAttribArray (GLuint index) {
  std::map<size_t, size_t> count_map;
  FUNC_BODY(glEnableVertexAttribArray, index);
}
GLAPI void APIENTRY glDeleteVertexArrays (GLsizei n, const GLuint *arrays) {
  std::map<size_t, size_t> count_map {{1,n}};
  FUNC_BODY(glDeleteVertexArrays, n, arrays);
}
GLAPI void APIENTRY glLinkProgram (GLuint program) {
  std::map<size_t, size_t> count_map;
  FUNC_BODY(glLinkProgram, program);
}
GLAPI void APIENTRY glGetProgramiv (GLuint program, GLenum pname, GLint *params) {
  std::map<size_t, size_t> count_map;
  FUNC_BODY(glGetProgramiv, program, pname, params);
}
GLAPI void APIENTRY glGetProgramInfoLog (GLuint program, GLsizei bufSize, GLsizei *length, GLchar *infoLog) {
  std::map<size_t, size_t> count_map;
  FUNC_BODY(glGetProgramInfoLog, program, bufSize, length, infoLog);
}
GLAPI void APIENTRY glDeleteProgram (GLuint program) {
  std::map<size_t, size_t> count_map;
  FUNC_BODY(glDeleteProgram, program);
}
GLAPI void APIENTRY glDeleteBuffers (GLsizei n, const GLuint *buffers) {
  std::map<size_t, size_t> count_map {{1,n}};
  FUNC_BODY(glDeleteBuffers, n, buffers);
}
GLAPI GLint APIENTRY glGetUniformLocation (GLuint program, const GLchar *name) {
  std::map<size_t, size_t> count_map;
  FUNC_BODY(glGetUniformLocation, program, name);
}
GLAPI void APIENTRY glUniformMatrix4fv (GLint location, GLsizei count, GLboolean transpose, const GLfloat *value) {
  std::map<size_t, size_t> count_map {{3,16*count}};
  FUNC_BODY(glUniformMatrix4fv, location, count, transpose, value);
}
GLAPI void APIENTRY glDrawArrays (GLenum mode, GLint first, GLsizei count) {
  std::map<size_t, size_t> count_map;
  FUNC_BODY(glDrawArrays, mode, first, count);
}
GLAPI void APIENTRY glDrawElements (GLenum mode, GLsizei count, GLenum type, const GLvoid *indices) {
  std::map<size_t, size_t> count_map {{3, type}};
  FUNC_BODY(glDrawElements, mode, count, type, indices);
}

}