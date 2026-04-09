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

    def test_generate_returns_main_handle_and_static_raii(self, mini_registry: Registry) -> None:
        gen = CppGenerator(mini_registry)
        files = gen.generate("gl", "1.0")
        # Per-API gl_raii.hpp is no longer generated. Instead, the API-agnostic
        # raii.hpp is shipped verbatim alongside the generated per-API headers.
        assert len(files) == 3
        assert "include/glaze/gl.hpp" in files
        assert "include/glaze/gl_handle.hpp" in files
        assert "include/glaze/raii.hpp" in files
        assert "include/glaze/gl_raii.hpp" not in files

    def test_generate_file_key_uses_api_name(self, mini_registry: Registry) -> None:
        gen = CppGenerator(mini_registry)
        files = gen.generate("gles2", "2.0")
        assert any("gles2.hpp" in key for key in files)
        assert any("gles2_handle.hpp" in key for key in files)
        # Per-API RAII header is no longer generated.
        assert not any("gles2_raii.hpp" in key for key in files)


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


class TestCppGeneratorVersionGating:
    def test_glaze_gl_version_comes_from_c_header(self, mini_registry: Registry) -> None:
        """The C++ header relies on the C header for the GLAZE_GL_VERSION default."""
        gen = CppGenerator(mini_registry)
        files = gen.generate("gl", "4.5")
        hpp = files["include/glaze/gl.hpp"]
        # gl.hpp delegates the macro definition to gl.h, which it includes.
        assert '#include "gl.h"' in hpp
        # gl.hpp itself does not redefine GLAZE_GL_VERSION.
        assert "#  define GLAZE_GL_VERSION" not in hpp
        assert "#define GLAZE_GL_VERSION" not in hpp

    def test_functor_gated_by_version(self, mini_registry: Registry) -> None:
        """A 4.5-only functor sits inside ``#if GLAZE_GL_VERSION >= 45``."""
        gen = CppGenerator(mini_registry)
        files = gen.generate("gl", "4.5")
        hpp = files["include/glaze/gl.hpp"]
        guard = "#if GLAZE_GL_VERSION >= 45"
        assert guard in hpp
        guard_idx = hpp.find(guard)
        named_idx = hpp.find("_NamedBufferDataFn", guard_idx)
        assert named_idx > guard_idx
        # And a 1.0 functor sits inside a >=10 guard, not the >=45 guard
        clear_guard_idx = hpp.find("#if GLAZE_GL_VERSION >= 10")
        assert clear_guard_idx != -1
        clear_idx = hpp.find("_ClearFn", clear_guard_idx)
        assert 0 < clear_idx < guard_idx

    def test_enum_class_unconditional(self, mini_registry: Registry) -> None:
        """Enum classes are inert constants, deliberately not version-gated."""
        gen = CppGenerator(mini_registry)
        files = gen.generate("gl", "4.5")
        hpp = files["include/glaze/gl.hpp"]
        idx = hpp.find("enum class ClearBufferMask")
        assert idx != -1
        # Walk backwards to the previous non-blank line and confirm it isn't an
        # `#if GLAZE_GL_VERSION` guard.
        prefix = hpp[:idx].rstrip()
        prev_line = prefix.rsplit("\n", 1)[-1]
        assert "GLAZE_GL_VERSION" not in prev_line

    def test_handle_traits_gated(self, mini_registry: Registry) -> None:
        """``HandleTraits<Buffer>`` is gated by the version that introduces both creator and deleter."""
        gen = CppGenerator(mini_registry)
        files = gen.generate("gl", "4.5")
        hpp = files["include/glaze/gl.hpp"]
        # Buffer creator/deleter come from GL 1.5; the line directly above the
        # specialization must be the matching #if guard.
        target = "template<> struct HandleTraits<Buffer>"
        idx = hpp.find(target)
        assert idx != -1
        line_start = hpp.rfind("\n", 0, idx) + 1
        prev_line_end = line_start - 1
        prev_line_start = hpp.rfind("\n", 0, prev_line_end) + 1
        prev_line = hpp[prev_line_start:prev_line_end]
        assert prev_line == "#if GLAZE_GL_VERSION >= 15"

    def test_handle_header_method_gated(self, mini_registry: Registry) -> None:
        """``handle::*`` method bodies are wrapped in their own version guards."""
        gen = CppGenerator(mini_registry)
        files = gen.generate("gl", "4.5")
        handle_hpp = files["include/glaze/gl_handle.hpp"]
        # gl_handle.hpp inherits GLAZE_GL_VERSION via gl.hpp -> gl.h
        assert '#include "gl.hpp"' in handle_hpp
        # At least one method body should be guarded by GLAZE_GL_VERSION
        assert "#if GLAZE_GL_VERSION >= " in handle_hpp


