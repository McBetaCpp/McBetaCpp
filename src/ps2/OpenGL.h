#pragma once

#include <tamtypes.h>
#include <libgs.h>

// PS2 OpenGL-like definitions for compatibility
#define GL_TEXTURE_2D           0x0DE1
#define GL_DEPTH_TEST           0x0B71
#define GL_LEQUAL               0x0203
#define GL_ALPHA_TEST           0x0BC0
#define GL_GREATER              0x0204
#define GL_BACK                 0x0405
#define GL_CULL_FACE            0x0B44
#define GL_BLEND                0x0BE2
#define GL_LIGHTING             0x0B50
#define GL_LIGHT0               0x4000
#define GL_LIGHT1               0x4001
#define GL_COLOR_MATERIAL       0x0B57
#define GL_SMOOTH               0x1D01
#define GL_FLAT                 0x1D00

#define GL_NO_ERROR             0
#define GL_INVALID_ENUM         0x0500
#define GL_INVALID_VALUE        0x0501
#define GL_INVALID_OPERATION    0x0502
#define GL_STACK_OVERFLOW       0x0503
#define GL_STACK_UNDERFLOW      0x0504
#define GL_OUT_OF_MEMORY        0x0505

#define GL_QUADS                0x0007
#define GL_LINES                0x0001
#define GL_TRIANGLES            0x0004

#define GL_COLOR_BUFFER_BIT     0x00004000
#define GL_DEPTH_BUFFER_BIT     0x00000100

#define GL_MODELVIEW            0x1700
#define GL_PROJECTION           0x1701
#define GL_TEXTURE              0x1702

#define GL_PACK_ALIGNMENT       0x0D05
#define GL_UNPACK_ALIGNMENT     0x0CF5

#define GL_BGR_EXT              0x80E0
#define GL_UNSIGNED_BYTE        0x1401

typedef int GLenum;
typedef void GLvoid;
typedef unsigned int GLuint;
typedef float GLfloat;
typedef int GLint;
typedef int GLsizei;
typedef unsigned char GLubyte;
typedef unsigned int GLbitfield;

// GL function stubs for PS2
inline GLenum glGetError() { return GL_NO_ERROR; }
inline void glEnable(GLenum cap) { }
inline void glDisable(GLenum cap) { }
inline void glShadeModel(GLenum mode) { }
inline void glClearDepth(GLfloat depth) { }
inline void glDepthFunc(GLenum func) { }
inline void glAlphaFunc(GLenum func, GLfloat ref) { }
inline void glCullFace(GLenum mode) { }
inline void glMatrixMode(GLenum mode) { }
inline void glLoadIdentity() { }
inline void glOrtho(GLfloat left, GLfloat right, GLfloat bottom, GLfloat top, GLfloat zNear, GLfloat zFar) { }
inline void glTranslatef(GLfloat x, GLfloat y, GLfloat z) { }
inline void glViewport(GLint x, GLint y, GLsizei width, GLsizei height) { }
inline void glClearColor(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha) { }
inline void glClear(GLbitfield mask) { }
inline void glColor4f(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha) { }
inline void glBindTexture(GLenum target, GLuint texture) { }
inline void glLineWidth(GLfloat width) { }
inline void glPixelStorei(GLenum pname, GLint param) { }
inline void glReadPixels(GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, GLvoid *pixels) { }
