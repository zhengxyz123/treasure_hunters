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
#include "../global.h"
#include "../setting.h"
#include "../translation.h"
#include "../ui/text/text.h"
#include "background.h"
#include <SDL_mixer.h>
#include <SDL_ttf.h>

extern GameApp game_app;
extern Setting game_setting;

Scene setting_scene = {
    .init = SettingSceneInit,
    .free = SettingSceneFree,
    .tick = SettingSceneTick,
    .on_key_down = SettingSceneOnKeyDown,
    .on_cbutton_down = SettingSceneOnControllerButtonDown
};

int fullscreen_data = 0;
SliderData music_volume_data = {.min = 0.0, .max = MIX_MAX_VOLUME};
SliderData sfx_volume_data = {.min = 0.0, .max = MIX_MAX_VOLUME};
int mute_data = 0;
int lang_data = 0, prev_lang_data = 0;
char* language_list[] = {"English (United States)", "简体中文", NULL};

#if defined(__PSP__)
SettingItem settings_array[] = {
    {SETTING_TYPE_SLIDER,
     "setting_scene.music_volume",
     {.slider = &music_volume_data}},
    {SETTING_TYPE_SLIDER,
     "setting_scene.sfx_volume",
     {.slider = &sfx_volume_data}},
    {SETTING_TYPE_OPTION, "setting_scene.mute_all", {.option = &mute_data}},
    {SETTING_TYPE_BUTTON, "setting_scene.bind_keys", {.button = 0}},
    {SETTING_TYPE_BUTTON, "setting_scene.reset_settings", {.button = 1}},
    {SETTING_TYPE_BUTTON, "setting_scene.back", {.button = 2}}
};
#elif defined(__vita__)
SettingItem settings_array[] = {
    {SETTING_TYPE_COMBOBOX,
     "setting_scene.language",
     {.combobox = {.str = language_list, .data = &lang_data}}},
    {SETTING_TYPE_SLIDER,
     "setting_scene.music_volume",
     {.slider = &music_volume_data}},
    {SETTING_TYPE_SLIDER,
     "setting_scene.sfx_volume",
     {.slider = &sfx_volume_data}},
    {SETTING_TYPE_OPTION, "setting_scene.mute_all", {.option = &mute_data}},
    {SETTING_TYPE_BUTTON, "setting_scene.bind_keys", {.button = 0}},
    {SETTING_TYPE_BUTTON, "setting_scene.reset_settings", {.button = 1}},
    {SETTING_TYPE_BUTTON, "setting_scene.back", {.button = 2}}
};
#else
SettingItem settings_array[] = {
    {SETTING_TYPE_SUBTITLE, "setting_scene.display"},
    {SETTING_TYPE_COMBOBOX,
     "setting_scene.language",
     {.combobox = {.str = language_list, .data = &lang_data}}},
    {SETTING_TYPE_OPTION,
     "setting_scene.fullscreen",
     {.option = &fullscreen_data}},
    {SETTING_TYPE_SUBTITLE, "setting_scene.sound"},
    {SETTING_TYPE_SLIDER,
     "setting_scene.music_volume",
     {.slider = &music_volume_data}},
    {SETTING_TYPE_SLIDER,
     "setting_scene.sfx_volume",
     {.slider = &sfx_volume_data}},
    {SETTING_TYPE_OPTION,
     "setting_scene.mute_when_lost_focus",
     {.option = &mute_data}},
    {SETTING_TYPE_SPACE},
    {SETTING_TYPE_BUTTON, "setting_scene.bind_keys", {.button = 0}},
    {SETTING_TYPE_BUTTON, "setting_scene.reset_settings", {.button = 1}},
    {SETTING_TYPE_BUTTON, "setting_scene.back", {.button = 2}}
};
#endif

void SettingSceneInit() {
#if !defined(__PSP__) && !defined(__vita__)
    fullscreen_data = game_setting.fullscreen;
#endif
#if !defined(__PSP__)
    if (SDL_strcmp(game_setting.language, "zh_cn") == 0) {
        lang_data = 1;
    } else {
        lang_data = 0;
    }
    prev_lang_data = lang_data;
#endif
    music_volume_data.now = game_setting.music_volume;
    sfx_volume_data.now = game_setting.sfx_volume;
#if defined(__PSP__) || defined(__vita__)
    mute_data = game_setting.mute_all;
#else
    mute_data = game_setting.mute_when_unfocused;
#endif
}

void SettingSceneFree() {}

