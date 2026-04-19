
#pragma once 

#include <cstdint>
#include <cstddef>
#include <array>

#include <xe/DataType.h>
#include <xe/graphics/GraphicsDevice.h>
#include <xe/graphics/BufferDescriptor.h>

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
		static const uint32_t SubTypeMask = 0xFu;
		static const uint32_t GenMask = 0xFFu;
		static const uint32_t TypeMask = 0xFu;

		static const uint32_t IndexShift = 0;
		static const uint32_t SubTypeShift = 16;
		static const uint32_t GenShift = 20;
		static const uint32_t TypeShift = 28;

		uint32_t raw = 0;

		uint32_t index() const {
			return (raw >> IndexShift) & IndexMask;
		}

		//! Resource-specific subtype (e.g. TextureType for HandleTexture). Value 0 for handles that don't use it.
		uint32_t subType() const {
			return (raw >> SubTypeShift) & SubTypeMask;
		}

		uint32_t gen() const {
			return (raw >> GenShift) & GenMask;
		}

		uint32_t type() const {
			return (raw >> TypeShift) & TypeMask;
		}

		static Handle make(HandleType type, uint32_t gen, uint32_t index, uint32_t subType = 0) {
			const uint32_t raw = (type << TypeShift) | (gen << GenShift) | ((subType & SubTypeMask) << SubTypeShift) | (index << IndexShift);
			return Handle{raw};
		}
	};

	static_assert(sizeof(Handle) == 4);

	struct RenderDeviceBackendContext {
		// TODO: Put here common utilities / data usable for all contexts (allocators?, logging? profiling?)
	};

	enum class TextureType { Tex1D, Tex2D, Tex3D, TexCubeMap, Tex2DArray };

	/**
	 * @brief Non-owning view of a single mip level's source data.
	 * Held by the caller; pointers must outlive the createTexture call.
	 */
	struct MipLevel {
		//! Pixel data for this level. nullptr means "allocate storage only".
		const void *data = nullptr;
	};

	/**
	 * @brief Describes a texture to create on the backend.
	 *
	 * The mipLevels array is non-owning. For non-cube textures it holds
	 * mipLevelCount entries indexed as mipLevels[mip]. For cube textures it
	 * holds mipLevelCount == mipCount * 6 entries indexed as
	 * mipLevels[mip * 6 + face] (mip-major, face-minor, matches KTX).
	 * mipLevels == nullptr / mipLevelCount == 0 implies a single implicit
	 * level 0 with null data (storage-only allocation).
	 */
	struct TextureDescriptor {
		TextureType type = TextureType::Tex2D;
		PixelFormat format = PixelFormat::R8G8B8;
		ivec3 size;
		PixelFormat sourceFormat;
		DataType sourceDataType;

		//! Borrowed pointer to an array of MipLevel entries. Must outlive createTexture().
		const MipLevel *mipLevels = nullptr;

		//! Number of entries in mipLevels.
		size_t mipLevelCount = 0;

		//! Generate the mip chain from level 0 via glGenerateMipmap. Mutually exclusive with supplying >1 mip level.
		bool generateMipmaps = false;
	};

	/**
	 * @brief Describes a partial read from an existing buffer.
	 * The backend fills [data, data + size) with bytes from the source buffer
	 * starting at byte 'offset'. Synchronous; prefer for tooling / debug / snapshot use.
	 */
	struct BufferReadDescriptor {
		//! Byte offset into the source buffer.
		size_t offset = 0;

		//! Number of bytes to read.
		size_t size = 0;

		//! Destination, caller-allocated, at least 'size' bytes.
		void *data = nullptr;
	};

	/**
	 * @brief Describes a read of (part of) an existing texture.
	 *
	 * The offset/size fields describe the sub-region to read. The GL 3.3 backend
	 * supports whole-mip reads only (glGetTexImage has no region variant until
	 * GL 4.5 / glGetTextureSubImage); it asserts that offset == {0,0,0} and that
	 * size matches the mip level's full extent. Future backends (GL 4.5+, PBO-based,
	 * Vulkan) may honor arbitrary regions transparently.
	 *
	 * Synchronous; prefer for tooling / debug / snapshot use.
	 */
	struct TextureReadDescriptor {
		//! Region origin inside the target mip level, in texels. Must be {0,0,0} on the GL 3.3 backend.
		ivec3 offset = { 0, 0, 0 };

		//! Region extent in texels. Must equal the mip level's full extent on the GL 3.3 backend.
		ivec3 size;

		//! Target mip level index.
		int mipLevel = 0;

		//! Cubemap face index [0,6). Ignored for non-cube targets.
		int faceIndex = 0;

		//! Desired pixel layout of the destination buffer.
		PixelFormat destFormat = PixelFormat::Unknown;

		//! Desired component data type of the destination buffer.
		DataType destDataType = DataType::Unknown;

		//! Destination, caller-allocated, sized for the described region.
		void *data = nullptr;
	};

	/**
	 * @brief Describes a partial update to an existing texture.
	 * offset + size define the region inside the target mip level (and face, for cubemaps).
	 */
	struct TextureUpdateDescriptor {
		//! Region origin inside the target mip level, in texels.
		ivec3 offset = { 0, 0, 0 };

		//! Region extent in texels. Unused axes should be 1.
		ivec3 size;

		//! Target mip level index.
		int mipLevel = 0;

		//! Cubemap face index [0,6). Ignored for non-cube targets.
		int faceIndex = 0;

		//! Pixel layout of sourceData.
		PixelFormat sourceFormat = PixelFormat::Unknown;

		//! Component data type of sourceData.
		DataType sourceDataType = DataType::Unknown;

		//! Source pixel data. Must cover the region described by size.
		const void *sourceData = nullptr;
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