class TestCppGeneratorExtensions:
    """Tests that exercise extension emission with explicit vendor opt-in."""

    def _gen(self, registry: Registry, **kwargs) -> CppGenerator:
        return CppGenerator(registry, extension_vendors=["ARB"], **kwargs)

    def test_extension_struct_present(self, mini_registry: Registry) -> None:
        gen = self._gen(mini_registry)
        files = gen.generate("gl", "4.5")
        hpp = files["include/glaze/gl.hpp"]
        assert "struct Extension {" in hpp
        assert "namespace exts {" in hpp

    def test_extension_token_constexpr(self, mini_registry: Registry) -> None:
        gen = self._gen(mini_registry)
        files = gen.generate("gl", "4.5")
        hpp = files["include/glaze/gl.hpp"]
        assert (
            'constexpr Extension ARB_buffer_storage{ "GL_ARB_buffer_storage", '
            "&::GLAZE_EXT_GL_ARB_buffer_storage };" in hpp
        )
        assert (
            'constexpr Extension ARB_draw_instanced{ "GL_ARB_draw_instanced", '
            "&::GLAZE_EXT_GL_ARB_draw_instanced };" in hpp
        )

    def test_extension_token_per_extension_guard(self, mini_registry: Registry) -> None:
        gen = self._gen(mini_registry)
        files = gen.generate("gl", "4.5")
        hpp = files["include/glaze/gl.hpp"]
        # Each constexpr token sits inside its own #ifndef GLAZE_GL_NO_EXT_<short>
        target = "constexpr Extension ARB_draw_instanced"
        idx = hpp.find(target)
        assert idx != -1
        line_start = hpp.rfind("\n", 0, idx) + 1
        prev_line_end = line_start - 1
        prev_line_start = hpp.rfind("\n", 0, prev_line_end) + 1
        prev_line = hpp[prev_line_start:prev_line_end]
        assert prev_line == "#ifndef GLAZE_GL_NO_EXT_ARB_draw_instanced"

    def test_supports_overloads_present(self, mini_registry: Registry) -> None:
        gen = self._gen(mini_registry)
        files = gen.generate("gl", "4.5")
        hpp = files["include/glaze/gl.hpp"]
        assert "inline bool supports(const Extension &ext)" in hpp
        assert "inline bool supports(const char *name)" in hpp
        assert "inline bool supports(int major, int minor)" in hpp

    def test_extension_functor_emitted_and_guarded(self, mini_registry: Registry) -> None:
        gen = self._gen(mini_registry)
        files = gen.generate("gl", "4.5")
        hpp = files["include/glaze/gl.hpp"]
        struct_target = "struct _DrawArraysInstancedARBFn"
        idx = hpp.find(struct_target)
        assert idx != -1
        # The line directly above must be the per-extension guard.
        line_start = hpp.rfind("\n", 0, idx) + 1
        prev_line_end = line_start - 1
        prev_line_start = hpp.rfind("\n", 0, prev_line_end) + 1
        prev_line = hpp[prev_line_start:prev_line_end]
        assert prev_line == "#ifndef GLAZE_GL_NO_EXT_ARB_draw_instanced"
        # Two lines up must be the master switch.
        prev_prev_end = prev_line_start - 1
        prev_prev_start = hpp.rfind("\n", 0, prev_prev_end) + 1
        prev_prev = hpp[prev_prev_start:prev_prev_end]
        assert prev_prev == "#ifndef GLAZE_GL_NO_EXTENSIONS"

    def test_extension_runtime_inside_master_switch(self, mini_registry: Registry) -> None:
        gen = self._gen(mini_registry)
        files = gen.generate("gl", "4.5")
        hpp = files["include/glaze/gl.hpp"]
        master_idx = hpp.find("// ── Extension runtime")
        assert master_idx != -1
        # The master `#ifndef GLAZE_GL_NO_EXTENSIONS` opens immediately after.
        opener_idx = hpp.find("#ifndef GLAZE_GL_NO_EXTENSIONS", master_idx)
        closer_idx = hpp.find("#endif // GLAZE_GL_NO_EXTENSIONS", opener_idx)
        assert opener_idx != -1 and closer_idx > opener_idx
        block = hpp[opener_idx:closer_idx]
        assert "namespace exts" in block
        assert "supports(const Extension" in block

    def test_default_no_extensions(self, mini_registry: Registry) -> None:
        """With no extension filters, no extensions should be emitted."""
        gen = CppGenerator(mini_registry)
        files = gen.generate("gl", "4.5")
        hpp = files["include/glaze/gl.hpp"]
        assert "GL_ARB_buffer_storage" not in hpp
        assert "GL_ARB_draw_instanced" not in hpp
        assert "GL_NV_shader_buffer_load" not in hpp

    def test_vendor_filter_includes_only_matching(self, mini_registry: Registry) -> None:
        gen = CppGenerator(mini_registry, extension_vendors=["NV"])
        files = gen.generate("gl", "4.5")
        hpp = files["include/glaze/gl.hpp"]
        assert "GL_NV_shader_buffer_load" in hpp
        assert "GL_ARB_buffer_storage" not in hpp

    def test_vendor_and_name_filters_are_additive(self, mini_registry: Registry) -> None:
        gen = CppGenerator(mini_registry, extension_vendors=["ARB"], extension_names=["GL_KHR_debug"])
        files = gen.generate("gl", "4.5")
        hpp = files["include/glaze/gl.hpp"]
        assert "GL_ARB_buffer_storage" in hpp
        assert "GL_KHR_debug" in hpp
        assert "GL_NV_shader_buffer_load" not in hpp


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
        # glCreateProgram should be wrapped to return Program, not GLuint
        assert "Program" in content
        assert "return Program(glCreateProgram(" in content

    def test_create_shader_returns_handle(self, mini_registry: Registry) -> None:
        gen = CppGenerator(mini_registry)
        files = gen.generate("gl", "2.0")
        content = next(iter(files.values()))
        assert "Shader" in content
        assert "return Shader(glCreateShader(" in content


