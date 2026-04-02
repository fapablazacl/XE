from abc import ABC, abstractmethod
from typing import Dict

from glaze.model import Registry


class Generator(ABC):
    """Abstract base for all language generators.

    Each generator receives a fully-parsed Registry and produces one or more
    output files keyed by their relative path.
    """

    def __init__(self, registry: Registry):
        self.registry = registry

    @abstractmethod
    def generate(self, api: str, version: str) -> Dict[str, str]:
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
