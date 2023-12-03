#ifndef VERTEX_BUFFER_LAYOUT_H
#define VERTEX_BUFFER_LAYOUT_H

#include <vector>
#include <string>
#include <glad/glad.h>
#include <ShaderDataTypes.h>

struct BufferAttribute {
    // Constructor
    BufferAttribute(ShaderDataType type, const std::string &name, GLboolean normalized = false)
        : Name(name), Type(type), Size(ShaderDataTypeSize(type)), Offset(0),
            Normalized(normalized) {}

    std::string Name;
    ShaderDataType Type;
    GLuint Size;
    GLuint Offset;
    GLboolean Normalized;
};

class BufferLayout {
public:
    BufferLayout() {}
    BufferLayout(const std::initializer_list<BufferAttribute> &attributes)
        : Attributes(attributes) {
        this->CalculateOffsetAndStride();
    }

    inline const std::vector<BufferAttribute>& GetAttributes() const { return this->Attributes; }
    inline GLsizei GetStride() const { return this->Stride; }

    std::vector<BufferAttribute>::iterator begin() { return this->Attributes.begin(); }
    std::vector<BufferAttribute>::iterator end() { return this->Attributes.end(); }
    std::vector<BufferAttribute>::const_iterator begin() const { return this->Attributes.begin(); }
    std::vector<BufferAttribute>::const_iterator end() const { return this->Attributes.end(); }

private:
    void CalculateOffsetAndStride() {
        GLsizei offset = 0;
        this->Stride = 0;
        for (auto &attribute : Attributes) {
            attribute.Offset = offset;
            offset += attribute.Size;
            this->Stride += attribute.Size;
        }
    }

private:
    std::vector<BufferAttribute> Attributes;
    GLsizei Stride;
};

#endif