class TestCppGeneratorRaii:
    def test_generator_no_longer_emits_raii_header(self, mini_registry: Registry) -> None:
        # The static glaze/raii.hpp ships via Conan; it is not generated.
        files = CppGenerator(mini_registry).generate("gl", "2.0")
        assert "include/glaze/gl_raii.hpp" not in files
        assert "include/glaze/gl.hpp" in files

    def test_gl_hpp_includes_static_raii_header(self, mini_registry: Registry) -> None:
        files = CppGenerator(mini_registry).generate("gl", "1.0")
        gl_hpp = files["include/glaze/gl.hpp"]
        assert '#include "raii.hpp"' in gl_hpp

    def test_gl_hpp_emits_glaze_traits_for_raw_handles(self, mini_registry: Registry) -> None:
        # gl 1.5 has glGenBuffers/glDeleteBuffers → buffer handle gets a raw trait.
        # glGenBuffers has only (count, output*) — no leading params — so the
        # Traits::create(...) signature must be nullary. Explicitly pin the
        # zero-arg signature so a future regression that over-propagates
        # params here would be caught.
        files = CppGenerator(mini_registry).generate("gl", "1.5")
        gl_hpp = files["include/glaze/gl.hpp"]
        assert "namespace glaze" in gl_hpp
        assert "struct Traits<::gl::Buffer>" in gl_hpp
        assert "static ::gl::Buffer create() {" in gl_hpp
        assert "::gl::genBuffer()" in gl_hpp
        assert "::gl::deleteBuffer(h)" in gl_hpp

    def test_gl_hpp_emits_glaze_traits_for_program_handle(self, mini_registry: Registry) -> None:
        # gl 2.0 introduces glCreateProgram/glDeleteProgram → program handle trait.
        files = CppGenerator(mini_registry).generate("gl", "2.0")
        gl_hpp = files["include/glaze/gl.hpp"]
        assert "struct Traits<::gl::Program>" in gl_hpp
        assert "static ::gl::Program create() {" in gl_hpp
        assert "::gl::createProgram()" in gl_hpp
        assert "::gl::deleteProgram(h)" in gl_hpp

    def test_gl_hpp_query_traits_propagates_create_target(
        self, mini_registry: Registry
    ) -> None:
        # glCreateQueries(target, n, ids) is a multi-object creator whose
        # singular wrapper _CreateQuerieFn::operator()(QueryTarget) keeps the
        # leading target parameter. The Traits specialization must propagate
        # that parameter into create(...) — otherwise Traits<gl::Query>::create()
        # calls a functor that requires arguments and compilation fails with
        # C2064 "term does not evaluate to a function taking 0 arguments".
        files = CppGenerator(mini_registry).generate("gl", "4.5")
        gl_hpp = files["include/glaze/gl.hpp"]
        # Isolate the Traits<::gl::Query> block so nothing elsewhere in the
        # file (e.g. glBindBuffer using static_cast<GLenum>(target) for an
        # unrelated enum param) can accidentally satisfy the assertions.
        start = gl_hpp.index("struct Traits<::gl::Query>")
        end = gl_hpp.index("};", start)
        block = gl_hpp[start:end]
        assert "static ::gl::Query create(::gl::QueryTarget target) {" in block
        # Body must forward target verbatim to the C++ functor — no
        # static_cast<GLenum>(target), which would fail to match the
        # functor's QueryTarget operator().
        assert "::gl::createQuerie(target)" in block
        assert "::gl::createQuerie(static_cast" not in block

    def test_gl_hpp_shader_traits_propagates_create_param(
        self, mini_registry: Registry
    ) -> None:
        # glCreateShader takes a mandatory GLenum type parameter. The Traits
        # specialization must propagate it into create(...) and forward it to
        # the underlying functor, otherwise the generated header fails to
        # compile as soon as Traits<gl::Shader>::create is instantiated.
        files = CppGenerator(mini_registry).generate("gl", "2.0")
        gl_hpp = files["include/glaze/gl.hpp"]
        assert "struct Traits<::gl::Shader>" in gl_hpp
        assert "static ::gl::Shader create(" in gl_hpp
        assert " type)" in gl_hpp
        # The body calls the C++ functor, which already accepts the strong
        # type used in the signature, so the parameter must be forwarded
        # verbatim without any static_cast — otherwise the functor would
        # refuse the raw GLenum.
        assert "::gl::createShader(type)" in gl_hpp
        assert "static_cast<GLenum>(type)" not in gl_hpp

    def test_gl_hpp_no_glaze_traits_in_1_0(self, mini_registry: Registry) -> None:
        files = CppGenerator(mini_registry).generate("gl", "1.0")
        gl_hpp = files["include/glaze/gl.hpp"]
        # MINI_XML has no creator/deleter pair at GL 1.0 → no Traits specializations,
        # only the empty `namespace glaze {}` block.
        assert "struct Traits<" not in gl_hpp

    def test_gl_handle_hpp_emits_glaze_traits_for_legacy_wrappers(
        self, mini_registry: Registry
    ) -> None:
        files = CppGenerator(mini_registry).generate("gl", "2.0")
        h = files["include/glaze/gl_handle.hpp"]
        # gl::handle::Program / Shader are legacy wrapper classes — each gets a Traits.
        assert "namespace glaze" in h
        assert "struct Traits<::gl::handle::Program>" in h
        assert "static ::gl::handle::Program create() {" in h
        assert "::gl::handle::Program(::gl::createProgram())" in h
        assert "::gl::deleteProgram(w.id())" in h
        # gl::handle::Shader's Traits must propagate glCreateShader's GLenum type
        # parameter through create(...) — see the matching assertion for the raw
        # handle trait in test_gl_hpp_shader_traits_propagates_create_param.
        assert "struct Traits<::gl::handle::Shader>" in h
        assert "static ::gl::handle::Shader create(" in h
        assert " type)" in h
        assert "::gl::handle::Shader(::gl::createShader(type))" in h

    def test_static_raii_hpp_shipped_verbatim(self, mini_registry: Registry) -> None:
        files = CppGenerator(mini_registry).generate("gl", "1.0")
        raii = files["include/glaze/raii.hpp"]
        # Sanity-check the static header's namespace, customization point, and templates.
        assert "namespace glaze" in raii
        assert "struct Traits;" in raii
        assert "class Unique" in raii
        assert "class Shared" in raii
        assert "class Weak" in raii
        # The factory helpers are variadic so they can forward arguments to
        # Traits<T>::create(...) for types whose creator requires parameters
        # (e.g. gl::Shader).
        assert "Unique<T> makeUnique(Args&&... args)" in raii
        assert "Shared<T> makeShared(Args&&... args)" in raii
        assert "Traits<T>::create(std::forward<Args>(args)...)" in raii

    def test_gl_hpp_emits_handle_traits_for_buffer(self, mini_registry: Registry) -> None:
        files = CppGenerator(mini_registry).generate("gl", "1.5")
        gl_hpp = files["include/glaze/gl.hpp"]
        assert "template<typename H> struct HandleTraits;" in gl_hpp
        # In MINI_XML the buffer handle is just "Buffer" (no collision).
        assert "struct HandleTraits<Buffer>" in gl_hpp
        assert "using creator = _GenBufferFn;" in gl_hpp
        assert "using deleter = _DeleteBufferFn;" in gl_hpp

    def test_gl_hpp_emits_handle_traits_for_program(self, mini_registry: Registry) -> None:
        files = CppGenerator(mini_registry).generate("gl", "2.0")
        gl_hpp = files["include/glaze/gl.hpp"]
        assert "struct HandleTraits<Program>" in gl_hpp
        assert "using creator = _CreateProgramFn;" in gl_hpp
        assert "using deleter = _DeleteProgramFn;" in gl_hpp

    def test_gl_hpp_no_handle_traits_in_1_0(self, mini_registry: Registry) -> None:
        files = CppGenerator(mini_registry).generate("gl", "1.0")
        gl_hpp = files["include/glaze/gl.hpp"]
        # No resources at GL 1.0 → traits block omitted
        assert "HandleTraits" not in gl_hpp


