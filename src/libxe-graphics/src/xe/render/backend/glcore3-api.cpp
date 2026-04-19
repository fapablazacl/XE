
#include <glaze/gl.hpp>
#include <glaze/raii.hpp>
#include <xe/math/Vector.h>
#include <xe/render/RenderBackend.h>
#include <vector>
#include <iostream>
#include <array>

#include "glcore3-api.h"

namespace xe {
	struct PipelineGL {
		xe::vec4 clearColor = { 0.0f, 0.0f, 0.0f, 1.0f };
		gl::Flags<gl::ClearBufferMask> clearMask = gl::ClearBufferMask::eColorBufferBit;

		//! @note: Consider a Weak ptr
		gl::Program shaderProgram;
	};

	struct RenderDeviceBackendContextGL : RenderDeviceBackendContext {
		std::vector<glaze::Unique<gl::BufferId>> buffers;
		std::vector<glaze::Unique<gl::Program>> shaderPrograms;
		std::vector<glaze::Unique<gl::Texture>> textures;
	};

	inline RenderDeviceBackendContextGL* glctx(RenderDeviceBackendContext *ctx) {
		assert(ctx);
		return static_cast<RenderDeviceBackendContextGL* >(ctx);
	}

	Handle createBufferGL(RenderDeviceBackendContext* ctx, const BufferDescriptor& desc) {
		auto &buffers = glctx(ctx)->buffers;

		//! TODO: Derive from the descriptor
		auto const target = gl::BufferTarget::eArrayBuffer;

		//! TODO: Derive from the descriptor
		auto const usage = gl::BufferUsage::eDynamicDraw;

		auto buffer = glaze::makeUnique<gl::BufferId>();
		gl::bindBuffer(target, buffer);
		gl::bufferData(target, desc.size, desc.data, usage);
		gl::bindBuffer(target, {});

		uint32_t index = buffers.size();

		// TODO: Implement a mechanism to reuse free buffer slots
		buffers.push_back(std::move(buffer));

		return Handle::make(HandleBuffer, 0, index);
	}

	void destroyBufferGL(RenderDeviceBackendContext* ctx, Handle handle) {
		glctx(ctx)->buffers[handle.index()].reset({});
	}

	gl::TextureTarget toGL(const TextureType type) {
		switch (type) {
		case TextureType::Tex1D:  return gl::TextureTarget::eTexture1d;
		case TextureType::Tex2D:  return gl::TextureTarget::eTexture2d;
		case TextureType::Tex3D:  return gl::TextureTarget::eTexture3d;
		case TextureType::TexCubeMap:  return gl::TextureTarget::eTextureCubeMap;
		case TextureType::Tex2DArray: return gl::TextureTarget::eTexture2dArray;
		default: 
			assert(false && "toGL: Invalid TextureType");
		}
	}

	gl::InternalFormat toInternalFormatGL(const PixelFormat pixelFormat) {
		switch (pixelFormat) {
		case PixelFormat::R8G8B8: return gl::InternalFormat::eRgb;
		case PixelFormat::R8G8B8A8: return gl::InternalFormat::eRgba;
		default:
			assert(false && "toGL: Invalid InternalFormat");
		}
	}

	gl::PixelFormat toPixelFormatGL(const PixelFormat pixelFormat) {
		switch (pixelFormat) {
		case PixelFormat::R8G8B8: return gl::PixelFormat::eRgb;
		case PixelFormat::R8G8B8A8: return gl::PixelFormat::eRgba;
		default:
			assert(false && "toGL: Invalid InternalFormat");
		}
	}

	gl::PixelType toPixelTypeGL(const DataType dataType) {
		switch (dataType) {

		case DataType::Int8:
			return gl::PixelType::eByte;

		case DataType::UInt8:
			return gl::PixelType::eUnsignedByte;

		case DataType::Int16:
			return gl::PixelType::eShort;
	
		case DataType::UInt16:
			return gl::PixelType::eUnsignedShort;

		case DataType::Int32:
			return gl::PixelType::eInt;

		case DataType::UInt32:
			return gl::PixelType::eUnsignedInt;

		case DataType::Float32:
			return gl::PixelType::eFloat;

		case DataType::Float16:
			return gl::PixelType::eHalfFloat;
		}
	}