void SettingSceneTick(float dt) {
    int win_w, win_h;
    SDL_GetWindowSize(game_app.window, &win_w, &win_h);
#if defined(__PSP__)
    float space = 1.5;
    int text_size = 16, char_h = 24;
    SetFontSize(text_size);
#elif defined(__vita__)
    float space = 1.7;
    int text_size = 30, char_h = 36;
    SetFontSize(text_size);
#else
    float space = 1.7;
    int text_w1, text_w2, test_h, text_h1, text_h2;
    SetFontSize(24);
    MeasureTextSize(
        GetTransaltionText(settings_array[5].name), &text_w1, &text_h1
    );
    SetFontSize(32);
    MeasureTextSize(
        GetTransaltionText(settings_array[5].name), &text_w2, &text_h2
    );
    // calculate a perfect font size using linear function
    int text_size =
        24 + ((32 - 24) * (win_w / 3 - text_w1)) / (text_w2 - text_w1);
    SetFontSize(text_size);
    MeasureTextSize("M", NULL, &test_h);
    // cannot draw all setting items in the whole window, calculate font size
    // again
    if ((space * SDL_arraysize(settings_array) - 0.5) * test_h > 0.9 * win_h) {
        int desired_h = 0.9 * win_h /
                        ((space * SDL_arraysize(settings_array) - (space - 1)));
        text_size =
            24 + ((32 - 24) * (desired_h - text_h1)) / (text_h2 - text_h1);
        SetFontSize(text_size);
    }
    int char_h;
    MeasureTextSize("M", NULL, &char_h);
#endif
    int widget_y =
        (win_h -
         (space * SDL_arraysize(settings_array) - (space - 1)) * char_h) /
        2;
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
            CalcButtonTextSize(
                GetTransaltionText(settings_array[i].name), &text_w, &text_h
            );
            if (WidgetButton(
                    (win_w - text_w) / 2.0, widget_y,
                    GetTransaltionText(settings_array[i].name), 0
                )) {
                button_clicked = settings_array[i].data.button;
            }
            widget_y += text_h * space;
            break;
        case SETTING_TYPE_COMBOBOX:
            MeasureTextSize(
                GetTransaltionText(settings_array[i].name), &text_w, &text_h
            );
            WidgetComboBox(
                win_w / 2.0 + 10, widget_y, settings_array[i].data.combobox.str,
                settings_array[i].data.combobox.data
            );
            SetFontAnchor(TEXT_ANCHOR_X_RIGHT | TEXT_ANCHOR_Y_TOP);
            SetFontColor(0, 0, 0, WidgetIsHovering() ? 128 : 255);
            DrawText(
                win_w / 2.0 - 10, widget_y,
                GetTransaltionText(settings_array[i].name)
            );
            widget_y += text_h * space;
            break;
        case SETTING_TYPE_OPTION:
            MeasureTextSize(
                GetTransaltionText(settings_array[i].name), &text_w, &text_h
            );
            WidgetOption(
                win_w / 2.0 + 10 + text_size / 2.0,
                widget_y + (text_h - text_size) / 2.0, text_size,
                settings_array[i].data.option
            );
            SetFontAnchor(TEXT_ANCHOR_X_RIGHT | TEXT_ANCHOR_Y_TOP);
            SetFontColor(0, 0, 0, WidgetIsHovering() ? 128 : 255);
            DrawText(
                win_w / 2.0 - 10, widget_y,
                GetTransaltionText(settings_array[i].name)
            );
            widget_y += text_h * space;
            break;
        case SETTING_TYPE_SLIDER:
            MeasureTextSize(
                GetTransaltionText(settings_array[i].name), &text_w, &text_h
            );
            WidgetSlider(
                win_w / 2.0 + 10, widget_y + (text_h - text_size) / 2.0,
                slider_w, text_size, settings_array[i].data.slider
            );
            SetFontAnchor(TEXT_ANCHOR_X_RIGHT | TEXT_ANCHOR_Y_TOP);
            SetFontColor(0, 0, 0, WidgetIsHovering() ? 128 : 255);
            DrawText(
                win_w / 2.0 - 10, widget_y,
                GetTransaltionText(settings_array[i].name)
            );
            widget_y += text_h * space;
            break;
        case SETTING_TYPE_SUBTITLE:
            MeasureTextSize(
                GetTransaltionText(settings_array[i].name), &text_w, &text_h
            );
            SetFontAnchor(TEXT_ANCHOR_X_CENTER | TEXT_ANCHOR_Y_TOP);
            SetFontColor(0, 0, 0, 255);
            DrawText(
                win_w / 2.0, widget_y,
                GetTransaltionText(settings_array[i].name)
            );
            widget_y += text_h * space;
            break;
        case SETTING_TYPE_SPACE:
            MeasureTextSize(" ", NULL, &text_h);
            widget_y += text_h * space;
            break;
        }
    }
    WidgetEnd();
    SetFontAnchor(TEXT_ANCHOR_X_LEFT | TEXT_ANCHOR_Y_TOP);
    if (button_clicked == 1) {
        fullscreen_data = 0;
        music_volume_data.now = 64.0;
        sfx_volume_data.now = 64.0;
#if defined(__PSP__) || defined(__vita__)
        mute_data = 0;
#else
        mute_data = 1;
#endif
    } else if (button_clicked == 2) {
        BackToPrevScene();
    }
#if !defined(__PSP__)
    if (prev_lang_data != lang_data) {
        switch (lang_data) {
        case 0:
            SetSettingLanguage("en_us");
            SetTranslationLanguage("en_us");
            break;
        case 1:
            SetSettingLanguage("zh_cn");
            SetTranslationLanguage("zh_cn");
            break;
        }
        prev_lang_data = lang_data;
        ClearWidgets();
    }
#endif
#if defined(__PSP__) || defined(__vita__)
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
