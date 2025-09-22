
#ifndef __XE_GRAPHICS_GL_GRAPHICSDEVICEGL_HPP__
#define XE_GRAPHICS_GL_GRAPHICSDEVICEGL_HPP_

#include <xe/graphics/GraphicsContext.h>
#include <xe/graphics/GraphicsDevice.h>
#include <xe/graphics/Viewport.h>

namespace XE {
    class ProgramGL;
    class SubsetGL;

    class GraphicsDeviceGL : public GraphicsDevice {
    public:
        explicit GraphicsDeviceGL(GraphicsContext *context);

        ~GraphicsDeviceGL() override;

        Subset *createSubset(const SubsetDescriptor &desc) override;

        Buffer *createBuffer(const BufferDescriptor &bufferDescriptor) override;

        Texture2D *createTexture2D(PixelFormat format, const Vector2i &size, PixelFormat sourceFormat, DataType sourceDataType, const void *sourceData) override;

        Texture3D *createTexture3D(PixelFormat format, const Vector3i &size, PixelFormat sourceFormat, DataType sourceDataType, const void *sourceData) override;

        Texture2DArray *createTexture2DArray(PixelFormat format, const Vector2i &size, int count) override;

        TextureCubeMap *
        createTextureCubeMap(PixelFormat format, const Vector2i &size, PixelFormat sourceFormat, DataType sourceDataType, const void **sourceData) override;

        Program *createProgram(const ProgramDescriptor &desc) override;

        void setMaterial(const Material *material) override;

        const Material *getMaterial() const override;

        void setProgram(const Program *program) override;

        const Program *getProgram() const override;

        void applyUniform(const UniformMatrix *uniformMatrix, size_t count, const void *data) override;

        void applyUniform(const Uniform *uniform, size_t count, const void *data) override;

        void beginFrame(ClearFlags flags, const Vector4 &color, float depth, int stencil) override;

        void draw(const Subset *subset, const SubsetEnvelope *envelopes, size_t envelopeCount) override;

        void endFrame() override;

        void setViewport(const Viewport &viewport) override;

        Viewport getViewport() const override;

    private:
        static void preRenderMaterial(const Material *material);

        static void postRenderMaterial(const Material *material);

    
        GraphicsContext *context = nullptr;

        const ProgramGL *m_program = nullptr;
        const Material *m_material = nullptr;

        Viewport m_viewport;
    };
} // namespace XE

#endif
