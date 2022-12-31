
#include <xe/graphics/ES2/GraphicsDeviceES2.h>

#include <glad/glad.h>

#include <xe/graphics/ES2/ConversionES.h>
#include <xe/graphics/ES2/BufferES2.h>
#include <xe/graphics/ES2/SubsetES2.h>
#include <xe/graphics/ES2/Texture2DES2.h>
#include <xe/graphics/ES2/ProgramES2.h>
#include <xe/graphics/ES2/Util.h>

#include <xe/graphics/Material.h>
#include <xe/graphics/Uniform.h>
#include <xe/graphics/Subset.h>
#include <xe/graphics/Texture3D.h>
#include <xe/graphics/Texture2DArray.h>
#include <xe/graphics/TextureCubeMap.h>
#include <xe/graphics/GraphicsContext.h>
#include <iostream>

namespace XE {
    static std::string hexstr(const GLenum value) {
        std::string str;
        
        str.resize(16, ' ');
        std::sprintf(str.data(), "%x", value);
        
        return str;
    }
    
    static std::string stringval(const GLenum err) {
        switch (err) {
        case GL_INVALID_ENUM: return "GL_INVALID_ENUM";
        case GL_INVALID_VALUE: return "GL_INVALID_VALUE";
        case GL_INVALID_OPERATION: return "GL_INVALID_OPERATION";
            
#if defined(GL_STACK_OVERFLOW)
        case GL_STACK_OVERFLOW: return "GL_STACK_OVERFLOW";
#endif

#if defined(GL_STACK_UNDERFLOW)
        case GL_STACK_UNDERFLOW: return "GL_STACK_UNDERFLOW";
#endif
            
        case GL_OUT_OF_MEMORY: return "GL_OUT_OF_MEMORY";
        case GL_INVALID_FRAMEBUFFER_OPERATION: return "GL_INVALID_FRAMEBUFFER_OPERATION";
            
#if defined(GL_CONTEXT_LOST)
        case GL_CONTEXT_LOST: return "GL_CONTEXT_LOST";
#endif
            
#if defined(GL_TABLE_TOO_LARGE)
        case GL_TABLE_TOO_LARGE: return "GL_TABLE_TOO_LARGE";
#endif
            
        default:
            return "UNNOWN_ERR_CODE_" + hexstr(err);
        }
    }
    
    
    void GraphicsDeviceES_callback(const char *name, void *, int , ...) {
        if (std::string(name) == "glGetError") {
            return;
        }
        
        GLenum err = glGetError();
        
        if (err != GL_NO_ERROR) {
            std::cerr << "GraphicsDeviceES: Error while calling function " << name << std::endl;
            std::cerr << "GraphicsDeviceES: Errors generated:" << std::endl;
            
            while (err != GL_NO_ERROR) {
                std::cerr << "GraphicsDeviceES:" << stringval(err) << std::endl;
                err = glGetError();
            }
            
            throw std::runtime_error("GraphicsDeviceES: Error while calling function " + std::string(name));
        }
    }
    
    GraphicsDeviceES2::GraphicsDeviceES2(GraphicsContext *context) : context(context) {
        assert(context);

        if (!gladLoadGLES2Loader((GLADloadproc)context->getProcAddressFunctionGL())) {
            throw std::runtime_error("Couldn't load ES extensions");
        }
        
#ifndef NDEBUG
        glad_set_post_callback_gl(GraphicsDeviceES_callback);
        glad_set_post_callback(GraphicsDeviceES_callback);
#endif

        XE_GRAPHICS_GL_CHECK_ERROR();
    }


    GraphicsDeviceES2::~GraphicsDeviceES2() {}
    

