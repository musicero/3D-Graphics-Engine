#include "VertexArray.h"
#include "ShaderDataTypes.h"

#include <memory>

#include <glad/glad.h>

VertexArray::VertexArray() { glGenVertexArrays(1, &vertexArrayID); }

VertexArray::~VertexArray() { glDeleteVertexArrays(1, &vertexArrayID); }

void VertexArray::Bind() const { glBindVertexArray(vertexArrayID); }

void VertexArray::AddVertexBuffer(
    const std::shared_ptr<VertexBuffer> &vertexBuffer) {
  Bind();
  vertexBuffer->Bind();

  const BufferLayout &layout = vertexBuffer->GetLayout();
  unsigned attributeIndex = 0;
  for (const auto &attribute : layout) {
    glEnableVertexAttribArray(attributeIndex);
    glVertexAttribPointer(
        attributeIndex, ShaderDataTypeComponentCount(attribute.Type),
        ShaderDataTypeToOpenGLBaseType(attribute.Type), attribute.Normalized,
        layout.GetStride(), (const void *)attribute.Offset);
    attributeIndex++;
  }
}

void VertexArray::SetIndexBuffer(
    const std::shared_ptr<IndexBuffer> &indexBuffer) {
  Bind();
  indexBuffer->Bind();
  IdxBuffer = indexBuffer;
}

void VertexArray::Unbind() const { glBindVertexArray(0); }
