
#include <XE/Graphics/GL/GraphicsDeviceGL.h>

#include <XE/Graphics/GL/Conversion.h>
#include <XE/Graphics/GL/BufferGL.h>
#include <XE/Graphics/GL/SubsetGL.h>
#include <XE/Graphics/GL/Texture2DGL.h>
#include <XE/Graphics/GL/Texture2DArrayGL.h>
#include <XE/Graphics/GL/Texture3DGL.h>
#include <XE/Graphics/GL/ProgramGL.h>
#include <XE/Graphics/GL/IGraphicsContextGL.h>
#include <XE/Graphics/GL/Util.h>

#include <XE/Graphics/Material.h>
#include <XE/Graphics/Uniform.h>
#include <XE/Graphics/Subset.h>
#include <XE/Graphics/Texture3D.h>
#include <XE/Graphics/Texture2DArray.h>
#include <XE/Graphics/TextureCubeMap.h>
#include <iostream>

namespace XE {
/*#if defined(GLAD_DEBUG)
    void gladPostCallback(const char *name, void *funcptr, int len_args, ...) {
        if (std::string(name) != "glGetError") {
            GLenum error = glGetError();
            assert(error == GL_NO_ERROR);
        }
    }
#endif
*/

    GraphicsDeviceGL::GraphicsDeviceGL(IGraphicsContextGL *context) {
        this->context = context;

        std::cout << "[GL] Loading OpenGL Extensions ..." << std::endl;
        if (!gladLoadGL()) {
            throw std::runtime_error("Failed to load OpenGL extensions");
        }

/*
#if defined(GLAD_DEBUG)
    // glad_set_pre_callback(gladPreCallback);
    // glad_set_post_callback(gladPostCallback);
#endif
*/
        XE_GRAPHICS_GL_CHECK_ERROR();
    }

    GraphicsDeviceGL::~GraphicsDeviceGL() {}
    
    std::unique_ptr<Subset> GraphicsDeviceGL::createSubset(
            SubsetDescriptor& desc, 
            std::vector<std::unique_ptr<Buffer>> buffers, 
            const std::map<std::string, int> &bufferMapping, 
            std::unique_ptr<Buffer> indexBuffer) {
        return std::make_unique<SubsetGL>(desc, std::move(buffers), bufferMapping, std::move(indexBuffer));
    }
        
    std::unique_ptr<Buffer> GraphicsDeviceGL::createBuffer(const BufferDescriptor &desc) {
        return std::make_unique<BufferGL>(desc);
    }
        
    std::unique_ptr<Texture2D> GraphicsDeviceGL::createTexture2D(const PixelFormat format, const Vector2i &size, const PixelFormat sourceFormat, const DataType sourceDataType, const void *sourceData) {
        return std::make_unique<Texture2DGL>(format, size, sourceFormat, sourceDataType, sourceData);
    }
        
    std::unique_ptr<Texture3D> GraphicsDeviceGL::createTexture3D(const PixelFormat format, const Vector3i &size, const PixelFormat sourceFormat, const DataType sourceDataType, const void *sourceData) {
        return std::make_unique<Texture3DGL>(format, size, sourceFormat, sourceDataType, sourceData);
    }

    std::unique_ptr<Texture2DArray> GraphicsDeviceGL::createTexture2DArray(const PixelFormat format, const Vector2i &size, const int count) {
        return std::make_unique<Texture2DArrayGL>(format, size, count);
    }
        
    std::unique_ptr<TextureCubeMap> GraphicsDeviceGL::createTextureCubeMap(const PixelFormat format, const Vector2i &size, const PixelFormat sourceFormat, const DataType sourceDataType, const void **sourceData) {
        return std::unique_ptr<TextureCubeMap>();
    }
        
    std::unique_ptr<Program> GraphicsDeviceGL::createProgram(const ProgramDescriptor &desc) {
        return std::make_unique<ProgramGL>(desc);
    }
    
