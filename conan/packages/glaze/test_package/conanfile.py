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
            apis = str(self.dependencies[self.tested_reference_str].options.apis)
            api_names = [a.split(":")[0] for a in apis.split(",") if a.strip()]
            for api in api_names:
                for name in (f"test_{api}", f"test_{api}_c"):
                    cmd = os.path.join(self.cpp.build.bindir, name)
                    if os.path.isfile(cmd) or os.path.isfile(cmd + ".exe"):
                        self.run(cmd, env="conanrun")

            # API-agnostic smoke test for glaze::Unique / Shared / Weak.
            raii_cmd = os.path.join(self.cpp.build.bindir, "test_raii")
            if os.path.isfile(raii_cmd) or os.path.isfile(raii_cmd + ".exe"):
                self.run(raii_cmd, env="conanrun")
