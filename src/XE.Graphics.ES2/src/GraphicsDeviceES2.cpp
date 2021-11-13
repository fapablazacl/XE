
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
    
    Subset* GraphicsDeviceES2::createSubset(const SubsetDescriptor2& desc) {
        return nullptr;
    }
        
    Buffer* GraphicsDeviceES2::createBuffer(const BufferDescriptor &desc) {
        return new BufferES2(desc);
    }
        
    Texture2D* GraphicsDeviceES2::createTexture2D(const PixelFormat format, const Vector2i &size, const PixelFormat sourceFormat, const DataType sourceDataType, const void *sourceData) {
        return new Texture2DES(format, size, sourceFormat, sourceDataType, sourceData);
    }
        
    Texture3D* GraphicsDeviceES2::createTexture3D(const PixelFormat format, const Vector3i &size, const PixelFormat sourceFormat, const DataType sourceDataType, const void *sourceData) {
        return nullptr;
    }

    Texture2DArray* GraphicsDeviceES2::createTexture2DArray(const PixelFormat format, const Vector2i &size, const int count) {
        return nullptr;
    }
        
    TextureCubeMap* GraphicsDeviceES2::createTextureCubeMap(const PixelFormat format, const Vector2i &size, const PixelFormat sourceFormat, const DataType sourceDataType, const void **sourceData) {
        return nullptr;
    }
        
    Program* GraphicsDeviceES2::createProgram(const ProgramDescriptor &desc) {
        return new ProgramES(desc);
    }
    
    void GraphicsDeviceES2::draw(const Subset *subset, const SubsetEnvelope *envelopes, const int envelopeCount) {
        /*
        auto subsetGL = static_cast<const SubsetES *>(subset);
        auto descriptor = subsetGL->GetDescriptor();

        throw std::runtime_error("ES2: VAO rendering isn't supported");

        auto indexBuffer = subsetGL->getIndexBuffer();

        if (!indexBuffer) {
            for (int i=0; i<envelopeCount; i++) {
                const SubsetEnvelope &env = envelopes[i];
                const GLenum primitiveGL = convertToGL(env.Primitive);

                glDrawArrays(primitiveGL, env.VertexStart, env.VertexCount);
            }
        } else {
            const GLenum indexTypeGL = convertToGL(descriptor.indexType);

            for (int i=0; i<envelopeCount; i++) {
                const SubsetEnvelope &env = envelopes[i];
                const GLenum primitiveGL = convertToGL(env.Primitive);
                
                if (env.VertexStart == 0) {
                    glDrawElements(primitiveGL, env.VertexCount, indexTypeGL, nullptr);
                } else {
                    throw std::runtime_error("BaseVertex rendering isn't supported");
                }
            }
        }

        XE_GRAPHICS_GL_CHECK_ERROR();
        */
    }


    void GraphicsDeviceES2::beginFrame(const ClearFlags flags, const Vector4f &color, const float, const int stencil) {
        const GLenum clearFlagsGL =
            (flags & ClearFlags::Color   ? GL_COLOR_BUFFER_BIT   : 0) | 
            (flags & ClearFlags::Depth   ? GL_DEPTH_BUFFER_BIT   : 0) | 
            (flags & ClearFlags::Stencil ? GL_STENCIL_BUFFER_BIT : 0) ;

        glClearColor(color.X, color.Y, color.Z, color.W);
        glClearStencil(stencil);
        glClear(clearFlagsGL);

        XE_GRAPHICS_GL_CHECK_ERROR();
    }
        
    void GraphicsDeviceES2::endFrame() {
        glFlush();

        context->present();

        XE_GRAPHICS_GL_CHECK_ERROR();
    }

    void GraphicsDeviceES2::preRenderMaterial(const Material *material) {
        const auto &rs = material->renderState;

        // depth buffer configuration
        if (rs.depthTest) {
            glEnable(GL_DEPTH_TEST);
        } else {
            glDisable(GL_DEPTH_TEST);
        }

        const GLenum depthFuncGL = convertToGL(rs.depthFunc);
        glDepthFunc(depthFuncGL);

        // front face definition
        const GLenum faceGL = convertToGL(rs.frontFace);
        glFrontFace(faceGL);

        // back face culling
        if (rs.cullBackFace) {
            glEnable(GL_CULL_FACE);
        } else {
            glDisable(GL_CULL_FACE);
        }

        // point and line sizing
        glLineWidth(rs.lineWidth);

        // blending
        if (rs.blendEnable) {
            glEnable(GL_BLEND);

            const GLenum sfactorGL = convertToGL(rs.blendSource);
            const GLenum dfactorGL = convertToGL(rs.blendDestination);
            glBlendFunc(sfactorGL, dfactorGL);
        } else {
            glDisable(GL_BLEND);
        }

        // texture layers
        for (int i=0; i<material->layerCount; i++) {
            const auto &layer = material->layers[i];

            if (!layer.texture) {
                continue;
            }

            // FIXME: This will cause segfaults if the real implementation isn't derived from the Texture/TextureBaseGL family
            auto textureBaseGL = dynamic_cast<const TextureBaseES*>(layer.texture);
            auto target = textureBaseGL->GetTarget();
            
            std::cout << target << ", " << textureBaseGL->GetID() << std::endl;

            glActiveTexture(GL_TEXTURE0 + i);
            glBindTexture(target, textureBaseGL->GetID());
            glTexParameteri(target, GL_TEXTURE_MAG_FILTER, convertToGL(layer.minFilter));
            glTexParameteri(target, GL_TEXTURE_MIN_FILTER, convertToGL(layer.magFilter));
            glTexParameteri(target, GL_TEXTURE_WRAP_S, convertToGL(layer.wrapS));
            glTexParameteri(target, GL_TEXTURE_WRAP_T, convertToGL(layer.wrapT));
            /*glTexParameteri(target, GL_TEXTURE_WRAP_R, convertToGL(layer.wrapR));*/
        }

        XE_GRAPHICS_GL_CHECK_ERROR();
    }

    void GraphicsDeviceES2::postRenderMaterial(const Material *material) {
        const auto &rs = material->renderState;

        // depth buffer configuration
        if (rs.depthTest) {
            glDisable(GL_DEPTH_TEST);
        }

        // back face culling
        if (rs.cullBackFace) {
            glDisable(GL_CULL_FACE);
        }

        // blending
        if (rs.blendEnable) {
            glDisable(GL_BLEND);
        }

        // texture layers
        for (int i=0; i<material->layerCount; i++) {
            const auto &layer = material->layers[i];

            if (!layer.texture) {
                continue;
            }

            // FIXME: This will cause segfaults if the real implementation isn't derived from the Texture/TextureBaseGL family
            auto textureBaseGL = reinterpret_cast<const TextureBaseES*>(layer.texture);

            glActiveTexture(GL_TEXTURE0 + i);
            glBindTexture(textureBaseGL->GetTarget(), 0);
        }

        glActiveTexture(GL_TEXTURE0);

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
        m_program = static_cast<const ProgramES*>(program);

        if (m_program) {
            glUseProgram(m_program->GetID());
        } else {
            glUseProgram(0);
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
            const GLint location = m_program->getUniformLoction(current->name);

            switch (current->type) {
            case DataType::Float32: {
                const auto values = (const GLfloat*)&data[offset];
                
                switch (current->shape) {
                    case UniformMatrixShape::R2C2: glUniformMatrix2fv(location, current->count, GL_FALSE, values); break;
                    case UniformMatrixShape::R3C3: glUniformMatrix3fv(location, current->count, GL_FALSE, values); break;
                    case UniformMatrixShape::R4C4: glUniformMatrix4fv(location, current->count, GL_FALSE, values); break;
                    default:
                        assert(false);
                }
                break;
            }

            default:
                assert(false);
            }

            offset += ComputeByteSize(current->type) * countElements(current->shape) * current->count;
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
            const GLint location = m_program->getUniformLoction(current->name);

            assert(location >= 0);

            switch (current->type) {
            case DataType::Int32:
                switch (current->dimension) {
                    case UniformDimension::D1: glUniform1iv(location, current->count, (const GLint*)&data[offset]); break;
                    case UniformDimension::D2: glUniform2iv(location, current->count, (const GLint*)&data[offset]); break;
                    case UniformDimension::D3: glUniform3iv(location, current->count, (const GLint*)&data[offset]); break;
                    case UniformDimension::D4: glUniform4iv(location, current->count, (const GLint*)&data[offset]); break;
                    default: assert(false);
                }
                break;
            
            case DataType::Float32:
                switch (current->dimension) {
                    case UniformDimension::D1: glUniform1fv(location, current->count, (const GLfloat*)&data[offset]); break;
                    case UniformDimension::D2: glUniform2fv(location, current->count, (const GLfloat*)&data[offset]); break;
                    case UniformDimension::D3: glUniform3fv(location, current->count, (const GLfloat*)&data[offset]); break;
                    case UniformDimension::D4: glUniform4fv(location, current->count, (const GLfloat*)&data[offset]); break;
                    default: assert(false);
                }
                break;

            default:
                assert(false);
            }

            offset += ComputeByteSize(current->type) * countElements(current->dimension) * current->count;
        }

        XE_GRAPHICS_GL_CHECK_ERROR();
    }

    void GraphicsDeviceES2::setViewport(const Viewport &viewport) {
        GLint x = static_cast<GLint>(viewport.position.X);
        GLint y = static_cast<GLint>(viewport.position.Y);

        GLint w = static_cast<GLint>(viewport.size.X);
        GLint h = static_cast<GLint>(viewport.size.Y);

        glViewport(x, y, w, h);

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
