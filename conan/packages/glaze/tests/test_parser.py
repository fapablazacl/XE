"""Tests for glaze.parser.RegistryParser."""

import xml.dom.minidom

import pytest

from glaze.model import Registry
from glaze.parser import RegistryParser


def _parse_snippet(xml_str: str) -> Registry:
    """Wrap xml_str in <registry> and parse it."""
    doc = xml.dom.minidom.parseString(f"<registry>{xml_str}</registry>")
    return RegistryParser().parse(doc.documentElement)


class TestParseTypes:
    def test_parse_named_type(self) -> None:
        reg = _parse_snippet("""
        <types>
            <type>typedef unsigned int <name>GLuint</name>;</type>
        </types>
        """)
        assert len(reg.types_list) == 1
        assert reg.types_list[0].name == "GLuint"
        assert "unsigned int" in reg.types_list[0].c_definition

    def test_parse_type_with_requires(self) -> None:
        reg = _parse_snippet("""
        <types>
            <type requires="khrplatform" name="GLint">typedef khronos_int32_t GLint;</type>
        </types>
        """)
        assert len(reg.types_list) == 1
        assert reg.types_list[0].name == "GLint"
        assert reg.types_list[0].requires == "khrplatform"

    def test_empty_types(self) -> None:
        reg = _parse_snippet("<types></types>")
        assert len(reg.types_list) == 0

    def test_no_types_node(self) -> None:
        reg = _parse_snippet("")
        assert len(reg.types_list) == 0


class TestParseEnums:
    def test_parse_enum_group(self) -> None:
        reg = _parse_snippet("""
        <enums namespace="GL" group="BufferTarget" type="enum">
            <enum value="0x8892" name="GL_ARRAY_BUFFER" group="BufferTarget"/>
            <enum value="0x8893" name="GL_ELEMENT_ARRAY_BUFFER" group="BufferTarget"/>
        </enums>
        """)
        assert len(reg.enum_groups) == 1
        eg = reg.enum_groups[0]
        assert eg.namespace == "GL"
        assert eg.group == "BufferTarget"
        assert "GL_ARRAY_BUFFER" in eg.enums
        assert eg.enums["GL_ARRAY_BUFFER"].value == "0x8892"

    def test_enum_groups_populated(self) -> None:
        reg = _parse_snippet("""
        <enums namespace="GL" group="BufferTarget" type="enum">
            <enum value="0x8892" name="GL_ARRAY_BUFFER" group="BufferTarget"/>
        </enums>
        """)
        enum = reg.enum_by_name["GL_ARRAY_BUFFER"]
        assert "BufferTarget" in enum.groups

    def test_enum_with_alias(self) -> None:
        reg = _parse_snippet("""
        <enums namespace="GL">
            <enum value="0x8892" name="GL_ARRAY_BUFFER_ARB" alias="GL_ARRAY_BUFFER"/>
        </enums>
        """)
        enum = reg.enum_by_name["GL_ARRAY_BUFFER_ARB"]
        assert enum.alias == "GL_ARRAY_BUFFER"


class TestParseCommands:
    def test_parse_simple_command(self) -> None:
        reg = _parse_snippet("""
        <commands namespace="GL">
            <command>
                <proto>void <name>glClear</name></proto>
                <param><ptype>GLbitfield</ptype> <name>mask</name></param>
            </command>
        </commands>
        """)
        assert len(reg.commands_list) == 1
        cmd = reg.commands_list[0]
        assert cmd.name == "glClear"
        assert cmd.return_type.name == "void"
        assert len(cmd.params) == 1
        assert cmd.params[0].name == "mask"
        assert cmd.params[0].type == "GLbitfield"

    def test_parse_command_with_return_type(self) -> None:
        reg = _parse_snippet("""
        <commands namespace="GL">
            <command>
                <proto>const <ptype>GLubyte</ptype> *<name>glGetString</name></proto>
                <param><ptype>GLenum</ptype> <name>name</name></param>
            </command>
        </commands>
        """)
        cmd = reg.commands_list[0]
        assert cmd.name == "glGetString"
        assert cmd.return_type.is_const is True
        assert cmd.return_type.is_pointer is True
        assert cmd.return_type.name == "GLubyte"

    def test_parse_command_with_class(self) -> None:
        reg = _parse_snippet("""
        <commands namespace="GL">
            <command>
                <proto>void <name>glBindTexture</name></proto>
                <param><ptype>GLenum</ptype> <name>target</name></param>
                <param class="texture"><ptype>GLuint</ptype> <name>texture</name></param>
            </command>
        </commands>
        """)
        cmd = reg.commands_list[0]
        assert cmd.params[1].class_ == "texture"
        assert cmd.get_class() is None  # first param has no class
        # But if first param had class, it would work
        assert cmd.params[1].has_class() is True


