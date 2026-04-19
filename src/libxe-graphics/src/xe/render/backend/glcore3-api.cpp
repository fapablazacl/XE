
#include <glaze/gl.hpp>
#include <glaze/raii.hpp>
#include <xe/math/Vector.h>
#include <xe/render/RenderBackend.h>
#include <cassert>
#include <utility>
#include <vector>
#include <iostream>
#include <array>

#include "glcore3-api.h"

namespace xe {
	namespace {
		struct PipelineGL {
			xe::vec4 clearColor = { 0.0f, 0.0f, 0.0f, 1.0f };
			gl::Flags<gl::ClearBufferMask> clearMask = gl::ClearBufferMask::eColorBufferBit;

			//! @note: Consider a Weak ptr
			gl::Program shaderProgram;
		};

		inline RenderDeviceBackendContextGL* glctx(RenderDeviceBackendContext *ctx) {
			assert(ctx);
			return static_cast<RenderDeviceBackendContextGL* >(ctx);
		}

		// 16-bit index field in the handle bit layout; caps every pool at 65536 entries.
		constexpr uint32_t kHandleIndexLimit = 0x10000u;
		// 8-bit gen field in the handle bit layout; rolls over every 256 reuses per slot.
		constexpr uint32_t kHandleGenMask = 0xFFu;

		/**
		 * @brief Reuse an empty slot (or append a new one) and return (index, gen).
		 *
		 * Linear-scans pool looking for a slot whose RAII holder is empty - those are free slots
		 * awaiting reuse. On hit, moves obj into that slot and bumps its gen counter. On miss,
		 * appends a new slot. Returns the index and the final gen value for the caller to pack
		 * into the typed handle. Scan cost is O(n); resource creation is not on the frame hot
		 * path so this is intentional.
		 *
		 * @tparam T underlying GL resource type (gl::BufferId, gl::Texture, gl::Program, ...)
		 * @param pool the pool vector to acquire a slot in; grows if no free slot is available
		 * @param obj RAII holder for the GL object to install at the acquired slot
		 * @return {index, gen} pair ready to be passed to HandleT::make
		 */
		template <class T>
		std::pair<uint32_t, uint8_t>
		acquireSlot(std::vector<Slot<T>>& pool, glaze::Unique<T>&& obj) {
			for (uint32_t i = 0; i < pool.size(); ++i) {
				if (!pool[i].obj) {
					pool[i].obj = std::move(obj);
					pool[i].gen = static_cast<uint8_t>((pool[i].gen + 1) & kHandleGenMask);
					return { i, pool[i].gen };
				}
			}
			assert(pool.size() < kHandleIndexLimit && "handle index exhausted (16-bit field)");
			pool.push_back({ std::move(obj), 0 });
			return { static_cast<uint32_t>(pool.size() - 1), 0 };
		}
	}

	RenderDeviceBackendContext* createContextGL() {
		return new RenderDeviceBackendContextGL();
	}

	void destroyContextGL(RenderDeviceBackendContext* ctx) {
		delete glctx(ctx);
	}

	inline const void* mipLevelDataAt(const TextureDescriptor &desc, size_t mip, size_t face, size_t faceCount) {
		if (desc.mipLevels == nullptr) {
			return nullptr;
		}
		return desc.mipLevels[mip * faceCount + face].data;
	}

	inline int halveDimension(int size) {
		return size > 1 ? size / 2 : 1;
	}

	static constexpr std::array<gl::TextureTarget, 6> cubeMapSides {
		gl::TextureTarget::eTextureCubeMapPositiveX,
		gl::TextureTarget::eTextureCubeMapNegativeX,
		gl::TextureTarget::eTextureCubeMapPositiveY,
		gl::TextureTarget::eTextureCubeMapNegativeY,
		gl::TextureTarget::eTextureCubeMapPositiveZ,
		gl::TextureTarget::eTextureCubeMapNegativeZ,
	};

	gl::BufferTarget toBufferTargetGL(BufferType type) {
		switch (type) {
		case BufferType::Vertex:
			return gl::BufferTarget::eArrayBuffer;

		case BufferType::Index:
			return gl::BufferTarget::eElementArrayBuffer;

		default:
			assert(false && "toTextureTargetGL: Invalid BufferType");
			return gl::BufferTarget::eArrayBuffer;
		}
	}

