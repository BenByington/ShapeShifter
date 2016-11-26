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

// Declare this, so that we don't overwrite the #defines and etc from the real
// gl3.h.  Probably unecessary, but I just want to be safe.
#define DONT_SIPHON_TYPES
#include "rendering/OpenglWrapper.h"

// Defines used to control what extra debugging steps occur
// The last three are subsets of each other.  LOG_PARAMETERS will enable
// LOG_FUNCTIONS, and DETAIL_LOG_PARAMETERS will enable both the others.
//#define ERROR_CHECKING
//#define LOG_FUNCTIONS
//#define LOG_PARAMETERS
//#define DETAIL_LOG_PARAMETERS

// Ugly hack, but in the count_map, we will toss in a GL_ENUM instead of
// a count, so that sections that took a void pointer to a single value
// can decode the type for printing.
static constexpr size_t TYPE_IDX = std::numeric_limits<size_t>::max();

// Type to string conversions for pretty printing
// Warning: For array types, there is some magic going on here, where we omit the
// [] now but add it to the value printing.
template <class T>
std::string type();
template <> std::string type<long>() { return "long";}
template <> std::string type<unsigned long>() { return "unsigned long";}
template <> std::string type<uint32_t>() { return "uint32_t";}
template <> std::string type<int32_t>() { return "int32_t";}
template <> std::string type<char const* const*>() { return "const char*";}
template <> std::string type<char const*>() { return "const char*";}
template <> std::string type<char*>() { return "const char*";}
template <> std::string type<uint8_t>() { return "uint8_t";}
template <> std::string type<float const*>() { return "float";}
template <> std::string type<int const*>() { return "int";}
template <> std::string type<int*>() { return "int";}
template <> std::string type<uint32_t const*>() { return "uint32_t";}
template <> std::string type<uint32_t *>() { return "uint32_t";}
template <> std::string type<void const*>() { return "void*";}

/*
 * Classes for printing out values.  There are a number of special cases to
 * handled.  Normal types can just be printed, but we also need to know when
 * we are a pointer so that we can print out the full array, or a pointer to
 * char so we can print out a string, or even a double pointer to char so
 * that we can print out an array of strings.  The 'count' parameter is only
 * used for printing out arrays, but is included in all functions for symmetry.
 * Note: An exception to that is the 'count' parameter for the void* function
 * really is a GL_ENUM with type information.
 * 
 * Note: We also print the '=' sign here, since for some cases we are going to
 * need to place a [] after the variable name and before the '='
 */
template <class T>
typename std::enable_if<!std::is_pointer<T>::value, std::string>::type
value(size_t count, T v) { return " = " + std::to_string(v); }

template <class T>
std::string value(size_t count, const T* v) {
#ifdef DETAIL_LOG_PARAMETERS
  size_t max = count;
  std::string cont = "";
#else
  size_t max = std::min(9ul, count);
  std::string cont = max < count ? ", ..." : "";
#endif

  if (count == 0) return "[] = {}";
  std::stringstream ss;
  ss << "[] = { ";
  for (size_t i = 0; i < max-1; ++i) {
    ss << v[i] << ", ";
  }
  ss << v[max-1] << cont << " }";
  return ss.str();

}

template <>
std::string value<char>(size_t count, const char* v) {
  assert(count == 1);
  return " = \"" + std::string(v) + "\"";
}

std::string value(size_t count, const char* const * v) {
#ifdef DETAIL_LOG_PARAMETERS
  size_t max = count;
  std::string cont = "";
#else
  size_t max = std::min(9ul, count);
  std::string cont = max < count ? ", ..." : "";
#endif

  if (count == 0) return "[] = {}";
  std::stringstream ss;
  ss << "[] = { ";
  for (size_t i = 0; i < max-1; ++i) {
    ss << "R\"(" << v[i] << ")\"" << ", ";
  }
  ss << "R\"(" << v[max-1] << ")\"" << cont << " }";
  return ss.str();
}

