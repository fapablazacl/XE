
#pragma once

#include <cstdint>
#include <cstddef>
#include <array>
#include <type_traits>

#include <xe/DataType.h>
#include <xe/graphics/GraphicsDevice.h>
#include <xe/graphics/BufferDescriptor.h>

namespace xe {
	/**
	 * @brief Compile-time tag discriminating handle kinds.
	 * Encoded in the C++ type system via HandleT's Tag template parameter; no longer stored in the
	 * handle bit payload (the former type:4 field is now reserved).
	 */
	enum HandleType : uint32_t {
		HandleBuffer = 0x01,
		HandleShader,
		HandleTexture,
		HandleVertexLayout,
		HandlePipeline,
		HandleGeometry
	};

	namespace detail {
		//! Sentinel subtype enum used by handles whose resource class has no meaningful subtype.
		enum class NoSubtype : uint32_t { None = 0 };

		/**
		 * @brief Zero-cost strongly-typed handle to a backend resource.
		 *
		 * Instances of HandleT with different Tag (HandleType) or SubEnum template parameters are
		 * distinct C++ types, so mixing a buffer handle with a texture handle is a compile error.
		 * The raw 32-bit payload is laid out as:
		 *
		 *   bit: 31       28 27        20 19      16 15                    0
		 *       +-----------+-------------+----------+------------------------+
		 *       | subType:4 |    gen:8    | reserved |       index:16         |
		 *       +-----------+-------------+----------+------------------------+
		 *
		 * @tparam Tag compile-time HandleType tag (identifies resource class)
		 * @tparam SubEnum resource-specific subtype enum (e.g. TextureType for textures). Defaults to
		 *         NoSubtype when the resource class has no subtype dimension.
		 */
		template <HandleType Tag, class SubEnum = NoSubtype>
		struct HandleT {
			static constexpr uint32_t IndexMask    = 0xFFFFu;
			static constexpr uint32_t ReservedMask = 0xFu;
			static constexpr uint32_t GenMask      = 0xFFu;
			static constexpr uint32_t SubTypeMask  = 0xFu;

			static constexpr uint32_t IndexShift    = 0;
			static constexpr uint32_t ReservedShift = 16;
			static constexpr uint32_t GenShift      = 20;
			static constexpr uint32_t SubTypeShift  = 28;

			//! Compile-time tag surface, for diagnostics and SFINAE.
			static constexpr HandleType tag = Tag;

			//! Raw bit payload. A default-constructed handle (raw == 0) is invalid.
			uint32_t raw = 0;

			/**
			 * @brief Test whether this handle refers to a live resource.
			 * A zero-initialized handle is the canonical invalid sentinel.
			 */
			constexpr bool isValid() const { return raw != 0; }

			//! Pool slot index assigned by the backend.
			constexpr uint32_t index() const { return (raw >> IndexShift) & IndexMask; }

			//! Generation counter captured at creation time; used by the backend to detect use-after-free.
			constexpr uint32_t gen() const { return (raw >> GenShift) & GenMask; }

			//! Strongly-typed subtype (e.g. TextureType::Tex2D for a TextureHandle).
			constexpr SubEnum subType() const {
				return static_cast<SubEnum>((raw >> SubTypeShift) & SubTypeMask);
			}

			/**
			 * @brief Pack an (index, gen, subType) triple into a typed handle.
			 * Call site responsibility to ensure each field fits its bit width; debug builds assert.
			 * @param gen generation counter supplied by the backend's pool
			 * @param index pool slot index
			 * @param sub resource-specific subtype (defaults to SubEnum's zero value)
			 */
			static constexpr HandleT make(uint32_t gen, uint32_t index, SubEnum sub = SubEnum{}) {
				const uint32_t packed =
					((static_cast<uint32_t>(sub) & SubTypeMask) << SubTypeShift) |
					((gen                        & GenMask)     << GenShift)     |
					((index                      & IndexMask)   << IndexShift);
				return HandleT{packed};
			}

			friend constexpr bool operator==(HandleT a, HandleT b) { return a.raw == b.raw; }
			friend constexpr bool operator!=(HandleT a, HandleT b) { return a.raw != b.raw; }
		};
	} // namespace detail

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

	// Typed handle aliases - use these at every API boundary. Each alias is a distinct C++ type,
	// so mixing kinds (e.g. passing a ShaderHandle where a BufferHandle is expected) is a compile
	// error. Runtime payload is still a single uint32_t - zero overhead vs. the original untyped
	// Handle.
	using BufferHandle       = detail::HandleT<HandleBuffer,       BufferType>;
	using TextureHandle      = detail::HandleT<HandleTexture,      TextureType>;
	using ShaderHandle       = detail::HandleT<HandleShader>;
	using VertexLayoutHandle = detail::HandleT<HandleVertexLayout>;
	using PipelineHandle     = detail::HandleT<HandlePipeline>;
	using GeometryHandle     = detail::HandleT<HandleGeometry>;

	static_assert(sizeof(BufferHandle)       == 4);
	static_assert(sizeof(TextureHandle)      == 4);
	static_assert(sizeof(ShaderHandle)       == 4);
	static_assert(sizeof(VertexLayoutHandle) == 4);
	static_assert(sizeof(PipelineHandle)     == 4);
	static_assert(sizeof(GeometryHandle)     == 4);
	static_assert(std::is_trivially_copyable_v<BufferHandle>);
	static_assert(std::is_trivially_copyable_v<TextureHandle>);

    struct GeometryDescriptor {
		VertexLayoutHandle layoutHandle;
        std::vector<BufferHandle> buffers;
        BufferHandle indexBufferHandle;
    };

	struct ShaderProgramDescriptor {
		std::string glslVertexShader;
		std::string glslFragmentShader;
	};

	struct PipelineDescriptor {
		VertexLayoutHandle layoutHandle;
		ShaderHandle shaderProgramHandle;
		ClearFlags clearFlags = ClearFlags::Color;
		vec4 clearColor = { 0.0f, 0.0f, 0.0f, 1.0f };
	};
}