	gl::BufferUsage toBufferUsageGL(const BufferUsage usage) {
		switch (usage) {
		case BufferUsage::StreamDraw: return gl::BufferUsage::eStreamDraw;
		case BufferUsage::StreamRead: return gl::BufferUsage::eStreamRead;
		case BufferUsage::StreamCopy: return gl::BufferUsage::eStreamCopy;
		case BufferUsage::StaticDraw: return gl::BufferUsage::eStaticDraw;
		case BufferUsage::StaticRead: return gl::BufferUsage::eStaticRead;
		case BufferUsage::StaticCopy: return gl::BufferUsage::eStaticCopy;
		case BufferUsage::DynamicDraw: return gl::BufferUsage::eDynamicDraw;
		case BufferUsage::DynamicRead: return gl::BufferUsage::eDynamicRead;
		case BufferUsage::DynamicCopy: return gl::BufferUsage::eDynamicCopy;
		}

		assert(false && "toBufferUsageGL: Invalid BufferUsage");
		return gl::BufferUsage::eStreamDraw;
	}

	BufferHandle createBufferGL(RenderDeviceBackendContext* ctx, const BufferDescriptor& desc) {
		auto &buffers = glctx(ctx)->buffers;

		gl::BufferTarget const target = toBufferTargetGL(desc.type);
		gl::BufferUsage const usage = toBufferUsageGL(desc.usage);

		auto buffer = glaze::makeUnique<gl::BufferId>();
		gl::bindBuffer(target, buffer);
		gl::bufferData(target, desc.size, desc.data, usage);

		auto const [index, gen] = acquireSlot(buffers, std::move(buffer));
		return BufferHandle::make(gen, index, desc.type);
	}

	void destroyBufferGL(RenderDeviceBackendContext* ctx, BufferHandle handle) {
		auto &buffers = glctx(ctx)->buffers;
		uint32_t const index = handle.index();
		assert(index < buffers.size() && "destroyBufferGL: handle index out of range");
		auto &slot = buffers[index];
		assert(slot.obj             && "destroyBufferGL: slot already free (double destroy)");
		assert(slot.gen == handle.gen() && "destroyBufferGL: stale handle (generation mismatch)");
		// gen is bumped on next acquireSlot, not here: keeps the bump tied to actual reuse so
		// destroy-without-reacquire doesn't prematurely burn through the 8-bit field.
		slot.obj.reset({});
	}

	void readBufferGL(RenderDeviceBackendContext* ctx, BufferHandle handle, const BufferReadDescriptor &desc) {
		assert(desc.data != nullptr && "BufferReadDescriptor: data must not be null");
		assert(desc.size > 0 && "BufferReadDescriptor: size must be greater than zero");

		auto &buffers = glctx(ctx)->buffers;
		uint32_t const index = handle.index();
		assert(index < buffers.size() && "readBufferGL: handle index out of range");
		auto &slot = buffers[index];
		assert(slot.obj             && "readBufferGL: use of freed handle");
		assert(slot.gen == handle.gen() && "readBufferGL: stale handle (generation mismatch)");

		gl::BufferTarget const target = toBufferTargetGL(handle.subType());

		gl::bindBuffer(target, slot.obj);
		gl::getBufferSubData(target,
			static_cast<GLintptr>(desc.offset),
			static_cast<GLsizeiptr>(desc.size),
			desc.data);
	}

	gl::TextureTarget toTextureTargetGL(const TextureType type) {
		switch (type) {
		case TextureType::Tex1D:  return gl::TextureTarget::eTexture1d;
		case TextureType::Tex2D:  return gl::TextureTarget::eTexture2d;
		case TextureType::Tex3D:  return gl::TextureTarget::eTexture3d;
		case TextureType::TexCubeMap:  return gl::TextureTarget::eTextureCubeMap;
		case TextureType::Tex2DArray: return gl::TextureTarget::eTexture2dArray;
		}
		assert(false && "toTextureTargetGL: Invalid TextureType");
		return gl::TextureTarget::eTexture2d;
	}

	gl::InternalFormat toInternalFormatGL(const PixelFormat pixelFormat) {
		switch (pixelFormat) {
		case PixelFormat::R8G8B8: return gl::InternalFormat::eRgb;
		case PixelFormat::R8G8B8A8: return gl::InternalFormat::eRgba;
		default: break;
		}
		assert(false && "toInternalFormatGL: Invalid PixelFormat");
		return gl::InternalFormat::eRgba;
	}

