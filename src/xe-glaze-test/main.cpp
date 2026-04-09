#include <glaze/gl.hpp>
#include <glaze/raii.hpp>
#include <glaze/gl_handle.hpp>
#include <GLFW/glfw3.h>
#include <iostream>
#include <cstdlib>

static const char* VERT_SRC = R"glsl(
#version 330 core
layout(location = 0) in vec2 position;
uniform float angle;
void main() {
    float c = cos(angle);
    float s = sin(angle);
    vec2 r = vec2(c * position.x - s * position.y,
                  s * position.x + c * position.y);
    gl_Position = vec4(r, 0.0, 1.0);
}
)glsl";

static const char* FRAG_SRC = R"glsl(
#version 330 core
out vec4 fragColor;
void main() {
    fragColor = vec4(1.0, 0.5, 0.2, 1.0);
}
)glsl";


static glaze::Unique<gl::handle::Shader> compileShader(gl::ShaderType type, const char* src) {
    auto shader = glaze::makeUnique<gl::handle::Shader>(type);

    shader->source(1, &src, nullptr);
    shader->compile();

    if (!shader->getiv(gl::ShaderParameterName::eCompileStatus)) {
        std::cerr << "Shader compile error:\n" << shader->getInfoLog() << std::endl;
        std::exit(1);
    }
    return shader;
}

int main() {
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(800, 600, "glaze - spinning triangle", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glazeLoadFunctions(glfwGetProcAddress);

	// Inner scope so every RAII-managed GL resource is destroyed while the
	// context is still current. Without it the Unique<> destructors fire
	// after glfwDestroyWindow/glfwTerminate, which tears down the GL context
	// and causes glDeleteProgram/glDeleteShader/... to raise
	// GL_INVALID_OPERATION (caught by GLAZE_GL_CHECK in debug builds).
	{
		// Geometry — equilateral triangle centred at origin
		static const float verts[] = {
				0.0f,  0.5f,
			-0.433f, -0.25f,
				0.433f, -0.25f,
		};

		auto vao = glaze::makeUnique<gl::VertexArray>();
		gl::bindVertexArray(vao.get());

		auto vbo = glaze::makeUnique<gl::BufferId>();

		gl::bindBuffer(gl::BufferTarget::eArray, vbo.get());
		gl::bufferData(gl::BufferTarget::eArray, sizeof(verts), verts, gl::BufferUsage::eStaticDraw);
		gl::vertexAttribPointer(gl::AttribLocation(0), 2, gl::VertexAttribPointerType::eFloat, GL_FALSE, 0, nullptr);
		gl::enableVertexAttribArray(gl::AttribLocation(0));

		// Shader program
		auto prog = glaze::makeUnique<gl::handle::Program>();

		{
			glaze::Unique<gl::handle::Shader> vert = compileShader(gl::ShaderType::eVertex, VERT_SRC);
			glaze::Unique<gl::handle::Shader> frag = compileShader(gl::ShaderType::eFragment, FRAG_SRC);

			prog->attachShader(*vert);
			prog->attachShader(*frag);
		}

		prog->link();

		if (!prog->getiv(gl::ProgramProperty::eLinkStatus)) {
			std::cerr << "Link error:\n" << prog->getInfoLog() << std::endl;
			return -1;
		}

		gl::UniformLocation const angleLoc = prog->getUniformLocation("angle");

		while (!glfwWindowShouldClose(window)) {
			glfwPollEvents();

			int w, h;
			glfwGetFramebufferSize(window, &w, &h);

			gl::viewport(0, 0, w, h);

			gl::clearColor(0.15f, 0.15f, 0.2f, 1.0f);
			gl::clear(gl::ClearBufferMask::eColorBit);

			gl::useProgram(prog->id());
			gl::uniform1f(angleLoc, static_cast<float>(glfwGetTime()));

			gl::bindVertexArray(vao.get());
			gl::drawArrays(gl::PrimitiveType::eTriangles, 0, 3);
			gl::bindVertexArray({});

			gl::useProgram({});

			glfwSwapBuffers(window);
		}
	}

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
