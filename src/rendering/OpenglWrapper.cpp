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
#include <iostream>
#include <sstream>
#include <vector>

#define DONT_SIPHON_TYPES
#include "rendering/OpenglWrapper.h"

template <class T>
std::string type();
template <> std::string type<long>() { return "long";}
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
value(T v) { return std::to_string(v); }
template <class T>
std::string value(const T* v) { return "{ " + std::to_string(*v) + " }"; }
template <class T>
std::string value(const T* const * v) { return "**(" + std::to_string(**v) + ")"; }
template<> std::string value<void>(const void* v) { return "Cannot access void ptr data"; }

template <class...Args>
std::vector<std::pair<std::string, std::string>> types_values(Args&&... args) {
  return {
        {
            type<typename std::decay<typename std::remove_pointer<Args>::type>::type>()
          , value(args)
        }...
    };
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
  auto tvs_ = types_values(__VA_ARGS__); \
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
  FUNC_BODY(glGetIntegerv, pname, params);
}
GLAPI GLuint APIENTRY glCreateProgram (void) {
  FUNC_BODY(glCreateProgram);
}
GLAPI GLuint APIENTRY glCreateShader (GLenum type) {
  FUNC_BODY(glCreateShader, type);
}
GLAPI void APIENTRY glShaderSource (GLuint shader, GLsizei count, const GLchar* const *string, const GLint *length) {
  FUNC_BODY(glShaderSource, shader, count, string, length);
}
GLAPI void APIENTRY glCompileShader (GLuint shader) {
  FUNC_BODY(glCompileShader, shader);
}
GLAPI void APIENTRY glGetShaderiv (GLuint shader, GLenum pname, GLint *params) {
  FUNC_BODY(glGetShaderiv, shader, pname, params);
}
GLAPI void APIENTRY glGetShaderInfoLog (GLuint shader, GLsizei bufSize, GLsizei *length, GLchar *infoLog) {
  FUNC_BODY(glGetShaderInfoLog, shader, bufSize, length, infoLog);
}
GLAPI void APIENTRY glDeleteShader (GLuint shader) {
  FUNC_BODY(glDeleteShader, shader);
}
GLAPI void APIENTRY glUseProgram (GLuint program) {
  FUNC_BODY(glUseProgram, program);
}
GLAPI void APIENTRY glGenVertexArrays (GLsizei n, GLuint *arrays) {
  FUNC_BODY(glGenVertexArrays, n, arrays);
}
GLAPI void APIENTRY glBindVertexArray (GLuint array) {
  FUNC_BODY(glBindVertexArray, array);
}
GLAPI void APIENTRY glGenBuffers (GLsizei n, GLuint *buffers) {
  FUNC_BODY(glGenBuffers, n, buffers);
}
GLAPI void APIENTRY glBindBuffer (GLenum target, GLuint buffer) {
  FUNC_BODY(glBindBuffer, target, buffer);
}
GLAPI void APIENTRY glBufferData (GLenum target, GLsizeiptr size, const GLvoid *data, GLenum usage) {
  FUNC_BODY(glBufferData, target, size, data, usage);
}
GLAPI void APIENTRY glAttachShader (GLuint program, GLuint shader) {
  FUNC_BODY(glAttachShader, program, shader);
}
GLAPI void APIENTRY glVertexAttribPointer (GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const GLvoid *pointer) {
  FUNC_BODY(glVertexAttribPointer, index, size, type, normalized, stride, pointer);
}
GLAPI void APIENTRY glEnableVertexAttribArray (GLuint index) {
  FUNC_BODY(glEnableVertexAttribArray, index);
}
GLAPI void APIENTRY glDeleteVertexArrays (GLsizei n, const GLuint *arrays) {
  FUNC_BODY(glDeleteVertexArrays, n, arrays);
}
GLAPI void APIENTRY glLinkProgram (GLuint program) {
  FUNC_BODY(glLinkProgram, program);
}
GLAPI void APIENTRY glGetProgramiv (GLuint program, GLenum pname, GLint *params) {
  FUNC_BODY(glGetProgramiv, program, pname, params);
}
GLAPI void APIENTRY glGetProgramInfoLog (GLuint program, GLsizei bufSize, GLsizei *length, GLchar *infoLog) {
  FUNC_BODY(glGetProgramInfoLog, program, bufSize, length, infoLog);
}
GLAPI void APIENTRY glDeleteProgram (GLuint program) {
  FUNC_BODY(glDeleteProgram, program);
}
GLAPI void APIENTRY glDeleteBuffers (GLsizei n, const GLuint *buffers) {
  FUNC_BODY(glDeleteBuffers, n, buffers);
}
GLAPI GLint APIENTRY glGetUniformLocation (GLuint program, const GLchar *name) {
  FUNC_BODY(glGetUniformLocation, program, name);
}
GLAPI void APIENTRY glUniformMatrix4fv (GLint location, GLsizei count, GLboolean transpose, const GLfloat *value) {
  FUNC_BODY(glUniformMatrix4fv, location, count, transpose, value);
}
GLAPI void APIENTRY glDrawArrays (GLenum mode, GLint first, GLsizei count) {
  FUNC_BODY(glDrawArrays, mode, first, count);
}
GLAPI void APIENTRY glDrawElements (GLenum mode, GLsizei count, GLenum type, const GLvoid *indices) {
  FUNC_BODY(glDrawElements, mode, count, type, indices);
}

}