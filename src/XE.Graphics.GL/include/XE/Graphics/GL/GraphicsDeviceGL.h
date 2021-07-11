
#ifndef __XE_GRAPHICS_GL_GRAPHICSDEVICEGL_HPP__
#define __XE_GRAPHICS_GL_GRAPHICSDEVICEGL_HPP__

#include <XE/Graphics/Viewport.h>
#include <XE/Graphics/GraphicsDevice.h>

#include <glad/glad.h>

namespace XE {
    class ProgramGL;

    class IGraphicsContextGL;

    class GraphicsDeviceGL : public GraphicsDevice {
    public:
        explicit GraphicsDeviceGL(IGraphicsContextGL *context);

        virtual ~GraphicsDeviceGL();

        virtual Subset* createSubset(
            const SubsetDescriptor& desc, 
            std::vector<Buffer*> buffers,
            const std::map<std::string, int> &bufferMapping, 
            Buffer* indexBuffer) override;
        
        virtual Buffer* createBuffer(const BufferDescriptor &bufferDescriptor) override;
        
        virtual Texture2D* createTexture2D(const PixelFormat format, const Vector2i &size, const PixelFormat sourceFormat, const DataType sourceDataType, const void *sourceData) override;
        
        virtual Texture3D* createTexture3D(const PixelFormat format, const Vector3i &size, const PixelFormat sourceFormat, const DataType sourceDataType, const void *sourceData) override;
        
        virtual Texture2DArray* createTexture2DArray(const PixelFormat format, const Vector2i &size, const int count) override;
        
        virtual TextureCubeMap* createTextureCubeMap(const PixelFormat format, const Vector2i &size, const PixelFormat sourceFormat, const DataType sourceDataType, const void **sourceData) override;
        
        virtual Program* createProgram(const ProgramDescriptor &programDescriptor) override;
        
        virtual void setMaterial(const Material *material) override;

        virtual const Material* getMaterial() const override;

        virtual void setProgram(const Program *program) override;

        virtual const Program* getProgram() const override;

        virtual void applyUniform(const UniformMatrix *uniformMatrix, const int count, const std::byte *data) override;

        virtual void applyUniform(const Uniform *uniform, const int count, const std::byte *data) override;

        virtual void beginFrame(const ClearFlags flags, const Vector4f &color, const float depth, const int stencil) override;
        
        virtual void draw(const Subset *subset, const SubsetEnvelope *envelopes, const int envelopeCount) override;

        virtual void endFrame() override;

        virtual void setViewport(const Viewport &viewport) override;

        virtual Viewport getViewport() const override;

    private:
        void preRenderMaterial(const Material *material);

        void postRenderMaterial(const Material *material);

    private:
        IGraphicsContextGL *context = nullptr;

        const ProgramGL *m_program = nullptr;
        const Material *m_material = nullptr;

        Viewport m_viewport;
    };
}

#endif
