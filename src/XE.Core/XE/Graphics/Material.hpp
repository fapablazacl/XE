
#ifndef __XE_GRAPHICS_MATERIAL_HPP__
#define __XE_GRAPHICS_MATERIAL_HPP__

#include <cstddef>
#include <array>
#include <XE/Predef.hpp>

namespace XE::Graphics {
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
        const Texture* texture = nullptr;
        TextureFilter minFilter = TextureFilter::Nearest;
        TextureFilter magFilter = TextureFilter::Nearest;
        TextureWrap wrapS = TextureWrap::Repeat;
        TextureWrap wrapT = TextureWrap::Repeat;
        TextureWrap wrapR = TextureWrap::Repeat;
    };

    /**
     * @brirf Fill mode for polygon
     */
    enum class PolygonMode {
        Point,
        Line,
        Fill
    };

    enum class DepthFunc {
        Never,
        Less,
        Equal,
        LesserEqual,
        Greater,
        NotEqual,
        GreaterEqual,
        Always
    };

    enum class FrontFaceOrder {
        Clockwise,
        CounterClockwise
    };

    enum class BlendParam {
        Zero, 
        One, 
        SourceColor,
        OneMinusSourceColor,
        DestinationColor,
        OneMinusDestinationColor,
        SourceAlpha,
        OneMinusSourceAlpha,
        DestinationAlpha,
        OneMinusDestinationAlpha,
        ConstantColor,
        OneMinusConstantColor,
        ConstantAlpha,
        OneMinusConstantAlpha
    };

    /**
     * @brief Graphics Device current render states
     */
    struct MaterialRenderState {
        bool depthTest = false;
        bool stencilTest = false;
        bool cullBackFace = false;
        bool blendEnable = false;

        BlendParam blendSource;
        BlendParam blendDestination;

        float pointSize = 1.0f;
        float lineWidth = 1.0f;
        DepthFunc depthFunc = DepthFunc::LesserEqual;
        FrontFaceOrder frontFace = FrontFaceOrder::Clockwise;
        PolygonMode polygonMode = PolygonMode::Fill;
        std::array<bool, 8> clipDistances;
        int clipDistanceCount = 0;
    };

    /**
     * @brief Describes the genric visual appearance of all the objects. For other properties, use uniforms.
     * @note Very unstable interface.
     */
    class Material {
    public:
        ~Material();

        std::array<MaterialLayer, 8> layers;
        int layerCount = 1;
        MaterialRenderState renderState;
    };
}

#endif
