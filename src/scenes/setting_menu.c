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

SliderData ui_size_data = {.min = 1.0, .max = 5.0};
int auto_detect_size_data = 0;
int fullscreen_data = 0;
SliderData music_volume_data = {.min = 0.0, .max = 128.0};
SliderData sfx_volume_data = {.min = 0.0, .max = 128.0};
int slience_data = 0;
SettingItem settings_array[] = {
    {SETTING_TYPE_SUBTITLE, "Display"},
    {SETTING_TYPE_SLIDER, "Interface size", {.slider = &ui_size_data}},
    {SETTING_TYPE_OPTION, "Auto detect size", {.option = &auto_detect_size_data}
    },
    {SETTING_TYPE_OPTION, "Fullscreen", {.option = &fullscreen_data}},
    {SETTING_TYPE_SUBTITLE, "Sound"},
    {SETTING_TYPE_SLIDER, "Music volume", {.slider = &music_volume_data}},
    {SETTING_TYPE_SLIDER, "SFX volume", {.slider = &sfx_volume_data}},
    {SETTING_TYPE_OPTION, "Slience", {.option = &slience_data}},
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
    .anchor = TEXT_ANCHOR_X_RIGHT | TEXT_ANCHOR_Y_TOP
};
TextStyle item_text_style_active = {
    .size = 1.0,
    .color = {0, 0, 0, 128},
    .anchor = TEXT_ANCHOR_X_RIGHT | TEXT_ANCHOR_Y_TOP
};

void SettingSceneInit() {
    ui_size_data.now = global_setting.interface_size;
    auto_detect_size_data = global_setting.auto_detect_size;
    fullscreen_data = global_setting.fullscreen;
    music_volume_data.now = global_setting.music_volume;
    sfx_volume_data.now = global_setting.sfx_volume;
    slience_data = global_setting.slience;
}

void SettingSceneTick() {
    int win_w, win_h;
    SDL_GetWindowSize(global_app.window, &win_w, &win_h);
    float items_h_coeff =
        2.25 * sizeof(settings_array) / sizeof(SettingItem) - 1.25;
    float max_text_w;
    item_text_style_normal.size = 1.0;
    CalcSmallTextSize(
        settings_array[2].name, &item_text_style_normal, &max_text_w, NULL
    );
    if (global_setting.auto_detect_size) {
        global_setting.interface_size =
            0.75 * win_h / items_h_coeff / SMALL_TEXT_HEIGHT;
        if (win_w / 2.0 - 20 - global_setting.interface_size * max_text_w < 0) {
            global_setting.interface_size = (win_w / 2.0 - 20) / max_text_w;
        }
        if (global_setting.interface_size < 2) {
            global_setting.interface_size = 2;
        }
    }
    float slider_w = max_text_w * global_setting.interface_size - 20;
    float widget_y =
        ((float)win_h -
         items_h_coeff * global_setting.interface_size * SMALL_TEXT_HEIGHT) /
        2.0;
    subtitle_text_style.size = global_setting.interface_size;
    item_text_style_normal.size = global_setting.interface_size;
    item_text_style_active.size = global_setting.interface_size;
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
                widget_y += text_h * 2.25;
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
                    win_w / 2.0 + 10 + 4 * global_setting.interface_size,
                    widget_y, settings_array[i].data.option
                );
                widget_y += text_h * 2.25;
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
                    win_w / 2.0 + 10, widget_y, slider_w, text_h,
                    settings_array[i].data.slider
                );
                widget_y += text_h * 2.25;
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
                widget_y += text_h * 2.25;
                break;
        }
    }
    WidgetEnd();
    if (button_clicked == 0) {
        ui_size_data.now = 2.5;
        auto_detect_size_data = 1;
        fullscreen_data = 0;
        music_volume_data.now = 64.0;
        sfx_volume_data.now = 64.0;
        slience_data = 0;
    } else if (button_clicked == 1) {
        BackToPrevScene();
    }
    if (!ui_size_data.is_dragging) {
        global_setting.interface_size = ui_size_data.now;
    }
    global_setting.auto_detect_size = auto_detect_size_data;
    global_setting.fullscreen = fullscreen_data;
    global_setting.music_volume = (int)music_volume_data.now;
    global_setting.sfx_volume = (int)sfx_volume_data.now;
    global_setting.slience = slience_data;
    SDL_SetWindowFullscreen(global_app.window, global_setting.fullscreen);
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
