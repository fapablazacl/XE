from abc import ABC, abstractmethod

import jinja2

from glaze.model import Registry

_env = jinja2.Environment(
    loader=jinja2.PackageLoader("glaze", "templates"),
    trim_blocks=True,
    lstrip_blocks=True,
    keep_trailing_newline=True,
)


class Generator(ABC):
    """Abstract base for all language generators.

    Each generator receives a fully-parsed Registry and produces one or more
    output files keyed by their relative path.
    """

    def __init__(self, registry: Registry):
        self.registry = registry

    @abstractmethod
    def generate(self, api: str, version: str) -> dict[str, str]:
        """Generate output for the given API and version.

        Returns a dict mapping relative file paths to their string contents.
        Example: {"include/oglhpp/gl.h": "...", "src/gl.c": "..."}
        """
        ...

    @property
    @abstractmethod
    def name(self) -> str:
        """Human-readable generator name (e.g., 'c', 'cpp')."""
        ...

    def _render_template(self, template_path: str, context: dict) -> str:
        """Render a Jinja2 template from glaze/templates/ with the given context."""
        template = _env.get_template(template_path)
        result: str = template.render(**context)
        return result
