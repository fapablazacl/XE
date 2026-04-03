"""Parser for OpenGL-Refpages DocBook XML documentation.

Extracts brief descriptions and parameter documentation from the Khronos
OpenGL reference pages and provides them as a simple lookup index.
"""

import os
import xml.dom.minidom
from dataclasses import dataclass, field
from xml.dom.minidom import Node


@dataclass
class FunctionDoc:
    """Documentation for a single GL function."""

    brief: str
    params: dict[str, str] = field(default_factory=dict)


# Mapping from Glaze API name to the refpages subdirectory that best covers it.
_API_TO_REFDIR: dict[str, str] = {
    "gl": "gl4",
    "gles1": "es1.1",
    "gles2": "es3.0",
    "glsc2": "gl4",
}

_DOCBOOK_NS = "http://docbook.org/ns/docbook"


def _text_content(node: Node) -> str:
    """Recursively extract plain text from a DocBook node, stripping all markup."""
    parts: list[str] = []
    for child in node.childNodes:
        if child.nodeType == Node.TEXT_NODE:
            parts.append(child.nodeValue)
        elif child.nodeType == Node.ELEMENT_NODE:
            parts.append(_text_content(child))
    return "".join(parts)


def _strip_whitespace(text: str) -> str:
    """Collapse internal whitespace and strip leading/trailing."""
    return " ".join(text.split())


def _find_elements_by_local_name(parent: Node, local_name: str) -> list[Node]:
    """Find all direct child elements matching a local tag name (namespace-agnostic)."""
    results: list[Node] = []
    for child in parent.childNodes:
        if child.nodeType == Node.ELEMENT_NODE and child.localName == local_name:
            results.append(child)
    return results


def _find_element_by_local_name(parent: Node, local_name: str) -> Node | None:
    """Find the first direct child element matching a local tag name."""
    for child in parent.childNodes:
        if child.nodeType == Node.ELEMENT_NODE and child.localName == local_name:
            return child
    return None


def _find_deep(parent: Node, local_name: str) -> list[Node]:
    """Recursively find all descendant elements with the given local name."""
    results: list[Node] = []
    for child in parent.childNodes:
        if child.nodeType == Node.ELEMENT_NODE:
            if child.localName == local_name:
                results.append(child)
            results.extend(_find_deep(child, local_name))
    return results


def _parse_refpage(path: str) -> list[FunctionDoc] | None:
    """Parse a single refpage XML file, returning FunctionDoc(s) or None on error.

    Returns a list because a single refpage may document multiple related functions
    (e.g., glClear covers just glClear, but glGenBuffers covers glGenBuffers and
    glCreateBuffers).  All share the same brief and param docs.
    """
    try:
        doc = xml.dom.minidom.parse(path)
    except Exception:
        return None

    root = doc.documentElement

    # Extract brief from <refpurpose>
    purposes = _find_deep(root, "refpurpose")
    if not purposes:
        return None
    brief = _strip_whitespace(_text_content(purposes[0]))
    if not brief:
        return None

    # Extract parameter descriptions from <refsect1 xml:id="parameters">
    params: dict[str, str] = {}
    for sect in _find_deep(root, "refsect1"):
        sect_id = sect.getAttribute("xml:id") or sect.getAttribute("id") or ""
        if sect_id != "parameters":
            continue
        for varentry in _find_deep(sect, "varlistentry"):
            terms = _find_deep(varentry, "term")
            listitems = _find_deep(varentry, "listitem")
            if not terms or not listitems:
                continue
            # The parameter name is inside <term><parameter>name</parameter></term>
            param_nodes = _find_deep(terms[0], "parameter")
            if not param_nodes:
                continue
            param_name = _strip_whitespace(_text_content(param_nodes[0]))
            # The description is in the first <para> inside <listitem>
            paras = _find_deep(listitems[0], "para")
            if paras:
                desc = _strip_whitespace(_text_content(paras[0]))
                if desc:
                    params[param_name] = desc
        break  # Only process the first parameters section

    func_doc = FunctionDoc(brief=brief, params=params)

    # Collect all function names documented in this refpage
    names: list[str] = []
    for refname in _find_deep(root, "refname"):
        name = _strip_whitespace(_text_content(refname))
        if name:
            names.append(name)

    if not names:
        return None

    return [FunctionDoc(brief=func_doc.brief, params=dict(func_doc.params)) for _ in names], names


def parse_refpages(refpages_dir: str, api: str = "gl") -> dict[str, FunctionDoc]:
    """Parse all refpage XML files in the appropriate subdirectory.

    Args:
        refpages_dir: Path to the OpenGL-Refpages root directory.
        api: The Glaze API name (e.g., 'gl', 'gles2').

    Returns:
        A dict mapping GL function names (e.g., 'glClear') to their FunctionDoc.
    """
    subdir = _API_TO_REFDIR.get(api, "gl4")
    search_dir = os.path.join(refpages_dir, subdir)
    if not os.path.isdir(search_dir):
        return {}

    index: dict[str, FunctionDoc] = {}
    for filename in os.listdir(search_dir):
        if not filename.endswith(".xml") or not filename.startswith("gl"):
            continue
        filepath = os.path.join(search_dir, filename)
        result = _parse_refpage(filepath)
        if result is None:
            continue
        docs, names = result
        for doc, name in zip(docs, names, strict=True):
            index[name] = doc

    return index
