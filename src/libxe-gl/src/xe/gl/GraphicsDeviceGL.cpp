
#include "GraphicsDeviceGL.h"

#include "gl.h"

#include "Conversion.h"
#include "ProgramGL.h"
#include "SubsetGL.h"
#include "Texture2DArrayGL.h"
#include "Texture2DGL.h"
#include "Texture3DGL.h"
#include "TextureCubeMapGL.h"
#include "xe/DataType.h"
#include "xe/gl/TextureBaseGL.h"
#include "xe/graphics/GraphicsContext.h"
#include "xe/graphics/GraphicsDevice.h"
#include "xe/graphics/Program.h"
#include "xe/graphics/Texture2D.h"
#include "xe/graphics/Viewport.h"
#include "xe/math/Vector.h"

#include <array>
#include <cassert>
#include <cstddef>
#include <cstdio>
#include <iostream>
#include <stdexcept>
#include <string>
#include <xe/graphics/Material.h>
#include <xe/graphics/Subset.h>
#include <xe/graphics/Texture2DArray.h>
#include <xe/graphics/Texture3D.h>
#include <xe/graphics/TextureCubeMap.h>
#include <xe/graphics/Uniform.h>

namespace XE {
    static std::string hexstr(const GLenum value) {
        std::string str;
        str.resize(16, ' ');
        std::snprintf(str.data(), str.size(), "%x", value);

        return str;
    }

    static std::string stringval(const GLenum err) {
        switch (err) {
        case GL_INVALID_ENUM:
            return "GL_INVALID_ENUM";
        case GL_INVALID_VALUE:
            return "GL_INVALID_VALUE";
        case GL_INVALID_OPERATION:
            return "GL_INVALID_OPERATION";
        case GL_STACK_OVERFLOW:
            return "GL_STACK_OVERFLOW";
        case GL_STACK_UNDERFLOW:
            return "GL_STACK_UNDERFLOW";
        case GL_OUT_OF_MEMORY:
            return "GL_OUT_OF_MEMORY";
        case GL_INVALID_FRAMEBUFFER_OPERATION:
            return "GL_INVALID_FRAMEBUFFER_OPERATION";
        case GL_CONTEXT_LOST:
            return "GL_CONTEXT_LOST";

#if defined(GL_TABLE_TOO_LARGE)
        case GL_TABLE_TOO_LARGE:
            return "GL_TABLE_TOO_LARGE";
#endif

        default:
            return "UNKNOWN_ERR_CODE_" + hexstr(err);
        }
    }

    static void GraphicsDeviceGL_callback(const char *name, void * /*unused*/, int /*unused*/, ...) {
        if (std::string(name) == "glGetError") {
            return;
        }

        GLenum err = glGetError();

        if (err != GL_NO_ERROR) {
            std::cerr << "GraphicsDeviceGL: Error while calling function " << name << '\n';
            std::cerr << "GraphicsDeviceGL: Errors generated:" << '\n';

            while (err != GL_NO_ERROR) {
                std::cerr << "GraphicsDeviceGL:" << stringval(err) << '\n';
                err = glGetError();
            }

            throw std::runtime_error("GraphicsDeviceGL: Error while calling function " + std::string(name));
        }
    }

    GraphicsDeviceGL::GraphicsDeviceGL(GraphicsContext *context) : context(context) {
        assert(context);

        std::cout << "[GL] Loading OpenGL Extensions ..." << '\n';
        gladLoadGL();

#ifndef NDEBUG
        glad_set_post_callback_gl(GraphicsDeviceGL_callback);
        glad_set_post_callback(GraphicsDeviceGL_callback);
#endif
    }

    GraphicsDeviceGL::~GraphicsDeviceGL() {
    }

    Subset *GraphicsDeviceGL::createSubset(const SubsetDescriptor &desc) {
        return new SubsetGL(desc);
    }

    Buffer *GraphicsDeviceGL::createBuffer(const BufferDescriptor & /*desc*/) {
        return nullptr;
    }

    Texture2D *
    GraphicsDeviceGL::createTexture2D(const PixelFormat format, const Vector2i &size, const PixelFormat sourceFormat, const DataType sourceDataType, const void *sourceData) {
        return new Texture2DGL(format, size, sourceFormat, sourceDataType, sourceData);
    }

    Texture3D *
    GraphicsDeviceGL::createTexture3D(const PixelFormat format, const Vector3i &size, const PixelFormat sourceFormat, const DataType sourceDataType, const void *sourceData) {
        return new Texture3DGL(format, size, sourceFormat, sourceDataType, sourceData);
    }

