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

#include "scene.h"
#include "../global.h"
#include "../ui/widget.h"
#include "background.h"
#include <assert.h>

extern GameApp game_app;

Scene* scene_array[MAX_SCENE];
SceneID now_scene = START_SCENE;
SceneID next_scene = START_SCENE;
struct {
    SceneID data[MAX_SCENE];
    int top;
} scene_stack;

void InitSceneSystem() {
    InitBackground();
    scene_array[now_scene]->init();
    int mouse_x, mouse_y;
    SDL_GetMouseState(&mouse_x, &mouse_y);
    scene_array[now_scene]->mouse_x = mouse_x;
    scene_array[now_scene]->mouse_y = mouse_y;
    scene_stack.top = 0;
    scene_stack.data[scene_stack.top] = now_scene;
}

void QuitSceneSystem() {
    scene_array[now_scene]->free();
    QuitBackground();
}

void SwitchScene(SceneID scene_id) {
    if (now_scene == next_scene) {
        next_scene = scene_id;
        scene_stack.data[++scene_stack.top] = next_scene;
    }
}

void BackToPrevScene() {
    if (now_scene == next_scene) {
        assert(scene_stack.top > 0);
        next_scene = scene_stack.data[--scene_stack.top];
    }
}

void HandleSceneEvent(SDL_Event* event) {
    if (now_scene != next_scene) {
        int mouse_x = scene_array[now_scene]->mouse_x;
        int mouse_y = scene_array[now_scene]->mouse_y;
        scene_array[now_scene]->free();
        ClearWidgets();
        now_scene = next_scene;
        scene_array[now_scene]->init();
        scene_array[now_scene]->mouse_x = mouse_x;
        scene_array[now_scene]->mouse_y = mouse_y;
    }
    switch (event->type) {
    case SDL_WINDOWEVENT:
        if (scene_array[now_scene]->on_window_resize != NULL) {
            if (event->window.event == SDL_WINDOWEVENT_RESIZED) {
                scene_array[now_scene]->on_window_resize(
                    event->window.data1, event->window.data2
                );
            }
        }
        break;
    case SDL_KEYDOWN:
        if (scene_array[now_scene]->on_key_down != NULL) {
            scene_array[now_scene]->on_key_down(event->key.keysym.sym);
        }
        break;
    case SDL_KEYUP:
        if (scene_array[now_scene]->on_key_up != NULL) {
            scene_array[now_scene]->on_key_up(event->key.keysym.sym);
        }
        break;
    case SDL_MOUSEMOTION:
        scene_array[now_scene]->mouse_x = event->motion.x;
        scene_array[now_scene]->mouse_y = event->motion.y;
        if (scene_array[now_scene]->on_mouse_motion != NULL) {
            scene_array[now_scene]->on_mouse_motion(
                event->motion.x, event->motion.y, event->motion.xrel,
                event->motion.yrel
            );
        }
        break;
    case SDL_MOUSEBUTTONDOWN:
        if (scene_array[now_scene]->on_mouse_down != NULL) {
            scene_array[now_scene]->on_mouse_down(
                event->button.x, event->button.y, event->button.button,
                event->button.clicks
            );
        }
        break;
    case SDL_MOUSEBUTTONUP:
        if (scene_array[now_scene]->on_mouse_up != NULL) {
            scene_array[now_scene]->on_mouse_up(
                event->button.x, event->button.y, event->button.button,
                event->button.clicks
            );
        }
        break;
    case SDL_MOUSEWHEEL:
        if (scene_array[now_scene]->on_mouse_scroll != NULL) {
            scene_array[now_scene]->on_mouse_scroll(
                scene_array[now_scene]->mouse_x,
                scene_array[now_scene]->mouse_y, event->wheel.x, event->wheel.y
            );
        }
        break;
    case SDL_CONTROLLERAXISMOTION:
        if (scene_array[now_scene]->on_caxis_motion != NULL) {
            scene_array[now_scene]->on_caxis_motion(
                event->caxis.axis, event->caxis.value
            );
        }
        break;
    case SDL_CONTROLLERBUTTONDOWN:
        if (scene_array[now_scene]->on_cbutton_down != NULL) {
            scene_array[now_scene]->on_cbutton_down(event->cbutton.button);
        }
        break;
    case SDL_CONTROLLERBUTTONUP:
        if (scene_array[now_scene]->on_cbutton_up != NULL) {
            scene_array[now_scene]->on_cbutton_up(event->cbutton.button);
        }
        break;
    }
}

void TickScene(float dt) {
    scene_array[now_scene]->tick(dt);
}
