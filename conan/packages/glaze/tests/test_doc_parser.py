"""Tests for glaze.doc_parser."""

import os
import tempfile

from glaze.doc_parser import FunctionDoc, parse_refpages

MINI_REFPAGE = """\
<?xml version="1.0" encoding="UTF-8"?>
<refentry xmlns="http://docbook.org/ns/docbook" version="5.0" xml:id="glTestFunc">
    <refnamediv>
        <refname>glTestFunc</refname>
        <refpurpose>do something useful</refpurpose>
    </refnamediv>
    <refsect1 xml:id="parameters"><title>Parameters</title>
        <variablelist>
        <varlistentry>
            <term><parameter>target</parameter></term>
            <listitem>
                <para>Specifies the target.</para>
            </listitem>
        </varlistentry>
        <varlistentry>
            <term><parameter>value</parameter></term>
            <listitem>
                <para>The <function>value</function> to set.</para>
            </listitem>
        </varlistentry>
        </variablelist>
    </refsect1>
</refentry>
"""


class TestParseRefpages:
    def test_parses_brief(self) -> None:
        with tempfile.TemporaryDirectory() as tmpdir:
            subdir = os.path.join(tmpdir, "gl4")
            os.makedirs(subdir)
            with open(os.path.join(subdir, "glTestFunc.xml"), "w") as f:
                f.write(MINI_REFPAGE)
            index = parse_refpages(tmpdir, "gl")
            assert "glTestFunc" in index
            assert index["glTestFunc"].brief == "do something useful"

    def test_parses_params(self) -> None:
        with tempfile.TemporaryDirectory() as tmpdir:
            subdir = os.path.join(tmpdir, "gl4")
            os.makedirs(subdir)
            with open(os.path.join(subdir, "glTestFunc.xml"), "w") as f:
                f.write(MINI_REFPAGE)
            index = parse_refpages(tmpdir, "gl")
            doc = index["glTestFunc"]
            assert "target" in doc.params
            assert doc.params["target"] == "Specifies the target."

    def test_strips_inline_markup(self) -> None:
        with tempfile.TemporaryDirectory() as tmpdir:
            subdir = os.path.join(tmpdir, "gl4")
            os.makedirs(subdir)
            with open(os.path.join(subdir, "glTestFunc.xml"), "w") as f:
                f.write(MINI_REFPAGE)
            index = parse_refpages(tmpdir, "gl")
            doc = index["glTestFunc"]
            # <function>value</function> should be stripped to plain "value"
            assert doc.params["value"] == "The value to set."

    def test_missing_dir_returns_empty(self) -> None:
        index = parse_refpages("/nonexistent/path", "gl")
        assert index == {}

    def test_non_gl_files_ignored(self) -> None:
        with tempfile.TemporaryDirectory() as tmpdir:
            subdir = os.path.join(tmpdir, "gl4")
            os.makedirs(subdir)
            # Write a non-gl file
            with open(os.path.join(subdir, "abs.xml"), "w") as f:
                f.write(MINI_REFPAGE)
            index = parse_refpages(tmpdir, "gl")
            assert len(index) == 0


class TestFunctionDoc:
    def test_dataclass(self) -> None:
        doc = FunctionDoc(brief="test", params={"a": "param a"})
        assert doc.brief == "test"
        assert doc.params["a"] == "param a"
