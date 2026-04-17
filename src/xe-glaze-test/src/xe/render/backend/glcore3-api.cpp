

#include <glaze/gl.hpp>
#include <glaze/raii.hpp>
#include <xe/math/Vector.h>
#include <xe/render/RenderBackend.h>
#include <vector>

#include "glcore3-api.h"

namespace xe {

	namespace glcore3 {
		struct Pipeline {
			xe::vec4 clearColor = { 0.0f, 0.0f, 0.0f, 1.0f };
			gl::Flags<gl::ClearBufferMask> clearMask = gl::ClearBufferMask::eColorBufferBit;
			glaze::Unique<gl::Program> shaderProgram;
		};

		struct BackendContextGLCore3 : BackendContext {
			std::vector<glaze::Unique<gl::BufferId>> buffers;

			Handle createBuffer(const BufferDescriptor& desc) {
				auto const target = gl::BufferTarget::eArrayBuffer;
				auto const usage = gl::BufferUsage::eDynamicDraw;

				auto buffer = glaze::makeUnique<gl::BufferId>();
				gl::bindBuffer(target, buffer);
				gl::bufferData(target, desc.size, desc.data, usage);
				gl::bindBuffer(target, {});

				Handle const bufferHandle = buffers.size();

				// TODO: Search for a free Handle
				buffers.push_back(std::move(buffer));

				return bufferHandle;
			}

			void destroyBuffer(Handle bufferHandle) {
				buffers[bufferHandle].reset({});
			}

			void beginFrame() {
				gl::clearColor(0.2f, 0.2f, 8.0f, 1.0f);
				gl::clear(gl::ClearBufferMask::eColorBufferBit | gl::ClearBufferMask::eDepthBufferBit);
			}

			void endFrame() {
				gl::flush();
			}

			void present() {
				// no-op
			}
		};

		Handle createBuffer(BackendContext* ctx, const BufferDescriptor& desc) {
			return static_cast<BackendContextGLCore3*>(ctx)->createBuffer(desc);
		}

		void destroyBuffer(BackendContext* ctx, Handle buffer) {
			static_cast<BackendContextGLCore3*>(ctx)->destroyBuffer(buffer);
		}

		void beginFrame(BackendContext* ctx) {
			static_cast<BackendContextGLCore3*>(ctx)->beginFrame();
		}

		void endFrame(BackendContext* ctx) {
			static_cast<BackendContextGLCore3*>(ctx)->endFrame();
		}

		void present(BackendContext* ctx) {
			static_cast<BackendContextGLCore3*>(ctx)->present();
		}

		void initializeBackendTable(BackendTable* vtable) {
			vtable->createBuffer = &createBuffer;
			vtable->destroyBuffer = &destroyBuffer;
			vtable->beginFrame = &beginFrame;
			vtable->endFrame = &endFrame;
			vtable->present = &present;
		}
	}	
}