    void GraphicsDeviceGL::draw(const Subset *subset, const SubsetEnvelope *envelopes, const int envelopeCount) {
        auto subsetGL = static_cast<const SubsetGL *>(subset);
        auto descriptor = subsetGL->GetDescriptor();
        ::glBindVertexArray(subsetGL->GetID());

        auto indexBuffer = subsetGL->getIndexBuffer();

        if (!indexBuffer) {
            for (int i=0; i<envelopeCount; i++) {
                const SubsetEnvelope &env = envelopes[i];
                const GLenum primitiveGL = convertToGL(env.Primitive);

                ::glDrawArrays(primitiveGL, env.VertexStart, env.VertexCount);
            }
        } else {
            const GLenum indexTypeGL = convertToGL(descriptor.indexType);

            for (int i=0; i<envelopeCount; i++) {
                const SubsetEnvelope &env = envelopes[i];
                const GLenum primitiveGL = convertToGL(env.Primitive);
                
                if (env.VertexStart == 0) {
                    ::glDrawElements(primitiveGL, env.VertexCount, indexTypeGL, nullptr);
                } else {
                    ::glDrawElementsBaseVertex(primitiveGL, env.VertexCount, indexTypeGL, nullptr, env.VertexStart);
                }
            }
        }

        ::glBindVertexArray(0);
        XE_GRAPHICS_GL_CHECK_ERROR();
    }
        
    void GraphicsDeviceGL::beginFrame(const ClearFlags flags, const Vector4f &color, const float depth, const int stencil) {
        
        GLenum clearFlagsGL = 0;
        
        if (flags & ClearFlags::Color) {
            clearFlagsGL |= GL_COLOR_BUFFER_BIT;
        }
        
        if (flags & ClearFlags::Depth) {
            clearFlagsGL |= GL_DEPTH_BUFFER_BIT;
        }
        
        if (flags & ClearFlags::Stencil) {
            clearFlagsGL |= GL_STENCIL_BUFFER_BIT;
        }

        ::glClearColor(color.X, color.Y, color.Z, color.W);
        ::glClearDepth(static_cast<GLdouble>(depth));
        ::glClearStencil(stencil);
        ::glClear(clearFlagsGL);

        XE_GRAPHICS_GL_CHECK_ERROR();
    }
        
    void GraphicsDeviceGL::endFrame() {
        ::glFlush();

        context->present();

        XE_GRAPHICS_GL_CHECK_ERROR();
    }

    void GraphicsDeviceGL::preRenderMaterial(const Material *material) {
        const auto &rs = material->renderState;

        if (rs.depthTest) {
            ::glEnable(GL_DEPTH_TEST);
        } else {
            ::glDisable(GL_DEPTH_TEST);
        }
        
        const GLenum depthFuncGL = convertToGL(rs.depthFunc);
        ::glDepthFunc(depthFuncGL);

        const GLenum faceGL = convertToGL(rs.frontFace);
        ::glFrontFace(faceGL);

        if (rs.cullBackFace) {
            ::glEnable(GL_CULL_FACE);
        } else {
            ::glDisable(GL_CULL_FACE);
        }

        ::glLineWidth(rs.lineWidth);

        if (rs.blendEnable) {
            ::glEnable(GL_BLEND);

            const GLenum sfactorGL = convertToGL(rs.blendSource);
            const GLenum dfactorGL = convertToGL(rs.blendDestination);
            ::glBlendFunc(sfactorGL, sfactorGL);
        } else {
            ::glDisable(GL_BLEND);
        }

        for (int i=0; i<material->layerCount; i++) {
            const auto &layer = material->layers[i];

            if (!layer.texture) {
                continue;
            }

            // FIXME: This will cause segfaults if the real implementation isn't derived from the Texture/TextureBaseGL family
            auto textureBaseGL = dynamic_cast<const TextureBaseGL*>(layer.texture);
            auto target = textureBaseGL->GetTarget();
            
            std::cout << target << ", " << textureBaseGL->GetID() << std::endl;

            ::glActiveTexture(GL_TEXTURE0 + i);
            ::glBindTexture(target, textureBaseGL->GetID());
            ::glTexParameteri(target, GL_TEXTURE_MAG_FILTER, convertToGL(layer.magFilter));
            ::glTexParameteri(target, GL_TEXTURE_MIN_FILTER, convertToGL(layer.minFilter));
            ::glTexParameteri(target, GL_TEXTURE_WRAP_S, convertToGL(layer.wrapS));
            ::glTexParameteri(target, GL_TEXTURE_WRAP_T, convertToGL(layer.wrapT));
            ::glTexParameteri(target, GL_TEXTURE_WRAP_R, convertToGL(layer.wrapR));
        }

        XE_GRAPHICS_GL_CHECK_ERROR();
    }

