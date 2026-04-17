
#pragma once 

#include <cstdint>
#include <cstddef>

#include <xe/graphics/BufferDescriptor.h>
#include <xe/graphics/GraphicsDevice.h>

namespace xe {
    using Handle = uint32_t;

	struct RenderDeviceBackendContext {
		// TODO: Put here common utilities / data usable for all contexts (allocators?, logging? profiling?)
	};

	struct TextureDescriptor {
		PixelFormat format;
		ivec3 size;
		PixelFormat sourceFormat;
		DataType sourceDataType;
		const void *sourceData;
	};

    //! semantic vertex attribute
    enum class VertexAttribSemantic : int {
		Position,
		Normal,
		TexCoord0
    };

	enum class VertexAttribFormat {
		int1,
		int2,
		int3,
		int4,
		float1,
		float2,
		float3,
		float4
	};

    /**
    * @brief Describes a Vertex Attribute for use in the vertex shader
    */
    struct VertexAttrib {
        VertexAttribSemantic semantic;
        int location = 0;

		VertexAttribFormat format;
        bool normalized = false;
    };

    enum class GeometryIndexType {
        uint16,
        uint32
    };

    enum class VertexLayoutResolveMode {
		Semantic,
		Explicit
    };

    struct VertexLayoutDescriptor {
        std::vector<VertexAttrib> attribs;
        GeometryIndexType indexType = GeometryIndexType::uint16;
        VertexLayoutResolveMode resolveMode = VertexLayoutResolveMode::Semantic;
    };

    struct GeometryDescriptor {
		Handle layoutHandle = 0;
        std::vector<Handle> buffers;
        Handle indexBufferHandle = 0;
    };

	struct ShaderProgramDescriptor {
		std::string glslVertexShader;
		std::string glslFragmentShader;
	};

	struct PipelineDescriptor {
		Handle layoutHandle = 0;
		Handle shaderProgramHandle;
		ClearFlags clearFlags = ClearFlags::Color;
		vec4 clearColor = { 0.0f, 0.0f, 0.0f, 1.0f };
	};
}
