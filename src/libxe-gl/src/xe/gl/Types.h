
#pragma once

#include "gl.h"

namespace xe::gl {
    /**
     * Defines a strongly-typed alias for a basic type.
     * This ensures that common errors, like binding a Texture where a Buffer was expected, are caught at build time.
     */
    template <typename Tag, typename T>
    struct TaggedType {
        typedef T Type;
        typedef Tag TagType;

        T id = {};
    };

    /*
    template<typename T, typename DeleterFn>
    struct Resource {
        Resource() = default;

        explicit Resource(const T id)
            : id(id) {}

        ~Resource() { 
            delete_();
        }

        void delete_() {
            if (id) {
                DeleterFn fn;
                fn(id);
                id = {};
            }
        }

        T id = {};
    };
    */

    struct buffer_tag {};
    struct texture_tag {};
    struct shader_tag {};
    struct program_tag {};
    struct vertex_array_tag {};

    struct Buffer : TaggedType<buffer_tag, GLuint> {
        GLenum target = {};

        Buffer() = default;

        Buffer(GLuint id, GLenum target) {
            this->id = id;
            this->target = target;
        }
    };

    struct Texture : TaggedType<texture_tag, GLuint> {
        GLenum target = {};

        Texture() = default;

        Texture(GLuint id, GLenum target) {
            this->id = id;
            this->target = target;
        }
    };

    using Shader = TaggedType<shader_tag, GLuint>;
    using Program = TaggedType<program_tag, GLuint>;
    using VertexArray = TaggedType<vertex_array_tag, GLuint>;

    /**
     * Like std::span, but it should be used in all places where a typeless memory region is expected.
     *
     * Utility methods will be added on as-needed basis
     */
    class MemoryRegion {
    public:
        MemoryRegion() = default;

        MemoryRegion(const void* data, size_t size) : m_data(data), m_size(size) {}

        [[nodiscard]]
        size_t size() const { return m_size; }

        [[nodiscard]]
        const void* data() const { return m_data; }

    private:
        const void* m_data = nullptr;
        size_t m_size = 0;
    };
}