	gl::PixelFormat toPixelFormatGL(const PixelFormat pixelFormat) {
		switch (pixelFormat) {
		case PixelFormat::R8G8B8: return gl::PixelFormat::eRgb;
		case PixelFormat::R8G8B8A8: return gl::PixelFormat::eRgba;
		default: break;
		}
		assert(false && "toPixelFormatGL: Invalid PixelFormat");
		return gl::PixelFormat::eRgba;
	}

	gl::PixelType toPixelTypeGL(const DataType dataType) {
		switch (dataType) {
		case DataType::Int8:    return gl::PixelType::eByte;
		case DataType::UInt8:   return gl::PixelType::eUnsignedByte;
		case DataType::Int16:   return gl::PixelType::eShort;
		case DataType::UInt16:  return gl::PixelType::eUnsignedShort;
		case DataType::Int32:   return gl::PixelType::eInt;
		case DataType::UInt32:  return gl::PixelType::eUnsignedInt;
		case DataType::Float32: return gl::PixelType::eFloat;
		case DataType::Float16: return gl::PixelType::eHalfFloat;
		default: break;
		}
		assert(false && "toPixelTypeGL: Invalid DataType");
		return gl::PixelType::eUnsignedByte;
	}

	TextureHandle createTextureGL(RenderDeviceBackendContext* ctx, const TextureDescriptor &desc) {
		auto &textures = glctx(ctx)->textures;
		auto texture = glaze::makeUnique<gl::Texture>();

		gl::TextureTarget const target = toTextureTargetGL(desc.type);
		gl::InternalFormat const internalFormat = toInternalFormatGL(desc.format);
		gl::PixelFormat const pixelFormat = toPixelFormatGL(desc.sourceFormat);
		gl::PixelType const pixelType = toPixelTypeGL(desc.sourceDataType);

		size_t const faceCount = (desc.type == TextureType::TexCubeMap) ? 6u : 1u;
		size_t const mipCount = desc.mipLevelCount == 0
			? 1u
			: (desc.mipLevelCount / faceCount);

		assert(!(mipCount > 1 && desc.generateMipmaps)
			&& "TextureDescriptor: generateMipmaps is mutually exclusive with supplying >1 mip level");

		if (desc.mipLevels != nullptr) {
			assert(desc.mipLevelCount == mipCount * faceCount
				&& "TextureDescriptor: mipLevelCount must equal mipCount * faceCount");
		}

		gl::bindTexture(target, texture);

		int w = desc.size.x;
		int h = desc.size.y;
		int d = desc.size.z;

		for (size_t mip = 0; mip < mipCount; ++mip) {
			GLint const level = static_cast<GLint>(mip);

			switch (desc.type) {
			case TextureType::Tex1D:
				gl::texImage1D(target, level, internalFormat, w, 0, pixelFormat, pixelType, mipLevelDataAt(desc, mip, 0, faceCount));
				break;

			case TextureType::Tex2D:
				gl::texImage2D(target, level, internalFormat, w, h, 0, pixelFormat, pixelType, mipLevelDataAt(desc, mip, 0, faceCount));
				break;

			case TextureType::Tex3D:
			case TextureType::Tex2DArray:
				gl::texImage3D(target, level, internalFormat, w, h, d, 0, pixelFormat, pixelType, mipLevelDataAt(desc, mip, 0, faceCount));
				break;

			case TextureType::TexCubeMap:
				for (size_t face = 0; face < 6u; ++face) {
					gl::texImage2D(cubeMapSides[face], level, internalFormat, w, h, 0, pixelFormat, pixelType, mipLevelDataAt(desc, mip, face, faceCount));
				}
				break;

			default:
				assert(false && "TextureType is unknown");
				return {};
			}

			w = halveDimension(w);
			h = halveDimension(h);
			d = halveDimension(d);
		}

		if (desc.generateMipmaps) {
			gl::generateMipmap(target);
		} else {
			gl::texParameteri(target, gl::TextureParameterName::eTextureBaseLevel, 0);
			gl::texParameteri(target, gl::TextureParameterName::eTextureMaxLevel, static_cast<GLint>(mipCount - 1));
		}

		// TODO: Use a Sampler Resource instead for later
		bool const hasMips = desc.generateMipmaps || mipCount > 1;

		GLint const param = hasMips ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR;
		gl::texParameteri(target, gl::TextureParameterName::eTextureMinFilter, param);
		gl::texParameteri(target, gl::TextureParameterName::eTextureMagFilter, GL_LINEAR);
		gl::texParameteri(target, gl::TextureParameterName::eTextureWrapS, GL_REPEAT);
		gl::texParameteri(target, gl::TextureParameterName::eTextureWrapT, GL_REPEAT);
		gl::texParameteri(target, gl::TextureParameterName::eTextureWrapR, GL_REPEAT);

		auto const [index, gen] = acquireSlot(textures, std::move(texture));
		return TextureHandle::make(gen, index, desc.type);
	}

