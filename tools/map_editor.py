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

"""Game map editor."""

from dataclasses import dataclass
from pathlib import Path
import struct

import pyglet
from pyglet.gl import (
    GL_NEAREST,
    GL_SCISSOR_TEST,
    glClearColor,
    glDisable,
    glEnable,
    glScissor,
)
from pyglet.image import Texture
from pyglet.image import load as load_image, ImageGrid
from pyglet.shapes import Line, Rectangle
from pyglet.sprite import Sprite
from pyglet.text import Label, Weight
from pyglet.window import Window
from pyglet.window.mouse import LEFT, MIDDLE


@dataclass
class AnimatedTileSet:
    is_playing: bool
    frames: list[tuple[int, int, int, int]]

    def to_bytes(self) -> bytes:
        return struct.pack(
            f"<BH{len(self.frames) * 4}h",
            self.is_playing,
            len(self.frames),
            *[item for sublist in self.frames for item in sublist],
        )


window_frame: list[tuple[int, int, int, int]] = []
for n in range(10, 12):
    for m in range(12, 16):
        window_frame.append((m, n, 1, 1))
for n in range(12, 15):
    for m in range(17):
        window_frame.append((m, n, 1, 1))
for i in range(15):
    window_frame.append((i, 15, 1, 1))
window_anime = AnimatedTileSet(is_playing=True, frames=window_frame)
light_anime = AnimatedTileSet(
    is_playing=True,
    frames=[
        (14, 1, 1, 1),
        (15, 1, 1, 1),
        (12, 2, 1, 1),
        (13, 2, 1, 1),
        (14, 2, 1, 1),
        (15, 2, 1, 1),
    ],
)
glow_anime = AnimatedTileSet(
    is_playing=True, frames=[(12, 3, 1, 1), (13, 3, 1, 1), (14, 3, 1, 1), (15, 3, 1, 1)]
)


@dataclass
class TileMapObject:
    obj_type: int
    flag: int
    class_name: str
    object_name: str
    data: tuple[int, int, int, int]

    def to_bytes(self) -> bytes:
        binary = struct.pack(
            "<2B8s8s4h",
            self.obj_type,
            self.flag & 0xFF,
            (
                self.class_name[:8]
                if len(self.class_name) <= 8
                else self.class_name.ljust(8)
            ),
            (
                self.object_name[:8]
                if len(self.object_name) <= 8
                else self.object_name.ljust(8)
            ),
            *self.data,
        )
        return binary


@dataclass
class TileMapLayer:
    area: tuple[int, int, int, int]
    data: list[int]

    def to_bytes(self) -> bytes:
        return struct.pack(
            f"<4h{len(self.data)}h",
            *self.area,
            *self.data,
        )


class TileMap:
    def __init__(self, tileset_path: Path) -> None:
        self.tileset = ImageGrid(load_image(str(tileset_path)), 20, 16)
        self.animated_tiles: list[AnimatedTileSet] = []
        self.objects: list[TileMapObject] = []
        self.layers: list[TileMapLayer] = []

    @classmethod
    def from_bytes(cls, content: bytes) -> "TileMap":
        map = cls(
            Path(__file__).parent.parent / "resources" / "images" / "tilesets.png"
        )
        now = 0
        if struct.unpack_from("<3s", content)[0] != b"MAP":
            raise ValueError("wrong file header")
        now += struct.calcsize("<3s")
        if struct.unpack_from("<B", content, now)[0] != 1:
            raise ValueError("wrong format version")
        now += struct.calcsize("<B")
        _, _ = struct.unpack_from("<2H", content, now)
        now += struct.calcsize("<2H")
        animated_tiles_count, objects_count, layers_count = struct.unpack_from(
            "3H", content, now
        )
        now += struct.calcsize("<3H")
        for _ in range(animated_tiles_count):
            is_playing = bool(struct.unpack_from("<B", content, now)[0])
            now += struct.calcsize("<B")
            length = int(struct.unpack_from("<H", content, now)[0])
            now += struct.calcsize("<H")
            data = struct.unpack_from(f"<{length * 4}h", content, now)
            now += struct.calcsize(f"<{length * 4}h")
            frames = []
            for n in range(length):
                frames.append(data[4 * n : 4 * n + 4])
            map.animated_tiles.append(AnimatedTileSet(is_playing, frames))
        for _ in range(objects_count):
            obj_type, flag, class_name, object_name = struct.unpack_from("<2B8s8s", content, now)
            now += struct.calcsize("<2B8s8s")
            data = struct.unpack_from("<4h", content, now)
            now += struct.calcsize("<4h")
            map.objects.append(TileMapObject(obj_type, flag, class_name, object_name, data))  # type: ignore
        for _ in range(layers_count):
            area = struct.unpack_from("<4h", content, now)
            now += struct.calcsize("<4h")
            data = list(struct.unpack_from(f"<{area[2] * area[3]}h", content, now))
            now += struct.calcsize(f"<{area[2] * area[3]}h")
            map.layers.append(TileMapLayer(area, data))
        return map

    def to_bytes(self) -> bytes:
        binary = struct.pack("<3sB", b"MAP", 1)
        binary += struct.pack("2H", 32, 32)
        binary += struct.pack(
            "3H", len(self.animated_tiles), len(self.objects), len(self.layers)
        )
        for tile in self.animated_tiles:
            binary += tile.to_bytes()
        for obj in self.objects:
            binary += obj.to_bytes()
        for layer in self.layers:
            binary += layer.to_bytes()
        return binary


