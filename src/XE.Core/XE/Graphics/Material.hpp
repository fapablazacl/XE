
#ifndef __XE_GRAPHICS_MATERIAL_HPP__
#define __XE_GRAPHICS_MATERIAL_HPP__

#include <cstddef>
#include <array>

namespace XE::Graphics {
    class Program;
    class Texture;

    enum class TextureFilter {
        Nearest,
        Linear
    };

    enum class TextureWrap {
        Repeat,
        Clamp
    };

    struct MaterialLayer {
        const Texture* Texture = nullptr;
        TextureFilter MinFilter = TextureFilter::Nearest;
        TextureFilter MagFilter = TextureFilter::Nearest;
        TextureWrap WrapS = TextureWrap::Repeat;
        TextureWrap WrapT = TextureWrap::Repeat;
        TextureWrap WrapR = TextureWrap::Repeat;
    };

    enum class PolygonMode {
        Point,
        Line,
        Triangle
    };

    struct MaterialRenderState {
        bool DepthTest = true;
        bool StencilTest = false;

        PolygonMode PolygonMode = PolygonMode::Triangle;
    };

    class Material {
    public:
        virtual ~Material();

        MaterialLayer GetLayer(const int index) const {
            return m_layers[index];
        }

        void SetLayer(const int index, const MaterialLayer& layer) {
            m_layers[index] = layer;
        }

        int GetLayerCount() const {
            return 8;
        }

        void SetRenderState(const MaterialRenderState &renderState);

        virtual std::byte* GetAttributePtr() const = 0;

        virtual int GetAttributeSize() const = 0;

    public:

    protected:
        std::array<MaterialLayer, 8> m_layers;
        MaterialRenderState m_renderState;
    };
}

#endif
