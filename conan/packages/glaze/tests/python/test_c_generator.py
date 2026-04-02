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


class TestCommandPtrTypeName:
    def test_standard_command(self, mini_registry: Registry) -> None:
        gen = CGenerator(mini_registry)
        assert gen._command_ptr_type_name("glClear") == "PFNGLCLEARPROC"

    def test_longer_command(self, mini_registry: Registry) -> None:
        gen = CGenerator(mini_registry)
        assert gen._command_ptr_type_name("glBindBuffer") == "PFNGLBINDBUFFERPROC"
