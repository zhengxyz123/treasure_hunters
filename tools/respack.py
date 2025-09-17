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

"""Resource packager writer and reader.

Its internal internal implementation is a hash table whose key is string and
value is array of byte.
"""

import argparse
import json
import os
import subprocess
import tempfile
import ctypes
import shutil
import sys
from io import BytesIO
from pathlib import Path


class RespackHeader(ctypes.LittleEndianStructure):
    _pack_ = 8
    _fields_ = [
        ("magic", ctypes.c_char * 4),
        ("version", ctypes.c_uint8),
        ("entry_count", ctypes.c_uint16),
        ("key_index_offset", ctypes.c_uint64),
        ("value_index_offset", ctypes.c_uint64),
    ]


class RespackEntry(ctypes.LittleEndianStructure):
    _pack_ = 8
    _fields_ = [
        ("key_offset", ctypes.c_uint64),
        ("key_length", ctypes.c_uint8),
        ("key_hash", ctypes.c_uint32),
        ("value_offset", ctypes.c_uint64),
        ("value_length", ctypes.c_uint32),
    ]


def _fnv1a_32(s: str) -> int:
    hval = 2166136261
    for c in s:
        hval ^= ord(c)
        hval *= 16777619
        hval = ctypes.c_uint32(hval).value
    return hval


def dumps(obj: dict[str, bytes]) -> bytes:
    header = RespackHeader(b"RPKG", 1, len(obj))
    entries: list[RespackEntry] = []
    keys_data = b""
    values_data = b""
    key_offset = 0
    value_offset = 0

    for key, value in obj.items():
        key_bytes = key.encode("utf-8")
        key_len = len(key_bytes)
        value_len = len(value)

        entry = RespackEntry(
            key_offset, key_len, _fnv1a_32(key), value_offset, value_len
        )
        entries.append(entry)
        keys_data += key_bytes
        values_data += value
        key_offset += key_len
        value_offset += value_len

    # calculate offsets
    header_size = ctypes.sizeof(RespackHeader)
    entries_size = len(entries) * ctypes.sizeof(RespackEntry)
    entries_end_offset = header_size + entries_size
    header.key_index_offset = entries_end_offset
    header.value_index_offset = entries_end_offset + len(keys_data)

    # write to buffer
    buf = BytesIO()
    buf.write(header)
    for entry in entries:
        buf.write(entry)
    buf.write(keys_data)
    buf.write(values_data)
    return buf.getvalue()


def loads(b: bytes) -> dict[str, bytes]:
    buf = BytesIO(b)
    # read header
    header = RespackHeader()
    buf.readinto(header)
    if header.magic != b"RPKG":
        raise ValueError("invalid magic number")
    if header.version != 1:
        raise ValueError("unsupported version")
    entry_count = header.entry_count

    # read entries
    entries: list[RespackEntry] = []
    for _ in range(entry_count):
        entry = RespackEntry()
        buf.readinto(entry)
        entries.append(entry)

    buf.seek(header.key_index_offset)
    keys_data = buf.read(header.value_index_offset - header.key_index_offset)
    buf.seek(header.value_index_offset)
    values_data = buf.read()

    result = {}
    for entry in entries:
        key_bytes = keys_data[entry.key_offset : entry.key_offset + entry.key_length]
        key = key_bytes.decode("utf-8")
        if _fnv1a_32(key) != entry.key_hash:
            raise ValueError(f"hash mismatch for key: {key}")
        value = values_data[
            entry.value_offset : entry.value_offset + entry.value_length
        ]
        result[key] = value
    return result


def _subcmd_gen(args: argparse.Namespace) -> int:
    if shutil.which("tiled") is None:
        raise RuntimeError("tiled must be installed")
    should_use_xvfb = False
    if sys.platform.startswith("linux") and "DISPLAY" not in os.environ:
        if shutil.which("xvfb-run") is None:
            raise RuntimeError("xvfb-run must be installed")
        else:
            should_use_xvfb = True
    if not Path(args.src).is_dir():
        raise RuntimeError(f"{args.src!r} must be a directory")

    obj: dict[str, bytes] = {}
    for root, _, files in Path(args.src).walk():
        for file in files:
            key = (root / file).relative_to(Path(args.src))
            if key.match("*.tsx") or key.match("*.tiled-session"):
                continue
            elif key.match("*.tmx"):
                fd, map_path = tempfile.mkstemp()
                os.close(fd)
                command = [
                    str(shutil.which("tiled")),
                    "--embed-tilesets",
                    "--minimize",
                    "--export-map",
                    "json",
                    root / file,
                    map_path,
                ]
                if should_use_xvfb:
                    command.insert(0, "xvfb-run")
                subprocess.run(command)
                json_map = json.load(Path(map_path).open())
                for tileset in json_map["tilesets"]:
                    tileset["image"] = Path(tileset["image"]).name
                value = json.dumps(json_map, separators=(",", ":")).encode()
                os.remove(map_path)
            else:
                value = Path(root / file).read_bytes()
            obj[str(key.as_posix())] = value

    args.dest.write(dumps(obj))
    return 0


def _main() -> int:
    parser = argparse.ArgumentParser()
    subparser = parser.add_subparsers(required=True)

    parser_gen = subparser.add_parser("gen", help="generate resource pack")
    parser_gen.add_argument("src", help="source directory")
    parser_gen.add_argument("dest", help="output file", type=argparse.FileType("wb"))
    parser_gen.set_defaults(func=_subcmd_gen)

    args = parser.parse_args()
    return args.func(args)


if __name__ == "__main__":
    try:
        sys.exit(_main())
    except Exception as err:
        sys.exit(err.args[0])
    except KeyboardInterrupt:
        sys.exit("interrupted by user")
