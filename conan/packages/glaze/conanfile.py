
import os
import shutil
from conan import ConanFile
from conan.tools.cmake import CMake, CMakeToolchain, cmake_layout
from conan.tools.files import copy

class GlazeConan(ConanFile):
    name = "glaze"
    version = "1.0.0"
    
    exports_sources = (
        "glaze_cli.py",
        "glaze/*",
        "pyproject.toml",
    )

    settings = "os", "compiler", "build_type", "arch"

    options = {
        "apis": ["ANY"],
        "language": ["c", "cpp", "both"],
        "shared": [True, False],
        "fPIC": [True, False],
        "with_docs": [True, False],
    }

    default_options = {
        "apis": "gl:3.3,gles1:1.0,gles2:3.2,glsc2:2.0",
        "language": "cpp",
        "shared": False,
        "fPIC": True,
        "with_docs": True,
    }

    def config_options(self):
        if self.settings.os == "Windows":
            del self.options.fPIC

    def configure(self):
        if self.options.shared:
            self.options.rm_safe("fPIC")

    def source(self):
        from conan.tools.scm import Git

        repos = [
            ("OpenGL-Registry", "https://github.com/KhronosGroup/OpenGL-Registry.git",
             ["xml/gl.xml"]),
            ("EGL-Registry", "https://github.com/KhronosGroup/EGL-Registry.git",
             ["api/KHR"]),
        ]

        if self.options.with_docs:
            repos.append(
                ("OpenGL-Refpages", "https://github.com/KhronosGroup/OpenGL-Refpages.git",
                 ["gl4", "es1.1", "es3.0"]),
            )

        for folder, url, sparse_paths in repos:
            dest = os.path.join(self.source_folder, folder)
            if os.path.isdir(dest):
                continue
            os.makedirs(dest, exist_ok=True)
            git = Git(self, folder=folder)
            git.run("init")
            git.run("remote add origin " + url)
            git.run("sparse-checkout init --cone")
            git.run("sparse-checkout set " + " ".join(sparse_paths))
            git.run("pull --depth 1 origin main")

    def build_requirements(self):
        self.tool_requires("cpython/3.10.14", options={"shared": True})

    def layout(self):
        cmake_layout(self)

    def generate(self):
        tc = CMakeToolchain(self)
        tc.generate()

    def get_python(self):
        return self.dependencies.build["cpython"].conf_info.get("user.cpython:python")
        
    def build(self):
        api_list = [api.strip() for api in str(self.options.apis).split(",") if api.strip()]
        
        cmake_content = ["cmake_minimum_required(VERSION 3.15)", "project(glaze_apis C)"]
        
        langs_args = []
        if self.options.language in ("c", "cpp", "both"):
            langs_args.extend(["--lang", "c"])
        if self.options.language in ("cpp", "both"):
            langs_args.extend(["--lang", "cpp"])
        
        out_dir = os.path.join(self.build_folder, "generated")
        os.makedirs(out_dir, exist_ok=True)
        
        self.prepare_python(self.get_python())

        refpages_dir = os.path.join(self.source_folder, "OpenGL-Refpages")
        refpages_args = []
        if self.options.with_docs and os.path.isdir(refpages_dir):
            refpages_args = ["--refpages", refpages_dir]

        cmd = [
            self.get_python(),
            os.path.join(self.source_folder, "glaze_cli.py"),
            "generate",
            "--output-dir", out_dir
        ] + langs_args + refpages_args

        for api_item in api_list:
            parts = api_item.split(":")
            api_name = parts[0]
            api_version = parts[1] if len(parts) > 1 else 'latest'
            cmd.extend(["--api", api_name, api_version])
            
        self.output.info(f"Running generator for APIs: {' '.join(cmd)}")
        self.run(" ".join(cmd))
        
        for api_item in api_list:
            parts = api_item.split(":")
            api_name = parts[0]
            
            if self.options.language in ("c", "both"):
                lib_type = "SHARED" if self.options.shared else "STATIC"
                cmake_content.append(f"add_library(glaze_{api_name} {lib_type} {out_dir}/src/{api_name}.c)")
                cmake_content.append(f"target_include_directories(glaze_{api_name} PUBLIC $<BUILD_INTERFACE:{out_dir}/include>)")
                cmake_content.append(f"target_include_directories(glaze_{api_name} INTERFACE $<INSTALL_INTERFACE:include>)")
                if self.options.shared:
                    cmake_content.append(f"target_compile_definitions(glaze_{api_name} PRIVATE GLAZE_BUILD_DLL)")

        khr_src = os.path.join(self.source_folder, "EGL-Registry", "api", "KHR", "khrplatform.h")
        khr_dst_dir = os.path.join(out_dir, "include", "KHR")
        os.makedirs(khr_dst_dir, exist_ok=True)
        shutil.copy2(khr_src, khr_dst_dir)

        if self.options.language in ("c", "both"):
            with open(os.path.join(self.build_folder, "CMakeLists.txt"), "w") as f:
                f.write("\n".join(cmake_content) + "\n")

            cmake = CMake(self)
            cmake.configure(build_script_folder=self.build_folder)
            cmake.build()

    def prepare_python(self, python):
        site_pkgs = os.path.join(self.build_folder, "site-packages")
        self.run(f'"{python}" -m pip install --target "{site_pkgs}" "{self.source_folder}"')
        os.environ["PYTHONPATH"] = site_pkgs + os.pathsep + os.environ.get("PYTHONPATH", "")

    def package(self):
        out_dir = os.path.join(self.build_folder, "generated")
        copy(self, "*.h", src=os.path.join(out_dir, "include"), dst=os.path.join(self.package_folder, "include"))
        copy(self, "*.hpp", src=os.path.join(out_dir, "include"), dst=os.path.join(self.package_folder, "include"))
            
        if self.options.language in ("c", "both"):
            copy(self, "*.a", src=self.build_folder, dst=os.path.join(self.package_folder, "lib"), keep_path=False)
            copy(self, "*.lib", src=self.build_folder, dst=os.path.join(self.package_folder, "lib"), keep_path=False)
            copy(self, "*.so*", src=self.build_folder, dst=os.path.join(self.package_folder, "lib"), keep_path=False)
            copy(self, "*.dylib", src=self.build_folder, dst=os.path.join(self.package_folder, "lib"), keep_path=False)
            copy(self, "*.dll", src=self.build_folder, dst=os.path.join(self.package_folder, "bin"), keep_path=False)

    def package_info(self):
        self.cpp_info.set_property("cmake_file_name", "glaze")
        api_list = [api.strip() for api in str(self.options.apis).split(",") if api.strip()]
        
        for api_item in api_list:
            api_name = api_item.split(":")[0]
            comp = self.cpp_info.components[api_name]
            comp.set_property("cmake_target_name", f"glaze::{api_name}")
            comp.includedirs = ["include"]
            
            if self.options.language in ("c", "both"):
                comp.libs = [f"glaze_{api_name}"]
                if self.options.shared:
                    comp.defines = ["GLAZE_DLL"]
            else:
                comp.bindirs = []
                comp.libdirs = []
