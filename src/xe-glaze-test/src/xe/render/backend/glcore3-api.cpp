
// TODO: How to deal with nullptr ctx?


#include <glaze/gl.hpp>
#include <glaze/raii.hpp>
#include <xe/math/Vector.h>
#include <xe/render/RenderBackend.h>
#include <vector>
#include <iostream>

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
	};

	RenderDeviceBackendContextGL* glctx(RenderDeviceBackendContext *ctx) {
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
	}
}
