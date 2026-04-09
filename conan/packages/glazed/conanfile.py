"""glazed — pre-generated OpenGL bindings derived from glaze.

`glaze` is a Python-driven code generator that runs at consumer-package-build
time. `glazed` runs glaze once on the publishing machine across the full API
matrix and ships the prebuilt headers + loader sources, so consumers pulling
glazed from the Conan registry never need Python, gl.xml, or Jinja.

The generation matrix is intentionally small: glaze emits a single
`{api}.hpp` file per API family regardless of version, with compile-time
feature gates keyed off macros like `GLAZE_GL_VERSION`. Generating one
binding per family at the highest supported version covers every lower
version too — consumers pick the version with `-DGLAZED_GL_VERSION=33`.
"""

import os
import shutil

import jinja2
from conan import ConanFile
from conan.tools.files import copy


# (api_name, version) — versions chosen as the highest supported per API
# family. Lower versions are reachable via the per-API GLAZE_*_VERSION macro.
GLAZED_MATRIX = [
    ("gl", "4.6"),
    ("gl_compat", "4.6"),
    ("gles1", "1.0"),
    ("gles2", "3.2"),
    ("glsc2", "2.0"),
]


class GlazedConan(ConanFile):
    name = "glazed"
    version = "1.0.0"

    description = (
        "Pre-generated OpenGL C and C++ bindings for every API family supported "
        "by glaze. No Python required at consumer build time."
    )
    license = "MIT"
    url = "https://github.com/anthropics/xe"
    topics = ("opengl", "bindings", "code-generation", "glaze")

    settings = "os", "compiler", "build_type", "arch"

    options = {
        "with_docs": [True, False],
        "fPIC": [True, False],
    }
    default_options = {
        "with_docs": True,
        "fPIC": True,
    }

    exports_sources = (
        "templates/*",
        "README.md",
        "CLAUDE.md",
    )

    def config_options(self):
        if self.settings.os == "Windows":
            del self.options.fPIC

    def layout(self):
        self.folders.build = "build"
        self.folders.generators = "build/generators"

    def build_requirements(self):
        # glaze ships its Python generator sources to share/glaze/ inside its
        # package; we run them at build time only. tool_requires are not
        # transitive, so glazed must also pull cpython directly.
        self.tool_requires("glaze/1.0.0")
        self.tool_requires("cpython/3.10.14", options={"shared": True, "with_tkinter": False})

    def source(self):
        """Sparse-checkout the Khronos registry and refpages.

        glaze ships only the generator code, not the registry XML or
        documentation, so glazed clones them itself. Mirrors glaze's own
        source() method.
        """
        from conan.tools.scm import Git

        repos = [
            ("OpenGL-Registry", "https://github.com/KhronosGroup/OpenGL-Registry.git",
             ["xml/gl.xml"]),
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

    def _python_exe(self):
        return self.dependencies.build["cpython"].conf_info.get("user.cpython:python")

    def _glaze_share_dir(self):
        return self.dependencies.build["glaze"].conf_info.get("user.glaze:source_dir")

    def _glaze_cli(self):
        return self.dependencies.build["glaze"].conf_info.get("user.glaze:cli_path")

    def _prepare_python(self, python_exe, glaze_share_dir):
        """Pip-install glaze's shipped Python tree into a local site-packages
        so `import glaze.parser` etc. resolves when glaze_cli.py runs.
        Mirrors glaze's own `prepare_python` helper."""
        site_pkgs = os.path.join(self.build_folder, "site-packages")
        os.makedirs(site_pkgs, exist_ok=True)
        self.run(f'"{python_exe}" -m pip install --target "{site_pkgs}" "{glaze_share_dir}"')
        os.environ["PYTHONPATH"] = site_pkgs + os.pathsep + os.environ.get("PYTHONPATH", "")

    def build(self):
        out_dir = os.path.join(self.build_folder, "generated")
        os.makedirs(out_dir, exist_ok=True)

        python_exe = self._python_exe()
        glaze_share_dir = self._glaze_share_dir()
        glaze_cli = self._glaze_cli()

        if not (python_exe and glaze_share_dir and glaze_cli):
            raise RuntimeError(
                "glazed: missing tool_requires conf — ensure glaze/1.0.0 is built "
                "with the additive share/glaze/ packaging patch and cpython is on PATH."
            )

        self._prepare_python(python_exe, glaze_share_dir)

        registry_path = os.path.join(self.source_folder, "OpenGL-Registry", "xml", "gl.xml")
        refpages_dir = os.path.join(self.source_folder, "OpenGL-Refpages")

        cmd = [
            f'"{python_exe}"',
            f'"{glaze_cli}"',
            "generate",
            "--registry", f'"{registry_path}"',
            "--output-dir", f'"{out_dir}"',
            "--lang", "c",
            "--lang", "cpp",
        ]
        if self.options.with_docs and os.path.isdir(refpages_dir):
            cmd.extend(["--refpages", f'"{refpages_dir}"'])

        for api_name, api_version in GLAZED_MATRIX:
            cmd.extend(["--api", api_name, api_version])

        self.output.info("glazed: generating full OpenGL binding matrix")
        self.run(" ".join(cmd))

        # Render the per-API CMake build module that consumers will pull in
        # via cmake_build_modules. The .cmake file declares INTERFACE targets
        # whose sources include the loader .c so the consumer's compiler
        # builds them as part of its own translation units.
        templates_dir = os.path.join(self.recipe_folder, "templates")
        env = jinja2.Environment(
            loader=jinja2.FileSystemLoader(templates_dir),
            trim_blocks=True,
            lstrip_blocks=True,
            keep_trailing_newline=True,
        )
        cmake_module = env.get_template("cmake/glazed-targets.cmake.j2").render(
            apis=[api for api, _ in GLAZED_MATRIX],
        )
        cmake_out = os.path.join(self.build_folder, "glazed-targets.cmake")
        with open(cmake_out, "w", encoding="utf-8") as f:
            f.write(cmake_module)

    def package(self):
        out_dir = os.path.join(self.build_folder, "generated")
        copy(self, "*.h", src=os.path.join(out_dir, "include"),
             dst=os.path.join(self.package_folder, "include"))
        copy(self, "*.hpp", src=os.path.join(out_dir, "include"),
             dst=os.path.join(self.package_folder, "include"))
        copy(self, "*.c", src=os.path.join(out_dir, "src"),
             dst=os.path.join(self.package_folder, "src"))

        copy(self, "glazed-targets.cmake", src=self.build_folder,
             dst=os.path.join(self.package_folder, "share", "glazed"))

    def package_info(self):
        self.cpp_info.set_property("cmake_file_name", "glazed")
        # The build module wires up every glazed::* target. Conan includes it
        # automatically when consumers do find_package(glazed).
        self.cpp_info.set_property(
            "cmake_build_modules",
            [os.path.join("share", "glazed", "glazed-targets.cmake")],
        )
        self.cpp_info.includedirs = ["include"]
        self.cpp_info.bindirs = []
        self.cpp_info.libdirs = []
