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

#include "setting_menu.h"
#include "../translation.h"
#include "../global.h"
#include "../setting.h"
#include "../ui/text/text.h"
#include "../ui/text/ttf.h"
#include "background.h"
#include <SDL_mixer.h>
#include <SDL_ttf.h>

extern GameApp game_app;
extern Setting game_setting;

Scene setting_scene = {
    .init = SettingSceneInit,
    .tick = SettingSceneTick,
    .free = SettingSceneFree,
    .on_key_down = SettingSceneOnKeyDown,
    .on_cbutton_down = SettingSceneOnControllerButtonDown
};

int fullscreen_data = 0;
SliderData music_volume_data = {.min = 0.0, .max = MIX_MAX_VOLUME};
SliderData sfx_volume_data = {.min = 0.0, .max = MIX_MAX_VOLUME};
int mute_data = 0;
SettingItem settings_array[] = {
#if !defined(__PSP__)
    {SETTING_TYPE_SUBTITLE, "setting_scene.display"},
    {SETTING_TYPE_OPTION, "setting_scene.fullscreen", {.option = &fullscreen_data}},
    {SETTING_TYPE_SUBTITLE, "setting_scene.sound"},
#endif
    {SETTING_TYPE_SLIDER, "setting_scene.music_volume", {.slider = &music_volume_data}},
    {SETTING_TYPE_SLIDER, "setting_scene.sfx_volume", {.slider = &sfx_volume_data}},
#if defined(__PSP__)
    {SETTING_TYPE_OPTION, "setting_scene.mute_all", {.option = &mute_data}},
#else
    {SETTING_TYPE_OPTION, "setting_scene.mute_when_lost_focus", {.option = &mute_data}},
    {SETTING_TYPE_SPACE},
#endif
    {SETTING_TYPE_BUTTON, "setting_scene.bind_keys", {.button = 0}},
    {SETTING_TYPE_BUTTON, "setting_scene.reset_settings", {.button = 1}},
    {SETTING_TYPE_BUTTON, "setting_scene.back", {.button = 2}}
};

void SettingSceneInit() {
#if !defined(__PSP__)
    fullscreen_data = game_setting.fullscreen;
#endif
    music_volume_data.now = game_setting.music_volume;
    sfx_volume_data.now = game_setting.sfx_volume;
#if defined(__PSP__)
    mute_data = game_setting.mute_all;
#else
    mute_data = game_setting.mute_when_unfocused;
#endif
}