    Texture2DArray *GraphicsDeviceGL::createTexture2DArray(const PixelFormat format, const Vector2i &size, const int count) {
        return new Texture2DArrayGL(format, size, count);
    }

    TextureCubeMap *
    GraphicsDeviceGL::createTextureCubeMap(const PixelFormat format, const Vector2i &size, const PixelFormat sourceFormat, const DataType sourceDataType, const void **sourceData) {

        const std::array<TextureCubeMapSide, 6> sides = {
            TextureCubeMapSide::PositiveX,
            TextureCubeMapSide::PositiveY,
            TextureCubeMapSide::PositiveZ,
            TextureCubeMapSide::NegativeX,
            TextureCubeMapSide::NegativeY,
            TextureCubeMapSide::NegativeZ,
        };

        return new TextureCubeMapGL(format, size, sourceFormat, sourceDataType, sides, sourceData);
    }

    Program *GraphicsDeviceGL::createProgram(const ProgramDescriptor &desc) {
        return new ProgramGL(desc);
    }

    void GraphicsDeviceGL::draw(const Subset *subset, const SubsetEnvelope *envelopes, const size_t envelopeCount) {
        assert(subset);
        assert(envelopes);
        assert(envelopeCount > 0);

        const auto *subsetGL = dynamic_cast<const SubsetGL *>(subset);

        glBindVertexArray(subsetGL->getID());

        const auto *indexBuffer = subsetGL->getIndexBuffer();

        if (indexBuffer == nullptr) {
            for (size_t i = 0; i < envelopeCount; i++) {
                const SubsetEnvelope &env = envelopes[i];
                const GLenum primitiveGL = convertToGL(env.primitive);

                glDrawArrays(primitiveGL, env.vertexStart, env.vertexCount);
            }
        } else {
            // TODO: Obtain dynamically the index data-type
            const GLenum indexTypeGL = GL_UNSIGNED_INT;

            for (size_t i = 0; i < envelopeCount; i++) {
                const SubsetEnvelope &env = envelopes[i];
                const GLenum primitiveGL = convertToGL(env.primitive);

                if (env.vertexStart == 0) {
                    glDrawElements(primitiveGL, env.vertexCount, indexTypeGL, nullptr);
                } else {
                    glDrawElementsBaseVertex(primitiveGL, env.vertexCount, indexTypeGL, nullptr, env.vertexStart);
                }
            }
        }

        glBindVertexArray(0);
    }

    void GraphicsDeviceGL::beginFrame(const ClearFlags flags, const Vector4 &color, const float depth, const int stencil) {
        GLenum clearFlagsGL = 0;

        if (flags & ClearFlags::Color) {
            clearFlagsGL |= GL_COLOR_BUFFER_BIT;
            glClearColor(color.X, color.Y, color.Z, color.W);
        }

        if (flags & ClearFlags::Depth) {
            clearFlagsGL |= GL_DEPTH_BUFFER_BIT;
            glClearDepth(static_cast<GLdouble>(depth));
        }

        if (flags & ClearFlags::Stencil) {
            clearFlagsGL |= GL_STENCIL_BUFFER_BIT;
            glClearStencil(stencil);
        }

        glClear(clearFlagsGL);
    }

    void GraphicsDeviceGL::endFrame() {
        glFlush();

        context->present();
    }

    void GraphicsDeviceGL::preRenderMaterial(const Material *material) {
        const auto &rs = material->renderState;

        if (rs.depthTest) {
            glEnable(GL_DEPTH_TEST);
        } else {
            glDisable(GL_DEPTH_TEST);
        }

        const GLenum depthFuncGL = convertToGL(rs.depthFunc);
        glDepthFunc(depthFuncGL);

        const GLenum faceGL = convertToGL(rs.frontFace);
        glFrontFace(faceGL);

        if (rs.cullBackFace) {
            glEnable(GL_CULL_FACE);
        } else {
            glDisable(GL_CULL_FACE);
        }

        glLineWidth(rs.lineWidth);

        if (rs.blendEnable) {
            glEnable(GL_BLEND);

            const GLenum sfactorGL = convertToGL(rs.blendSource);
            const GLenum dfactorGL = convertToGL(rs.blendDestination);
            glBlendFunc(sfactorGL, dfactorGL);
        } else {
            glDisable(GL_BLEND);
        }

        for (int i = 0; i < material->layerCount; i++) {
            const auto &layer = material->layers[i];

            if (layer.texture == nullptr) {
                continue;
            }

            // FIXME: This will cause segfaults if the real implementation isn't derived from the Texture/TextureBaseGL family
            const auto *textureBaseGL = dynamic_cast<const TextureBaseGL *>(layer.texture);
            auto target = textureBaseGL->GetTarget();

            glActiveTexture(GL_TEXTURE0 + i);
            glBindTexture(target, textureBaseGL->GetID());
            glTexParameteri(target, GL_TEXTURE_MAG_FILTER, convertToGL(layer.magFilter));
            glTexParameteri(target, GL_TEXTURE_MIN_FILTER, convertToGL(layer.minFilter));
            glTexParameteri(target, GL_TEXTURE_WRAP_S, convertToGL(layer.wrapS));
            glTexParameteri(target, GL_TEXTURE_WRAP_T, convertToGL(layer.wrapT));
            glTexParameteri(target, GL_TEXTURE_WRAP_R, convertToGL(layer.wrapR));
        }
    }

