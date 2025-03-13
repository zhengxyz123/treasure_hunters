#!/usr/bin/env python3
# Copyright (c) 2025 zhengxyz123
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.

"""Binary resources writer.

This Python script writes resource files such as images, sounds, etc. to a C array.

Usually, resources directory is `resources/`, source file is `src/resources/assets.c`,
header file is `src/resources/assets.h`.
"""

import sys
from pathlib import Path

prefix_info = "This file was automatically generated, please do not modify it!"

source_content = """\
// {{info}}
// clang-format off

#include "{{header}}"

{{content}}

//clang-format on
"""

header_content = """\
// {{info}}

#ifndef {{header}}
#define {{header}}

{{content}}

#endif
"""


def main(res_dir: Path, source: Path, header: Path) -> int:
    if not res_dir.exists() or not res_dir.is_dir():
        print(f"'{res_dir}' must be a directory")
        return 1
    if not source.is_file():
        print(f"'{source}' must be a file")
        return 1
    if not header.is_file():
        print(f"'{header}' must be a file")
        return 1
    if source.parent != header.parent:
        print("source file and header file must be in the same directory")
        return 1
    source_content_line = []
    header_content_line = []
    header_name = f"_TH_{header.name.upper().replace(".", "_")}_"
    for root, _, files in res_dir.walk():
        for file in files:
            path = Path(root) / file
            var_name = path.name.replace(".", "_") + "_content"
            var_value = f"{{{', '.join([str(c) for c in path.read_bytes()])}}};"
            source_content_line.append(
                f"const unsigned char {var_name}[] = {var_value}"
            )
            header_content_line.append(
                f"extern const unsigned char {var_name}[{len(path.read_bytes())}];"
            )
    c_source_content = (
        source_content.replace("{{info}}", prefix_info)
        .replace("{{header}}", header.name)
        .replace("{{content}}", "\n".join(source_content_line))
    )
    c_header_content = (
        header_content.replace("{{info}}", prefix_info)
        .replace("{{header}}", header_name)
        .replace("{{content}}", "\n".join(header_content_line))
    )
    source.write_text(c_source_content)
    header.write_text(c_header_content)
    return 0


if __name__ == "__main__":
    if len(sys.argv) != 4:
        print(
            f"usage: {sys.argv[0]} [resources directory] [C source file] [C header file]"
        )
        exit(1)
    else:
        exit(main(*[Path(arg) for arg in sys.argv[1:]]))
