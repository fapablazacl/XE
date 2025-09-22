
#include "RendererGL.h"

#include <iostream>

namespace xe::gl {
    static std::string errorCodeToString(GLenum error) {
        switch (error) {
        case GL_INVALID_ENUM:
            return "GL_INVALID_ENUM";
        case GL_INVALID_VALUE:
            return "GL_INVALID_VALUE";
        case GL_INVALID_OPERATION:
            return "GL_INVALID_OPERATION";
        case GL_OUT_OF_MEMORY:
            return "GL_OUT_OF_MEMORY";
        default:
            return "<Unknown Error Value: " + std::to_string(error) + ">";
        }
    }

    struct GLScopedErrorChecker {
        GLScopedErrorChecker(const char *file, const int line) : file(file), line(line) {
            check();
        }

        ~GLScopedErrorChecker() {
            check();
        }

        void check() const {
            if (const GLenum error = glGetError(); error) {
                const auto str = errorCodeToString(error);
                std::fprintf(stderr, "Error %s generated while checking with guard (%s:%d) \n", str.c_str(), file, line);
                std::fprintf(stderr, "Aborting ...\n");
                abort();
            }
        }

        const char *file;
        const int line;
    };

#define XE_GL_SCOPED_ERROR_CHECK() GLScopedErrorChecker __gl_error_raii(__FILE__, __LINE__)

#if defined(GLAD_DEBUG)
    void pre_call_callback_gl(const char *name, void *funcptr, int len_args, ...) {
        (void)name;
        (void)funcptr;
        (void)len_args;
    }

    void post_call_callback_gl(const char *name, void *funcptr, int len_args, ...) {
        (void)funcptr;
        (void)len_args;

        if (const GLenum error_code = glad_glGetError(); error_code) {
            const auto errorCodeString = errorCodeToString(error_code);
            std::fprintf(stderr, "Error %s generated while executing command %s\n", name, errorCodeString.c_str());
        }
    }
#endif

    std::unique_ptr<RendererGL> RendererGL::create() {
        return create(nullptr);
    }

    std::unique_ptr<RendererGL> RendererGL::create(GetProcAddress getProcAddress) {
        const auto loadproc = reinterpret_cast<GLADloadproc>(getProcAddress);
        const int result = getProcAddress ? gladLoadGLLoader(loadproc) : gladLoadGL();

        if (!result) {
            std::fprintf(stderr, "GLAD: Couldn't load GL functions. Error code %d \n", result);
            return {};
        }

#if defined(GLAD_DEBUG)
        glad_set_pre_callback_gl(pre_call_callback_gl);
        glad_set_post_callback_gl(post_call_callback_gl);
#endif

        return std::unique_ptr<RendererGL>{new RendererGL()};
    }

    RendererGL::RendererGL() {
        const auto info = getInfo();
        std::printf("OpenGL info:\n");
        std::printf("GL_VENDOR: %s\n", info.vendor.c_str());
        std::printf("GL_RENDERER: %s\n", info.renderer.c_str());
        std::printf("GL_VERSION: %s\n", info.version.c_str());
        std::printf("GL_SHADING_LANGUAGE_VERSION: %s\n", info.shadingLanguageVersion.c_str());

        GLint extensionCount;
        glGetIntegerv(GL_NUM_EXTENSIONS, &extensionCount);

        std::printf("Supported extensions %d\n", extensionCount);

        for (int i = 0; i < extensionCount; i++) {
            const auto str = glGetStringi(GL_EXTENSIONS, i);
            const auto cstr = reinterpret_cast<const char *>(str);

            std::printf("%s ", cstr);
        }

        std::printf("\n");

        glVertexAttribXfv[0] = glVertexAttrib1fv;
        glVertexAttribXfv[1] = glVertexAttrib2fv;
        glVertexAttribXfv[2] = glVertexAttrib3fv;
        glVertexAttribXfv[3] = glVertexAttrib4fv;

        glVertexAttribXiv[0] = nullptr;
        glVertexAttribXiv[1] = nullptr;
        glVertexAttribXiv[2] = nullptr;
        glVertexAttribXiv[3] = glVertexAttrib4iv;

        glUniformXfv[0] = glUniform1fv;
        glUniformXfv[1] = glUniform2fv;
        glUniformXfv[2] = glUniform3fv;
        glUniformXfv[3] = glUniform4fv;

        glUniformXiv[0] = glUniform1iv;
        glUniformXiv[1] = glUniform2iv;
        glUniformXiv[2] = glUniform3iv;
        glUniformXiv[3] = glUniform4iv;

        glUniformXuiv[0] = glUniform1uiv;
        glUniformXuiv[1] = glUniform2uiv;
        glUniformXuiv[2] = glUniform3uiv;
        glUniformXuiv[3] = glUniform4uiv;

        glUniformMatrixXfv[0] = glUniformMatrix2fv;
        glUniformMatrixXfv[1] = glUniformMatrix2x3fv;
        glUniformMatrixXfv[2] = glUniformMatrix2x4fv;
        glUniformMatrixXfv[3] = glUniformMatrix3x2fv;
        glUniformMatrixXfv[4] = glUniformMatrix3fv;
        glUniformMatrixXfv[5] = glUniformMatrix3x4fv;
        glUniformMatrixXfv[6] = glUniformMatrix4x2fv;
        glUniformMatrixXfv[7] = glUniformMatrix4x3fv;
        glUniformMatrixXfv[8] = glUniformMatrix4fv;

        glUniformMatrixXdv[0] = glUniformMatrix2dv;
        glUniformMatrixXdv[1] = glUniformMatrix2x3dv;
        glUniformMatrixXdv[2] = glUniformMatrix2x4dv;
        glUniformMatrixXdv[3] = glUniformMatrix3x2dv;
        glUniformMatrixXdv[4] = glUniformMatrix3dv;
        glUniformMatrixXdv[5] = glUniformMatrix3x4dv;
        glUniformMatrixXdv[6] = glUniformMatrix4x2dv;
        glUniformMatrixXdv[7] = glUniformMatrix4x3dv;
        glUniformMatrixXdv[8] = glUniformMatrix4dv;

        glXable[0] = glDisable;
        glXable[1] = glEnable;
    }

