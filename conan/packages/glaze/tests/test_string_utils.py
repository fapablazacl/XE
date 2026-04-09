"""Tests for glaze.utils.string_utils."""

import pytest

from glaze.utils.string_utils import camel_case, is_capitalized, split_capitalized, version_to_int


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


@pytest.mark.parametrize(
    ("value", "expected"),
    [
        ("1.0", 10),
        ("1.5", 15),
        ("2.0", 20),
        ("3.3", 33),
        ("4.6", 46),
    ],
)
def test_version_to_int(value: str, expected: int) -> None:
    assert version_to_int(value) == expected


@pytest.mark.parametrize("value", ["", "1", "1.2.3", "x.y", "1.x"])
def test_version_to_int_rejects_malformed(value: str) -> None:
    with pytest.raises(ValueError):
        version_to_int(value)
