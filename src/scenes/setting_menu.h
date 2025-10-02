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

#ifndef _TH_SETTING_MENU_H_
#define _TH_SETTING_MENU_H_

#include "../ui/widget.h"
#include "scene.h"

typedef enum {
    SETTING_TYPE_BUTTON,
    SETTING_TYPE_SLIDER,
    SETTING_TYPE_SUBTITLE,
    SETTING_TYPE_OPTION,
    SETTING_TYPE_SPACE
} SettingType;

typedef struct {
    SettingType type;
    char* name;
    union {
        int button;
        int* option;
        SliderData* slider;
    } data;
} SettingItem;

extern Scene setting_scene;

void SettingSceneInit();
void SettingSceneTick(float dt);
void SettingSceneFree();
void SettingSceneOnKeyDown(SDL_KeyCode key);
void SettingSceneOnControllerButtonDown(int button);

#endif
