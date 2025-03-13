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

from pathlib import Path

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
from pyglet.image import load as load_image
from pyglet.shapes import Line, Rectangle
from pyglet.sprite import Sprite
from pyglet.window import Window
from pyglet.window.mouse import LEFT, MIDDLE


class MapEditorWindow(Window):
    def __init__(self, *args, **kwargs) -> None:
        super().__init__(*args, **kwargs)
        self.set_caption("Map Editor")
        self.set_minimum_size(640, 480)
        self.maximize()

        self.tileset_path = (
            Path(__file__).parent.parent / "resources" / "images" / "tilesets.png"
        )
        self.vbar = Rectangle(self.width / 3, 0, 3, self.height, color=(85, 85, 85))
        self.hbar = Rectangle(
            0, 2 * self.height / 3, self.vbar.x, 3, color=(85, 85, 85)
        )
        self.split_line = Line(0, 0, 0, 0, color=(0, 0, 0))
        self.tileset_sprite = Sprite(load_image(str(self.tileset_path)))
        self.tileset_sprite.position = (0, self.hbar.y - self.tileset_sprite.height, 0)
        self.selected_tile = [0, 0]
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
                self.CURSOR_DEFAULT  # pyright: ignore
            )
            self.set_mouse_cursor(cursor)
        self.vbar.x = min(self.width - 8, max(self.vbar.x, 5))
        self.vbar.draw()
        self.hbar.width = self.vbar.x
        self.hbar.draw()
        glEnable(GL_SCISSOR_TEST)
        glScissor(0, 0, int(self.vbar.x), int(self.hbar.y))
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
            (x, y) in self.vbar and self.hbar.y - 3 < y < self.hbar.y + self.hbar.height + 3
        ):
            self.hbar.color = (170, 170, 170)
        else:
            self.hbar.color = (85, 85, 85)

    def on_mouse_press(self, x: int, y: int, button: int, modifiers: int) -> None:
        if (x, y) in self.vbar and self.hbar.y - 3 < y < self.hbar.y + self.hbar.height + 3:
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
                sx = (x - self.tileset_sprite.x) // (32 * self.tileset_sprite.scale)
                sy = (y - self.tileset_sprite.y) // (32 * self.tileset_sprite.scale)
                self.selected_tile = [sx, 15 - sy]
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
    win = MapEditorWindow(resizable=True)
    pyglet.app.run()
