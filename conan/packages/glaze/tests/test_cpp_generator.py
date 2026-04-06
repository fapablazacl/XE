"""Tests for glaze.generators.cpp_generator.CppGenerator."""

import pytest

from glaze.generators.cpp_generator import (
    CppGenerator,
    _build_group_rename,
    _Capitalizer,
    _EnumIdentifierConverter,
    _is_string_return_command,
    _to_handle_name,
)
from glaze.model import Command, CommandParam, Registry, TypeDecl

# ── Module-level helpers ──────────────────────────────────────────────────────


class TestToHandleName:
    def test_single_word(self) -> None:
        assert _to_handle_name("texture") == "Texture"

    def test_multi_word(self) -> None:
        assert _to_handle_name("program pipeline") == "ProgramPipeline"

    def test_already_capitalized(self) -> None:
        assert _to_handle_name("buffer") == "Buffer"


class TestIsStringReturnCommand:
    def test_const_glubyte_pointer(self) -> None:
        cmd = Command(
            name="glGetString",
            return_type=TypeDecl("GLubyte", is_const=True, is_pointer=True),
            return_type_str="const GLubyte*",
            params=[CommandParam(name="name", type_parts=["GLenum"])],
        )
        assert _is_string_return_command(cmd) is True

    def test_void_return(self) -> None:
        cmd = Command(
            name="glClear",
            return_type=TypeDecl("void"),
            return_type_str="void",
            params=[CommandParam(name="mask", type_parts=["GLbitfield"])],
        )
        assert _is_string_return_command(cmd) is False


class TestBuildGroupRename:
    def test_strips_arb_suffix(self) -> None:
        rename = _build_group_rename({"BufferTargetARB", "ClearBufferMask"})
        assert rename == {"BufferTargetARB": "BufferTarget"}

    def test_no_collision(self) -> None:
        # If clean name already exists, no rename
        rename = _build_group_rename({"BufferTargetARB", "BufferTarget"})
        assert "BufferTargetARB" not in rename

    def test_empty_set(self) -> None:
        assert _build_group_rename(set()) == {}

    def test_no_suffix_match(self) -> None:
        rename = _build_group_rename({"ClearBufferMask"})
        assert rename == {}


# ── Capitalizer ───────────────────────────────────────────────────────────────


class TestCapitalizer:
    def test_normal_word(self) -> None:
        cap = _Capitalizer()
        assert cap.capitalize("buffer") == "Buffer"

    def test_excluded_values(self) -> None:
        cap = _Capitalizer()
        assert cap.capitalize("2D") == "2D"
        assert cap.capitalize("3D") == "3D"


# ── EnumIdentifierConverter ───────────────────────────────────────────────────


class TestEnumIdentifierConverter:
    def test_strips_group_name_parts(self) -> None:
        cap = _Capitalizer()
        conv = _EnumIdentifierConverter("BufferTarget", cap)
        result = conv.convert("GL_ARRAY_BUFFER")
        # "Buffer" and "Target" are stripped as they match enum name parts
        assert result == "eArray"

    def test_keeps_all_parts_when_no_match(self) -> None:
        cap = _Capitalizer()
        conv = _EnumIdentifierConverter("SomeOther", cap)
        result = conv.convert("GL_ARRAY_BUFFER")
        assert result == "eArrayBuffer"

    def test_e_prefix(self) -> None:
        cap = _Capitalizer()
        conv = _EnumIdentifierConverter("ClearBufferMask", cap)
        result = conv.convert("GL_COLOR_BUFFER_BIT")
        assert result.startswith("e")


# ── CppGenerator ──────────────────────────────────────────────────────────────


