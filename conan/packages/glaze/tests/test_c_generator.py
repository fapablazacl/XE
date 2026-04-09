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


class TestCGeneratorExtensions:
    """Tests that exercise extension emission with explicit vendor opt-in."""

    def _gen(self, registry: Registry, **kwargs) -> CGenerator:
        return CGenerator(registry, extension_vendors=["ARB"], **kwargs)

    def test_emits_per_extension_guard(self, mini_registry: Registry) -> None:
        gen = self._gen(mini_registry)
        files = gen.generate("gl", "4.5")
        header = files["include/glaze/gl.h"]
        assert "#ifndef GLAZE_GL_NO_EXT_ARB_buffer_storage" in header
        assert "#ifndef GLAZE_GL_NO_EXT_ARB_draw_instanced" in header

    def test_emits_khronos_define(self, mini_registry: Registry) -> None:
        gen = self._gen(mini_registry)
        files = gen.generate("gl", "4.5")
        header = files["include/glaze/gl.h"]
        assert "#define GL_ARB_buffer_storage 1" in header
        assert "#define GL_ARB_draw_instanced 1" in header

    def test_emits_extension_flag_extern(self, mini_registry: Registry) -> None:
        gen = self._gen(mini_registry)
        files = gen.generate("gl", "4.5")
        header = files["include/glaze/gl.h"]
        assert "GLAZE_API extern int GLAZE_EXT_GL_ARB_buffer_storage;" in header
        assert "GLAZE_API extern int GLAZE_EXT_GL_ARB_draw_instanced;" in header

    def test_extension_command_emitted(self, mini_registry: Registry) -> None:
        gen = self._gen(mini_registry)
        files = gen.generate("gl", "4.5")
        header = files["include/glaze/gl.h"]
        # The extension's typedef appears, and it appears AFTER the per-ext guard.
        guard_idx = header.find("#ifndef GLAZE_GL_NO_EXT_ARB_draw_instanced")
        typedef_idx = header.find("PFNGLDRAWARRAYSINSTANCEDARBPROC", guard_idx)
        assert guard_idx != -1 and typedef_idx > guard_idx

    def test_dedups_enum_already_in_core(self, mini_registry: Registry) -> None:
        """An extension enum that core already requires must not be re-emitted."""
        gen = self._gen(mini_registry)
        files = gen.generate("gl", "4.5")
        header = files["include/glaze/gl.h"]
        # GL_ARRAY_BUFFER is required by core 1.5 AND by GL_ARB_buffer_storage —
        # the extension block must NOT contain a second `#define GL_ARRAY_BUFFER`.
        assert header.count("#define GL_ARRAY_BUFFER ") == 1

    def test_glaze_load_extensions_prototype(self, mini_registry: Registry) -> None:
        gen = self._gen(mini_registry)
        files = gen.generate("gl", "4.5")
        header = files["include/glaze/gl.h"]
        assert "GLAZE_API void glazeLoadExtensions(GLAZE_GETPROCADDRESS getProcAddress);" in header
        assert "GLAZE_API int  glazeHasExtension(const char *name);" in header
        assert "GLAZE_API int  glazeIsVersionSupported(int major, int minor);" in header

    def test_master_no_extensions_switch(self, mini_registry: Registry) -> None:
        """Both header and source extension sections sit inside the master switch."""
        gen = self._gen(mini_registry)
        files = gen.generate("gl", "4.5")
        header = files["include/glaze/gl.h"]
        source = files["src/gl.c"]
        assert "#ifndef GLAZE_GL_NO_EXTENSIONS" in header
        assert "#endif /* GLAZE_GL_NO_EXTENSIONS */" in header
        assert "#ifndef GLAZE_GL_NO_EXTENSIONS" in source
        assert "#endif /* GLAZE_GL_NO_EXTENSIONS */" in source

    def test_extension_loader_sets_flag_and_loads_pointers(self, mini_registry: Registry) -> None:
        gen = self._gen(mini_registry)
        files = gen.generate("gl", "4.5")
        source = files["src/gl.c"]
        # The runtime sets the flag and loads the function pointer for the
        # command-bearing extension.
        assert "GLAZE_EXT_GL_ARB_draw_instanced = 1;" in source
        assert 'getProcAddress("glDrawArraysInstancedARB")' in source

    def test_default_no_extensions(self, mini_registry: Registry) -> None:
        """With no extension filters, no extensions should be emitted."""
        gen = CGenerator(mini_registry)
        files = gen.generate("gl", "4.5")
        header = files["include/glaze/gl.h"]
        assert "GL_ARB_buffer_storage" not in header
        assert "GL_ARB_draw_instanced" not in header
        assert "GL_NV_shader_buffer_load" not in header
        assert "GL_KHR_debug" not in header

    def test_vendor_filter_includes_only_matching(self, mini_registry: Registry) -> None:
        gen = CGenerator(mini_registry, extension_vendors=["NV"])
        files = gen.generate("gl", "4.5")
        header = files["include/glaze/gl.h"]
        assert "GL_NV_shader_buffer_load" in header
        assert "GL_ARB_buffer_storage" not in header
        assert "GL_KHR_debug" not in header

    def test_name_filter_includes_only_matching(self, mini_registry: Registry) -> None:
        gen = CGenerator(mini_registry, extension_names=["GL_KHR_debug"])
        files = gen.generate("gl", "4.5")
        header = files["include/glaze/gl.h"]
        assert "GL_KHR_debug" in header
        assert "GL_ARB_buffer_storage" not in header
        assert "GL_NV_shader_buffer_load" not in header

    def test_vendor_and_name_filters_are_additive(self, mini_registry: Registry) -> None:
        gen = CGenerator(mini_registry, extension_vendors=["ARB"], extension_names=["GL_KHR_debug"])
        files = gen.generate("gl", "4.5")
        header = files["include/glaze/gl.h"]
        assert "GL_ARB_buffer_storage" in header
        assert "GL_ARB_draw_instanced" in header
        assert "GL_KHR_debug" in header
        assert "GL_NV_shader_buffer_load" not in header

    def test_vendor_filter_case_insensitive(self, mini_registry: Registry) -> None:
        gen = CGenerator(mini_registry, extension_vendors=["arb"])
        files = gen.generate("gl", "4.5")
        header = files["include/glaze/gl.h"]
        assert "GL_ARB_buffer_storage" in header


class TestExtensionFilterValidation:
    def test_unknown_vendor_returns_error(self, mini_registry: Registry) -> None:
        gen = CGenerator(mini_registry, extension_vendors=["FAKE_VENDOR"])
        errors = gen.validate_extension_filters("gl")
        assert len(errors) == 1
        assert "FAKE_VENDOR" in errors[0]

    def test_unknown_extension_name_returns_error(self, mini_registry: Registry) -> None:
        gen = CGenerator(mini_registry, extension_names=["GL_FOO_nonexistent"])
        errors = gen.validate_extension_filters("gl")
        assert len(errors) == 1
        assert "GL_FOO_nonexistent" in errors[0]

    def test_valid_vendor_returns_no_errors(self, mini_registry: Registry) -> None:
        gen = CGenerator(mini_registry, extension_vendors=["ARB", "NV"])
        errors = gen.validate_extension_filters("gl")
        assert errors == []

    def test_valid_extension_name_returns_no_errors(self, mini_registry: Registry) -> None:
        gen = CGenerator(mini_registry, extension_names=["GL_ARB_buffer_storage"])
        errors = gen.validate_extension_filters("gl")
        assert errors == []


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