	Handle createTextureGL(RenderDeviceBackendContext* ctx, const TextureDescriptor &desc) {
		auto &textures = glctx(ctx)->textures;
		auto texture = glaze::makeUnique<gl::Texture>();

		constexpr std::array<gl::TextureTarget, 6> cubeMapSides {
			gl::TextureTarget::eTextureCubeMapPositiveX,
			gl::TextureTarget::eTextureCubeMapNegativeX,
			gl::TextureTarget::eTextureCubeMapPositiveY,
			gl::TextureTarget::eTextureCubeMapNegativeY,
			gl::TextureTarget::eTextureCubeMapPositiveZ,
			gl::TextureTarget::eTextureCubeMapNegativeZ,
		};

		gl::TextureTarget const target = toGL(desc.type);
		gl::InternalFormat const internalFormat = toInternalFormatGL(desc.format);
		gl::PixelFormat const pixelFormat = toPixelFormatGL(desc.sourceFormat);
		gl::PixelType const pixelType = toPixelTypeGL(desc.sourceDataType);

		GLsizei const width = desc.size.x;
		GLsizei const height = desc.size.y;
		GLsizei const depth = desc.size.z;

		switch (desc.type) {
		case TextureType::Tex1D:
			gl::bindTexture(target, texture);
			gl::texImage1D(target, 0, internalFormat, width, 0, pixelFormat, pixelType, *desc.sourceData);
			break;

		case TextureType::Tex2D:
			gl::bindTexture(target, texture);
			gl::texImage2D(target, 0, internalFormat, width, height, 0, pixelFormat, pixelType, *desc.sourceData);
			break;

		case TextureType::Tex3D:
			gl::bindTexture(target, texture);
			gl::texImage3D(target, 0, internalFormat, width, height, depth, 0, pixelFormat, pixelType, *desc.sourceData);
			break;

		case TextureType::TexCubeMap: {
			gl::bindTexture(target, texture);

			for (size_t i = 0; i < cubeMapSides.size(); i++) {
				gl::TextureTarget const sideTarget = cubeMapSides[i];
				gl::texImage2D(sideTarget, 0, internalFormat, width, height, 0, pixelFormat, pixelType, desc.sourceData[i]);
			}

			break;
		}

		case TextureType::Tex2DArray:
			gl::bindTexture(target, texture);
			gl::texImage3D(target, 0, internalFormat, width, height, depth, 0, pixelFormat, pixelType, *desc.sourceData);
			break;

		default:
			assert(false && "TextureType is unknown");
		}

		uint32_t index = textures.size();
		textures.push_back(std::move(texture));

		return Handle::make(xe::HandleTexture, 0, index);
	}
	
	void destroyTextureGL(RenderDeviceBackendContext* ctx, Handle handle) {
		glctx(ctx)->buffers[handle.index()].reset({});
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
		uint32_t index = shaderPrograms.size();
		shaderPrograms.push_back(std::move(shaderProgram));

		return Handle::make(HandleShader, 0, index);
	}

	void destroyShaderProgramGL(RenderDeviceBackendContext *ctx, Handle handle) {
		auto& shaderPrograms = glctx(ctx)->shaderPrograms;

		shaderPrograms[handle.index()].reset({});
	}

	void initializeBackendTableGL(RenderDeviceBackendVTable* vtable) {
		vtable->createBuffer = &createBufferGL;
		vtable->destroyBuffer = &destroyBufferGL;
		vtable->createShaderProgram = &createShaderProgramGL;
		vtable->destroyShaderProgram = &destroyShaderProgramGL;
		vtable->createTexture = &createTextureGL;
		vtable->destroyTexture = &destroyTextureGL;
	}
}