    void GraphicsDeviceGL::postRenderMaterial(const Material *material) {
        const auto &rs = material->renderState;

        if (rs.depthTest) {
            ::glDisable(GL_DEPTH_TEST);
        }

        if (rs.cullBackFace) {
            ::glDisable(GL_CULL_FACE);
        }

        if (rs.blendEnable) {
            ::glDisable(GL_BLEND);
        }

        for (int i=0; i<material->layerCount; i++) {
            const auto &layer = material->layers[i];

            if (!layer.texture) {
                continue;
            }

            // FIXME: This will cause segfaults if the real implementation isn't derived from the Texture/TextureBaseGL family
            auto textureBaseGL = reinterpret_cast<const TextureBaseGL*>(layer.texture);

            ::glActiveTexture(GL_TEXTURE0 + i);
            ::glBindTexture(textureBaseGL->GetTarget(), 0);
        }

        ::glActiveTexture(GL_TEXTURE0);

        XE_GRAPHICS_GL_CHECK_ERROR();
    }

    void GraphicsDeviceGL::setMaterial(const Material *material) {
        if (m_material == material) {
            return;
        }

        if (m_material) {
            this->postRenderMaterial(m_material);
        }

        this->preRenderMaterial(material);

        m_material = material;

        XE_GRAPHICS_GL_CHECK_ERROR();
    }

    void GraphicsDeviceGL::setProgram(const Program *program) {
        m_program = static_cast<const ProgramGL*>(program);

        if (m_program) {
            ::glUseProgram(m_program->GetID());
        } else {
            ::glUseProgram(0);
        }

        XE_GRAPHICS_GL_CHECK_ERROR();
    }
    
    const Program* GraphicsDeviceGL::getProgram() const {
        return m_program;
    }

    void GraphicsDeviceGL::applyUniform(const UniformMatrix *uniformMatrix, const int count, const std::byte *data) {
        // TODO: Add support for matrix transposition

        assert(m_program);
        assert(uniformMatrix);
        assert(count > 0);
        assert(data);

        int offset = 0;

        for (int i=0; i<count; i++) {
            const UniformMatrix *current = &uniformMatrix[i];
            const GLint location = m_program->getUniformLoction(current->Name);

            switch (current->Type) {
            case DataType::Float32: 
                switch (current->Rows) {
                case 2:
                    switch (current->Columns) {
                    case 2: ::glUniformMatrix2fv(location, current->Count, GL_FALSE, (const GLfloat*)&data[offset]); break;
                    case 3: ::glUniformMatrix2x3fv(location, current->Count, GL_FALSE, (const GLfloat*)&data[offset]); break;
                    case 4: ::glUniformMatrix2x4fv(location, current->Count, GL_FALSE, (const GLfloat*)&data[offset]); break;
                    }
                    break;

                case 3:
                    switch (current->Columns) {
                    case 2: ::glUniformMatrix3x2fv(location, current->Count, GL_FALSE, (const GLfloat*)&data[offset]); break;
                    case 3: ::glUniformMatrix3fv(location, current->Count, GL_FALSE, (const GLfloat*)&data[offset]); break;
                    case 4: ::glUniformMatrix3x4fv(location, current->Count, GL_FALSE, (const GLfloat*)&data[offset]); break;
                    }
                    break;

                case 4:
                    switch (current->Columns) {
                    case 2: ::glUniformMatrix4x2fv(location, current->Count, GL_FALSE, (const GLfloat*)&data[offset]); break;
                    case 3: ::glUniformMatrix4x3fv(location, current->Count, GL_FALSE, (const GLfloat*)&data[offset]); break;
                    case 4: ::glUniformMatrix4fv(location, current->Count, GL_FALSE, (const GLfloat*)&data[offset]); break;
                    }
                    break;
                }
                break;

            case DataType::Float64: 
                switch (current->Rows) {
                case 2:
                    switch (current->Columns) {
                    case 2: ::glUniformMatrix2dv(location, current->Count, GL_FALSE, (const GLdouble*)&data[offset]); break;
                    case 3: ::glUniformMatrix2x3dv(location, current->Count, GL_FALSE, (const GLdouble*)&data[offset]); break;
                    case 4: ::glUniformMatrix2x4dv(location, current->Count, GL_FALSE, (const GLdouble*)&data[offset]); break;
                    }
                    break;

                case 3:
                    switch (current->Columns) {
                    case 2: ::glUniformMatrix3x2dv(location, current->Count, GL_FALSE, (const GLdouble*)&data[offset]); break;
                    case 3: ::glUniformMatrix3dv(location, current->Count, GL_FALSE, (const GLdouble*)&data[offset]); break;
                    case 4: ::glUniformMatrix3x4dv(location, current->Count, GL_FALSE, (const GLdouble*)&data[offset]); break;
                    }
                    break;

                case 4:
                    switch (current->Columns) {
                    case 2: ::glUniformMatrix4x2dv(location, current->Count, GL_FALSE, (const GLdouble*)&data[offset]); break;
                    case 3: ::glUniformMatrix4x3dv(location, current->Count, GL_FALSE, (const GLdouble*)&data[offset]); break;
                    case 4: ::glUniformMatrix4dv(location, current->Count, GL_FALSE, (const GLdouble*)&data[offset]); break;
                    }
                    break;
                }
                break;
                    
                default:
                    assert(false);
            }

            offset += ComputeByteSize(current->Type) * current->Rows * current->Columns * current->Count;
        }

        XE_GRAPHICS_GL_CHECK_ERROR();
    }

/*
    template<typename T>
    using UniformFunction = void (*)(GLint location, GLsizei count, const T *value);

    static const std::array<UniformFunction<GLint>, 4> uniformFunctionsI = {
        glUniform1iv, glUniform2iv, glUniform3iv, glUniform4iv
    };


    static const std::array<UniformFunction<GLfloat>, 4> uniformFunctionsF = {
        glUniform1fv, glUniform2fv, glUniform3fv, glUniform4fv
    };

    static const std::array<UniformFunction<GLuint>, 4> uniformFunctionsUI = {
        glUniform1uiv, glUniform2uiv, glUniform3uiv, glUniform4uiv
    };
*/


