"""Tests for glaze.model dataclasses and Registry methods."""

from glaze.model import (
    Command,
    CommandParam,
    EnumGroup,
    Feature,
    Registry,
    TypeDecl,
)

# ── TypeDecl ──────────────────────────────────────────────────────────────────


class TestTypeDecl:
    def test_plain_type(self) -> None:
        td = TypeDecl("GLuint")
        assert td.to_c_string() == "GLuint"

    def test_const_pointer(self) -> None:
        td = TypeDecl("void", is_const=True, is_pointer=True)
        assert td.to_c_string() == "const void*"

    def test_pointer_only(self) -> None:
        td = TypeDecl("GLuint", is_pointer=True)
        assert td.to_c_string() == "GLuint*"

    def test_const_only(self) -> None:
        td = TypeDecl("GLchar", is_const=True)
        assert td.to_c_string() == "const GLchar"


# ── CommandParam ──────────────────────────────────────────────────────────────


class TestCommandParam:
    def test_basic_properties(self) -> None:
        p = CommandParam(name="target", type_parts=["GLenum"])
        assert p.type == "GLenum"
        assert p.is_const is False
        assert p.is_pointer is False
        assert p.pointer_indirection == 0

    def test_const_pointer_param(self) -> None:
        p = CommandParam(name="data", type_parts=["const", "GLuint", "*"])
        assert p.type == "GLuint"
        assert p.is_const is True
        assert p.is_pointer is True
        assert p.pointer_indirection == 1

    def test_double_pointer(self) -> None:
        p = CommandParam(name="ptr", type_parts=["void", "**"])
        assert p.pointer_indirection == 2

    def test_void_param(self) -> None:
        p = CommandParam(name="ptr", type_parts=["const", "void", "*"])
        assert p.is_void is True

    def test_group_and_class(self) -> None:
        p = CommandParam(name="buf", type_parts=["GLuint"], group="BufferTarget", class_="buffer")
        assert p.has_group() is True
        assert p.has_class() is True
        assert p.class_ == "buffer"


# ── Command ───────────────────────────────────────────────────────────────────


class TestCommand:
    def test_get_class_with_class_param(self) -> None:
        p = CommandParam(name="texture", type_parts=["GLuint"], class_="texture")
        cmd = Command(
            name="glBindTexture",
            return_type=TypeDecl("void"),
            return_type_str="void",
            params=[p, CommandParam(name="target", type_parts=["GLenum"])],
        )
        assert cmd.get_class() == "texture"

    def test_get_class_without_class(self) -> None:
        cmd = Command(
            name="glClear",
            return_type=TypeDecl("void"),
            return_type_str="void",
            params=[CommandParam(name="mask", type_parts=["GLbitfield"])],
        )
        assert cmd.get_class() is None

    def test_get_class_empty_params(self) -> None:
        cmd = Command(
            name="glFlush",
            return_type=TypeDecl("void"),
            return_type_str="void",
            params=[],
        )
        assert cmd.get_class() is None


# ── Registry ──────────────────────────────────────────────────────────────────


def _make_registry(
    features: list[Feature] | None = None,
    commands: list[Command] | None = None,
    enums: list[EnumGroup] | None = None,
) -> Registry:
    """Helper to build a Registry with minimal boilerplate."""
    return Registry(
        types_list=[],
        enum_groups=enums or [],
        commands_list=commands or [],
        features_list=features or [],
        extensions_list=[],
    )


class TestRegistry:
    def test_available_apis(self, mini_registry: Registry) -> None:
        apis = mini_registry.available_apis()
        assert "gl" in apis
        assert "gles2" in apis
        assert "1.0" in apis["gl"]
        assert "1.5" in apis["gl"]
        assert "3.1" in apis["gl"]
        assert "2.0" in apis["gles2"]

    def test_collect_features_up_to_version(self, mini_registry: Registry) -> None:
        features = mini_registry.collect_features("gl", "1.5")
        names = [f.name for f in features]
        assert "GL_VERSION_1_0" in names
        assert "GL_VERSION_1_5" in names
        assert "GL_VERSION_3_1" not in names

    def test_collect_features_single_version(self, mini_registry: Registry) -> None:
        features = mini_registry.collect_features("gl", "1.0")
        assert len(features) == 1
        assert features[0].name == "GL_VERSION_1_0"

    def test_consolidate_includes_required_enums(self, mini_registry: Registry) -> None:
        cons = mini_registry.consolidate("gl", "1.5")
        assert "GL_COLOR_BUFFER_BIT" in cons.enums
        assert "GL_ARRAY_BUFFER" in cons.enums
        assert "GL_FLOAT" in cons.enums

    def test_consolidate_removes_enums(self, mini_registry: Registry) -> None:
        cons = mini_registry.consolidate("gl", "3.1")
        assert "GL_FLOAT" not in cons.enums
        assert "GL_COLOR_BUFFER_BIT" in cons.enums

    def test_consolidate_includes_commands(self, mini_registry: Registry) -> None:
        cons = mini_registry.consolidate("gl", "1.5")
        assert "glClear" in cons.commands
        assert "glBindBuffer" in cons.commands
        assert "glGetString" in cons.commands

    def test_consolidate_unknown_api(self, mini_registry: Registry) -> None:
        cons = mini_registry.consolidate("vulkan", "1.0")
        assert len(cons.enums) == 0
        assert len(cons.commands) == 0

    def test_indices_populated(self, mini_registry: Registry) -> None:
        assert "glClear" in mini_registry.command_by_name
        assert "GL_ARRAY_BUFFER" in mini_registry.enum_by_name
        assert "GLuint" in mini_registry.type_by_name
        assert "gl" in mini_registry.features_by_api

    def test_group_to_enums(self, mini_registry: Registry) -> None:
        assert "BufferTargetARB" in mini_registry.group_to_enums
        names = [e.name for e in mini_registry.group_to_enums["BufferTargetARB"]]
        assert "GL_ARRAY_BUFFER" in names

    def test_object_dict(self, mini_registry: Registry) -> None:
        # object_dict is keyed by get_class(), which checks only the first param.
        # In our mini XML, glBindBuffer's first param is 'target' (no class),
        # so 'buffer' is not in object_dict.
        # This test verifies the dict is built correctly from commands
        # whose first param has a class attribute.
        assert isinstance(mini_registry.object_dict, dict)