template<> std::string value<void>(size_t count, const void* v) {
  switch(count) {
    case GL_FLOAT:
      assert(size_t(v) / sizeof(float) == 0);
      return " = " + std::to_string((size_t(v)) / sizeof(float)) + " // (float)";
    case GL_UNSIGNED_INT:
      assert(size_t(v) / sizeof(uint32_t) == 0);
      return " = " + std::to_string((size_t(v)) / sizeof(uint32_t)) + " //(uint32)";
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
  // Extract the count information if it exists.  Be default it will always be 1.
  auto count = [&](size_t index) -> size_t{
    if (count_map.count(index)) {
      return count_map[index];
    } else {
      return 1;
    }
  };

  return {{
      type<typename std::decay<typename std::remove_pointer<Args>::type>::type>(),
      value(count(Indexes), args)
  }...};
}

struct types_values {
  types_values(const std::map<size_t, size_t>& map) : count_map(map) {}
  template <class...Args>
  std::vector<std::pair<std::string, std::string>>
  operator()(Args&&... args) {
    constexpr auto idxs = std::make_index_sequence<sizeof...(args)>();
    return types_values_helper(count_map, idxs, args...);
  }
  const std::map<size_t, size_t>& count_map;
};

std::vector<std::string> param_list(std::string raw) {
  std::vector<std::string> ret;
  std::replace(raw.begin(), raw.end(), ',', ' ');
  std::stringstream stream(raw);
  std::string token;
  while (stream >> token) ret.push_back(token);
  return ret;
}

void check(bool first) {
    for(GLenum err; (err = glGetError()) != GL_NO_ERROR;)
    {
      if(first) {
        std::cerr << "unexpected error...! Are there unwrapped opengl calls?\n";
      }
      std::cerr << "Encountered opengl error: " << err << std::endl;
    }
}

#if defined(ERROR_CHECKING)
  #define CHECK(first) check(first);
#else
  #define CHECK(first)
#endif

#if defined(LOG_PARAMETERS) or defined(DETAIL_LOG_PARAMETERS)
  #define PRINT_PARAMS(...) \
    auto param_names_ = param_list(#__VA_ARGS__); \
    auto tvs_ = types_values(count_map)(__VA_ARGS__); \
    for (size_t i = 0; i < param_names_.size(); ++i)  \
      std::cerr << "  " << tvs_[i].first << " " \
          << param_names_[i]  \
          << tvs_[i].second << ";\n";
#else
  #define PRINT_PARAMS(...)
#endif

#if defined(LOG_PARAMETERS) or defined(DETAIL_LOG_PARAMETERS)
  #define PRINT_RETURN \
    std::cerr << "  return: " << std::to_string(ret) << std::endl;
#else
  #define PRINT_RETURN
#endif

#if defined(LOG_FUNCTIONS) or defined(LOG_PARAMETERS) or defined(DETAIL_LOG_PARAMETERS)
  #define PRINT_CALL(function, ...) \
    std::cerr << "  " << #function << "(" << #__VA_ARGS__ << ");\n";
#else
  #define PRINT_CALL(function, ...)
#endif

#if defined(LOG_FUNCTIONS) or defined(LOG_PARAMETERS) or defined(DETAIL_LOG_PARAMETERS)
  #define OPEN_SCOPE std::cerr << "\n{\n";
  #define CLOSE_SCOPE std::cerr << "}\n";
#else
  #define OPEN_SCOPE
  #define CLOSE_SCOPE
#endif

// ISSUE: clean all this up.  All the MACROS called from here can be functions
//        instead.  We only needed the macro here for the '::function' line. 
//        Everything else can be runtime string manipulations.
#define FUNC_BODY(function, ...)  \
  CHECK(true) \
  ::function(__VA_ARGS__); \
  CHECK(false) \
  OPEN_SCOPE \
  PRINT_PARAMS(__VA_ARGS__); \
  PRINT_CALL(function, __VA_ARGS__); \
  CLOSE_SCOPE \
  return;

#define FUNC_BODY_RETURN(function, ...)  \
  CHECK(true) \
  auto ret = ::function(__VA_ARGS__); \
  CHECK(false) \
  OPEN_SCOPE \
  PRINT_PARAMS(__VA_ARGS__); \
  PRINT_CALL(function, __VA_ARGS__); \
  PRINT_RETURN \
  CLOSE_SCOPE \
  return ret;

namespace ShapeShifter {
void glGetIntegerv (GLenum pname, GLint *params) {
  std::map<size_t, size_t> count_map;
  FUNC_BODY(glGetIntegerv, pname, params);
}
GLuint glCreateProgram (void) {
  std::map<size_t, size_t> count_map;
  FUNC_BODY_RETURN(glCreateProgram);
}
GLuint glCreateShader (GLenum type) {
  std::map<size_t, size_t> count_map;
  FUNC_BODY_RETURN(glCreateShader, type);
}
void glShaderSource (GLuint shader, GLsizei count, const GLchar* const *string, const GLint *length) {
  std::map<size_t, size_t> count_map {{3, count}};
  FUNC_BODY(glShaderSource, shader, count, string, length);
}
void glCompileShader (GLuint shader) {
  std::map<size_t, size_t> count_map;
  FUNC_BODY(glCompileShader, shader);
}
void glGetShaderiv (GLuint shader, GLenum pname, GLint *params) {
  std::map<size_t, size_t> count_map;
  FUNC_BODY(glGetShaderiv, shader, pname, params);
}
void glGetShaderInfoLog (GLuint shader, GLsizei bufSize, GLsizei *length, GLchar *infoLog) {
  std::map<size_t, size_t> count_map;
  FUNC_BODY(glGetShaderInfoLog, shader, bufSize, length, infoLog);
}
void glDeleteShader (GLuint shader) {
  std::map<size_t, size_t> count_map;
  FUNC_BODY(glDeleteShader, shader);
}
void glUseProgram (GLuint program) {
  std::map<size_t, size_t> count_map;
  FUNC_BODY(glUseProgram, program);
}
void glGenVertexArrays (GLsizei n, GLuint *arrays) {
  std::map<size_t, size_t> count_map {{1,n}};
  FUNC_BODY(glGenVertexArrays, n, arrays);
}
void glBindVertexArray (GLuint array) {
  std::map<size_t, size_t> count_map;
  FUNC_BODY(glBindVertexArray, array);
}
void glGenBuffers (GLsizei n, GLuint *buffers) {
  std::map<size_t, size_t> count_map {{1,n}};
  FUNC_BODY(glGenBuffers, n, buffers);
}
void glBindBuffer (GLenum target, GLuint buffer) {
  std::map<size_t, size_t> count_map;
  FUNC_BODY(glBindBuffer, target, buffer);
}
void glBufferData (GLenum target, const std::vector<float>& data, GLenum usage) {
  std::map<size_t, size_t> count_map {{2, data.size()}};
  auto size = data.size()*sizeof(float);
  auto dat = data.data();
  FUNC_BODY(glBufferData, target, size, dat, usage);
}
void glBufferData (GLenum target, const std::vector<uint32_t>& data, GLenum usage) {
  std::map<size_t, size_t> count_map {{2, data.size()}};
  auto size = data.size()*sizeof(uint32_t);
  auto dat = data.data();
  FUNC_BODY(glBufferData, target, size, dat, usage);
}
void glAttachShader (GLuint program, GLuint shader) {
  std::map<size_t, size_t> count_map;
  FUNC_BODY(glAttachShader, program, shader);
}
void glVertexAttribPointer (GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const GLvoid *pointer) {
  std::map<size_t, size_t> count_map {{5,type}};
  FUNC_BODY(glVertexAttribPointer, index, size, type, normalized, stride, pointer);
}
void glEnableVertexAttribArray (GLuint index) {
  std::map<size_t, size_t> count_map;
  FUNC_BODY(glEnableVertexAttribArray, index);
}
void glDeleteVertexArrays (GLsizei n, const GLuint *arrays) {
  std::map<size_t, size_t> count_map {{1,n}};
  FUNC_BODY(glDeleteVertexArrays, n, arrays);
}
void glLinkProgram (GLuint program) {
  std::map<size_t, size_t> count_map;
  FUNC_BODY(glLinkProgram, program);
}
void glGetProgramiv (GLuint program, GLenum pname, GLint *params) {
  std::map<size_t, size_t> count_map;
  FUNC_BODY(glGetProgramiv, program, pname, params);
}
void glGetProgramInfoLog (GLuint program, GLsizei bufSize, GLsizei *length, GLchar *infoLog) {
  std::map<size_t, size_t> count_map;
  FUNC_BODY(glGetProgramInfoLog, program, bufSize, length, infoLog);
}
void glDeleteProgram (GLuint program) {
  std::map<size_t, size_t> count_map;
  FUNC_BODY(glDeleteProgram, program);
}
void glDeleteBuffers (GLsizei n, const GLuint *buffers) {
  std::map<size_t, size_t> count_map {{1,n}};
  FUNC_BODY(glDeleteBuffers, n, buffers);
}
GLint glGetUniformLocation (GLuint program, const GLchar *name) {
  std::map<size_t, size_t> count_map;
  FUNC_BODY_RETURN(glGetUniformLocation, program, name);
}
void glUniformMatrix4fv (GLint location, GLsizei count, GLboolean transpose, const GLfloat *value) {
  std::map<size_t, size_t> count_map {{3,16*count}};
  FUNC_BODY(glUniformMatrix4fv, location, count, transpose, value);
}
void glDrawArrays (GLenum mode, GLint first, GLsizei count) {
  std::map<size_t, size_t> count_map;
  FUNC_BODY(glDrawArrays, mode, first, count);
}
void glDrawElements (GLenum mode, GLsizei count, GLenum type, const GLvoid *indices) {
  std::map<size_t, size_t> count_map {{3, type}};
  FUNC_BODY(glDrawElements, mode, count, type, indices);
}
GLboolean glIsVertexArray (GLuint array) {
  std::map<size_t, size_t> count_map;
  FUNC_BODY_RETURN(glIsVertexArray, array);
}

}
