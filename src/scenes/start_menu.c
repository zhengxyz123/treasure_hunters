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
#include "../global.h"
#include "../translation.h"
#include "../ui/text/bitmap.h"
#include "../ui/text/text.h"
#include "../ui/text/ttf.h"
#include "../ui/widget.h"
#include "background.h"

extern GameApp game_app;

Scene start_scene = {
    .init = StartSceneInit, .tick = StartSceneTick, .free = StartSceneFree
};
BitmapTextStyle title_text_style = {
    1.0, 2, 4, TEXT_ALIGN_CENTER, TEXT_ANCHOR_X_CENTER | TEXT_ANCHOR_Y_CENTER
};

void StartSceneInit() {}

void StartSceneTick(float dt) {
    int win_w, win_h;
    SDL_GetWindowSize(game_app.window, &win_w, &win_h);
#if defined(__PSP__)
    title_text_style.size = 2;
    SetFontSize(18);
#else
    title_text_style.size =
        0.11 * win_h / 15 * 96 > win_w ? 0.98 * win_w : 0.11 * win_h / 15;
    int text_size = 2.5 * win_h / (40 * 1.2);
    if (text_size < 16) {
        text_size = 16;
    }
    SetFontSize(text_size);
#endif
    DrawBackground(dt);
    DrawBigBitmapText(
        win_w / 2.0, 0.2 * win_h, &title_text_style, "TREASURE\nHUNTERS"
    );
    int text_w, text_h;
    WidgetBegin();
    CalcButtonTextSize(
        TransaltionGetText("start_scene.new_game"), &text_w, &text_h
    );
    if (WidgetButton(
            (win_w - text_w) / 2.0, win_h / 2.0 - 1.7 * text_h,
            TransaltionGetText("start_scene.new_game"), 0
        )) {
        SwitchScene(WORLD_SCENE);
    }
    CalcButtonTextSize(
        TransaltionGetText("start_scene.continue_game"), &text_w, &text_h
    );
    WidgetButton(
        (win_w - text_w) / 2.0, win_h / 2.0 - 0.6 * text_h,
        TransaltionGetText("start_scene.continue_game"), 1
    );
    CalcButtonTextSize(
        TransaltionGetText("start_scene.settings"), &text_w, &text_h
    );
    if (WidgetButton(
            (win_w - text_w) / 2.0, win_h / 2.0 + 0.5 * text_h,
            TransaltionGetText("start_scene.settings"), 0
        )) {
        SwitchScene(SETTING_SCENE);
    }
    CalcButtonTextSize(
        TransaltionGetText("start_scene.credits"), &text_w, &text_h
    );
    WidgetButton(
        (win_w - text_w) / 2.0, win_h / 2.0 + 1.6 * text_h,
        TransaltionGetText("start_scene.credits"), 0
    );
    CalcButtonTextSize(
        TransaltionGetText("start_scene.exit"), &text_w, &text_h
    );
    if (WidgetButton(
            (win_w - text_w) / 2.0, win_h / 2.0 + 2.7 * text_h,
            TransaltionGetText("start_scene.exit"), 0
        )) {
        game_app.should_quit = 1;
    }
    WidgetEnd();
}

void StartSceneFree() {}
