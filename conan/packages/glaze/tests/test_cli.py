"""Tests for glaze_cli argument parsing."""

from glaze_cli import build_parser


class TestBuildParser:
    def test_returns_parser(self) -> None:
        parser = build_parser()
        assert parser is not None

    def test_parse_generate_args(self) -> None:
        parser = build_parser()
        args = parser.parse_args(
            ["generate", "--api", "gl", "3.3", "--lang", "c", "--output-dir", "/tmp/out"]
        )
        assert args.command == "generate"
        assert args.api == [["gl", "3.3"]]
        assert args.lang == ["c"]
        assert args.output_dir == "/tmp/out"

    def test_parse_multiple_apis(self) -> None:
        parser = build_parser()
        args = parser.parse_args(
            ["generate", "--api", "gl", "3.3", "--api", "gles2", "2.0", "--lang", "c"]
        )
        assert args.api == [["gl", "3.3"], ["gles2", "2.0"]]

    def test_parse_multiple_langs(self) -> None:
        parser = build_parser()
        args = parser.parse_args(["generate", "--api", "gl", "3.3", "--lang", "c", "--lang", "cpp"])
        assert args.lang == ["c", "cpp"]

    def test_parse_list_apis(self) -> None:
        parser = build_parser()
        args = parser.parse_args(["list-apis"])
        assert args.command == "list-apis"

    def test_default_output_dir(self) -> None:
        parser = build_parser()
        args = parser.parse_args(["generate", "--api", "gl", "3.3", "--lang", "c"])
        assert args.output_dir == "."

    def test_extension_vendors_arg(self) -> None:
        parser = build_parser()
        args = parser.parse_args(
            ["generate", "--api", "gl", "3.3", "--lang", "c", "--extension-vendors", "ARB,EXT"]
        )
        assert args.extension_vendors == "ARB,EXT"

    def test_extensions_arg(self) -> None:
        parser = build_parser()
        args = parser.parse_args(
            [
                "generate", "--api", "gl", "3.3", "--lang", "c",
                "--extensions", "GL_KHR_debug,GL_NV_shader_buffer_load",
            ]
        )
        assert args.extensions == "GL_KHR_debug,GL_NV_shader_buffer_load"

    def test_extension_args_default_empty(self) -> None:
        parser = build_parser()
        args = parser.parse_args(["generate", "--api", "gl", "3.3", "--lang", "c"])
        assert args.extension_vendors == ""
        assert args.extensions == ""