    Subset* GraphicsDeviceES2::createSubset(const SubsetDescriptor& desc) {
        return new SubsetES(desc);
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
    

    void GraphicsDeviceES2::draw(const Subset *subset, const SubsetEnvelope *envelopes, const size_t envelopeCount) {
        assert(subset);
        assert(envelopes);
        assert(envelopeCount > 0);

        auto subsetES = static_cast<const SubsetES *>(subset);

        subsetES->bind();
        
        auto indexBuffer = subsetES->getIndexBuffer();

        if (!indexBuffer) {
            for (size_t i=0; i<envelopeCount; i++) {
                const SubsetEnvelope &env = envelopes[i];
                const GLenum primitiveGL = convertToES(env.primitive);

                glDrawArrays(primitiveGL, env.vertexStart, env.vertexCount);
            }
        } else {
            const GLenum indexTypeGL = GL_INT;/* convertToES(descriptor.indexType)*/;

            for (size_t i=0; i<envelopeCount; i++) {
                const SubsetEnvelope &env = envelopes[i];
                const GLenum primitiveGL = convertToES(env.primitive);

                assert(env.vertexCount);
                
                glDrawElements(primitiveGL, env.vertexCount, indexTypeGL, nullptr);
            }
        }

        subsetES->unbind();

        XE_GRAPHICS_GL_CHECK_ERROR();
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

        const GLenum depthFuncGL = convertToES(rs.depthFunc);
        glDepthFunc(depthFuncGL);

        // front face definition
        const GLenum faceGL = convertToES(rs.frontFace);
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

            const GLenum sfactorGL = convertToES(rs.blendSource);
            const GLenum dfactorGL = convertToES(rs.blendDestination);
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
            glTexParameteri(target, GL_TEXTURE_MAG_FILTER, convertToES(layer.minFilter));
            glTexParameteri(target, GL_TEXTURE_MIN_FILTER, convertToES(layer.magFilter));
            glTexParameteri(target, GL_TEXTURE_WRAP_S, convertToES(layer.wrapS));
            glTexParameteri(target, GL_TEXTURE_WRAP_T, convertToES(layer.wrapT));
            /*glTexParameteri(target, GL_TEXTURE_WRAP_R, convertToES(layer.wrapR));*/
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

    void GraphicsDeviceES2::applyUniform(const UniformMatrix *uniformMatrix, const size_t count, const void *data) {
        // TODO: Add support for matrix transposition

        assert(m_program);
        assert(uniformMatrix);
        assert(count > 0);
        assert(data);

        size_t offset = 0;
        
        const auto ptr = reinterpret_cast<const std::byte*>(data);

        for (size_t i=0; i<count; i++) {
            const UniformMatrix *current = &uniformMatrix[i];
            const GLint location = m_program->getUniformLoction(current->name);

            switch (current->type) {
            case DataType::Float32: {
                const auto values = (const GLfloat*)&ptr[offset];
                
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

            offset += bytesize(current->type) * countElements(current->shape) * current->count;
        }

        XE_GRAPHICS_GL_CHECK_ERROR();
    }

    void GraphicsDeviceES2::applyUniform(const Uniform *uniform, const size_t count, const void *data) {
        assert(m_program);
        assert(uniform);
        assert(count > 0);
        assert(data);

        const auto ptr = reinterpret_cast<const std::byte*>(data);
        
        size_t offset = 0;

        for (size_t i=0; i<count; i++) {
            const Uniform *current = &uniform[i];
            const GLint location = m_program->getUniformLoction(current->name);

            assert(location >= 0);

            switch (current->type) {
            case DataType::Int32:
                switch (current->dimension) {
                    case UniformDimension::D1: glUniform1iv(location, current->count, (const GLint*)&ptr[offset]); break;
                    case UniformDimension::D2: glUniform2iv(location, current->count, (const GLint*)&ptr[offset]); break;
                    case UniformDimension::D3: glUniform3iv(location, current->count, (const GLint*)&ptr[offset]); break;
                    case UniformDimension::D4: glUniform4iv(location, current->count, (const GLint*)&ptr[offset]); break;
                    default: assert(false);
                }
                break;
            
            case DataType::Float32:
                switch (current->dimension) {
                    case UniformDimension::D1: glUniform1fv(location, current->count, (const GLfloat*)&ptr[offset]); break;
                    case UniformDimension::D2: glUniform2fv(location, current->count, (const GLfloat*)&ptr[offset]); break;
                    case UniformDimension::D3: glUniform3fv(location, current->count, (const GLfloat*)&ptr[offset]); break;
                    case UniformDimension::D4: glUniform4fv(location, current->count, (const GLfloat*)&ptr[offset]); break;
                    default: assert(false);
                }
                break;

            default:
                assert(false);
            }

            offset += bytesize(current->type) * elementcount(current->dimension) * current->count;
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
