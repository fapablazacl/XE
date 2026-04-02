import xml.dom.minidom
from xml.dom.minidom import Node

from glaze.model import (
    Command,
    CommandParam,
    CommandRef,
    Enum,
    EnumGroup,
    EnumRef,
    Extension,
    Feature,
    Registry,
    Remove,
    Require,
    Type,
    TypeDecl,
    TypeRef,
)


class RegistryParser:
    """Unified parser for the Khronos OpenGL XML registry.

    Combines the parsing logic of both legacy pipelines into a single pass
    that produces a Registry consumed by all generators.
    """

    def parse_file(self, path: str) -> Registry:
        tree = xml.dom.minidom.parse(path)
        return self.parse(tree.documentElement)

    def parse(self, root: Node) -> Registry:
        if root.nodeType != Node.ELEMENT_NODE or root.tagName != "registry":
            raise ValueError("Expected <registry> root node")

        return Registry(
            types_list=self._parse_types(root),
            enum_groups=self._parse_enum_groups(root),
            commands_list=self._parse_commands(root),
            features_list=self._parse_features(root),
            extensions_list=self._parse_extensions(root),
        )

    # ------------------------------------------------------------------ types

    def _parse_types(self, root: Node) -> list[Type]:
        types_node = self._find_child(root, "types")
        if types_node is None:
            return []

        result = []
        for child in types_node.childNodes:
            if child.nodeType != Node.ELEMENT_NODE:
                continue
            t = self._parse_type(child)
            if t is not None:
                result.append(t)
        return result

    def _parse_type(self, node: Node) -> Type | None:
        name = self._type_name_from_child(node) or self._attr(node, "name")
        if not name:
            return None

        requires = self._attr(node, "requires") or None
        comment = self._attr(node, "comment") or None
        c_definition = self._collect_text(node)

        return Type(name=name, c_definition=c_definition, requires=requires, comment=comment)

    def _type_name_from_child(self, node: Node) -> str | None:
        for child in node.childNodes:
            if child.nodeType == Node.ELEMENT_NODE and child.tagName == "name":
                return child.firstChild.data.strip() if child.firstChild else None
        return None

    def _collect_text(self, node: Node) -> str:
        """Recursively concatenate all text content (used for C type definitions)."""
        result = ""
        for child in node.childNodes:
            if child.nodeType == Node.ELEMENT_NODE:
                result += self._collect_text(child)
            elif child.nodeType == Node.TEXT_NODE:
                result += child.nodeValue
        return result

    # ----------------------------------------------------------------- enums

    def _parse_enum_groups(self, root: Node) -> list[EnumGroup]:
        result = []
        for child in root.childNodes:
            if child.nodeType == Node.ELEMENT_NODE and child.tagName == "enums":
                result.append(self._parse_enum_group(child))
        return result

    def _parse_enum_group(self, node: Node) -> EnumGroup:
        enums: dict = {}
        for child in node.childNodes:
            if child.nodeType == Node.ELEMENT_NODE and child.tagName == "enum":
                enum = self._parse_enum(child)
                enums[enum.name] = enum

        group = self._attr(node, "group") or None
        enum_group_type = self._attr(node, "type") or None
        vendor = self._attr(node, "vendor") or None
        comment = self._attr(node, "comment") or None

        return EnumGroup(
            namespace=self._attr(node, "namespace"),
            group=group,
            enum_group_type=enum_group_type,
            enums=enums,
            vendor=vendor,
            comment=comment,
        )

    def _parse_enum(self, node: Node) -> Enum:
        name = self._attr(node, "name")
        value = self._attr(node, "value")
        group_attr = self._attr(node, "group")
        groups = [g.strip() for g in group_attr.split(",")] if group_attr else []
        alias = self._attr(node, "alias") or None
        comment = self._attr(node, "comment") or None
        return Enum(name=name, value=value, groups=groups, alias=alias, comment=comment)

    # --------------------------------------------------------------- commands

    def _parse_commands(self, root: Node) -> list[Command]:
        result = []
        for commands_node in root.childNodes:
            if commands_node.nodeType != Node.ELEMENT_NODE or commands_node.tagName != "commands":
                continue
            namespace = self._attr(commands_node, "namespace") or "GL"
            for child in commands_node.childNodes:
                if child.nodeType == Node.ELEMENT_NODE and child.tagName == "command":
                    result.append(self._parse_command(child, namespace))
        return result

    def _parse_command(self, node: Node, namespace: str) -> Command:
        proto = self._find_child(node, "proto")
        name = self._find_child_text(proto, "name")
        return_type, return_type_str = self._parse_return_type(proto)

        params = []
        for child in node.childNodes:
            if child.nodeType == Node.ELEMENT_NODE and child.tagName == "param":
                params.append(self._parse_param(child))

        return Command(
            name=name,
            return_type=return_type,
            return_type_str=return_type_str,
            params=params,
            namespace=namespace,
        )

    def _parse_return_type(self, proto: Node) -> tuple[TypeDecl, str]:
        """Returns (TypeDecl, return_type_str) from a <proto> node."""
        type_parts = []

        for child in proto.childNodes:
            if child.nodeType == Node.TEXT_NODE:
                type_parts.extend(child.data.split())
            elif child.nodeType == Node.ELEMENT_NODE and child.tagName == "ptype":
                type_parts.append(child.firstChild.data.strip() if child.firstChild else "")

        return_type_str = " ".join(p for p in type_parts if p)

        is_const = "const" in type_parts
        is_pointer = "*" in type_parts or "**" in type_parts
        name = next(
            (p for p in type_parts if p and p not in ("const", "*", "**", "void")),
            "void",
        )

        return TypeDecl(name=name, is_const=is_const, is_pointer=is_pointer), return_type_str

    def _parse_param(self, node: Node) -> CommandParam:
        group = self._attr(node, "group") or None
        class_ = self._attr(node, "class") or None
        len_ = self._attr(node, "len") or None

        type_parts = []
        name = None

        for child in node.childNodes:
            if child.nodeType == Node.TEXT_NODE:
                type_parts.extend(child.data.split())
            elif child.nodeType == Node.ELEMENT_NODE:
                if child.tagName == "ptype":
                    type_parts.append(child.firstChild.data.strip() if child.firstChild else "")
                elif child.tagName == "name":
                    name = child.firstChild.data.strip() if child.firstChild else ""

        return CommandParam(name=name, type_parts=type_parts, group=group, class_=class_, len=len_)

    # --------------------------------------------------------------- features

    def _parse_features(self, root: Node) -> list[Feature]:
        result = []
        for child in root.childNodes:
            if child.nodeType == Node.ELEMENT_NODE and child.tagName == "feature":
                result.append(self._parse_feature(child))
        return result

    def _parse_feature(self, node: Node) -> Feature:
        api = self._attr(node, "api")
        name = self._attr(node, "name")
        number = self._attr(node, "number")
        require_list = []
        remove_list = []

        for child in node.childNodes:
            if child.nodeType != Node.ELEMENT_NODE:
                continue
            if child.tagName == "require":
                require_list.append(self._parse_require(child))
            elif child.tagName == "remove":
                remove_list.append(self._parse_remove(child))

        return Feature(
            api=api, name=name, number=number, require_list=require_list, remove_list=remove_list
        )

    def _parse_require(self, node: Node) -> Require:
        types, enums, commands = [], [], []
        for child in node.childNodes:
            if child.nodeType != Node.ELEMENT_NODE:
                continue
            comment = self._attr(child, "comment") or None
            if child.tagName == "type":
                types.append(TypeRef(name=self._attr(child, "name"), comment=comment))
            elif child.tagName == "enum":
                enums.append(EnumRef(name=self._attr(child, "name"), comment=comment))
            elif child.tagName == "command":
                commands.append(CommandRef(name=self._attr(child, "name"), comment=comment))
        return Require(types=types, enums=enums, commands=commands)

    def _parse_remove(self, node: Node) -> Remove:
        profile = self._attr(node, "profile")
        comment = self._attr(node, "comment") or None
        types, enums, commands = [], [], []
        for child in node.childNodes:
            if child.nodeType != Node.ELEMENT_NODE:
                continue
            c = self._attr(child, "comment") or None
            if child.tagName == "type":
                types.append(TypeRef(name=self._attr(child, "name"), comment=c))
            elif child.tagName == "enum":
                enums.append(EnumRef(name=self._attr(child, "name"), comment=c))
            elif child.tagName == "command":
                commands.append(CommandRef(name=self._attr(child, "name"), comment=c))
        return Remove(profile=profile, comment=comment, types=types, enums=enums, commands=commands)

    # ------------------------------------------------------------- extensions

    def _parse_extensions(self, root: Node) -> list[Extension]:
        extensions_node = self._find_child(root, "extensions")
        if extensions_node is None:
            return []

        result = []
        for child in extensions_node.childNodes:
            if child.nodeType == Node.ELEMENT_NODE and child.tagName == "extension":
                result.append(self._parse_extension(child))
        return result

    def _parse_extension(self, node: Node) -> Extension:
        name = self._attr(node, "name")
        supported_attr = self._attr(node, "supported")
        supported = supported_attr.split("|") if supported_attr else []
        require_list = []
        for child in node.childNodes:
            if child.nodeType == Node.ELEMENT_NODE and child.tagName == "require":
                require_list.append(self._parse_require(child))
        return Extension(name=name, supported=supported, require_list=require_list)

    # ----------------------------------------------------------------- helpers

    def _find_child(self, node: Node, tag: str) -> Node | None:
        for child in node.childNodes:
            if child.nodeType == Node.ELEMENT_NODE and child.tagName == tag:
                return child
        return None

    def _find_child_text(self, node: Node | None, tag: str) -> str | None:
        if node is None:
            return None
        child = self._find_child(node, tag)
        if child and child.firstChild:
            return child.firstChild.data.strip()
        return None

    def _attr(self, node: Node, name: str) -> str:
        return node.getAttribute(name)
