from dataclasses import dataclass, field


@dataclass
class ApiProfile:
    """Defines a virtual API profile that maps to a real registry API.

    registry_api: the real API name in gl.xml (e.g. "gl")
    skip_remove_profiles: set of profile names whose <remove> entries are ignored.
        For example, the compatibility profile skips "core" removals, keeping
        deprecated functions that the core profile strips.
    """

    registry_api: str
    skip_remove_profiles: frozenset[str] = frozenset()


# Virtual API profiles: maps virtual api name → ApiProfile.
# These APIs don't exist in gl.xml but are resolved to a real API with profile behavior.
API_PROFILES: dict[str, ApiProfile] = {
    "gl_compat": ApiProfile(registry_api="gl", skip_remove_profiles=frozenset({"core"})),
}


@dataclass
class TypeDecl:
    name: str
    is_const: bool = False
    is_pointer: bool = False

    def to_c_string(self) -> str:
        return f"{'const ' if self.is_const else ''}{self.name}{'*' if self.is_pointer else ''}"


@dataclass
class Type:
    name: str
    c_definition: str
    requires: str | None = None
    comment: str | None = None


@dataclass
class Enum:
    name: str
    value: str
    groups: list[str] = field(default_factory=list)
    alias: str | None = None
    comment: str | None = None


@dataclass
class EnumGroup:
    namespace: str
    group: str | None
    enum_group_type: str | None
    enums: dict[str, "Enum"] = field(default_factory=dict)
    vendor: str | None = None
    comment: str | None = None


@dataclass
class CommandParam:
    name: str
    type_parts: list[str]
    group: str | None = None
    class_: str | None = None
    len: str | None = None

    @property
    def type(self) -> str | None:
        """Base GL type name extracted from type_parts (e.g., 'GLuint', 'GLenum')."""
        for part in self.type_parts:
            if part and part not in ("const", "*", "**", "void"):
                return part
        return None

    @property
    def data_type(self) -> str | None:
        """Alias for type, used by C generator."""
        return self.type

    @property
    def pointer_indirection(self) -> int:
        return sum(part.count("*") for part in self.type_parts)

    @property
    def is_const(self) -> bool:
        return "const" in self.type_parts

    @property
    def is_pointer(self) -> bool:
        return self.pointer_indirection > 0

    @property
    def is_void(self) -> bool:
        return "void" in self.type_parts

    def has_group(self) -> bool:
        return bool(self.group)

    def has_class(self) -> bool:
        return self.class_ is not None


@dataclass
class Command:
    name: str
    return_type: TypeDecl
    return_type_str: str
    params: list[CommandParam]
    namespace: str = "GL"
    group: str | None = None

    def get_class(self) -> str | None:
        if self.params and self.params[0].has_class():
            return self.params[0].class_
        return None


@dataclass
class TypeRef:
    name: str
    comment: str | None = None


@dataclass
class EnumRef:
    name: str
    comment: str | None = None


@dataclass
class CommandRef:
    name: str
    comment: str | None = None


@dataclass
class Require:
    types: list[TypeRef] = field(default_factory=list)
    enums: list[EnumRef] = field(default_factory=list)
    commands: list[CommandRef] = field(default_factory=list)


@dataclass
class Remove:
    profile: str
    comment: str | None = None
    types: list[TypeRef] = field(default_factory=list)
    enums: list[EnumRef] = field(default_factory=list)
    commands: list[CommandRef] = field(default_factory=list)


@dataclass
class Feature:
    api: str
    name: str
    number: str
    require_list: list[Require] = field(default_factory=list)
    remove_list: list[Remove] = field(default_factory=list)


@dataclass
class Extension:
    name: str
    supported: list[str] = field(default_factory=list)
    require_list: list[Require] = field(default_factory=list)


class ConsolidatedRequire:
    """Flat sets of enum/command names for a specific API version."""

    def __init__(self, enums: set, commands: set):
        self.enums = enums
        self.commands = commands