    RendererInfo RendererGL::getInfo() const {
        RendererInfo info;

        info.vendor = reinterpret_cast<const char *>(glGetString(GL_VENDOR));
        info.renderer = reinterpret_cast<const char *>(glGetString(GL_RENDERER));
        info.version = reinterpret_cast<const char *>(glGetString(GL_VERSION));
        info.shadingLanguageVersion = reinterpret_cast<const char *>(glGetString(GL_SHADING_LANGUAGE_VERSION));

        return info;
    }

    Shader RendererGL::createShader(const GLenum type, const char *source) const {
        XE_GL_SCOPED_ERROR_CHECK();

        const auto shaderId = glCreateShader(type);

        const GLchar *const glsl = source;
        glShaderSource(shaderId, 1, &glsl, nullptr);
        glCompileShader(shaderId);

        // check for errors
        GLint status;
        glGetShaderiv(shaderId, GL_COMPILE_STATUS, &status);

        if (status == static_cast<GLint>(GL_FALSE)) {
            constexpr size_t INFO_LOG_BUFFER_SIZE = 4096;
            std::cerr << "Error while creating shader " << type << ": " << std::endl;

            char msg[INFO_LOG_BUFFER_SIZE] = {};
            glGetShaderInfoLog(shaderId, INFO_LOG_BUFFER_SIZE, nullptr, msg);

            std::cerr << msg << std::endl;

            return {};
        }

        return {shaderId};
    }

    Program RendererGL::createProgram(const tcb::span<Shader> &shaders) const {
        XE_GL_SCOPED_ERROR_CHECK();

        const auto programId = glCreateProgram();

        for (const auto &shader : shaders) {
            if (shader.id == 0) {
                std::cerr << "Empty shader was supplied" << std::endl;

                glDeleteProgram(programId);
                return {};
            }

            glAttachShader(programId, shader.id);
        }

        glLinkProgram(programId);

        GLint status;
        glGetProgramiv(programId, GL_LINK_STATUS, &status);

        if (status == static_cast<GLint>(GL_FALSE)) {
            constexpr size_t INFO_LOG_BUFFER_SIZE = 4096;

            std::cerr << "Shader linker error: " << std::endl;
            char msg[INFO_LOG_BUFFER_SIZE] = {};
            glGetProgramInfoLog(programId, INFO_LOG_BUFFER_SIZE, nullptr, msg);
            std::cerr << msg << std::endl;

            glDeleteProgram(programId);

            return {};
        }

        return Program{programId};
    }

    Buffer RendererGL::createBuffer(const GLenum target, const GLenum usage, const MemoryRegion &memory) const {
        XE_GL_SCOPED_ERROR_CHECK();

        GLuint bufferId = 0;

        glGenBuffers(1, &bufferId);
        glBindBuffer(target, bufferId);
        glBufferData(target, static_cast<GLsizeiptr>(memory.size()), memory.data(), usage);
        glBindBuffer(target, 0);

        return {bufferId, target};
    }

