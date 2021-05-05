
#include <XE/Graphics/ES2/GraphicsDeviceES2.h>

#include <XE/Graphics/ES2/Conversion.h>
#include <XE/Graphics/ES2/BufferES2.h>
#include <XE/Graphics/ES2/SubsetES2.h>
#include <XE/Graphics/ES2/Texture2DES2.h>
#include <XE/Graphics/ES2/ProgramES2.h>
#include <XE/Graphics/ES2/IGraphicsContextES2.h>
#include <XE/Graphics/ES2/Util.h>

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

    GraphicsDeviceES2::GraphicsDeviceES2(IGraphicsContextES2 *context) {
        this->context = context;

/*
#if defined(GLAD_DEBUG)
    // glad_set_pre_callback(gladPreCallback);
    // glad_set_post_callback(gladPostCallback);
#endif
*/
        XE_GRAPHICS_GL_CHECK_ERROR();
    }

    GraphicsDeviceES2::~GraphicsDeviceES2() {}
    
    std::unique_ptr<Subset> GraphicsDeviceES2::createSubset(
            SubsetDescriptor& desc, 
            std::vector<std::unique_ptr<Buffer>> buffers, 
            const std::map<std::string, int> &bufferMapping, 
            std::unique_ptr<Buffer> indexBuffer) {
        return std::make_unique<SubsetGL>(desc, std::move(buffers), bufferMapping, std::move(indexBuffer));
    }
        
    std::unique_ptr<Buffer> GraphicsDeviceES2::createBuffer(const BufferDescriptor &desc) {
        return std::make_unique<BufferES2>(desc);
    }
        
    std::unique_ptr<Texture2D> GraphicsDeviceES2::createTexture2D(const PixelFormat format, const Vector2i &size, const PixelFormat sourceFormat, const DataType sourceDataType, const void *sourceData) {
        return std::make_unique<Texture2DGL>(format, size, sourceFormat, sourceDataType, sourceData);
    }
        
    std::unique_ptr<Texture3D> GraphicsDeviceES2::createTexture3D(const PixelFormat format, const Vector3i &size, const PixelFormat sourceFormat, const DataType sourceDataType, const void *sourceData) {
        return std::unique_ptr<Texture3D>();
    }

    std::unique_ptr<Texture2DArray> GraphicsDeviceES2::createTexture2DArray(const PixelFormat format, const Vector2i &size, const int count) {
        return std::unique_ptr<Texture2DArray>();
    }
        
    std::unique_ptr<TextureCubeMap> GraphicsDeviceES2::createTextureCubeMap(const PixelFormat format, const Vector2i &size, const PixelFormat sourceFormat, const DataType sourceDataType, const void **sourceData) {
        return std::unique_ptr<TextureCubeMap>();
    }
        
    std::unique_ptr<Program> GraphicsDeviceES2::createProgram(const ProgramDescriptor &desc) {
        return std::make_unique<ProgramGL>(desc);
    }
    
    void GraphicsDeviceES2::draw(const Subset *subset, const SubsetEnvelope *envelopes, const int envelopeCount) {
        auto subsetGL = static_cast<const SubsetGL *>(subset);
        auto descriptor = subsetGL->GetDescriptor();

        throw std::runtime_error("ES2: VAO rendering isn't supported");

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
                    throw std::runtime_error("BaseVertex rendering isn't supported");
                }
            }
        }

        XE_GRAPHICS_GL_CHECK_ERROR();
    }


    void GraphicsDeviceES2::beginFrame(const ClearFlags flags, const Vector4f &color, const float, const int stencil) {
        const GLenum clearFlagsGL =
            (flags & ClearFlags::Color   ? GL_COLOR_BUFFER_BIT   : 0) | 
            (flags & ClearFlags::Depth   ? GL_DEPTH_BUFFER_BIT   : 0) | 
            (flags & ClearFlags::Stencil ? GL_STENCIL_BUFFER_BIT : 0) ;

        ::glClearColor(color.X, color.Y, color.Z, color.W);
        ::glClearStencil(stencil);
        ::glClear(clearFlagsGL);

        XE_GRAPHICS_GL_CHECK_ERROR();
    }
        
    void GraphicsDeviceES2::endFrame() {
        ::glFlush();

        context->present();

        XE_GRAPHICS_GL_CHECK_ERROR();
    }

    void GraphicsDeviceES2::preRenderMaterial(const Material *material) {
        const auto &rs = material->renderState;

        // depth buffer configuration
        if (rs.depthTest) {
            ::glEnable(GL_DEPTH_TEST);
        } else {
            ::glDisable(GL_DEPTH_TEST);
        }

        const GLenum depthFuncGL = convertToGL(rs.depthFunc);
        ::glDepthFunc(depthFuncGL);

        // front face definition
        const GLenum faceGL = convertToGL(rs.frontFace);
        ::glFrontFace(faceGL);

        // back face culling
        if (rs.cullBackFace) {
            ::glEnable(GL_CULL_FACE);
        } else {
            ::glDisable(GL_CULL_FACE);
        }

        // point and line sizing
        ::glLineWidth(rs.lineWidth);

        // blending
        if (rs.blendEnable) {
            ::glEnable(GL_BLEND);

            const GLenum sfactorGL = convertToGL(rs.blendSource);
            const GLenum dfactorGL = convertToGL(rs.blendDestination);
            ::glBlendFunc(sfactorGL, sfactorGL);
        } else {
            ::glDisable(GL_BLEND);
        }

        // texture layers
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
            ::glTexParameteri(target, GL_TEXTURE_MAG_FILTER, convertToGL(layer.minFilter));
            ::glTexParameteri(target, GL_TEXTURE_MIN_FILTER, convertToGL(layer.magFilter));
            ::glTexParameteri(target, GL_TEXTURE_WRAP_S, convertToGL(layer.wrapS));
            ::glTexParameteri(target, GL_TEXTURE_WRAP_T, convertToGL(layer.wrapT));
            /*::glTexParameteri(target, GL_TEXTURE_WRAP_R, convertToGL(layer.wrapR));*/
        }

        XE_GRAPHICS_GL_CHECK_ERROR();
    }

    void GraphicsDeviceES2::postRenderMaterial(const Material *material) {
        const auto &rs = material->renderState;

        // depth buffer configuration
        if (rs.depthTest) {
            ::glDisable(GL_DEPTH_TEST);
        }

        // back face culling
        if (rs.cullBackFace) {
            ::glDisable(GL_CULL_FACE);
        }

        // blending
        if (rs.blendEnable) {
            ::glDisable(GL_BLEND);
        }

        // texture layers
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

    void GraphicsDeviceES2::setMaterial(const Material *material) {
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

    void GraphicsDeviceES2::setProgram(const Program *program) {
        m_program = static_cast<const ProgramGL*>(program);

        if (m_program) {
            ::glUseProgram(m_program->GetID());
        } else {
            ::glUseProgram(0);
        }

        XE_GRAPHICS_GL_CHECK_ERROR();
    }
    
    const Program* GraphicsDeviceES2::getProgram() const {
        return m_program;
    }

    void GraphicsDeviceES2::applyUniform(const UniformMatrix *uniformMatrix, const int count, const std::byte *data) {
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
                    default: throw std::runtime_error("AAAAA");
                    }
                    break;

                case 3:
                    switch (current->Columns) {
                    case 3: ::glUniformMatrix3fv(location, current->Count, GL_FALSE, (const GLfloat*)&data[offset]); break;
                    default: throw std::runtime_error("AAAAA");
                    }
                    break;

                case 4:
                    switch (current->Columns) {
                    case 4: ::glUniformMatrix4fv(location, current->Count, GL_FALSE, (const GLfloat*)&data[offset]); break;
                    default: throw std::runtime_error("AAAAA");
                    }
                    break;
                }
                break;

            case DataType::Float64: 
                throw std::runtime_error("AAAAA");
                /*
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
                */
                break;
            }

            offset += ComputeByteSize(current->Type) * current->Rows * current->Columns * current->Count;
        }

        XE_GRAPHICS_GL_CHECK_ERROR();
    }

    void GraphicsDeviceES2::applyUniform(const Uniform *uniform, const int count, const std::byte *data) {
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
                // std::cout << current->Name << ": " << location << ", " << *((const GLint*)&data[offset]) << std::endl;
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
                throw std::runtime_error("AAAAA");
                /*
                switch (current->size) {
                    case 1: ::glUniform1uiv(location, current->Count, (const GLuint*)&data[offset]); break;
                    case 2: ::glUniform2uiv(location, current->Count, (const GLuint*)&data[offset]); break;
                    case 3: ::glUniform3uiv(location, current->Count, (const GLuint*)&data[offset]); break;
                    case 4: ::glUniform4uiv(location, current->Count, (const GLuint*)&data[offset]); break;
                    default: assert(false);
                }
                */
                break;

            default:
                assert(false);
            }

            offset += ComputeByteSize(current->Type) * current->size * current->Count;
        }

        XE_GRAPHICS_GL_CHECK_ERROR();
    }

    void GraphicsDeviceES2::setViewport(const Viewport &viewport) {
        GLint x = static_cast<GLint>(viewport.position.X);
        GLint y = static_cast<GLint>(viewport.position.Y);

        GLint w = static_cast<GLint>(viewport.size.X);
        GLint h = static_cast<GLint>(viewport.size.Y);

        ::glViewport(x, y, w, h);

        m_viewport = viewport;

        XE_GRAPHICS_GL_CHECK_ERROR();
    }

    Viewport GraphicsDeviceES2::getViewport() const {
        return m_viewport;
    }

    const Material* GraphicsDeviceES2::getMaterial() const {
        return m_material;
    }
}
