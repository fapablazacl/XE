
#include <glaze/gl.hpp>
#include <glaze/raii.hpp>
#include <xe/render/RenderBackend.h>
#include <cassert>

#include "glcore3-types.h"

namespace xe {
    void clear(vec4 color, float depth, uint8_t stencil, gl::Flags<gl::ClearBufferMask> flags) {
        gl::clearColor(color.x, color.y, color.z, color.w);
        gl::clearDepth(depth);
        gl::clearStencil(stencil);
        gl::clear(flags);
    }

    void bindPipeline(RenderDeviceBackendContextGL *glctx, const PipelineGL &pipeline) {
        // TODO: Apply list of enable / render state, cull mode, etc
        gl::useProgram(pipeline.shaderProgram);
    }

    gl::TextureUnit getTextureUnit(const uint32_t unit) {
        assert(unit < 32);
        return static_cast<gl::TextureUnit>(static_cast<int>(gl::TextureUnit::eTexture0) + unit);
    }

    void bindTexture(RenderDeviceBackendContextGL *glctx, const uint32_t unit, gl::TextureTarget target, const gl::Texture &texture) {
        gl::activeTexture(getTextureUnit(unit));
        gl::bindTexture(target, texture);
    }

    void dispatch(RenderDeviceBackendContextGL *glctx, const CommandClear &c) {
        // TODO: Add flags mapping to GL
        // clear(c.color, c.depth, c.depth, c.flags);
    }

    void dispatch(RenderDeviceBackendContextGL *glctx, const CommandBindPipeline &c) {
    }

    void dispatch(RenderDeviceBackendContextGL *glctx, const CommandBindTexture &c) {
    }

    void dispatch(RenderDeviceBackendContextGL *glctx, const CommandSetUniform &c) {
    }

    void dispatch(RenderDeviceBackendContextGL *glctx, const CommandSetUniformMatrix &c) {
    }

    void dispatch(RenderDeviceBackendContextGL *glctx, const CommandDraw &c) {
    }

    void submitCommandGL(RenderDeviceBackendContext *ctx, const CommandBuffer::Command *command, size_t count) {
        for (size_t i = 0; i < count; ++i) {
            switch (command[i].opcode) {
            case CommandOp::Clear:
                dispatch(glctx(ctx), command[i].cmd.clear);
                break;
            case CommandOp::BindPipeline:
                dispatch(glctx(ctx), command[i].cmd.bindPipeline);
                break;
            case CommandOp::BindTexture:
                dispatch(glctx(ctx), command[i].cmd.bindTexture);
                break;
            case CommandOp::SetUniform:
                dispatch(glctx(ctx), command[i].cmd.setUniform);
                break;
            case CommandOp::SetUniformMatrix:
                dispatch(glctx(ctx), command[i].cmd.setUniformMatrix);
                break;
            case CommandOp::Draw:
                dispatch(glctx(ctx), command[i].cmd.draw);
                break;
            }
        }
    }
} // namespace xe