class TestCppGeneratorBasics:
    def test_name(self, mini_registry: Registry) -> None:
        gen = CppGenerator(mini_registry)
        assert gen.name == "cpp"

    def test_generate_returns_single_hpp(self, mini_registry: Registry) -> None:
        gen = CppGenerator(mini_registry)
        files = gen.generate("gl", "1.0")
        assert len(files) == 1
        key = next(iter(files.keys()))
        assert key.endswith(".hpp")
        assert "gl.hpp" in key

    def test_generate_file_key_uses_api_name(self, mini_registry: Registry) -> None:
        gen = CppGenerator(mini_registry)
        files = gen.generate("gles2", "2.0")
        key = next(iter(files.keys()))
        assert "gles2.hpp" in key


class TestCppGeneratorValidation:
    def test_invalid_api_raises(self, mini_registry: Registry) -> None:
        gen = CppGenerator(mini_registry)
        with pytest.raises(ValueError, match="API 'vulkan' not found"):
            gen.generate("vulkan", "1.0")

    def test_invalid_version_raises(self, mini_registry: Registry) -> None:
        gen = CppGenerator(mini_registry)
        with pytest.raises(ValueError, match=r"Version '9\.9' not found"):
            gen.generate("gl", "9.9")


class TestCppGeneratorOutput:
    def test_output_contains_namespace(self, mini_registry: Registry) -> None:
        gen = CppGenerator(mini_registry)
        files = gen.generate("gl", "1.5")
        content = next(iter(files.values()))
        assert "namespace gl" in content

    def test_output_contains_enum_class(self, mini_registry: Registry) -> None:
        gen = CppGenerator(mini_registry)
        files = gen.generate("gl", "1.5")
        content = next(iter(files.values()))
        assert "enum class" in content

    def test_output_contains_clear(self, mini_registry: Registry) -> None:
        gen = CppGenerator(mini_registry)
        files = gen.generate("gl", "1.0")
        content = next(iter(files.values()))
        assert "glClear" in content


class TestCppGeneratorTimestamp:
    def test_output_contains_generation_header(self, mini_registry: Registry) -> None:
        gen = CppGenerator(mini_registry)
        files = gen.generate("gl", "1.0")
        content = next(iter(files.values()))
        assert "Auto-generated by Glaze" in content
        assert "DO NOT EDIT" in content


class TestCppGeneratorVersionTag:
    def test_version_tag_in_brief(self, mini_registry: Registry) -> None:
        gen = CppGenerator(mini_registry)
        files = gen.generate("gl", "1.5")
        content = next(iter(files.values()))
        assert "[GL 1.0]" in content
        assert "[GL 1.5]" in content


class TestCppGeneratorFlags:
    def test_flags_template_present(self, mini_registry: Registry) -> None:
        gen = CppGenerator(mini_registry)
        files = gen.generate("gl", "1.0")
        content = next(iter(files.values()))
        assert "class Flags" in content

    def test_bitmask_operators_generated(self, mini_registry: Registry) -> None:
        gen = CppGenerator(mini_registry)
        files = gen.generate("gl", "1.0")
        content = next(iter(files.values()))
        assert "operator|(ClearBufferMask" in content
        assert "operator&(ClearBufferMask" in content
        assert "operator^(ClearBufferMask" in content
        assert "operator~(ClearBufferMask" in content

    def test_non_bitmask_no_operators(self, mini_registry: Registry) -> None:
        gen = CppGenerator(mini_registry)
        files = gen.generate("gl", "1.5")
        content = next(iter(files.values()))
        assert "operator|(BufferTarget" not in content

    def test_bitmask_param_uses_flags(self, mini_registry: Registry) -> None:
        gen = CppGenerator(mini_registry)
        files = gen.generate("gl", "1.0")
        content = next(iter(files.values()))
        assert "Flags<ClearBufferMask>" in content

    def test_non_bitmask_param_is_plain_enum(self, mini_registry: Registry) -> None:
        gen = CppGenerator(mini_registry)
        files = gen.generate("gl", "1.5")
        content = next(iter(files.values()))
        assert "Flags<BufferTarget>" not in content
        assert "BufferTarget target" in content

    def test_bitmask_call_arg_uses_value(self, mini_registry: Registry) -> None:
        gen = CppGenerator(mini_registry)
        files = gen.generate("gl", "1.0")
        content = next(iter(files.values()))
        assert ".value())" in content


