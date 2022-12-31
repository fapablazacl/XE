
#ifndef __XE_GRAPHICS_MATERIAL_HPP__
#define __XE_GRAPHICS_MATERIAL_HPP__

#include <cstddef>
#include <array>
#include <xe/Predef.h>

namespace XE {
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
        bool depthTest = true;
        bool stencilTest = false;
        bool cullBackFace = false;
        bool blendEnable = false;

        BlendParam blendSource = BlendParam::One;
        BlendParam blendDestination = BlendParam::One;

        float lineWidth = 1.0f;
        DepthFunc depthFunc = DepthFunc::LesserEqual;
        FrontFaceOrder frontFace = FrontFaceOrder::Clockwise;
        int clipDistanceCount = 0;
    };

    /**
     * @brief Describes the generic visual appearance of all the objects. For other properties, use uniforms.
     * @note Very unstable interface.
     */
    class Material {
    public:
        ~Material();

        std::array<MaterialLayer, 8> layers;
        int layerCount = 0;
        MaterialRenderState renderState;
    };
}

#endif