    VertexArray RendererGL::createVertexArray(const tcb::span<const Attribute> &attributes, Buffer elementArrayBuffer) const {
        XE_GL_SCOPED_ERROR_CHECK();

        VertexArray vao;
        auto &id = vao.id;

        glGenVertexArrays(1, &id);
        glBindVertexArray(id);

        for (const auto &attr : attributes) {
            if (attr.buffer.id != 0) {
                glBindBuffer(attr.buffer.target, attr.buffer.id);
                glEnableVertexAttribArray(attr.index);

                GLenum type = GL_FLOAT;

                switch (attr.type) {
                case AttributeType::Float:
                    type = GL_FLOAT;
                    break;

                case AttributeType::Int:
                    type = GL_INT;
                    break;

                case AttributeType::UnsignedInt:
                    type = GL_UNSIGNED_INT;
                    break;

                case AttributeType::UnsignedByte:
                    type = GL_UNSIGNED_BYTE;
                    break;
                }

                const auto ptr = reinterpret_cast<const void*>(static_cast<long long>(attr.offset));
                glVertexAttribPointer(attr.index, static_cast<GLint>(attr.size) + 1, type, attr.normalized, attr.stride, ptr);
            } else {
                glDisableVertexAttribArray(attr.index);
            }
        }

        if (elementArrayBuffer.id) {
            assert(elementArrayBuffer.target == GL_ELEMENT_ARRAY_BUFFER);
            glBindBuffer(elementArrayBuffer.target, elementArrayBuffer.id);
        }

        return vao;
    }

    void RendererGL::draw(VertexArray vertexArray, GLenum primitiveType, const tcb::span<const VertexArrayPrimitive> &primitives) const {
        XE_GL_SCOPED_ERROR_CHECK();
        assert(!primitives.empty());
        assert(vertexArray.id);

        glBindVertexArray(vertexArray.id);

        for (const auto &primitive : primitives) {
            assert(primitive.count > 0);
            bindRenderState(primitive.attribs);
            glDrawArrays(primitiveType, primitive.start, primitive.count);
        }
    }

    void RendererGL::draw(VertexArray vertexArray, GLenum primitiveType, const VertexArrayMultiDraw &multiDraw) const {
        XE_GL_SCOPED_ERROR_CHECK();

        glBindVertexArray(vertexArray.id);
        glMultiDrawArrays(primitiveType, multiDraw.start, multiDraw.count, multiDraw.drawCount);
    }

    void RendererGL::draw(VertexArray vertexArray, GLenum primitiveType, const tcb::span<const VertexArrayPrimitive> &primitives, GLenum dataType) const {
        XE_GL_SCOPED_ERROR_CHECK();

        glBindVertexArray(vertexArray.id);

        for (const auto &primitive : primitives) {
            const auto indices = reinterpret_cast<const void *>(static_cast<long long>(primitive.start));

            bindRenderState(primitive.attribs);

            glDrawElements(primitiveType, primitive.count, dataType, indices);
        }
    }

    void RendererGL::bindRenderState(const tcb::span<const Attribute> &attribs) const {
        XE_GL_SCOPED_ERROR_CHECK();

        for (const auto &attrib : attribs) {
            const auto dim = static_cast<int>(attrib.size);

            switch (attrib.type) {
            case AttributeType::Float:
                glVertexAttribXfv[dim](attrib.index, static_cast<const GLfloat *>(attrib.data));
                break;

            default:
                fprintf(stderr, "Non float vertex attribute support is missing.");
                abort();
            }
        }
    }

    void RendererGL::bindRenderState(const tcb::span<const Uniform> &uniforms) const {
        XE_GL_SCOPED_ERROR_CHECK();

        for (const auto &uniform : uniforms) {
            const auto &[location, type, uniformDim, count, data] = uniform;
            const auto dim = static_cast<int>(uniformDim);

            switch (type) {
            case UniformType::Float:
                glUniformXfv[dim](location, count, static_cast<const GLfloat *>(data));
                break;

            case UniformType::Int:
                glUniformXiv[dim](location, count, static_cast<const GLint *>(data));
                break;

            case UniformType::UnsignedInt:
                glUniformXuiv[dim](location, count, static_cast<const GLuint *>(data));
                break;
            }
        }
    }

