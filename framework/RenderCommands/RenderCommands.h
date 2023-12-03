#ifndef RENDERCOMMANDS_H
#define RENDERCOMMANDS_H

#include "VertexArray.h"
#include "glm/glm.hpp"
#include <glad/glad.h>
#include <memory>

namespace RenderCommands
{
  inline void Clear(GLuint mode = GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT)
  {
    glClear(mode);
  }

  inline void SetPolygonMode(GLenum face, GLenum mode)
  {
    glPolygonMode(face, mode);
  }

  inline void DrawIndex(const std::shared_ptr<VertexArray>& vao, GLenum primitive = GL_TRIANGLES)
  {
    glDrawElements(primitive, vao->GetIndexBuffer()->GetCount(), GL_UNSIGNED_INT, nullptr);
  }

  inline void SetClearColor(glm::vec3 color){
	glClearColor(color.x, color.y, color.z, 1.0f);
  }

  inline void SetWireframeMode(GLenum face = GL_FRONT_AND_BACK){
	  glPolygonMode(face, GL_LINE);
  }

  inline void SetSolidMode(GLenum face = GL_FRONT_AND_BACK){
	  glPolygonMode(face, GL_FILL);
  }	


}

#endif
