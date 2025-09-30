import re
from pathlib import Path

INPUT_FILE = "webgpu_cpp_print.h"
OUTPUT_FILE = "webgpu_fmt_formatters.h"

# Regex to match the exact function signature line and extract EnumType
OVERLOAD_LINE_PATTERN = re.compile(
    r'std::basic_ostream<CharT, Traits>&\s+operator<<\s*\(\s*std::basic_ostream<CharT, Traits>&\s+o,\s+(\w+)\s+value\s*\)\s*{'
)

def extract_enum_types(content):
    return sorted(set(OVERLOAD_LINE_PATTERN.findall(content)))

def generate_formatter(enum_name):
    return f"""
template <>
struct fmt::formatter<wgpu::{enum_name}> {{
    constexpr auto parse(format_parse_context& ctx) {{ return ctx.begin(); }}

    template <typename FormatContext>
    auto format(wgpu::{enum_name} value, FormatContext& ctx) const {{
        std::ostringstream oss;
        oss << value;
        return fmt::format_to(ctx.out(), "{{}}", oss.str());
    }}
}};
"""

def generate():
    input_path = Path(INPUT_FILE)
    output_path = Path(OUTPUT_FILE)

    if not input_path.exists():
        print(f"❌ Error: '{INPUT_FILE}' not found.")
        return

    content = input_path.read_text()
    enum_types = extract_enum_types(content)

    if not enum_types:
        print("⚠️  No operator<< overloads found.")
        return

    result = [
        "#pragma once",
        "#include <spdlog/fmt/bundled/format.h>",
        "#include <sstream>",
        "#include <webgpu/webgpu_cpp_print.h>",
        "",
        "namespace fmt {"
    ]

    for enum in enum_types:
        result.append(generate_formatter(enum))

    result.append("} // namespace fmt")

    output_path.write_text("\n".join(result))
    print(f"✅ Generated {len(enum_types)} formatters in '{OUTPUT_FILE}'.")

def generate_fmt_formatters(
    input_path: Path,
    output_path: Path,
    namespace: str = "wgpu"
):
    OVERLOAD_LINE_PATTERN = re.compile(
        r'std::basic_ostream<CharT, Traits>&\s+operator<<\s*\(\s*std::basic_ostream<CharT, Traits>&\s+o,\s+(\w+)\s+value\s*\)\s*{'
    )

    if not input_path.exists():
        print(f"❌ Error: '{input_path}' not found.")
        return False

    content = input_path.read_text()
    enum_types = sorted(set(OVERLOAD_LINE_PATTERN.findall(content)))
    if not enum_types:
        print("⚠️  No operator<< overloads found.")
        return False

    def generate_formatter(enum_name):
        return f"""
template <>
struct fmt::formatter<{namespace}::{enum_name}> {{
    constexpr auto parse(format_parse_context& ctx) {{ return ctx.begin(); }}

    template <typename FormatContext>
    auto format({namespace}::{enum_name} value, FormatContext& ctx) const {{
        std::ostringstream oss;
        oss << value;
        return fmt::format_to(ctx.out(), "{{}}", oss.str());
    }}
}};
"""

    result = [
        "#pragma once",
        "#include <spdlog/fmt/bundled/format.h>",
        "#include <sstream>",
        "#include <webgpu/webgpu_cpp_print.h>",
        "",
        "namespace fmt {"
    ] + [generate_formatter(enum) for enum in enum_types] + ["} // namespace fmt"]

    output_path.parent.mkdir(parents=True, exist_ok=True)
    output_path.write_text("\n".join(result))
    print(f"✅ Generated {len(enum_types)} formatters in '{output_path}'")
    return True