	void destroyTextureGL(RenderDeviceBackendContext* ctx, TextureHandle handle) {
		auto &textures = glctx(ctx)->textures;
		uint32_t const index = handle.index();
		assert(index < textures.size() && "destroyTextureGL: handle index out of range");
		auto &slot = textures[index];
		assert(slot.obj             && "destroyTextureGL: slot already free (double destroy)");
		assert(slot.gen == handle.gen() && "destroyTextureGL: stale handle (generation mismatch)");
		slot.obj.reset({});
	}

	void updateTextureGL(RenderDeviceBackendContext* ctx, TextureHandle handle, const TextureUpdateDescriptor &desc) {
		auto &textures = glctx(ctx)->textures;
		uint32_t const index = handle.index();
		assert(index < textures.size() && "updateTextureGL: handle index out of range");
		auto &slot = textures[index];
		assert(slot.obj             && "updateTextureGL: use of freed handle");
		assert(slot.gen == handle.gen() && "updateTextureGL: stale handle (generation mismatch)");

		TextureType const type = handle.subType();
		gl::TextureTarget const target = toTextureTargetGL(type);
		gl::PixelFormat const pixelFormat = toPixelFormatGL(desc.sourceFormat);
		gl::PixelType const pixelType = toPixelTypeGL(desc.sourceDataType);

		GLint const level = desc.mipLevel;
		GLint const x = desc.offset.x;
		GLint const y = desc.offset.y;
		GLint const z = desc.offset.z;
		GLsizei const w = desc.size.x;
		GLsizei const h = desc.size.y;
		GLsizei const d = desc.size.z;

		gl::bindTexture(target, slot.obj);

		switch (type) {
		case TextureType::Tex1D:
			gl::texSubImage1D(target, level, x, w, pixelFormat, pixelType, desc.sourceData);
			break;

		case TextureType::Tex2D:
			gl::texSubImage2D(target, level, x, y, w, h, pixelFormat, pixelType, desc.sourceData);
			break;

		case TextureType::Tex3D:
		case TextureType::Tex2DArray:
			gl::texSubImage3D(target, level, x, y, z, w, h, d, pixelFormat, pixelType, desc.sourceData);
			break;

		case TextureType::TexCubeMap:
			assert(desc.faceIndex >= 0 && desc.faceIndex < 6 && "TextureUpdateDescriptor: faceIndex out of range");
			gl::texSubImage2D(cubeMapSides[desc.faceIndex], level, x, y, w, h, pixelFormat, pixelType, desc.sourceData);
			break;

		default:
			assert(false && "TextureType is unknown");
		}
	}

	void readTextureGL(RenderDeviceBackendContext* ctx, TextureHandle handle, const TextureReadDescriptor &desc) {
		assert(desc.data != nullptr && "TextureReadDescriptor: data must not be null");
		assert(desc.offset.x == 0 && desc.offset.y == 0 && desc.offset.z == 0
			&& "TextureReadDescriptor: GL 3.3 backend requires offset == {0,0,0}");

		auto &textures = glctx(ctx)->textures;
		uint32_t const index = handle.index();
		assert(index < textures.size() && "readTextureGL: handle index out of range");
		auto &slot = textures[index];
		assert(slot.obj             && "readTextureGL: use of freed handle");
		assert(slot.gen == handle.gen() && "readTextureGL: stale handle (generation mismatch)");

		TextureType const type = handle.subType();
		gl::TextureTarget const bindTarget = toTextureTargetGL(type);
		gl::PixelFormat const pixelFormat = toPixelFormatGL(desc.destFormat);
		gl::PixelType const pixelType = toPixelTypeGL(desc.destDataType);

		gl::TextureTarget const readTarget = (type == TextureType::TexCubeMap)
			? cubeMapSides[desc.faceIndex]
			: bindTarget;

		if (type == TextureType::TexCubeMap) {
			assert(desc.faceIndex >= 0 && desc.faceIndex < 6 && "TextureReadDescriptor: faceIndex out of range");
		}

		gl::bindTexture(bindTarget, slot.obj);

#ifndef NDEBUG
		GLint actualWidth = 0;
		GLint actualHeight = 0;
		gl::getTexLevelParameteriv(readTarget, desc.mipLevel,
			gl::GetTextureParameter::eTextureWidth, &actualWidth);
		gl::getTexLevelParameteriv(readTarget, desc.mipLevel,
			gl::GetTextureParameter::eTextureHeight, &actualHeight);
		assert(actualWidth == desc.size.x
			&& "TextureReadDescriptor: GL 3.3 backend requires size.x to match the mip's full width");
		assert((type == TextureType::Tex1D || actualHeight == desc.size.y)
			&& "TextureReadDescriptor: GL 3.3 backend requires size.y to match the mip's full height");
#endif

		gl::getTexImage(readTarget, desc.mipLevel, pixelFormat, pixelType, desc.data);
	}