class TestCppGeneratorDSA:
    def test_dsa_class_in_dsa_namespace(self, mini_registry: Registry) -> None:
        gen = CppGenerator(mini_registry)
        files = gen.generate("gl", "4.5")
        content = next(iter(files.values()))
        assert "namespace dsa" in content
        assert "class Buffer {" in content

    def test_dsa_method_name(self, mini_registry: Registry) -> None:
        gen = CppGenerator(mini_registry)
        assert gen._dsa_method_name("glNamedBufferData", "buffer") == "data"
        assert gen._dsa_method_name("glNamedBufferSubData", "buffer") == "subData"

    def test_dsa_method_present_in_output(self, mini_registry: Registry) -> None:
        gen = CppGenerator(mini_registry)
        files = gen.generate("gl", "4.5")
        content = next(iter(files.values()))
        assert "data(" in content
        assert "subData(" in content

    def test_dsa_not_present_for_old_version(self, mini_registry: Registry) -> None:
        gen = CppGenerator(mini_registry)
        files = gen.generate("gl", "1.5")
        content = next(iter(files.values()))
        assert "class Buffer {" not in content


class TestCppGeneratorArrayView:
    def test_array_view_template_present(self, mini_registry: Registry) -> None:
        gen = CppGenerator(mini_registry)
        files = gen.generate("gl", "4.5")
        content = next(iter(files.values()))
        assert "struct ArrayView" in content

    def test_array_view_overload_generated(self, mini_registry: Registry) -> None:
        gen = CppGenerator(mini_registry)
        files = gen.generate("gl", "4.5")
        content = next(iter(files.values()))
        assert "ArrayView<T>" in content
        assert "template<typename T>" in content

    def test_array_view_uses_size_bytes(self, mini_registry: Registry) -> None:
        gen = CppGenerator(mini_registry)
        files = gen.generate("gl", "4.5")
        content = next(iter(files.values()))
        assert "data.size_bytes()" in content


class TestCppGeneratorSingleObjectCreation:
    def test_singular_gen_functor_present(self, mini_registry: Registry) -> None:
        gen = CppGenerator(mini_registry)
        files = gen.generate("gl", "1.5")
        content = next(iter(files.values()))
        assert "genBuffer" in content
        assert "::glGenBuffers(1," in content

    def test_singular_delete_functor_present(self, mini_registry: Registry) -> None:
        gen = CppGenerator(mini_registry)
        files = gen.generate("gl", "1.5")
        content = next(iter(files.values()))
        assert "deleteBuffer" in content
        assert "::glDeleteBuffers(1," in content

    def test_create_program_returns_handle(self, mini_registry: Registry) -> None:
        gen = CppGenerator(mini_registry)
        files = gen.generate("gl", "2.0")
        content = next(iter(files.values()))
        # glCreateProgram should return Program, not GLuint
        assert "Program" in content
        assert "::glCreateProgram()" in content

    def test_create_shader_returns_handle(self, mini_registry: Registry) -> None:
        gen = CppGenerator(mini_registry)
        files = gen.generate("gl", "2.0")
        content = next(iter(files.values()))
        assert "Shader" in content
        assert "::glCreateShader(" in content


class TestConvertFunctionName:
    def test_standard_conversion(self, mini_registry: Registry) -> None:
        gen = CppGenerator(mini_registry)
        assert gen._convert_function_name("glClearColor") == "clearColor"

    def test_single_word(self, mini_registry: Registry) -> None:
        gen = CppGenerator(mini_registry)
        assert gen._convert_function_name("glClear") == "clear"

    def test_long_name(self, mini_registry: Registry) -> None:
        gen = CppGenerator(mini_registry)
        assert gen._convert_function_name("glBindBuffer") == "bindBuffer"
