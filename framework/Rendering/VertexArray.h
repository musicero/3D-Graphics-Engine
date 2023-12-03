#ifndef VERTEX_ARRAY_H
#define VERTEX_ARRAY_H

#include <memory>
#include <VertexBuffer.h>
#include <IndexBuffer.h>

class VertexArray {
public:
    // Constructor & Destructor
    VertexArray();
    ~VertexArray();

    // Bind vertex array
    void Bind() const;
    // Unbind vertex array
    void Unbind() const;

    // Add vertex buffer. This method utilizes the BufferLayout internal to
    // the vertex buffer to set up the vertex attributes. Notice that
    // this function opens for the definition of several vertex buffers.
    void AddVertexBuffer(const std::shared_ptr<VertexBuffer> &vertexBuffer);
    // Set index buffer
    void SetIndexBuffer(const std::shared_ptr<IndexBuffer> &indexBuffer);

    // Get the index buffer
    const std::shared_ptr<IndexBuffer> &GetIndexBuffer() const { return IdxBuffer; }

private:
    GLuint vertexArrayID;
    std::vector<std::shared_ptr<VertexBuffer>> VertexBuffers;
    std::shared_ptr<IndexBuffer> IdxBuffer;

    // Get the vertex buffers
    const std::vector<std::shared_ptr<VertexBuffer>> &GetVertexBuffers() const { return VertexBuffers; }
};
#endif