class MapEditorWindow(Window):
    def __init__(self, *args, **kwargs) -> None:
        super().__init__(*args, **kwargs)
        self.set_caption("Map Editor")
        self.set_minimum_size(1280, 720)

        self.tileset_path = (
            Path(__file__).parent.parent / "resources" / "images" / "tilesets.png"
        )
        self.tilemap = TileMap(self.tileset_path)
        self.tilemap.animated_tiles = [window_anime, light_anime, glow_anime]
        self.vbar = Rectangle(self.width / 3, 0, 3, self.height, color=(85, 85, 85))
        self.hbar = Rectangle(0, self.height / 2, self.vbar.x, 3, color=(85, 85, 85))
        self.split_line = Line(0, 0, 0, 0, color=(0, 0, 0))
        self.tileset_sprite = Sprite(load_image(str(self.tileset_path)))
        self.tileset_sprite.position = (0, self.hbar.y - self.tileset_sprite.height, 0)
        self.selected_tile = [0, 0]
        self.select_label1 = Label(
            "(0, 0): 1", 5, 3, color=(0, 0, 0), font_size=15, weight=Weight.BOLD
        )
        self.select_label2 = Label(
            "(0, 0): 1", 3, 5, color=(255, 255, 255), font_size=15, weight=Weight.BOLD
        )
        self.select_rect = Rectangle(0, 0, 0, 0, color=(85, 85, 255, 128))

        self.can_drag_vbar = False
        self.can_drag_hbar = False
        self.can_drag_bar = False
        # 0 for left, 1 for right, -1 for disabled
        self.drag_which_frame = -1

    def on_draw(self) -> None:
        self.clear()
        glClearColor(20 / 255, 21 / 255, 23 / 255, 1)
        if self.can_drag_vbar:
            cursor = self.get_system_mouse_cursor(self.CURSOR_SIZE_LEFT_RIGHT)
            self.set_mouse_cursor(cursor)
        elif self.can_drag_hbar:
            cursor = self.get_system_mouse_cursor(self.CURSOR_SIZE_UP_DOWN)
            self.set_mouse_cursor(cursor)
        elif self.drag_which_frame > -1 or self.can_drag_bar:
            cursor = self.get_system_mouse_cursor(self.CURSOR_SIZE)
            self.set_mouse_cursor(cursor)
        else:
            cursor = self.get_system_mouse_cursor(
                self.CURSOR_DEFAULT  # type: ignore
            )
            self.set_mouse_cursor(cursor)
        self.vbar.x = min(self.width - 8, max(self.vbar.x, 5))
        self.vbar.draw()
        self.hbar.width = self.vbar.x
        self.hbar.draw()
        glEnable(GL_SCISSOR_TEST)
        glScissor(0, 0, int(self.vbar.x), int(self.hbar.y))
        self.tileset_sprite.x = max(
            self.vbar.x - self.tileset_sprite.width, min(self.tileset_sprite.x, 0)
        )
        if self.vbar.x > self.tileset_sprite.width:
            self.tileset_sprite.x = 0
        self.tileset_sprite.y = max(
            self.hbar.y - self.tileset_sprite.height, min(self.tileset_sprite.y, 0)
        )
        self.tileset_sprite.draw()
        self.select_rect.x = (
            self.tileset_sprite.x
            + self.selected_tile[0] * self.tileset_sprite.scale * 32
        )
        self.select_rect.y = (
            self.tileset_sprite.y
            + self.tileset_sprite.height
            - (self.selected_tile[1] + 1) * self.tileset_sprite.scale * 32
        )
        self.select_rect.width = self.select_rect.height = (
            self.tileset_sprite.scale * 32
        )
        self.select_rect.draw()
        for i in range(
            int(self.tileset_sprite.height / self.tileset_sprite.scale / 32) + 1
        ):
            self.split_line.x = self.tileset_sprite.x
            self.split_line.y = (
                self.tileset_sprite.y + i * self.tileset_sprite.height / 16
            )
            self.split_line.x2 = self.split_line.x + self.tileset_sprite.width
            self.split_line.y2 = self.split_line.y
            self.split_line.draw()
        for i in range(
            int(self.tileset_sprite.width / self.tileset_sprite.scale / 32) + 1
        ):
            self.split_line.x = (
                self.tileset_sprite.x + i * self.tileset_sprite.height / 16
            )
            self.split_line.y = self.tileset_sprite.y
            self.split_line.x2 = self.split_line.x
            self.split_line.y2 = self.split_line.y + self.tileset_sprite.height
            self.split_line.draw()
        self.select_label1.draw()
        self.select_label2.draw()
        glDisable(GL_SCISSOR_TEST)

    def on_resize(self, width: int, height: int) -> None:
        self.vbar.height = height

    def on_mouse_scroll(self, x: int, y: int, scroll_x: float, scroll_y: float) -> None:
        if x < self.vbar.x and y < self.hbar.y:
            dx, dy = x - self.tileset_sprite.x, y - self.tileset_sprite.y
            prev_scale = self.tileset_sprite.scale
            self.tileset_sprite.scale += scroll_y
            self.tileset_sprite.scale = min(5, max(self.tileset_sprite.scale, 1))
            dx, dy = (
                self.tileset_sprite.scale / prev_scale * dx,
                self.tileset_sprite.scale / prev_scale * dy,
            )
            self.tileset_sprite.position = (x - dx, y - dy, 0)

    def on_mouse_drag(
        self, x: int, y: int, dx: int, dy: int, buttons: int, modifiers: int
    ) -> None:
        if self.can_drag_vbar or self.can_drag_bar:
            self.vbar.x = x
            self.vbar.x = min(self.width - 8, max(self.vbar.x, 5))
        if self.can_drag_hbar or self.can_drag_bar:
            self.hbar.y = y
            self.hbar.y = min(self.height - 8, max(self.hbar.y, 5))
        if self.drag_which_frame == 0:
            self.tileset_sprite.x += dx
            self.tileset_sprite.y += dy

    def on_mouse_motion(self, x: int, y: int, dx: int, dy: int) -> None:
        if (x, y) in self.vbar:
            self.vbar.color = (170, 170, 170)
        else:
            self.vbar.color = (85, 85, 85)
        if (x, y) in self.hbar or (
            (x, y) in self.vbar
            and self.hbar.y - 3 < y < self.hbar.y + self.hbar.height + 3
        ):
            self.hbar.color = (170, 170, 170)
        else:
            self.hbar.color = (85, 85, 85)

    def on_mouse_press(self, x: int, y: int, button: int, modifiers: int) -> None:
        if (
            x,
            y,
        ) in self.vbar and self.hbar.y - 3 < y < self.hbar.y + self.hbar.height + 3:
            self.can_drag_bar = True
        elif button == LEFT and (x, y) in self.vbar:
            self.can_drag_vbar = True
        elif button == LEFT and (x, y) in self.hbar:
            self.can_drag_hbar = True
        elif button == LEFT and x < self.vbar.x:
            if (
                self.tileset_sprite.x
                < x
                < self.tileset_sprite.x + self.tileset_sprite.width
                and self.tileset_sprite.y
                < y
                < self.tileset_sprite.y + self.tileset_sprite.height
            ):
                sx = int(
                    (x - self.tileset_sprite.x) // (32 * self.tileset_sprite.scale)
                )
                sy = 15 - int(
                    (y - self.tileset_sprite.y) // (32 * self.tileset_sprite.scale)
                )
                self.selected_tile = [sx, sy]
                self.select_label1.text = f"({sx}, {sy}): {sx + 20 * sy + 1}"
                self.select_label2.text = self.select_label1.text
        elif button == MIDDLE:
            if x < self.vbar.x and y < self.hbar.y:
                self.drag_which_frame = 0
            elif x > self.vbar.x:
                self.drag_which_frame = 1

    def on_mouse_release(self, x: int, y: int, button: int, modifiers: int) -> None:
        self.can_drag_vbar = False
        self.can_drag_hbar = False
        self.can_drag_bar = False
        self.drag_which_frame = -1


if __name__ == "__main__":
    Texture.default_min_filter = GL_NEAREST
    Texture.default_mag_filter = GL_NEAREST
    win = MapEditorWindow(1280, 720, resizable=True)
    pyglet.app.run()
