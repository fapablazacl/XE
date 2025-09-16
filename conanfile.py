from conan import ConanFile
from conan.tools.cmake import CMakeToolchain, CMake, cmake_layout, CMakeDeps
from conan.tools.env import VirtualRunEnv

class xeRecipe(ConanFile):
    name = "xe"
    version = "1.0"
    package_type = "application"

    # Optional metadata
    license = "<Put the package license here>"
    author = "<Put your name here> <And your email here>"
    url = "<Package recipe repository url here, for issues about the package>"
    description = "<Description of xenoide package here>"
    topics = ("<Put some tag here>", "<here>", "<and here>")

    # Binary configuration
    settings = "os", "compiler", "build_type", "arch"

    # Sources are located in the same place as this recipe, copy them to the recipe
    exports_sources = "CMakeLists.txt", "src/*"

    def layout(self):
        cmake_layout(self)

    def requirements(self):
        self.requires("assimp/5.4.3")
        self.requires("gtest/1.17.0")
        self.requires("glfw/3.4")
        self.requires("cgltf/1.13")
        self.requires("fmt/11.2.0")
        self.requires("sdl/2.32.8")
        self.requires("lodepng/cci.20230410")
        self.requires("ms-gsl/4.2.0")
        self.requires("devil/1.8.0")
        self.requires("glm/1.0.1")
        self.requires("nlohmann_json/3.12.0")
        self.requires("vulkan-loader/1.4.313.0")
        self.requires("glad/0.1.36", options={"spec": "gl", "gl_profile": "core", "gl_version": "4.6"})
        self.requires("imgui/1.92.2b")

    def generate(self):
        deps = CMakeDeps(self)
        deps.generate()
        tc = CMakeToolchain(self)
        tc.generate()

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()

    def package(self):
        cmake = CMake(self)
        cmake.install()
