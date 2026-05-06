
#pragma once

#include <glaze/gl.hpp>
#include <glaze/raii.hpp>

#include <xe/render/types.h>
#include <xe/render/RenderBackend.h>
#include "glcore3-common.h"

namespace xe {
    struct SamplerGL {
        gl::Sampler sampler;
    };
}
