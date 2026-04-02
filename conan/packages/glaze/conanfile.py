import os
from conan import ConanFile
from conan.tools.cmake import CMake, CMakeToolchain, cmake_layout
from conan.tools.files import copy

class GlazeConan(ConanFile):
    name = "glaze"
    version = "1.0.0"
    
    exports_sources = (
        "glaze_cli.py",
        "glaze/*",
        "requirements.txt",
    )

    settings = "os", "compiler", "build_type", "arch"

    options = {
        "apis": ["ANY"],
        "language": ["c", "cpp", "both"]
    }

    default_options = {
        "apis": "gl:3.3,gles1:1.0,gles2:3.2,glsc2:2.0",
        "language": "cpp"
    }

    def source(self):
        from conan.tools.scm import Git

        git_registry = Git(self, folder="OpenGL-Registry")
        git_registry.clone(
            url="https://github.com/KhronosGroup/OpenGL-Registry.git",
            target=".",
            args=["--depth", "1"],
        )

        git_refpages = Git(self, folder="OpenGL-Refpages")
        git_refpages.clone(
            url="https://github.com/KhronosGroup/OpenGL-Refpages.git",
            target=".",
            args=["--depth", "1"],
        )

    def layout(self):
        cmake_layout(self)

    def generate(self):
        tc = CMakeToolchain(self)
        tc.generate()

    def build(self):
        api_list = [api.strip() for api in str(self.options.apis).split(",") if api.strip()]
        
        cmake_content = ["cmake_minimum_required(VERSION 3.15)", "project(glaze_apis C)"]
        
        langs_args = []
        if self.options.language in ("c", "both"):
            langs_args.extend(["--lang", "c"])
        if self.options.language in ("cpp", "both"):
            langs_args.extend(["--lang", "cpp"])
        
        out_dir = os.path.join(self.build_folder, "generated")
        os.makedirs(out_dir, exist_ok=True)
            
        cmd = [
            "python3",
            os.path.join(self.source_folder, "glaze_cli.py"),
            "generate",
            "--output-dir", out_dir
        ] + langs_args

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
                cmake_content.append(f"add_library(glaze_{api_name} STATIC {out_dir}/src/{api_name}.c)")
                cmake_content.append(f"target_include_directories(glaze_{api_name} PUBLIC $<BUILD_INTERFACE:{out_dir}/include>)")
                cmake_content.append(f"target_include_directories(glaze_{api_name} INTERFACE $<INSTALL_INTERFACE:include>)")

        if self.options.language in ("c", "both"):
            with open(os.path.join(self.build_folder, "CMakeLists.txt"), "w") as f:
                f.write("\n".join(cmake_content) + "\n")

            cmake = CMake(self)
            cmake.configure(build_script_folder=self.build_folder)
            cmake.build()

    def package(self):
        out_dir = os.path.join(self.build_folder, "generated")
        copy(self, "*.h", src=os.path.join(out_dir, "include"), dst=os.path.join(self.package_folder, "include"))
        copy(self, "*.hpp", src=os.path.join(out_dir, "include"), dst=os.path.join(self.package_folder, "include"))
            
        if self.options.language in ("c", "both"):
            copy(self, "*.a", src=self.build_folder, dst=os.path.join(self.package_folder, "lib"), keep_path=False)
            copy(self, "*.lib", src=self.build_folder, dst=os.path.join(self.package_folder, "lib"), keep_path=False)

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
            else:
                comp.bindirs = []
                comp.libdirs = []
