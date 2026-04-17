
#pragma once 

#include <cstdint>
#include <cstddef>

#include <xe/graphics/BufferDescriptor.h>
#include <xe/graphics/GraphicsDevice.h>

namespace xe {
	enum HandleType : uint32_t {
		HandleBuffer = 0x01,
		HandleShader,
		HandleTexture,
		HandleVertexLayout,
		HandlePipeline,
		HandleGeometry
	};

	//! Resource Handle
	struct Handle {
		static const uint32_t IndexMask = 0xFFFFu;
		static const uint32_t GenMask = 0xFFu;
		static const uint32_t TypeMask = 0xFu;

		static const uint32_t IndexShift = 0;
		static const uint32_t GenShift = 20;
		static const uint32_t TypeShift = 28;

		uint32_t raw = 0;

		uint32_t index() const {
			return (raw >> IndexShift) & IndexMask;
		}

		uint32_t gen() const {
			return (raw >> GenShift) & GenMask;
		}

		uint32_t type() const {
			return (raw >> TypeShift) & TypeMask;
		}

		static Handle make(HandleType type, uint32_t gen, uint32_t index) {
			const uint32_t raw = (type << TypeShift) | (gen << GenShift) | (index << IndexShift);
			return Handle{raw};
		}
	};

	static_assert(sizeof(Handle) == 4);

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
		Handle layoutHandle;
        std::vector<Handle> buffers;
        Handle indexBufferHandle;
    };

	struct ShaderProgramDescriptor {
		std::string glslVertexShader;
		std::string glslFragmentShader;
	};

	struct PipelineDescriptor {
		Handle layoutHandle;
		Handle shaderProgramHandle;
		ClearFlags clearFlags = ClearFlags::Color;
		vec4 clearColor = { 0.0f, 0.0f, 0.0f, 1.0f };
	};
}
