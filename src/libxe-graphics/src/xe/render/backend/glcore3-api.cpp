
#include <glaze/gl.hpp>
#include <glaze/raii.hpp>
#include <xe/math/Vector.h>
#include <xe/render/RenderBackend.h>
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
	}

	RenderDeviceBackendContext* createContextGL() {
		return new RenderDeviceBackendContextGL();
	}

	void destroyContextGL(RenderDeviceBackendContext* ctx) {
		delete glctx(ctx);
	}

	inline TextureType textureTypeOf(Handle handle) {
		assert(handle.type() == HandleTexture);
		return static_cast<TextureType>(handle.subType());
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
			assert(false && "toTextureTargetGL: Invalid TextureType");
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

	Handle createBufferGL(RenderDeviceBackendContext* ctx, const BufferDescriptor& desc) {
		auto &buffers = glctx(ctx)->buffers;

		gl::BufferTarget const target = toBufferTargetGL(desc.type);
		gl::BufferUsage const usage = toBufferUsageGL(desc.usage);

		auto buffer = glaze::makeUnique<gl::BufferId>();
		gl::bindBuffer(target, buffer);
		gl::bufferData(target, desc.size, desc.data, usage);
		
		uint32_t index = static_cast<uint32_t>(buffers.size());

		// TODO: Implement a mechanism to reuse free buffer slots
		buffers.push_back(std::move(buffer));

		return Handle::make(HandleBuffer, 0, index, static_cast<uint32_t>(target));
	}

	void destroyBufferGL(RenderDeviceBackendContext* ctx, Handle handle) {
		glctx(ctx)->buffers[handle.index()].reset({});
	}

	void readBufferGL(RenderDeviceBackendContext* ctx, Handle handle, const BufferReadDescriptor &desc) {
		assert(desc.data != nullptr && "BufferReadDescriptor: data must not be null");
		assert(desc.size > 0 && "BufferReadDescriptor: size must be greater than zero");

		auto &buffer = glctx(ctx)->buffers[handle.index()];

		auto const target =  static_cast<gl::BufferTarget>(handle.subType());
		
		gl::bindBuffer(target, buffer);
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

	Handle createTextureGL(RenderDeviceBackendContext* ctx, const TextureDescriptor &desc) {
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
		gl::texParameteri(target, gl::TextureParameterName::eTextureMinFilter,
			hasMips ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR);
		gl::texParameteri(target, gl::TextureParameterName::eTextureMagFilter, GL_LINEAR);
		gl::texParameteri(target, gl::TextureParameterName::eTextureWrapS, GL_REPEAT);
		gl::texParameteri(target, gl::TextureParameterName::eTextureWrapT, GL_REPEAT);
		gl::texParameteri(target, gl::TextureParameterName::eTextureWrapR, GL_REPEAT);

		uint32_t index = static_cast<uint32_t>(textures.size());
		textures.push_back(std::move(texture));

		return Handle::make(xe::HandleTexture, 0, index, static_cast<uint32_t>(desc.type));
	}

	void destroyTextureGL(RenderDeviceBackendContext* ctx, Handle handle) {
		glctx(ctx)->textures[handle.index()].reset({});
	}

	void updateTextureGL(RenderDeviceBackendContext* ctx, Handle handle, const TextureUpdateDescriptor &desc) {
		auto &texture = glctx(ctx)->textures[handle.index()];
		TextureType const type = textureTypeOf(handle);
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

		gl::bindTexture(target, texture);

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

	void readTextureGL(RenderDeviceBackendContext* ctx, Handle handle, const TextureReadDescriptor &desc) {
		assert(desc.data != nullptr && "TextureReadDescriptor: data must not be null");
		assert(desc.offset.x == 0 && desc.offset.y == 0 && desc.offset.z == 0
			&& "TextureReadDescriptor: GL 3.3 backend requires offset == {0,0,0}");

		auto &texture = glctx(ctx)->textures[handle.index()];
		TextureType const type = textureTypeOf(handle);
		gl::TextureTarget const bindTarget = toTextureTargetGL(type);
		gl::PixelFormat const pixelFormat = toPixelFormatGL(desc.destFormat);
		gl::PixelType const pixelType = toPixelTypeGL(desc.destDataType);

		gl::TextureTarget const readTarget = (type == TextureType::TexCubeMap)
			? cubeMapSides[desc.faceIndex]
			: bindTarget;

		if (type == TextureType::TexCubeMap) {
			assert(desc.faceIndex >= 0 && desc.faceIndex < 6 && "TextureReadDescriptor: faceIndex out of range");
		}

		gl::bindTexture(bindTarget, texture);

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

	Handle createShaderProgramGL(RenderDeviceBackendContext *ctx, const ShaderProgramDescriptor &desc) {
		auto &shaderPrograms = glctx(ctx)->shaderPrograms;

		std::vector<glaze::Unique<gl::Shader>> shaders;
		shaders.push_back(std::move(compileShader(gl::ShaderType::eVertexShader, desc.glslVertexShader.c_str())));
		shaders.push_back(std::move(compileShader(gl::ShaderType::eFragmentShader, desc.glslFragmentShader.c_str())));

		glaze::Unique<gl::Program> shaderProgram = linkProgram(shaders);

		// TODO: Implement a mechanism to reuse free program slots
		uint32_t index = static_cast<uint32_t>(shaderPrograms.size());
		shaderPrograms.push_back(std::move(shaderProgram));

		return Handle::make(HandleShader, 0, index);
	}

	void destroyShaderProgramGL(RenderDeviceBackendContext *ctx, Handle handle) {
		auto& shaderPrograms = glctx(ctx)->shaderPrograms;

		shaderPrograms[handle.index()].reset({});
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