    void GraphicsDeviceGL::postRenderMaterial(const Material *material) {
        const auto &rs = material->renderState;

        if (rs.depthTest) {
            glDisable(GL_DEPTH_TEST);
        }

        if (rs.cullBackFace) {
            glDisable(GL_CULL_FACE);
        }

        if (rs.blendEnable) {
            glDisable(GL_BLEND);
        }

        for (int i = 0; i < material->layerCount; i++) {
            const auto &layer = material->layers[i];

            if (layer.texture == nullptr) {
                continue;
            }

            // FIXME: This will cause segfaults if the real implementation isn't derived from the Texture/TextureBaseGL family
            const auto *textureBaseGL = reinterpret_cast<const TextureBaseGL *>(layer.texture);

            glActiveTexture(GL_TEXTURE0 + i);
            glBindTexture(textureBaseGL->GetTarget(), 0);
        }

        glActiveTexture(GL_TEXTURE0);
    }

    void GraphicsDeviceGL::setMaterial(const Material *material) {
        if (m_material == material) {
            return;
        }

        if (m_material != nullptr) {
            this->postRenderMaterial(m_material);
        }

        this->preRenderMaterial(material);

        m_material = material;
    }

    void GraphicsDeviceGL::setProgram(const Program *program) {
        m_program = dynamic_cast<const ProgramGL *>(program);

        if (m_program != nullptr) {
            glUseProgram(m_program->GetID());
        } else {
            glUseProgram(0);
        }
    }

    const Program *GraphicsDeviceGL::getProgram() const {
        return m_program;
    }

    void GraphicsDeviceGL::applyUniform(const UniformMatrix *uniformMatrix, const size_t count, const void *data) {
        // TODO: Add support for matrix transposition
        assert(m_program);
        assert(uniformMatrix);
        assert(count > 0);
        assert(data);

        int offset = 0;
        const auto *const ptr = reinterpret_cast<const std::byte *>(data);

        for (size_t i = 0; i < count; i++) {
            const UniformMatrix *current = &uniformMatrix[i];
            const GLint location = m_program->getUniformLocation(current->name);

            assert(location >= 0);

            switch (current->type) {
            case DataType::Float32: {
                const auto *const values = (const GLfloat *)&ptr[offset];

                switch (current->shape) {
                case UniformMatrixShape::R2C2:
                    glUniformMatrix2fv(location, current->count, GL_TRUE, values);
                    break;
                case UniformMatrixShape::R2C3:
                    glUniformMatrix2x3fv(location, current->count, GL_TRUE, values);
                    break;
                case UniformMatrixShape::R2C4:
                    glUniformMatrix2x4fv(location, current->count, GL_TRUE, values);
                    break;
                case UniformMatrixShape::R3C2:
                    glUniformMatrix3x2fv(location, current->count, GL_TRUE, values);
                    break;
                case UniformMatrixShape::R3C3:
                    glUniformMatrix3fv(location, current->count, GL_TRUE, values);
                    break;
                case UniformMatrixShape::R3C4:
                    glUniformMatrix3x4fv(location, current->count, GL_TRUE, values);
                    break;
                case UniformMatrixShape::R4C2:
                    glUniformMatrix4x2fv(location, current->count, GL_TRUE, values);
                    break;
                case UniformMatrixShape::R4C3:
                    glUniformMatrix4x3fv(location, current->count, GL_TRUE, values);
                    break;
                case UniformMatrixShape::R4C4:
                    glUniformMatrix4fv(location, current->count, GL_TRUE, values);
                    break;
                }
                break;
            }

            case DataType::Float64: {
                const auto *const values = (const GLdouble *)&ptr[offset];

                switch (current->shape) {
                case UniformMatrixShape::R2C2:
                    glUniformMatrix2dv(location, current->count, GL_TRUE, values);
                    break;
                case UniformMatrixShape::R2C3:
                    glUniformMatrix2x3dv(location, current->count, GL_TRUE, values);
                    break;
                case UniformMatrixShape::R2C4:
                    glUniformMatrix2x4dv(location, current->count, GL_TRUE, values);
                    break;
                case UniformMatrixShape::R3C2:
                    glUniformMatrix3x2dv(location, current->count, GL_TRUE, values);
                    break;
                case UniformMatrixShape::R3C3:
                    glUniformMatrix3dv(location, current->count, GL_TRUE, values);
                    break;
                case UniformMatrixShape::R3C4:
                    glUniformMatrix3x4dv(location, current->count, GL_TRUE, values);
                    break;
                case UniformMatrixShape::R4C2:
                    glUniformMatrix4x2dv(location, current->count, GL_TRUE, values);
                    break;
                case UniformMatrixShape::R4C3:
                    glUniformMatrix4x3dv(location, current->count, GL_TRUE, values);
                    break;
                case UniformMatrixShape::R4C4:
                    glUniformMatrix4dv(location, current->count, GL_TRUE, values);
                    break;
                }
                break;
            }

            default: {
                assert(false && "Supplied DataType isn't supported");
            }
            }

            offset += bytesize(current->type) * countElements(current->shape) * current->count;
        }
    }

