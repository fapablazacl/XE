from dataclasses import dataclass, field
from typing import List, Optional, Dict


@dataclass
class TypeDecl:
    name: str
    is_const: bool = False
    is_pointer: bool = False

    def to_c_string(self) -> str:
        return f'{"const " if self.is_const else ""}{self.name}{"*" if self.is_pointer else ""}'


@dataclass
class Type:
    name: str
    c_definition: str
    requires: Optional[str] = None
    comment: Optional[str] = None


@dataclass
class Enum:
    name: str
    value: str
    groups: List[str] = field(default_factory=list)
    alias: Optional[str] = None
    comment: Optional[str] = None


@dataclass
class EnumGroup:
    namespace: str
    group: Optional[str]
    enum_group_type: Optional[str]
    enums: Dict[str, 'Enum'] = field(default_factory=dict)
    vendor: Optional[str] = None
    comment: Optional[str] = None


@dataclass
class CommandParam:
    name: str
    type_parts: List[str]
    group: Optional[str] = None
    class_: Optional[str] = None
    len: Optional[str] = None

    @property
    def type(self) -> Optional[str]:
        """Base GL type name extracted from type_parts (e.g., 'GLuint', 'GLenum')."""
        for part in self.type_parts:
            if part and part not in ('const', '*', '**', 'void'):
                return part
        return None

    @property
    def data_type(self) -> Optional[str]:
        """Alias for type, used by C generator."""
        return self.type

    @property
    def pointer_indirection(self) -> int:
        return sum(part.count('*') for part in self.type_parts)

    @property
    def is_const(self) -> bool:
        return 'const' in self.type_parts

    @property
    def is_pointer(self) -> bool:
        return self.pointer_indirection > 0

    @property
    def is_void(self) -> bool:
        return 'void' in self.type_parts

    def has_group(self) -> bool:
        return bool(self.group)

    def has_class(self) -> bool:
        return self.class_ is not None


@dataclass
class Command:
    name: str
    return_type: TypeDecl
    return_type_str: str
    params: List[CommandParam]
    namespace: str = "GL"
    group: Optional[str] = None

    def get_class(self) -> Optional[str]:
        if self.params and self.params[0].has_class():
            return self.params[0].class_
        return None


@dataclass
class TypeRef:
    name: str
    comment: Optional[str] = None


@dataclass
class EnumRef:
    name: str
    comment: Optional[str] = None


@dataclass
class CommandRef:
    name: str
    comment: Optional[str] = None


@dataclass
class Require:
    types: List[TypeRef] = field(default_factory=list)
    enums: List[EnumRef] = field(default_factory=list)
    commands: List[CommandRef] = field(default_factory=list)


@dataclass
class Remove:
    profile: str
    comment: Optional[str] = None
    types: List[TypeRef] = field(default_factory=list)
    enums: List[EnumRef] = field(default_factory=list)
    commands: List[CommandRef] = field(default_factory=list)


@dataclass
class Feature:
    api: str
    name: str
    number: str
    require_list: List[Require] = field(default_factory=list)
    remove_list: List[Remove] = field(default_factory=list)


@dataclass
class Extension:
    name: str
    supported: List[str] = field(default_factory=list)
    require_list: List[Require] = field(default_factory=list)


class ConsolidatedRequire:
    """Flat sets of enum/command names for a specific API version."""

    def __init__(self, enums: set, commands: set):
        self.enums = enums
        self.commands = commands


class Registry:
    def __init__(
        self,
        types_list: List[Type],
        enum_groups: List[EnumGroup],
        commands_list: List[Command],
        features_list: List[Feature],
        extensions_list: List[Extension],
    ):
        self.types_list = types_list
        self.enum_groups = enum_groups
        self.commands_list = commands_list
        self.features_list = features_list
        self.extensions_list = extensions_list
        self._build_indices()

    def _build_indices(self):
        self.type_by_name: Dict[str, Type] = {}
        for t in self.types_list:
            self.type_by_name[t.name] = t

        self.enum_by_name: Dict[str, Enum] = {}
        for eg in self.enum_groups:
            for name, enum in eg.enums.items():
                self.enum_by_name[name] = enum

        self.command_by_name: Dict[str, Command] = {}
        for cmd in self.commands_list:
            self.command_by_name[cmd.name] = cmd

        # Maps group name → list of Enum objects (used by C++ generator)
        self.group_to_enums: Dict[str, List[Enum]] = {}
        for eg in self.enum_groups:
            for enum in eg.enums.values():
                for group in enum.groups:
                    if group:
                        if group not in self.group_to_enums:
                            self.group_to_enums[group] = []
                        self.group_to_enums[group].append(enum)

        self.features_by_api: Dict[str, List[Feature]] = {}
        for feat in self.features_list:
            if feat.api not in self.features_by_api:
                self.features_by_api[feat.api] = []
            self.features_by_api[feat.api].append(feat)

        # Maps class name → list of commands (used by C++ generator)
        self.object_dict: Dict[str, List[Command]] = {}
        for cmd in self.commands_list:
            cls = cmd.get_class()
            if cls:
                if cls not in self.object_dict:
                    self.object_dict[cls] = []
                self.object_dict[cls].append(cmd)

    def available_apis(self) -> Dict[str, List[str]]:
        """Returns a dict of api → sorted list of version numbers."""
        result: Dict[str, List[str]] = {}
        for api, features in self.features_by_api.items():
            result[api] = sorted([f.number for f in features])
        return result

    def consolidate(self, api: str, number: str) -> ConsolidatedRequire:
        """Flatten all features up to `number` for `api` into sets of names."""
        features = self.features_by_api.get(api, [])

        enum_names: set = set()
        command_names: set = set()

        for feature in features:
            if feature.number > number:
                continue
            for require in feature.require_list:
                for ref in require.enums:
                    enum_names.add(ref.name)
                for ref in require.commands:
                    command_names.add(ref.name)

        for feature in features:
            if feature.number > number:
                continue
            for remove in feature.remove_list:
                for ref in remove.enums:
                    enum_names.discard(ref.name)
                for ref in remove.commands:
                    command_names.discard(ref.name)

        return ConsolidatedRequire(enums=enum_names, commands=command_names)

    def collect_features(self, api: str, number: str) -> List[Feature]:
        """Return features for `api` up to and including `number`, in order."""
        features = self.features_by_api.get(api, [])
        result = []
        for feature in features:
            if feature.number <= number:
                result.append(feature)
        return result
