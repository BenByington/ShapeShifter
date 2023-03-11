/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   OpenglWrapper.h
 * Author: bbyington
 *
 * Created on November 20, 2016, 5:09 PM
 */

#ifndef OPENGL_WRAPPER_H
#define OPENGL_WRAPPER_H

#include <cstdint>
#include <vector>

// For the sake of external code, include <gl.3> within a namespace, so they
// wont run into any function declarations, but still can take advantage of
// any macros or typedefs that would otherwise be supplied.
#ifndef DONT_SIPHON_TYPES
namespace hidden {
#include <gl3.h>
}
#endif

namespace ShapeShifter {

// Bring out some of the types we need to use.  It would be nice if I could
// just forward them from the hidden namespace rather than just redeclare them,
// but I couldn't figure out how.
#ifndef DONT_SIPHON_TYPES
using GLuint = uint32_t;
using GLvoid = void;
using GLint = int32_t;
using GLenum = uint32_t;
using GLsizei = int32_t;
using GLchar = char;
using GLsizeiptr = intptr_t;
using GLboolean = uint8_t;
using GLfloat = float;
#endif

void glGetIntegerv(GLenum pname, GLint* params);
GLuint glCreateProgram(void);
GLuint glCreateShader(GLenum type);
void glShaderSource(GLuint shader,
                    GLsizei count,
                    const GLchar* const* string,
                    const GLint* length);
void glCompileShader(GLuint shader);
void glGetShaderiv(GLuint shader, GLenum pname, GLint* params);
void glGetShaderInfoLog(GLuint shader, GLsizei bufSize, GLsizei* length, GLchar* infoLog);
void glDeleteShader(GLuint shader);
void glUseProgram(GLuint program);
void glGenVertexArrays(GLsizei n, GLuint* arrays);
void glBindVertexArray(GLuint array);
void glGenBuffers(GLsizei n, GLuint* buffers);
void glBindBuffer(GLenum target, GLuint buffer);
void glBufferData(GLenum target, const std::vector<float>& data, GLenum usage);
void glBufferData(GLenum target, const std::vector<uint32_t>& data, GLenum usage);
void glAttachShader(GLuint program, GLuint shader);
void glVertexAttribPointer(GLuint index,
                           GLint size,
                           GLenum type,
                           GLboolean normalized,
                           GLsizei stride,
                           const GLvoid* pointer);
void glEnableVertexAttribArray(GLuint index);
void glDeleteVertexArrays(GLsizei n, const GLuint* arrays);
void glLinkProgram(GLuint program);
void glGetProgramiv(GLuint program, GLenum pname, GLint* params);
void glGetProgramInfoLog(GLuint program, GLsizei bufSize, GLsizei* length, GLchar* infoLog);
void glDeleteProgram(GLuint program);
void glDeleteBuffers(GLsizei n, const GLuint* buffers);
GLint glGetUniformLocation(GLuint program, const GLchar* name);
void glUniformMatrix4fv(GLint location,
                        GLsizei count,
                        GLboolean transpose,
                        const GLfloat* value);
void glDrawArrays(GLenum mode, GLint first, GLsizei count);
void glDrawElements(GLenum mode, GLsizei count, GLenum type, const GLvoid* indices);
GLboolean glIsVertexArray(GLuint array);

} // namespace ShapeShifter
#endif /* OPENGL_WRAPPER_H */
