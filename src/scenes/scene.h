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

#ifndef _TH_SCENE_H_
#define _TH_SCENE_H_

#include "../global.h"

#define MAX_SCENE 16

typedef enum {
    START_SCENE,
    SETTING_SCENE,
    LAST_SCENE
} SceneID;

typedef struct {
    int mouse_x;
    int mouse_y;
    void (*init)(void);
    void (*tick)(float dt);
    void (*free)(void);
    void (*on_window_resize)(int, int);
    void (*on_key_down)(SDL_KeyCode);
    void (*on_key_up)(SDL_KeyCode);
    void (*on_mouse_motion)(int, int, int, int);
    void (*on_mouse_down)(int, int, int, int);
    void (*on_mouse_up)(int, int, int, int);
    void (*on_mouse_scroll)(int, int, int, int);
    void (*on_caxis_motion)(int, int);
    void (*on_cbutton_down)(int);
    void (*on_cbutton_up)(int);
} Scene;

extern Scene* scene_array[MAX_SCENE];

void InitSceneManager();
void SwitchScene(SceneID scene_id);
void BackToPrevScene();
void HandleSceneEvent(SDL_Event* event);
void TickScene(float dt);
void FreeScene(Scene* scene);
void FreeSceneManager();

#endif
