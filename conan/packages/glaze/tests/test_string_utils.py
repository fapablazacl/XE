"""Tests for glaze.utils.string_utils."""

import pytest

from glaze.utils.string_utils import camel_case, is_capitalized, split_capitalized


@pytest.mark.parametrize(
    ("value", "expected"),
    [
        ("Buffer", True),
        ("ARB", True),
        ("A", True),
        ("GL", True),
        ("BufferTarget", False),  # multi-word CamelCase is not a single capitalized word
        ("", False),
        ("buffer", False),
        ("bufferARB", False),
        ("123", False),
    ],
)
def test_is_capitalized(value: str, expected: bool) -> None:
    assert is_capitalized(value) == expected


@pytest.mark.parametrize(
    ("value", "expected"),
    [
        ("BufferTarget", ["Buffer", "Target"]),
        ("BufferTargetARB", ["Buffer", "Target", "ARB"]),
        ("Buffer", ["Buffer"]),
        ("ClearBufferMask", ["Clear", "Buffer", "Mask"]),
        ("GL", ["GL"]),
    ],
)
def test_split_capitalized(value: str, expected: list[str]) -> None:
    assert split_capitalized(value) == expected


@pytest.mark.parametrize(
    ("value", "expected"),
    [
        ("program pipeline", "ProgramPipeline"),
        ("texture", "Texture"),
        ("buffer", "Buffer"),
    ],
)
def test_camel_case(value: str, expected: str) -> None:
    assert camel_case(value) == expected