void SettingSceneTick(float dt) {
    int win_w, win_h;
    SDL_GetWindowSize(game_app.window, &win_w, &win_h);
    SetFontStyle(TTF_STYLE_BOLD);
#if defined(__PSP__)
    int text_size = 18, char_h = 26;
#else
    int text_w1, text_w2, test_h, text_h1, text_h2;
    SetFontSize(24);
    MeasureTextSize(TransaltionGetText(settings_array[5].name), &text_w1, &text_h1);
    SetFontSize(32);
    MeasureTextSize(TransaltionGetText(settings_array[5].name), &text_w2, &text_h2);
    // calculate a perfect font size using linear function
    int text_size =
        24 + ((32 - 24) * (win_w / 3 - text_w1)) / (text_w2 - text_w1);
    SetFontSize(text_size);
    MeasureTextSize("M", NULL, &test_h);
    // cannot draw all setting items in the whole window, calculate font size
    // again
    if ((1.5 * SDL_arraysize(settings_array) - 0.5) * test_h > 0.98 * win_h) {
        int desired_h =
            0.98 * win_h / ((1.5 * SDL_arraysize(settings_array) - 0.5));
        text_size =
            24 + ((32 - 24) * (desired_h - text_h1)) / (text_h2 - text_h1);
        SetFontSize(text_size);
    }
    int char_h;
    MeasureTextSize("M", NULL, &char_h);
#endif
    int widget_y =
        (win_h - (1.5 * SDL_arraysize(settings_array) - 0.5) * char_h) / 2;
#if defined(__PSP__)
    int slider_w = 200;
#else
    int slider_w = win_w / 4 + 100;
#endif
    DrawBackground(dt);
    WidgetBegin();
    int button_clicked = -1;
    for (int i = 0; i < SDL_arraysize(settings_array); ++i) {
        int text_w, text_h;
        switch (settings_array[i].type) {
        case SETTING_TYPE_BUTTON:
            MeasureTextSize(TransaltionGetText(settings_array[i].name), &text_w, &text_h);
            if (WidgetButton(
                    (win_w - text_w) / 2.0, widget_y, TransaltionGetText(settings_array[i].name), 0
                )) {
                button_clicked = settings_array[i].data.button;
            }
            widget_y += text_h * 1.5;
            break;
        case SETTING_TYPE_OPTION:
            MeasureTextSize(TransaltionGetText(settings_array[i].name), &text_w, &text_h);
            WidgetOption(
                win_w / 2.0 + 10 + text_size / 2.0,
                widget_y + (text_h - text_size) / 2.0, text_size,
                settings_array[i].data.option
            );
            SetFontAnchor(TEXT_ANCHOR_X_RIGHT | TEXT_ANCHOR_Y_TOP);
            SetFontColor(0, 0, 0, WidgetIsHovering() ? 128 : 255);
            DrawText(win_w / 2.0 - 10, widget_y, TransaltionGetText(settings_array[i].name));
            widget_y += text_h * 1.5;
            break;
        case SETTING_TYPE_SLIDER:
            MeasureTextSize(TransaltionGetText(settings_array[i].name), &text_w, &text_h);
            WidgetSlider(
                win_w / 2.0 + 10, widget_y + (text_h - text_size) / 2.0,
                slider_w, text_size, settings_array[i].data.slider
            );
            SetFontAnchor(TEXT_ANCHOR_X_RIGHT | TEXT_ANCHOR_Y_TOP);
            SetFontColor(0, 0, 0, WidgetIsHovering() ? 128 : 255);
            DrawText(win_w / 2.0 - 10, widget_y, TransaltionGetText(settings_array[i].name));
            widget_y += text_h * 1.5;
            break;
        case SETTING_TYPE_SUBTITLE:
            MeasureTextSize(TransaltionGetText(settings_array[i].name), &text_w, &text_h);
            SetFontAnchor(TEXT_ANCHOR_X_CENTER | TEXT_ANCHOR_Y_TOP);
            SetFontColor(0, 0, 0, 255);
            DrawText(win_w / 2.0, widget_y, TransaltionGetText(settings_array[i].name));
            widget_y += text_h * 1.5;
            break;
        case SETTING_TYPE_SPACE:
            MeasureTextSize(" ", NULL, &text_h);
            widget_y += text_h * 1.5;
            break;
        }
    }
    WidgetEnd();
    SetFontAnchor(TEXT_ANCHOR_X_LEFT | TEXT_ANCHOR_Y_TOP);
    SetFontStyle(TTF_STYLE_NORMAL);
    if (button_clicked == 1) {
        fullscreen_data = 0;
        music_volume_data.now = 64.0;
        sfx_volume_data.now = 64.0;
#if defined(__PSP__)
        mute_data = 0;
#else
        mute_data = 1;
#endif
    } else if (button_clicked == 2) {
        BackToPrevScene();
    }
#if defined(__PSP__)
    if (game_setting.music_volume != (int)music_volume_data.now &&
        !game_setting.mute_all) {
        Mix_Volume(MUSIC_CHANNEL, (int)music_volume_data.now);
    }
    game_setting.music_volume = (int)music_volume_data.now;
    if (game_setting.sfx_volume != (int)sfx_volume_data.now &&
        !game_setting.mute_all) {
        Mix_Volume(SFX_CHANNEL, (int)sfx_volume_data.now);
    }
    game_setting.sfx_volume = (int)sfx_volume_data.now;
    if (game_setting.mute_all != mute_data) {
        Mix_Volume(MUSIC_CHANNEL, mute_data ? 0 : game_setting.music_volume);
        Mix_Volume(SFX_CHANNEL, mute_data ? 0 : game_setting.sfx_volume);
    }
    game_setting.mute_all = mute_data;
#else
    if (game_setting.fullscreen != fullscreen_data) {
        SDL_SetWindowFullscreen(game_app.window, fullscreen_data);
    }
    game_setting.fullscreen = fullscreen_data;
    if (game_setting.music_volume != (int)music_volume_data.now &&
        game_app.window_focused) {
        Mix_Volume(MUSIC_CHANNEL, (int)music_volume_data.now);
    }
    game_setting.music_volume = (int)music_volume_data.now;
    if (game_setting.sfx_volume != (int)sfx_volume_data.now &&
        game_app.window_focused) {
        Mix_Volume(SFX_CHANNEL, (int)sfx_volume_data.now);
    }
    game_setting.sfx_volume = (int)sfx_volume_data.now;
    game_setting.mute_when_unfocused = mute_data;
#endif
}

void SettingSceneFree() {}

void SettingSceneOnKeyDown(SDL_KeyCode key) {
    switch (key) {
    case SDLK_ESCAPE:
        BackToPrevScene();
        break;
    default:
        break;
    }
}

void SettingSceneOnControllerButtonDown(int button) {
    if (button == SDL_CONTROLLER_BUTTON_B) {
        BackToPrevScene();
    }
}