    void RendererGL::bindRenderState(const tcb::span<const UniformMatrix> &uniforms) const {
        XE_GL_SCOPED_ERROR_CHECK();

        for (const auto &[location, type, dim, transpose, count, data] : uniforms) {
            assert(location >= 0);
            assert(count > 0);
            assert(data);

            const auto index = static_cast<int>(dim);

            switch (type) {
            case UniformMatrixType::Float:
                glUniformMatrixXfv[index](location, count, transpose, static_cast<const GLfloat *>(data));
                break;

            case UniformMatrixType::Double:
                glUniformMatrixXdv[index](location, count, transpose, static_cast<const GLdouble *>(data));
                break;
            }
        }
    }

    Texture RendererGL::createTexture(GLenum target, GLenum internalFormat, const ClientTextureImage1D &image, const CreateTextureOptions &options) const {
        XE_GL_SCOPED_ERROR_CHECK();

        GLuint textureId = 0;

        glGenTextures(1, &textureId);
        glBindTexture(target, textureId);

        glTexImage1D(target, 0, static_cast<GLint>(internalFormat), image.size, 0, image.format, image.type, image.pixels);

        if (options.flags & GenerateMipMaps) {
            glGenerateMipmap(target);
        }

        bindRenderState(target, options.parameters);

        glBindTexture(target, 0);

        return {textureId, target};
    }

    Texture RendererGL::createTexture(GLenum target, GLenum internalFormat, const ClientTextureImage2D &image, const CreateTextureOptions &options) const {
        XE_GL_SCOPED_ERROR_CHECK();

        GLuint textureId = 0;

        glGenTextures(1, &textureId);
        glBindTexture(target, textureId);

        glTexImage2D(target, 0, internalFormat, image.size.X, image.size.Y, 0, image.format, image.type, image.pixels);

        if (options.flags & GenerateMipMaps) {
            glGenerateMipmap(target);
        }

        bindRenderState(target, options.parameters);

        glBindTexture(target, 0);

        return {textureId, target};
    }

    Texture RendererGL::createTexture(GLenum target, GLenum internalFormat, const ClientTextureImage3D &image, const CreateTextureOptions &options) const {
        XE_GL_SCOPED_ERROR_CHECK();

        GLuint textureId = 0;

        glGenTextures(1, &textureId);
        glBindTexture(target, textureId);

        glTexImage3D(target, 0, internalFormat, image.size.X, image.size.Y, image.size.Z, 0, image.format, image.type, image.pixels);

        if (options.flags & GenerateMipMaps) {
            glGenerateMipmap(target);
        }

        bindRenderState(target, options.parameters);

        glBindTexture(target, 0);

        return {textureId, target};
    }

    void RendererGL::bindRenderState(GLenum textureTarget, const tcb::span<const TextureParameter> &parameters) const {
        XE_GL_SCOPED_ERROR_CHECK();

        for (const auto &parameter : parameters) {
            glTexParameteri(textureTarget, parameter.param, parameter.value);
        }
    }

    void RendererGL::bindRenderState(const tcb::span<const CapabilityStatus> &capabilities) const {
        XE_GL_SCOPED_ERROR_CHECK();

        for (const auto &[capability, enabled] : capabilities) {
            glXable[enabled](capability);
        }
    }

    void RendererGL::bindRenderState(const tcb::span<const TextureLayer> &layers) const {
        XE_GL_SCOPED_ERROR_CHECK();

        for (uint32_t i = 0; i < layers.size(); i++) {
            const auto &layer = layers[i];

            glActiveTexture(GL_TEXTURE0 + i);
            glBindTexture(layer.texture.target, layer.texture.id);

            bindRenderState(layer.texture.target, layer.parameters);
        }
    }

    void RendererGL::clear(const GLenum flags, std::optional<XE::Vector4> color, std::optional<float> depth, std::optional<int> stencil) const {
        if (color.has_value()) {
            glClearColor(color->X, color->Y, color->Z, color->W);
        }

        if (depth.has_value()) {
            glClearDepthf(depth.value());
        }

        if (stencil.has_value()) {
            glClearStencil(stencil.value());
        }

        glClear(flags);

        XE_GL_SCOPED_ERROR_CHECK();
    }

    void RendererGL::flush() const {
        XE_GL_SCOPED_ERROR_CHECK();

        glFlush();
    }

    void RendererGL::viewport(const XE::Vector2i &pos, const XE::Vector2i &size) const {
        XE_GL_SCOPED_ERROR_CHECK();

        glViewport(pos.X, pos.Y, size.X, size.Y);
    }

    void RendererGL::useProgram(const Program &program) const {
        XE_GL_SCOPED_ERROR_CHECK();

        glUseProgram(program.id);
    }
} // namespace xe::gl
