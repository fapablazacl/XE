import os
from conan import ConanFile
from conan.tools.cmake import CMake, cmake_layout
from conan.tools.build import can_run


class GlazeTestConan(ConanFile):
    settings = "os", "compiler", "build_type", "arch"
    generators = "CMakeDeps", "CMakeToolchain"

    def requirements(self):
        self.requires(self.tested_reference_str)

    def layout(self):
        cmake_layout(self)

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()

    def test(self):
        if can_run(self):
            for name in ("test_gl", "test_gles1", "test_gles2", "test_glsc2",
                          "test_gl_c", "test_gles1_c", "test_gles2_c", "test_glsc2_c"):
                cmd = os.path.join(self.cpp.build.bindir, name)
                self.run(cmd, env="conanrun")