	static glaze::Unique<gl::Shader> compileShader(gl::ShaderType type, const char* src) {
		auto shader = glaze::makeUnique<gl::Shader>(type);

		gl::shaderSource(shader, 1, &src, nullptr);
		gl::compileShader(shader);

		if (!gl::getShaderiv(shader, gl::ShaderParameterName::eCompileStatus)) {
			std::cerr << "Shader compile error:\n" << gl::getShaderInfoLog(shader) << std::endl;

			return {};
		}

		return shader;
	}

	static glaze::Unique<gl::Program> linkProgram(const std::vector<glaze::Unique<gl::Shader>> &shaders) {
		auto program = glaze::makeUnique<gl::Program>();

		for (size_t i = 0; i < shaders.size(); i++) {
			if (!shaders[i]) {
				std::cerr << "Can't link program: One of its shaders was not built successfully" << std::endl;
				return {};
			}

			gl::attachShader(program, shaders[i]);
		}

		gl::linkProgram(program);

		for (size_t i = 0; i < shaders.size(); i++) {
			gl::detachShader(program, shaders[i]);
		}

		if (!gl::getProgramiv(program, gl::ProgramProperty::eLinkStatus)) {
			std::cerr << "Shader link error:\n" << gl::getProgramInfoLog(program) << std::endl;
			return {};
		}

		return program;
	}

	ShaderHandle createShaderProgramGL(RenderDeviceBackendContext *ctx, const ShaderProgramDescriptor &desc) {
		auto &shaderPrograms = glctx(ctx)->shaderPrograms;

		std::vector<glaze::Unique<gl::Shader>> shaders;
		shaders.push_back(std::move(compileShader(gl::ShaderType::eVertexShader, desc.glslVertexShader.c_str())));
		shaders.push_back(std::move(compileShader(gl::ShaderType::eFragmentShader, desc.glslFragmentShader.c_str())));

		glaze::Unique<gl::Program> shaderProgram = linkProgram(shaders);

		if (!shaderProgram) {
			std::cerr << "Could not create a Shader Program" << std::endl;
			return {};
		}

		auto const [index, gen] = acquireSlot(shaderPrograms, std::move(shaderProgram));
		return ShaderHandle::make(gen, index);
	}

	void destroyShaderProgramGL(RenderDeviceBackendContext *ctx, ShaderHandle handle) {
		auto &shaderPrograms = glctx(ctx)->shaderPrograms;
		uint32_t const index = handle.index();
		assert(index < shaderPrograms.size() && "destroyShaderProgramGL: handle index out of range");
		auto &slot = shaderPrograms[index];
		assert(slot.obj             && "destroyShaderProgramGL: slot already free (double destroy)");
		assert(slot.gen == handle.gen() && "destroyShaderProgramGL: stale handle (generation mismatch)");
		slot.obj.reset({});
	}

	void initializeBackendTableGL(RenderDeviceBackendVTable* vtable) {
		vtable->createContext = &createContextGL;
		vtable->destroyContext = &destroyContextGL;
		vtable->createBuffer = &createBufferGL;
		vtable->destroyBuffer = &destroyBufferGL;
		vtable->readBuffer = &readBufferGL;
		vtable->createShaderProgram = &createShaderProgramGL;
		vtable->destroyShaderProgram = &destroyShaderProgramGL;
		vtable->createTexture = &createTextureGL;
		vtable->destroyTexture = &destroyTextureGL;
		vtable->updateTexture = &updateTextureGL;
		vtable->readTexture = &readTextureGL;
	}
}
