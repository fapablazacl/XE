import os

from conan import ConanFile
from conan.errors import ConanInvalidConfiguration
from conan.tools.cmake import CMake, CMakeToolchain, CMakeDeps, cmake_layout
from conan.tools.files import get, copy, rmdir


class AngleConan(ConanFile):
    name = "angle"

    description = "Google ANGLE — OpenGL ES implementation on top of Vulkan, D3D, Metal, and desktop GL"
    license = "BSD-3-Clause"
    url = "https://chromium.googlesource.com/angle/angle"

    settings = "os", "compiler", "build_type", "arch"

    options = {
        "shared": [True, False],
        "fPIC": [True, False],
        "with_vulkan": [True, False],
        "with_gl": [True, False],
        "with_metal": [True, False],
        "with_d3d11": [True, False],
    }

    default_options = {
        "shared": True,
        "fPIC": True,
        "with_vulkan": True,
        "with_gl": False,
        "with_metal": False,
        "with_d3d11": False,
    }

    def config_options(self):
        if self.settings.os == "Windows":
            del self.options.fPIC
            self.options.with_d3d11 = True
        else:
            del self.options.with_d3d11

        if self.settings.os != "Macos":
            del self.options.with_metal
        else:
            self.options.with_metal = True

    def configure(self):
        if self.options.shared:
            self.options.rm_safe("fPIC")

        has_backend = self.options.with_vulkan or self.options.with_gl
        if self.settings.os == "Windows":
            has_backend = has_backend or self.options.get_safe("with_d3d11")
        if self.settings.os == "Macos":
            has_backend = has_backend or self.options.get_safe("with_metal")

        if not has_backend:
            raise ConanInvalidConfiguration(
                "At least one rendering backend must be enabled "
                "(with_vulkan, with_gl, with_d3d11, or with_metal)"
            )

    def requirements(self):
        if self.options.with_vulkan:
            self.requires("vulkan-headers/[>=1.3.0]")
            self.requires("vulkan-loader/[>=1.3.0]")

    def build_requirements(self):
        self.tool_requires("cmake/[>=3.20]")
        self.tool_requires("ninja/[>=1.11.0]")

    def source(self):
        get(self, **self.conan_data["sources"][self.version], strip_root=True)

    def layout(self):
        cmake_layout(self)

    def generate(self):
        deps = CMakeDeps(self)
        deps.generate()

        tc = CMakeToolchain(self)

        tc.cache_variables["ANGLE_ENABLE_VULKAN"] = bool(self.options.with_vulkan)
        tc.cache_variables["ANGLE_ENABLE_OPENGL"] = bool(self.options.with_gl)
        tc.cache_variables["ANGLE_ENABLE_D3D9"] = False
        tc.cache_variables["ANGLE_ENABLE_D3D11"] = bool(self.options.get_safe("with_d3d11", False))
        tc.cache_variables["ANGLE_ENABLE_METAL"] = bool(self.options.get_safe("with_metal", False))
        tc.cache_variables["ANGLE_ENABLE_NULL"] = False
        tc.cache_variables["ANGLE_ENABLE_ESSL"] = True
        tc.cache_variables["ANGLE_ENABLE_GLSL"] = True

        tc.cache_variables["BUILD_SHARED_LIBS"] = bool(self.options.shared)

        tc.generate()

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()

    def package(self):
        copy(self, "LICENSE", src=self.source_folder, dst=os.path.join(self.package_folder, "licenses"))

        copy(self, "*.h", src=os.path.join(self.source_folder, "include"),
             dst=os.path.join(self.package_folder, "include"))

        for pattern in ("*.a", "*.lib"):
            copy(self, pattern, src=self.build_folder,
                 dst=os.path.join(self.package_folder, "lib"), keep_path=False)
        for pattern in ("*.so", "*.so.*", "*.dylib"):
            copy(self, pattern, src=self.build_folder,
                 dst=os.path.join(self.package_folder, "lib"), keep_path=False)
        copy(self, "*.dll", src=self.build_folder,
             dst=os.path.join(self.package_folder, "bin"), keep_path=False)

        rmdir(self, os.path.join(self.package_folder, "lib", "pkgconfig"))

    def package_info(self):
        self.cpp_info.set_property("cmake_file_name", "angle")

        # EGL component
        egl = self.cpp_info.components["EGL"]
        egl.set_property("cmake_target_name", "angle::EGL")
        egl.libs = ["EGL"]
        egl.includedirs = ["include"]
        if self.settings.os in ("Linux", "FreeBSD"):
            egl.system_libs = ["pthread", "dl"]
        elif self.settings.os == "Windows":
            egl.system_libs = ["d3d9", "dxguid", "gdi32", "user32"]

        # GLESv2 component
        glesv2 = self.cpp_info.components["GLESv2"]
        glesv2.set_property("cmake_target_name", "angle::GLESv2")
        glesv2.libs = ["GLESv2"]
        glesv2.includedirs = ["include"]
        glesv2.requires = ["EGL"]
        if self.settings.os in ("Linux", "FreeBSD"):
            glesv2.system_libs = ["pthread", "dl", "m"]
        elif self.settings.os == "Windows":
            if self.options.get_safe("with_d3d11"):
                glesv2.system_libs.extend(["d3d11", "dxgi", "dxguid"])