class TestCppGeneratorHandle:
    def test_generate_emits_handle_header(self, mini_registry: Registry) -> None:
        files = CppGenerator(mini_registry).generate("gl", "2.0")
        assert "include/glaze/gl_handle.hpp" in files

    def test_handle_header_uses_handle_namespace(self, mini_registry: Registry) -> None:
        h = CppGenerator(mini_registry).generate("gl", "2.0")["include/glaze/gl_handle.hpp"]
        assert "namespace handle" in h
        assert '#include "gl.hpp"' in h

    def test_program_class_methods(self, mini_registry: Registry) -> None:
        h = CppGenerator(mini_registry).generate("gl", "2.0")["include/glaze/gl_handle.hpp"]
        assert "class Program {" in h
        assert "link()" in h
        assert "attachShader(" in h
        assert "getUniformLocation(" in h
        assert "delete_()" in h  # glDeleteProgram (keyword collision)

    def test_handle_excludes_named_dsa_commands(self, mini_registry: Registry) -> None:
        h = CppGenerator(mini_registry).generate("gl", "4.5")["include/glaze/gl_handle.hpp"]
        # glNamedBufferData/SubData live in dsa::, not handle::.
        assert "::glNamedBufferData" not in h
        assert "::glNamedBufferSubData" not in h

    def test_handle_delegates_to_functor(self, mini_registry: Registry) -> None:
        h = CppGenerator(mini_registry).generate("gl", "2.0")["include/glaze/gl_handle.hpp"]
        # Method body delegates to the gl:: functor instance, not ::glXxx() directly,
        # so it picks up the strong return-type wrapping for free.
        assert "::gl::linkProgram(m_id)" in h
        assert "::glLinkProgram(" not in h

    def test_handle_returns_strong_uniform_location(self, mini_registry: Registry) -> None:
        h = CppGenerator(mini_registry).generate("gl", "2.0")["include/glaze/gl_handle.hpp"]
        # glGetUniformLocation returns UniformLocation (strong typedef), not GLint.
        assert "UniformLocation getUniformLocation(" in h

    def test_handle_classes_absent_in_1_0(self, mini_registry: Registry) -> None:
        # GL 1.0 has no first-param-handle commands in MINI_XML.
        h = CppGenerator(mini_registry).generate("gl", "1.0")["include/glaze/gl_handle.hpp"]
        assert "class Program" not in h