class TestParseFeatures:
    def test_parse_feature_with_require(self) -> None:
        reg = _parse_snippet("""
        <commands namespace="GL">
            <command>
                <proto>void <name>glClear</name></proto>
                <param><ptype>GLbitfield</ptype> <name>mask</name></param>
            </command>
        </commands>
        <enums namespace="GL">
            <enum value="0x00004000" name="GL_COLOR_BUFFER_BIT"/>
        </enums>
        <feature api="gl" name="GL_VERSION_1_0" number="1.0">
            <require>
                <enum name="GL_COLOR_BUFFER_BIT"/>
                <command name="glClear"/>
            </require>
        </feature>
        """)
        assert len(reg.features_list) == 1
        feat = reg.features_list[0]
        assert feat.api == "gl"
        assert feat.number == "1.0"
        assert len(feat.require_list) == 1
        assert feat.require_list[0].enums[0].name == "GL_COLOR_BUFFER_BIT"
        assert feat.require_list[0].commands[0].name == "glClear"

    def test_parse_feature_with_remove(self) -> None:
        reg = _parse_snippet("""
        <feature api="gl" name="GL_VERSION_3_1" number="3.1">
            <remove profile="core">
                <enum name="GL_FLOAT"/>
            </remove>
        </feature>
        """)
        feat = reg.features_list[0]
        assert len(feat.remove_list) == 1
        assert feat.remove_list[0].profile == "core"
        assert feat.remove_list[0].enums[0].name == "GL_FLOAT"


class TestParseExtensions:
    def test_parse_extension(self) -> None:
        reg = _parse_snippet("""
        <extensions>
            <extension name="GL_ARB_buffer_storage" supported="gl|gles2">
                <require>
                    <enum name="GL_ARRAY_BUFFER"/>
                </require>
            </extension>
        </extensions>
        """)
        assert len(reg.extensions_list) == 1
        ext = reg.extensions_list[0]
        assert ext.name == "GL_ARB_buffer_storage"
        assert ext.supported == ["gl", "gles2"]
        assert ext.require_list[0].enums[0].name == "GL_ARRAY_BUFFER"

    def test_no_extensions(self) -> None:
        reg = _parse_snippet("")
        assert len(reg.extensions_list) == 0


class TestParseFullMiniRegistry:
    def test_indices_populated(self, mini_registry: Registry) -> None:
        assert "glClear" in mini_registry.command_by_name
        assert "glBindBuffer" in mini_registry.command_by_name
        assert "glGetString" in mini_registry.command_by_name
        assert "GL_ARRAY_BUFFER" in mini_registry.enum_by_name
        assert "GLuint" in mini_registry.type_by_name
        assert "gl" in mini_registry.features_by_api
        assert "gles2" in mini_registry.features_by_api

    def test_features_count(self, mini_registry: Registry) -> None:
        assert len(mini_registry.features_by_api["gl"]) == 5
        assert len(mini_registry.features_by_api["gles2"]) == 1

    def test_extensions_count(self, mini_registry: Registry) -> None:
        assert len(mini_registry.extensions_list) == 4


class TestParseErrors:
    def test_invalid_root_raises(self) -> None:
        doc = xml.dom.minidom.parseString("<notregistry/>")
        with pytest.raises(ValueError, match="Expected <registry> root node"):
            RegistryParser().parse(doc.documentElement)
