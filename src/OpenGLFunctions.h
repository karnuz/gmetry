#ifndef _OPENGLFUNCTIONS_H_
#define _OPENGLFUNCTIONS_H_

#include <GL/glew.h>

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif
#ifdef __APPLE__
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#endif

#include <iostream>

using namespace std;

namespace util {
  
class OpenGLFunctions {

public:
  OpenGLFunctions() {
    //glutInit();
    glewExperimental = GL_TRUE;
    auto r = glewInit();
    if(r != GLEW_OK) {
      std::cout<< "glew not initialized\n";
      return;
    }
    std::cout << "Glew Initializeddd\n";
  }
  
  void glDeleteShader(int i) {
    ::glDeleteShader(i);
  }

  void glDeleteProgram(int i) {
    ::glDeleteProgram(i);
  }

  void glUseProgram(int i) {
    ::glUseProgram(i);
  }

  GLuint glCreateProgram() {
    GLuint g = ::glCreateProgram();
    return g;
  }

  GLuint glCreateShader(int i) {
    return ::glCreateShader(i);
  }

  void glShaderSource(GLuint shader,
		 GLsizei count,
		 const GLchar * const *string,
		 const GLint *length) {
    ::glShaderSource(shader, count, string, length);
  }

  void glCompileShader(GLuint shader) {
    ::glCompileShader(shader);
  }

  void glGetShaderiv(GLuint shader,
		     GLenum pname,
		     GLint *params) {
    ::glGetShaderiv(shader, pname, params);
  }

  void glAttachShader(GLuint program,
		      GLuint shader) {
    ::glAttachShader(program, shader);
  }

  void glLinkProgram(GLuint program) {
    ::glLinkProgram(program);
  }

  void glGetProgramiv(GLuint program,
		      GLenum pname,
		      GLint *params) {
    ::glGetProgramiv(program, pname, params);
  }

  GLint glGetUniformLocation(GLuint program,
			     const GLchar *name) {
    return ::glGetUniformLocation(program, name);
  }

  GLint glGetAttribLocation(GLuint program,
			    const GLchar *name) {
    return ::glGetAttribLocation(program, name);
  }

  void glGetShaderInfoLog(GLuint shader,
			  GLsizei maxLength,
			  GLsizei *length,
			  GLchar *infoLog) {
    ::glGetShaderInfoLog(shader, maxLength, length, infoLog);
  }

  void glGetActiveUniform(GLuint program,
			  GLuint index,
			  GLsizei bufSize,
			  GLsizei *length,
			  GLint *size,
			  GLenum *type,
			  GLchar *name) {
    ::glGetActiveUniform(program, index, bufSize, length, size, type, name);
  }

  void glGetActiveAttrib(GLuint program,
			 GLuint index,
			 GLsizei bufSize,
			 GLsizei *length,
			 GLint *size,
			 GLenum *type,
			 GLchar *name) {
    ::glGetActiveAttrib(program, index, bufSize, length, size, type, name);
  }
  

  void glClearColor(GLclampf red,
		    GLclampf green,
		    GLclampf blue,
		    GLclampf alpha) {
    ::glClearColor(red,green,blue,alpha);
  }

  void glClear(	GLbitfield mask) {
    ::glClear(mask);
  }

  void glUniform4fv(GLint location,
		    GLsizei count,
		    const GLfloat *value) {
    ::glUniform4fv(location, count, value);
  }


  void glUniformMatrix4fv(GLint location,
			  GLsizei count,
			  GLboolean transpose,
			  const GLfloat *value) {

    ::glUniformMatrix4fv(location, count, transpose, value);

  }

  void glFlush() {
    ::glFlush();
  }

  void glViewport(GLint x,
		  GLint y,
		  GLsizei width,
		  GLsizei height) {
    ::glViewport(x,y,width,height);
  }

  void glGenVertexArrays(GLsizei n,
			 GLuint *arrays) {
    ::glGenVertexArrays(n,arrays);
  }

  void glBindVertexArray(GLuint array) {
    ::glBindVertexArray(array);
  }


  void glGenBuffers(GLsizei n,
		    GLuint * buffers) {
    ::glGenBuffers(n, buffers);
  }

  void glBindBuffer(GLenum target,
		    GLuint buffer) {
    ::glBindBuffer(target, buffer);
  }

  void glBufferData(GLenum target,
		    GLsizeiptr size,
		    const void * data,
		    GLenum usage) {
    ::glBufferData(target, size, data, usage);
  }

  void glVertexAttribPointer(GLuint index,
			     GLint size,
			     GLenum type,
			     GLboolean normalized,
			     GLsizei stride,
			     const void * pointer) {
    ::glVertexAttribPointer(index, size, type, normalized, stride, pointer);
  }


  void glDeleteBuffers(GLsizei n,
		       const GLuint * buffers) {
    ::glDeleteBuffers(n, buffers);
  }

  void glDrawElements(GLenum mode,
		      GLsizei count,
		      GLenum type,
		      const void * indices) {
    ::glDrawElements(mode, count, type, indices);
  }

  void glEnableVertexAttribArray(GLuint index) {
    ::glEnableVertexAttribArray(index);
  }

  void glDeleteVertexArrays(GLsizei n,
			    const GLuint *arrays) {
    ::glDeleteVertexArrays(n, arrays);
  }








};

}

#endif
