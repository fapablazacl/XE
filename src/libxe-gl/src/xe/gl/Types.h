
#pragma once

#include "gl.h"

namespace xe::gl {
    /**
     * Defines a strongly-typed alias for a basic type.
     * This ensures that thing like binding a Texture wherea a Buffer was expected, are catched at build time
     */
    template <typename Tag, typename T>
    struct TaggedType {
        typedef T Type;
        typedef Tag TagType;

        T value = {};
    };

    struct buffer_tag {};
    struct texture_tag {};
    struct shader_tag {};
    struct program_tag {};

    using Buffer = TaggedType<buffer_tag, GLuint>;
    using Texture = TaggedType<buffer_tag, GLuint>;
    using Shader = TaggedType<buffer_tag, GLuint>;
    using Program = TaggedType<buffer_tag, GLuint>;
}