class TestCppGeneratorScalarQueryOverload:
    """glGet*iv-style per-object scalar queries get a value-returning overload."""

    def test_free_functor_returns_glint(self, mini_registry: Registry) -> None:
        # glGetShaderiv isn't in mini_registry — exercise via a minimal
        # synthetic command glued on top for coverage. Instead, rely on the
        # production gl.xml output being exercised by the regen tests elsewhere
        # and just pin the helper directly here.
        from glaze.generators.cpp_generator import _find_scalar_query_param

        cmd = Command(
            name="glGetShaderiv",
            return_type=TypeDecl("void"),
            return_type_str="void",
            params=[
                CommandParam(name="shader", type_parts=["GLuint"], class_="shader"),
                CommandParam(name="pname", type_parts=["GLenum"]),
                CommandParam(name="params", type_parts=["GLint", "*"]),
            ],
        )
        scalar = _find_scalar_query_param(cmd)
        assert scalar is not None
        assert scalar.name == "params"

    def test_non_handle_first_param_skipped(self) -> None:
        """glGetIntegerv-style multi-value queries must not get a scalar overload."""
        from glaze.generators.cpp_generator import _find_scalar_query_param

        cmd = Command(
            name="glGetIntegerv",
            return_type=TypeDecl("void"),
            return_type_str="void",
            params=[
                CommandParam(name="pname", type_parts=["GLenum"]),
                CommandParam(name="data", type_parts=["GLint", "*"]),
            ],
        )
        assert _find_scalar_query_param(cmd) is None

    def test_non_get_command_skipped(self) -> None:
        from glaze.generators.cpp_generator import _find_scalar_query_param

        cmd = Command(
            name="glBufferData",
            return_type=TypeDecl("void"),
            return_type_str="void",
            params=[
                CommandParam(name="buffer", type_parts=["GLuint"], class_="buffer"),
                CommandParam(name="out", type_parts=["GLint", "*"]),
            ],
        )
        assert _find_scalar_query_param(cmd) is None


class TestCppGeneratorHandleCrossReferences:
    """handle::Program::attachShader should take handle::Shader, not gl::Shader."""

    def test_attach_shader_param_is_handle_wrapper(self, mini_registry: Registry) -> None:
        h = CppGenerator(mini_registry).generate("gl", "2.0")["include/glaze/gl_handle.hpp"]
        # Param type must be ::gl::handle::Shader, not bare ::gl::Shader.
        assert "attachShader(::gl::handle::Shader shader)" in h

    def test_attach_shader_call_forwards_id(self, mini_registry: Registry) -> None:
        h = CppGenerator(mini_registry).generate("gl", "2.0")["include/glaze/gl_handle.hpp"]
        # The wrapper unwraps the handle via .id() before handing it to the
        # free functor, which still expects the raw handle-id type.
        assert "::gl::attachShader(m_id, shader.id())" in h


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
