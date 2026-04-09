import os

from conan import ConanFile
from conan.tools.build import can_run
from conan.tools.cmake import CMake, cmake_layout


class GlazedTestConan(ConanFile):
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
            for name in ("test_glazed_gl",):
                cmd = os.path.join(self.cpp.build.bindir, name)
                if os.path.isfile(cmd) or os.path.isfile(cmd + ".exe"):
                    self.run(cmd, env="conanrun")
