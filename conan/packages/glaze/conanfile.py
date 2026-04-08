
import os
import shutil

import jinja2
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
        "apis": "gl:3.3,gl_compat:4.6,gles1:1.0,gles2:3.2,glsc2:2.0",
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
            ("OpenGL-Refpages", "https://github.com/KhronosGroup/OpenGL-Refpages.git",
             ["gl4", "es1.1", "es3.0"]),
        ]

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
        self.tool_requires("cpython/3.10.14", options={"shared": True, "with_tkinter": False})

    def layout(self):
        cmake_layout(self)

    def generate(self):
        tc = CMakeToolchain(self)
        tc.generate()

    def get_python(self):
        return self.dependencies.build["cpython"].conf_info.get("user.cpython:python")
        
    def build(self):
        api_list = [api.strip() for api in str(self.options.apis).split(",") if api.strip()]

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

        api_names = [api_item.split(":")[0] for api_item in api_list]

        khr_src = os.path.join(self.source_folder, "EGL-Registry", "api", "KHR", "khrplatform.h")
        khr_dst_dir = os.path.join(out_dir, "include", "KHR")
        os.makedirs(khr_dst_dir, exist_ok=True)
        shutil.copy2(khr_src, khr_dst_dir)

        # Always build the C library — the C++ API depends on C function pointers
        if self.options.language in ("c", "cpp", "both"):
            templates_dir = os.path.join(self.source_folder, "glaze", "templates")
            env = jinja2.Environment(
                loader=jinja2.FileSystemLoader(templates_dir),
                trim_blocks=True,
                lstrip_blocks=True,
                keep_trailing_newline=True,
            )
            template = env.get_template("cmake/CMakeLists.txt.j2")
            cmake_content = template.render(
                apis=api_names,
                lib_type="SHARED" if self.options.shared else "STATIC",
                shared=bool(self.options.shared),
                out_dir=out_dir.replace("\\", "/"),
            )

            with open(os.path.join(self.build_folder, "CMakeLists.txt"), "w") as f:
                f.write(cmake_content)

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

        # Static, API-agnostic RAII smart-pointer header. Lives in the source
        # tree under glaze/templates/cpp/static/ and is copied verbatim into
        # the package; not generated, never templated.
        if self.options.language in ("cpp", "both"):
            copy(self, "raii.hpp",
                 src=os.path.join(self.source_folder, "glaze", "templates", "cpp", "static"),
                 dst=os.path.join(self.package_folder, "include", "glaze"))
            
        # Always package the C library — the C++ API depends on C function pointers
        if self.options.language in ("c", "cpp", "both"):
            copy(self, "*.a", src=self.build_folder, dst=os.path.join(self.package_folder, "lib"), keep_path=False)
            copy(self, "*.lib", src=self.build_folder, dst=os.path.join(self.package_folder, "lib"), keep_path=False)
            copy(self, "*.so*", src=self.build_folder, dst=os.path.join(self.package_folder, "lib"), keep_path=False)
            copy(self, "*.dylib", src=self.build_folder, dst=os.path.join(self.package_folder, "lib"), keep_path=False)
            copy(self, "*.dll", src=self.build_folder, dst=os.path.join(self.package_folder, "bin"), keep_path=False)

    def package_info(self):
        self.cpp_info.set_property("cmake_file_name", "glaze")
        api_list = [api.strip() for api in str(self.options.apis).split(",") if api.strip()]

        # Top-level header-only component for the API-agnostic glaze::Unique /
        # Shared / Weak templates. Downstream users who only want the smart
        # pointers (e.g. with their own handle types) link this directly.
        if self.options.language in ("cpp", "both"):
            raii = self.cpp_info.components["raii"]
            raii.set_property("cmake_target_name", "glaze::raii")
            raii.includedirs = ["include"]
            raii.bindirs = []
            raii.libdirs = []

        for api_item in api_list:
            api_name = api_item.split(":")[0]
            comp = self.cpp_info.components[api_name]
            comp.set_property("cmake_target_name", f"glaze::{api_name}")
            comp.includedirs = ["include"]

            # Always provide the C library — the C++ API depends on C function pointers
            if self.options.language in ("c", "cpp", "both"):
                comp.libs = [f"glaze_{api_name}"]
                if self.options.shared:
                    comp.defines = ["GLAZE_DLL"]
            else:
                comp.bindirs = []
                comp.libdirs = []

            # Header-only enriched-handle wrappers for legacy commands.
            if self.options.language in ("cpp", "both"):
                hwrap = self.cpp_info.components[f"{api_name}_handle"]
                hwrap.set_property("cmake_target_name", f"glaze::{api_name}_handle")
                hwrap.includedirs = ["include"]
                hwrap.bindirs = []
                hwrap.libdirs = []
                hwrap.requires = [api_name]
