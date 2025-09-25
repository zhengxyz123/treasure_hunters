/*
  Copyright (c) 2025 zhengxyz123

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in
  all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
  THE SOFTWARE.
*/

#include "world.h"
#include "../global.h"
#include "../map.h"
#include "background.h"

extern GameApp game_app;

Scene world_scene = {
    .init = WorldSceneInit,
    .tick = WorldSceneTick,
    .free = worldSceneFree,
    .on_key_down = WorldSceneOnKeyDown,
    .on_cbutton_down = WorldSceneOnControllerButtonDown
};

Map* map;
SDL_Point offset = {0, 0};

void WorldSceneInit() {
    map = LoadMap("maps/start.tmx");
}

void WorldSceneTick(float dt) {
    DrawBackground(dt);
    MapDrawLayer(map, TILEMAP_LAYERGROUP_BACK, &offset);
}

void worldSceneFree() {
    FreeMap(map);
}

void WorldSceneOnKeyDown(SDL_KeyCode key) {
    switch (key) {
    case SDLK_ESCAPE:
        BackToPrevScene();
        break;
    case SDLK_LEFT:
        offset.x -= 8;
        break;
    case SDLK_RIGHT:
        offset.x += 8;
        break;
    case SDLK_UP:
        offset.y -= 8;
        break;
    case SDLK_DOWN:
        offset.y += 8;
        break;
    default:
        break;
    }
}

void WorldSceneOnControllerButtonDown(int button) {
    switch (button) {
    case SDL_CONTROLLER_BUTTON_B:
        BackToPrevScene();
        break;
    case SDL_CONTROLLER_BUTTON_DPAD_LEFT:
        offset.x -= 8;
        break;
    case SDL_CONTROLLER_BUTTON_DPAD_RIGHT:
        offset.x += 8;
        break;
    case SDL_CONTROLLER_BUTTON_DPAD_UP:
        offset.y -= 8;
        break;
    case SDL_CONTROLLER_BUTTON_DPAD_DOWN:
        offset.y += 8;
        break;
    }
}