class Registry:
    def __init__(
        self,
        types_list: list[Type],
        enum_groups: list[EnumGroup],
        commands_list: list[Command],
        features_list: list[Feature],
        extensions_list: list[Extension],
    ):
        self.types_list = types_list
        self.enum_groups = enum_groups
        self.commands_list = commands_list
        self.features_list = features_list
        self.extensions_list = extensions_list
        self._build_indices()

    def _build_indices(self) -> None:
        self.type_by_name: dict[str, Type] = {}
        for t in self.types_list:
            self.type_by_name[t.name] = t

        self.enum_by_name: dict[str, Enum] = {}
        for eg in self.enum_groups:
            for name, enum in eg.enums.items():
                self.enum_by_name[name] = enum

        self.command_by_name: dict[str, Command] = {}
        for cmd in self.commands_list:
            self.command_by_name[cmd.name] = cmd

        # Maps group name → list of Enum objects (used by C++ generator)
        self.group_to_enums: dict[str, list[Enum]] = {}
        for eg in self.enum_groups:
            for enum in eg.enums.values():
                for group in enum.groups:
                    if group:
                        if group not in self.group_to_enums:
                            self.group_to_enums[group] = []
                        self.group_to_enums[group].append(enum)

        self.bitmask_groups: set[str] = set()
        for eg in self.enum_groups:
            if eg.group and eg.enum_group_type == "bitmask":
                self.bitmask_groups.add(eg.group)

        self.features_by_api: dict[str, list[Feature]] = {}
        for feat in self.features_list:
            if feat.api not in self.features_by_api:
                self.features_by_api[feat.api] = []
            self.features_by_api[feat.api].append(feat)

        # Maps class name → list of commands (used by C++ generator)
        self.object_dict: dict[str, list[Command]] = {}
        for cmd in self.commands_list:
            cls = cmd.get_class()
            if cls:
                if cls not in self.object_dict:
                    self.object_dict[cls] = []
                self.object_dict[cls].append(cmd)

    def resolve_api(self, api: str) -> ApiProfile | None:
        """Resolve a possibly-virtual API name to its ApiProfile, or None for real APIs."""
        return API_PROFILES.get(api)

    def _registry_api(self, api: str) -> str:
        """Return the real registry API name for a given (possibly virtual) API."""
        profile = self.resolve_api(api)
        return profile.registry_api if profile else api

    def available_apis(self) -> dict[str, list[str]]:
        """Returns a dict of api → sorted list of version numbers.

        Includes virtual APIs defined in API_PROFILES.
        """
        result: dict[str, list[str]] = {}
        for api, features in self.features_by_api.items():
            result[api] = sorted([f.number for f in features])

        # Add virtual APIs that share the same version list as their registry API
        for alias, profile in API_PROFILES.items():
            if profile.registry_api in result:
                result[alias] = list(result[profile.registry_api])

        return result

    def consolidate(self, api: str, number: str) -> ConsolidatedRequire:
        """Flatten all features up to `number` for `api` into sets of names.

        For virtual APIs (e.g. gl_compat), removals whose profile is in
        skip_remove_profiles are ignored, preserving deprecated symbols.
        """
        profile = self.resolve_api(api)
        registry_api = profile.registry_api if profile else api
        skip_profiles = profile.skip_remove_profiles if profile else frozenset()
        features = self.features_by_api.get(registry_api, [])

        enum_names: set = set()
        command_names: set = set()

        for feature in features:
            if feature.number > number:
                continue
            for require in feature.require_list:
                for enum_ref in require.enums:
                    enum_names.add(enum_ref.name)
                for cmd_ref in require.commands:
                    command_names.add(cmd_ref.name)

        for feature in features:
            if feature.number > number:
                continue
            for remove in feature.remove_list:
                if remove.profile in skip_profiles:
                    continue
                for enum_ref in remove.enums:
                    enum_names.discard(enum_ref.name)
                for cmd_ref in remove.commands:
                    command_names.discard(cmd_ref.name)

        return ConsolidatedRequire(enums=enum_names, commands=command_names)

    def collect_features(self, api: str, number: str) -> list[Feature]:
        """Return features for `api` up to and including `number`, in order."""
        features = self.features_by_api.get(self._registry_api(api), [])
        result = []
        for feature in features:
            if feature.number <= number:
                result.append(feature)
        return result
