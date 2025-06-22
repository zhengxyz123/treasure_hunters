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
#include "../setting.h"
#include "../ui/text.h"
#include "subscene.h"

extern GameApp global_app;
extern Setting global_setting;

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
    {SETTING_TYPE_SUBTITLE, "Display"},
    {SETTING_TYPE_OPTION, "Fullscreen", {.option = &fullscreen_data}},
    {SETTING_TYPE_SUBTITLE, "Sound"},
    {SETTING_TYPE_SLIDER, "Music volume", {.slider = &music_volume_data}},
    {SETTING_TYPE_SLIDER, "SFX volume", {.slider = &sfx_volume_data}},
    {SETTING_TYPE_OPTION, "Mute when\nlost focus", {.option = &mute_data}},
    {SETTING_TYPE_SUBTITLE, " "},
    {SETTING_TYPE_BUTTON, "Reset settings", {.button = 0}},
    {SETTING_TYPE_BUTTON, "Back", {.button = 1}}
};
TextStyle subtitle_text_style = {
    .size = 1.0,
    .color = {0, 0, 0, 255},
    .anchor = TEXT_ANCHOR_X_CENTER | TEXT_ANCHOR_Y_TOP
};
TextStyle item_text_style_normal = {
    .size = 1.0,
    .color = {0, 0, 0, 255},
    .align = TEXT_ALIGN_RIGHT,
    .anchor = TEXT_ANCHOR_X_RIGHT | TEXT_ANCHOR_Y_TOP
};
TextStyle item_text_style_active = {
    .size = 1.0,
    .color = {0, 0, 0, 128},
    .align = TEXT_ALIGN_RIGHT,
    .anchor = TEXT_ANCHOR_X_RIGHT | TEXT_ANCHOR_Y_TOP
};
TextStyle hint_style = {
    .size = 1.0,
    .color = {0, 0, 0, 255},
    .anchor = TEXT_ANCHOR_X_LEFT | TEXT_ANCHOR_Y_BOTTOM
};

void SettingSceneInit() {
    fullscreen_data = global_setting.fullscreen;
    music_volume_data.now = global_setting.music_volume;
    sfx_volume_data.now = global_setting.sfx_volume;
    mute_data = global_setting.mute_when_unfocused;
}

void SettingSceneTick() {
    int win_w, win_h;
    SDL_GetWindowSize(global_app.window, &win_w, &win_h);
    float items_h_coeff =
        2.25 * sizeof(settings_array) / sizeof(SettingItem) - 1.25;
    float max_text_w;
    item_text_style_normal.size = 1.0;
    CalcSmallTextSize(
        settings_array[3].name, &item_text_style_normal, &max_text_w, NULL
    );
    global_app.interface_size =
        0.75 * win_h / items_h_coeff / SMALL_TEXT_HEIGHT;
    if (win_w / 2.0 - 20 - global_app.interface_size * max_text_w < 0) {
        global_app.interface_size = (win_w / 2.0 - 20) / max_text_w;
    }
    if (global_app.interface_size < 1) {
        global_app.interface_size = 1;
    }
    float slider_w = max_text_w * global_app.interface_size - 20;
    float widget_y = ((float)win_h - items_h_coeff * global_app.interface_size *
                                         SMALL_TEXT_HEIGHT) /
                     2.0;
    subtitle_text_style.size = global_app.interface_size;
    item_text_style_normal.size = global_app.interface_size;
    item_text_style_active.size = global_app.interface_size;
    hint_style.size = global_app.interface_size / 1.2;
    DrawBackground();
    WidgetBegin();
    int now_widget = 0;
    int button_clicked = -1;
    for (int i = 0; i < SDL_arraysize(settings_array); ++i) {
        float text_w, text_h;
        switch (settings_array[i].type) {
            case SETTING_TYPE_BUTTON:
                CalcButtonTextSize(settings_array[i].name, &text_w, &text_h);
                if (WidgetButton(
                        (win_w - text_w) / 2.0, widget_y,
                        settings_array[i].name, 0
                    )) {
                    button_clicked = settings_array[i].data.button;
                }
                widget_y += text_h + global_app.interface_size *
                                         SMALL_TEXT_HEIGHT * 1.25;
                break;
            case SETTING_TYPE_OPTION:
                CalcSmallTextSize(
                    settings_array[i].name, &item_text_style_normal, &text_w,
                    &text_h
                );
                DrawSmallText(
                    win_w / 2.0 - 10, widget_y,
                    now_widget++ == GetCurrentWidget()
                        ? &item_text_style_active
                        : &item_text_style_normal,
                    settings_array[i].name
                );
                WidgetOption(
                    win_w / 2.0 + 10 + 4 * global_app.interface_size,
                    widget_y + (text_h -
                                global_app.interface_size * SMALL_TEXT_HEIGHT) /
                                   2,
                    settings_array[i].data.option
                );
                widget_y += text_h + global_app.interface_size *
                                         SMALL_TEXT_HEIGHT * 1.25;
                break;
            case SETTING_TYPE_SLIDER:
                CalcSmallTextSize(
                    settings_array[i].name, &item_text_style_normal, &text_w,
                    &text_h
                );
                DrawSmallText(
                    win_w / 2.0 - 10, widget_y,
                    now_widget++ == GetCurrentWidget()
                        ? &item_text_style_active
                        : &item_text_style_normal,
                    settings_array[i].name
                );
                WidgetSlider(
                    win_w / 2.0 + 10,
                    widget_y + (text_h -
                                global_app.interface_size * SMALL_TEXT_HEIGHT) /
                                   2,
                    slider_w, SMALL_TEXT_HEIGHT * global_app.interface_size,
                    settings_array[i].data.slider
                );
                widget_y += text_h + global_app.interface_size *
                                         SMALL_TEXT_HEIGHT * 1.25;
                break;
            case SETTING_TYPE_SUBTITLE:
                CalcSmallTextSize(
                    settings_array[i].name, &subtitle_text_style, &text_w,
                    &text_h
                );
                DrawSmallText(
                    win_w / 2.0, widget_y, &subtitle_text_style,
                    settings_array[i].name
                );
                widget_y += text_h + global_app.interface_size *
                                         SMALL_TEXT_HEIGHT * 1.25;
                break;
        }
    }
    WidgetEnd();
    if (global_app.joystick.available) {
        DrawSmallText(
            10, win_h - 5, &hint_style, "{1,13}{1,14}:Slider {1,1}:Back"
        );
    } else {
        DrawSmallText(10, win_h - 5, &hint_style, "ESC:Back");
    }
    if (button_clicked == 0) {
        fullscreen_data = 0;
        music_volume_data.now = 64.0;
        sfx_volume_data.now = 64.0;
        mute_data = 1;
    } else if (button_clicked == 1) {
        BackToPrevScene();
    }
    if (global_setting.fullscreen != fullscreen_data) {
        SDL_SetWindowFullscreen(global_app.window, fullscreen_data);
    }
    global_setting.fullscreen = fullscreen_data;
    if (global_setting.music_volume != (int)music_volume_data.now &&
        global_app.window_focused) {
        Mix_Volume(MUSIC_CHANNEL, (int)music_volume_data.now);
    }
    global_setting.music_volume = (int)music_volume_data.now;
    if (global_setting.sfx_volume != (int)sfx_volume_data.now &&
        global_app.window_focused) {
        Mix_Volume(SFX_CHANNEL, (int)sfx_volume_data.now);
    }
    global_setting.sfx_volume = (int)sfx_volume_data.now;
    global_setting.mute_when_unfocused = mute_data;
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