    void GraphicsDeviceGL::applyUniform(const Uniform *uniform, const size_t count, const void *data) {
        assert(m_program);
        assert(uniform);
        assert(count > 0);
        assert(data);

        int offset = 0;
        const auto *const ptr = reinterpret_cast<const std::byte *>(data);

        for (size_t i = 0; i < count; i++) {
            const Uniform *current = &uniform[i];
            const GLint location = m_program->getUniformLocation(current->name);

            assert(location >= 0);

            switch (current->type) {
            case DataType::Int32:
                switch (current->dimension) {
                case UniformDimension::D1:
                    glUniform1iv(location, current->count, (const GLint *)&ptr[offset]);
                    break;
                case UniformDimension::D2:
                    glUniform2iv(location, current->count, (const GLint *)&ptr[offset]);
                    break;
                case UniformDimension::D3:
                    glUniform3iv(location, current->count, (const GLint *)&ptr[offset]);
                    break;
                case UniformDimension::D4:
                    glUniform4iv(location, current->count, (const GLint *)&ptr[offset]);
                    break;
                default:
                    assert(false);
                }
                break;

            case DataType::Float32:
                switch (current->dimension) {
                case UniformDimension::D1:
                    glUniform1fv(location, current->count, (const GLfloat *)&ptr[offset]);
                    break;
                case UniformDimension::D2:
                    glUniform2fv(location, current->count, (const GLfloat *)&ptr[offset]);
                    break;
                case UniformDimension::D3:
                    glUniform3fv(location, current->count, (const GLfloat *)&ptr[offset]);
                    break;
                case UniformDimension::D4:
                    glUniform4fv(location, current->count, (const GLfloat *)&ptr[offset]);
                    break;
                default:
                    assert(false);
                }
                break;

            case DataType::UInt32:
                switch (current->dimension) {
                case UniformDimension::D1:
                    glUniform1uiv(location, current->count, (const GLuint *)&ptr[offset]);
                    break;
                case UniformDimension::D2:
                    glUniform2uiv(location, current->count, (const GLuint *)&ptr[offset]);
                    break;
                case UniformDimension::D3:
                    glUniform3uiv(location, current->count, (const GLuint *)&ptr[offset]);
                    break;
                case UniformDimension::D4:
                    glUniform4uiv(location, current->count, (const GLuint *)&ptr[offset]);
                    break;
                default:
                    assert(false);
                }
                break;

            default:
                assert(false && "Supplied DataType isn't supported");
            }

            offset += bytesize(current->type) * elementcount(current->dimension) * current->count;
        }
    }

    void GraphicsDeviceGL::setViewport(const Viewport &viewport) {
        GLint const x = static_cast<GLint>(viewport.position.X);
        GLint const y = static_cast<GLint>(viewport.position.Y);

        GLint const w = static_cast<GLint>(viewport.size.X);
        GLint const h = static_cast<GLint>(viewport.size.Y);

        glViewport(x, y, w, h);

        m_viewport = viewport;
    }

    Viewport GraphicsDeviceGL::getViewport() const {
        return m_viewport;
    }

    const Material *GraphicsDeviceGL::getMaterial() const {
        return m_material;
    }
} // namespace XE
