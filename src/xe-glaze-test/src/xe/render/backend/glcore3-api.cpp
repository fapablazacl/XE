
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

	Handle createBufferGL(RenderDeviceBackendContext* ctx, const BufferDescriptor& desc) {
		auto glctx = static_cast<RenderDeviceBackendContextGL*>(ctx);
		auto &buffers = glctx->buffers;

		//! TODO: Derive from the descriptor
		auto const target = gl::BufferTarget::eArrayBuffer;

		//! TODO: Derive from the descriptor
		auto const usage = gl::BufferUsage::eDynamicDraw;

		auto buffer = glaze::makeUnique<gl::BufferId>();
		gl::bindBuffer(target, buffer);
		gl::bufferData(target, desc.size, desc.data, usage);
		gl::bindBuffer(target, {});

		Handle const bufferHandle = buffers.size();

		// TODO: Implement a mechanism to reuse free buffer slots
		buffers.push_back(std::move(buffer));

		return bufferHandle;
	}

	void destroyBufferGL(RenderDeviceBackendContext* ctx, Handle handle) {
		auto glctx = static_cast<RenderDeviceBackendContextGL*>(ctx);

		glctx->buffers[handle].reset({});
	}

	static glaze::Unique<gl::Shader> compileShader(gl::ShaderType type, const char* src) {
		auto shader = glaze::makeUnique<gl::Shader>(type);

		gl::shaderSource(shader, 1, &src, nullptr);
		gl::compileShader(shader);

		if (!gl::getShaderiv(shader, gl::ShaderParameterName::eCompileStatus)) {
			// TODO: Define a way to handle errors
			std::cerr << "Shader compile error:\n" << gl::getShaderInfoLog(shader) << std::endl;
			std::exit(1);
		}

		return shader;
	}

	static glaze::Unique<gl::Program> linkProgram(const std::vector<glaze::Unique<gl::Shader>> &shaders ) {
		// Shader program
		auto prog = glaze::makeUnique<gl::Program>();

		for (const auto &shader : shaders) {
			gl::attachShader(prog, shader);
		}

		gl::linkProgram(prog);

		// TODO: Check for linking errors

		return prog;
	}

	Handle createShaderProgramGL(RenderDeviceBackendContext *ctx, const ShaderProgramDescriptor &desc) {
		auto glctx = static_cast<RenderDeviceBackendContextGL*>(ctx);
		auto &shaderPrograms = glctx->shaderPrograms;

		std::vector<glaze::Unique<gl::Shader>> shaders = {
			compileShader(gl::ShaderType::eVertexShader, desc.glslVertexShader.c_str()),
			compileShader(gl::ShaderType::eFragmentShader, desc.glslFragmentShader.c_str())
		};

		glaze::Unique<gl::Program> shaderProgram = linkProgram(shaders);

		// TODO: Implement a mechanism to reuse free program slots
		Handle handle = shaderPrograms.size();
		shaderPrograms.push_back(std::move(shaderProgram));

		return handle;
	}

	void destroyShaderProgramGL(RenderDeviceBackendContext *ctx, Handle handle) {
		auto glctx = static_cast<RenderDeviceBackendContextGL*>(ctx);
		auto& shaderPrograms = glctx->shaderPrograms;

		shaderPrograms[handle].reset({});
	}

	void initializeBackendTableGL(RenderDeviceBackendVTable* vtable) {
		vtable->createBuffer = &createBufferGL;
		vtable->destroyBuffer = &destroyBufferGL;
		vtable->createShaderProgram = &createShaderProgramGL;
		vtable->destroyShaderProgram = &destroyShaderProgramGL;
	}
}
