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

#include "start_menu.h"
#include "../setting.h"
#include "../ui/text.h"
#include "../ui/widget.h"
#include "subscene.h"

extern GameApp global_app;
extern Setting global_setting;

Scene start_scene = {
    .init = StartSceneInit, .tick = StartSceneTick, .free = StartSceneFree
};
TextStyle title_text_style = {
    1.0,
    2,
    4,
    TEXT_ALIGN_CENTER,
    TEXT_ANCHOR_X_CENTER | TEXT_ANCHOR_Y_CENTER,
    {255, 255, 255}
};

void StartSceneInit() {}

void StartSceneTick() {
    int win_w, win_h;
    SDL_GetWindowSize(global_app.window, &win_w, &win_h);
    float prev_size = global_setting.interface_size;
    if (global_setting.auto_detect_size) {
        global_setting.interface_size =
            0.25 * win_h / (4 * 1.5 * SMALL_TEXT_HEIGHT);
        if (global_setting.interface_size < 2.0) {
            global_setting.interface_size = 2.0;
        }
    }
    DrawBackground();
    title_text_style.size =
        0.11 * win_h / 15 * 96 > win_w ? win_w / 98.0 : 0.11 * win_h / 15;
    DrawBigText(
        win_w / 2.0, 0.2 * win_h, &title_text_style, "TREASURE\nHUNTERS"
    );
    float text_w, text_h;
    WidgetBegin();
    CalcButtonTextSize("Start Game", &text_w, &text_h);
    WidgetButton(
        (win_w - text_w) / 2.0, win_h / 2.0 - 2.75 * text_h, "Start Game", 0
    );
    CalcButtonTextSize("Continue Game", &text_w, &text_h);
    WidgetButton(
        (win_w - text_w) / 2.0, win_h / 2.0 - 1.25 * text_h, "Continue Game", 1
    );
    CalcButtonTextSize("Settings", &text_w, &text_h);
    if (WidgetButton(
            (win_w - text_w) / 2.0, win_h / 2.0 + 0.25 * text_h, "Settings", 0
        )) {
        SwitchScene(SETTING_SCENE);
    }
    CalcButtonTextSize("Exit", &text_w, &text_h);
    if (WidgetButton(
            (win_w - text_w) / 2.0, win_h / 2.0 + 1.75 * text_h, "Exit", 0
        )) {
        global_app.should_quit = 1;
    }
    WidgetEnd();
    global_setting.interface_size = prev_size;
}

void StartSceneFree() {}
