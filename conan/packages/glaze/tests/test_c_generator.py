"""Tests for glaze.generators.c_generator.CGenerator."""

import pytest

from glaze.generators.c_generator import CGenerator
from glaze.model import Registry


class TestCGeneratorBasics:
    def test_name(self, mini_registry: Registry) -> None:
        gen = CGenerator(mini_registry)
        assert gen.name == "c"

    def test_generate_returns_header_and_source(self, mini_registry: Registry) -> None:
        gen = CGenerator(mini_registry)
        files = gen.generate("gl", "1.0")
        assert "include/glaze/gl.h" in files
        assert "src/gl.c" in files

    def test_generate_file_keys_use_api_name(self, mini_registry: Registry) -> None:
        gen = CGenerator(mini_registry)
        files = gen.generate("gles2", "2.0")
        assert "include/glaze/gles2.h" in files
        assert "src/gles2.c" in files


class TestCGeneratorValidation:
    def test_invalid_api_raises(self, mini_registry: Registry) -> None:
        gen = CGenerator(mini_registry)
        with pytest.raises(ValueError, match="API 'vulkan' not found"):
            gen.generate("vulkan", "1.0")

    def test_invalid_version_raises(self, mini_registry: Registry) -> None:
        gen = CGenerator(mini_registry)
        with pytest.raises(ValueError, match=r"Version '9\.9' not found"):
            gen.generate("gl", "9.9")


class TestCGeneratorOutput:
    def test_header_contains_enum_defines(self, mini_registry: Registry) -> None:
        gen = CGenerator(mini_registry)
        files = gen.generate("gl", "1.5")
        header = files["include/glaze/gl.h"]
        assert "GL_ARRAY_BUFFER" in header
        assert "GL_COLOR_BUFFER_BIT" in header

    def test_header_contains_function_typedefs(self, mini_registry: Registry) -> None:
        gen = CGenerator(mini_registry)
        files = gen.generate("gl", "1.0")
        header = files["include/glaze/gl.h"]
        assert "PFNGLCLEARPROC" in header

    def test_source_contains_loader(self, mini_registry: Registry) -> None:
        gen = CGenerator(mini_registry)
        files = gen.generate("gl", "1.0")
        source = files["src/gl.c"]
        assert "glazeLoadFunctions" in source

    def test_source_contains_function_pointers(self, mini_registry: Registry) -> None:
        gen = CGenerator(mini_registry)
        files = gen.generate("gl", "1.0")
        source = files["src/gl.c"]
        assert "glClear" in source


class TestCGeneratorVersionTag:
    def test_version_tag_in_brief(self, mini_registry: Registry) -> None:
        gen = CGenerator(mini_registry)
        files = gen.generate("gl", "1.5")
        header = files["include/glaze/gl.h"]
        assert "[GL 1.0]" in header
        assert "[GL 1.5]" in header


class TestCGeneratorVersionGating:
    def test_emits_default_glaze_gl_version(self, mini_registry: Registry) -> None:
        """The default GLAZE_GL_VERSION matches the ceiling the header was generated for."""
        gen = CGenerator(mini_registry)
        files = gen.generate("gl", "4.5")
        header = files["include/glaze/gl.h"]
        assert "#ifndef GLAZE_GL_VERSION" in header
        assert "#  define GLAZE_GL_VERSION 45" in header

    def test_emits_standard_version_macros(self, mini_registry: Registry) -> None:
        """Each enabled feature emits the standard ``GL_VERSION_X_Y`` macro."""
        gen = CGenerator(mini_registry)
        files = gen.generate("gl", "4.5")
        header = files["include/glaze/gl.h"]
        assert "#define GL_VERSION_1_0 1" in header
        assert "#define GL_VERSION_1_5 1" in header
        assert "#define GL_VERSION_2_0 1" in header
        assert "#define GL_VERSION_4_5 1" in header

    def test_header_wraps_feature_block_in_if(self, mini_registry: Registry) -> None:
        """Each feature block is preceded by an ``#if GLAZE_GL_VERSION >= NN`` guard."""
        gen = CGenerator(mini_registry)
        files = gen.generate("gl", "4.5")
        header = files["include/glaze/gl.h"]
        # Within a 4.5 generation, glNamedBufferData must sit inside a >=45 block
        assert "#if GLAZE_GL_VERSION >= 45" in header
        guard_idx = header.find("#if GLAZE_GL_VERSION >= 45")
        named_idx = header.find("glNamedBufferData", guard_idx)
        assert named_idx > guard_idx
        # And glClear must sit inside a >=10 block, not the >=45 block
        assert "#if GLAZE_GL_VERSION >= 10" in header
        clear_guard_idx = header.find("#if GLAZE_GL_VERSION >= 10")
        clear_idx = header.find("glClear", clear_guard_idx)
        assert clear_idx > clear_guard_idx
        assert clear_idx < guard_idx

    def test_source_loader_grouped_by_version(self, mini_registry: Registry) -> None:
        """The loader emits one ``#if GLAZE_GL_VERSION >= NN`` block per feature."""
        gen = CGenerator(mini_registry)
        files = gen.generate("gl", "4.5")
        source = files["src/gl.c"]
        # The 4.5-only loader entry sits inside a >=45 guard
        assert "#if GLAZE_GL_VERSION >= 45" in source
        guard_idx = source.find("#if GLAZE_GL_VERSION >= 45")
        named_idx = source.find('getProcAddress("glNamedBufferData")', guard_idx)
        assert named_idx > guard_idx


class TestCGeneratorTimestamp:
    def test_header_contains_generation_header(self, mini_registry: Registry) -> None:
        gen = CGenerator(mini_registry)
        files = gen.generate("gl", "1.0")
        header = files["include/glaze/gl.h"]
        assert "Auto-generated by Glaze" in header
        assert "DO NOT EDIT" in header

    def test_source_contains_generation_header(self, mini_registry: Registry) -> None:
        gen = CGenerator(mini_registry)
        files = gen.generate("gl", "1.0")
        source = files["src/gl.c"]
        assert "Auto-generated by Glaze" in source


class TestCGeneratorGlCompat:
    def test_gl_compat_generates_files(self, mini_registry: Registry) -> None:
        gen = CGenerator(mini_registry)
        files = gen.generate("gl_compat", "4.5")
        assert "include/glaze/gl_compat.h" in files
        assert "src/gl_compat.c" in files

    def test_gl_compat_includes_all_versions(self, mini_registry: Registry) -> None:
        gen = CGenerator(mini_registry)
        files = gen.generate("gl_compat", "4.5")
        header = files["include/glaze/gl_compat.h"]
        assert "glClear" in header
        assert "glCreateProgram" in header
        assert "glNamedBufferData" in header

    def test_gl_compat_keeps_deprecated_enums(self, mini_registry: Registry) -> None:
        """Compat profile keeps GL_FLOAT even though core removes it in 3.1."""
        gen = CGenerator(mini_registry)
        files = gen.generate("gl_compat", "4.5")
        header = files["include/glaze/gl_compat.h"]
        assert "GL_FLOAT" in header


class TestCommandPtrTypeName:
    def test_standard_command(self, mini_registry: Registry) -> None:
        gen = CGenerator(mini_registry)
        assert gen._command_ptr_type_name("glClear") == "PFNGLCLEARPROC"

    def test_longer_command(self, mini_registry: Registry) -> None:
        gen = CGenerator(mini_registry)
        assert gen._command_ptr_type_name("glBindBuffer") == "PFNGLBINDBUFFERPROC"