    void GraphicsDeviceGL::applyUniform(const Uniform *uniform, const int count, const std::byte *data) {
        assert(m_program);
        assert(uniform);
        assert(count > 0);
        assert(data);

        int offset = 0;

        for (int i=0; i<count; i++) {
            const Uniform *current = &uniform[i];
            const GLint location = m_program->getUniformLoction(current->Name);

            assert(location >= 0);

            switch (current->Type) {
            case DataType::Int32:
                switch (current->size) {
                    case 1: ::glUniform1iv(location, current->Count, (const GLint*)&data[offset]); break;
                    case 2: ::glUniform2iv(location, current->Count, (const GLint*)&data[offset]); break;
                    case 3: ::glUniform3iv(location, current->Count, (const GLint*)&data[offset]); break;
                    case 4: ::glUniform4iv(location, current->Count, (const GLint*)&data[offset]); break;
                    default: assert(false);
                }
                break;
            
            case DataType::Float32:
                switch (current->size) {
                    case 1: ::glUniform1fv(location, current->Count, (const GLfloat*)&data[offset]); break;
                    case 2: ::glUniform2fv(location, current->Count, (const GLfloat*)&data[offset]); break;
                    case 3: ::glUniform3fv(location, current->Count, (const GLfloat*)&data[offset]); break;
                    case 4: ::glUniform4fv(location, current->Count, (const GLfloat*)&data[offset]); break;
                    default: assert(false);
                }
                break;

            case DataType::UInt32:
                switch (current->size) {
                    case 1: ::glUniform1uiv(location, current->Count, (const GLuint*)&data[offset]); break;
                    case 2: ::glUniform2uiv(location, current->Count, (const GLuint*)&data[offset]); break;
                    case 3: ::glUniform3uiv(location, current->Count, (const GLuint*)&data[offset]); break;
                    case 4: ::glUniform4uiv(location, current->Count, (const GLuint*)&data[offset]); break;
                    default: assert(false);
                }
                break;

            default:
                assert(false);
            }

            offset += ComputeByteSize(current->Type) * current->size * current->Count;
        }

        XE_GRAPHICS_GL_CHECK_ERROR();
    }

    void GraphicsDeviceGL::setViewport(const Viewport &viewport) {
        GLint x = static_cast<GLint>(viewport.position.X);
        GLint y = static_cast<GLint>(viewport.position.Y);

        GLint w = static_cast<GLint>(viewport.size.X);
        GLint h = static_cast<GLint>(viewport.size.Y);

        ::glViewport(x, y, w, h);

        m_viewport = viewport;

        XE_GRAPHICS_GL_CHECK_ERROR();
    }

    Viewport GraphicsDeviceGL::getViewport() const {
        return m_viewport;
    }

    const Material* GraphicsDeviceGL::getMaterial() const {
        return m_material;
    }
}
