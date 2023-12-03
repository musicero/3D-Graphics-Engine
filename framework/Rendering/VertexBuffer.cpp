#include "VertexBuffer.h"
#include "iostream"
#include <glad/glad.h>

VertexBuffer::VertexBuffer(const void *data, GLsizei size) {
  glGenBuffers(1, &VertexBufferID);
  glBindBuffer(GL_ARRAY_BUFFER, VertexBufferID);
  glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
}

VertexBuffer::~VertexBuffer() {
  if (glIsBuffer(VertexBufferID) == GL_TRUE) {
    glDeleteBuffers(1, &VertexBufferID);
  } else {
    std::cout << "VertexBuffer Error" << std::endl;
    GLenum error = glGetError();
    if (error != GL_NO_ERROR) {
      std::cerr << "OpenGL error in vertexbuffer destructor: " << error
                << std::endl;
    }
  }
}

void VertexBuffer::Bind() const {
  glBindBuffer(GL_ARRAY_BUFFER, VertexBufferID);
}

void VertexBuffer::Unbind() const { glBindBuffer(GL_ARRAY_BUFFER, 0); }

void VertexBuffer::BufferSubData(GLintptr offset, GLsizeiptr size,
                                 const void *data) const {
  glBufferSubData(GL_ARRAY_BUFFER, offset, size, data);
}
