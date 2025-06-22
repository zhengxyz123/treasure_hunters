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

/*
  The widget system supports both mouse and joystick.

  In order to avoid conflicting operations, mouse input will be ignored when
  the joystick is connected.
*/

#include "widget.h"
#include "../global.h"
#include "../resources/assets.h"
#include "../resources/loader.h"
#include "../setting.h"
#include "text.h"
#include <assert.h>
#include <stdlib.h>

extern GameApp global_app;
extern Setting global_setting;

WidgetRectList widget_list = {.size = 16, .aabb = {0, 0, 0, 0}, .data = NULL};
int mouse_in_rect = 0;
int should_update_widgets = 1;
unsigned long cooldown_time = 0;
unsigned long slider_cooldown_time = 0;

SDL_FPoint mouse_pos = {0, 0};
SDL_FPoint mouse_clicked_pos = {0, 0};
int mouse_clicked = 0;
int any_button_clicked = 0;
int any_option_clicked = 0;
int any_slider_clicked = 0;

SDL_Texture* button_texture = NULL;
SDL_Texture* slider_texture = NULL;
Mix_Chunk* click_sound = NULL;
Mix_Chunk* switch_sound = NULL;

TextStyle normal_text_style = {
    .size = 1.0,
    .anchor = TEXT_ANCHOR_X_LEFT | TEXT_ANCHOR_Y_TOP,
    .color = {0, 0, 0, 255},
    .has_shadow = 1,
    .shadow_offset = {1, 1},
    .shadow_color = {85, 85, 85, 255}
};
TextStyle hovering_text_style = {
    .size = 1.0,
    .align = TEXT_ALIGN_LEFT,
    .anchor = TEXT_ANCHOR_X_LEFT | TEXT_ANCHOR_Y_TOP,
    .color = {17, 160, 54, 255},
    .has_shadow = 1,
    .shadow_offset = {1, 1},
    .shadow_color = {4, 40, 13, 255}
};
TextStyle disabled_text_style = {
    .size = 1.0,
    .anchor = TEXT_ANCHOR_X_LEFT | TEXT_ANCHOR_Y_TOP,
    .color = {170, 170, 170, 255},
    .has_shadow = 1,
    .shadow_offset = {1, 1},
    .shadow_color = {42, 42, 42, 255}
};

void InitWidgetSystem() {
    widget_list.data = (SDL_FRect*)calloc(widget_list.size, sizeof(SDL_FRect));
    button_texture = LoadTexture(
        yellow_button_png_content, sizeof(yellow_button_png_content)
    );
    slider_texture =
        LoadTexture(slider_png_content, sizeof(slider_png_content));
    click_sound = LoadSound(click_ogg_content, sizeof(click_ogg_content));
    switch_sound = LoadSound(switch_ogg_content, sizeof(switch_ogg_content));
}

void QuitWidgetSystem() {
    free(widget_list.data);
    SDL_DestroyTexture(button_texture);
    SDL_DestroyTexture(slider_texture);
    Mix_FreeChunk(click_sound);
    Mix_FreeChunk(switch_sound);
}

void ClearWidgets() {
    widget_list.len = 0;
    widget_list.now = 0;
    widget_list.aabb = (SDL_FRect){0, 0, 0, 0};
    should_update_widgets = 1;
}

int GetCurrentWidget() {
    return widget_list.now;
}

void AppendWidget(SDL_FRect* rect) {
    if (widget_list.len + 1 >= widget_list.size) {
        widget_list.size *= 2;
        widget_list.data = (SDL_FRect*)realloc(
            widget_list.data, widget_list.size * sizeof(SDL_FRect)
        );
    }
    widget_list.data[widget_list.len++] = *rect;
    widget_list.aabb = widget_list.data[0];
    if (widget_list.len == 1) {
        return;
    }
    widget_list.aabb.w += widget_list.aabb.x;
    widget_list.aabb.h += widget_list.aabb.y;
    for (size_t i = 0; i < widget_list.len; ++i) {
        SDL_FRect rect = widget_list.data[i];
        widget_list.aabb.x = SDL_min(widget_list.aabb.x, rect.x);
        widget_list.aabb.y = SDL_min(widget_list.aabb.y, rect.y);
        widget_list.aabb.w = SDL_max(widget_list.aabb.w, rect.x + rect.w);
        widget_list.aabb.h = SDL_max(widget_list.aabb.h, rect.y + rect.h);
    }
    widget_list.aabb.w -= widget_list.aabb.x;
    widget_list.aabb.h -= widget_list.aabb.y;
}

void UpdateWidgetList() {
    if (!SDL_PointInFRect(&mouse_pos, &widget_list.aabb)) {
        mouse_in_rect = 0;
        return;
    }
    for (size_t i = 0; i < widget_list.len; ++i) {
        if (SDL_PointInFRect(&mouse_pos, &widget_list.data[i])) {
            if (!mouse_in_rect) {
                Mix_PlayChannel(SFX_CHANNEL, switch_sound, 0);
                mouse_in_rect = 1;
            }
            widget_list.now = i;
            return;
        }
    }
    mouse_in_rect = 0;
}

void HandleWidgetEvent(SDL_Event* event) {
    switch (event->type) {
        case SDL_MOUSEMOTION:
            if (!global_app.joystick.available) {
                mouse_pos = (SDL_FPoint){event->motion.x, event->motion.y};
                UpdateWidgetList();
            }
            break;
        case SDL_MOUSEBUTTONDOWN:
            if (!global_app.joystick.available) {
                if (event->button.button == SDL_BUTTON(1)) {
                    mouse_clicked = 1;
                }
                mouse_clicked_pos =
                    (SDL_FPoint){event->button.x, event->button.y};
            }
            break;
        case SDL_MOUSEBUTTONUP:
            if (!global_app.joystick.available) {
                mouse_clicked = 0;
                any_button_clicked = 0;
                any_option_clicked = 0;
                any_slider_clicked = 0;
            }
            break;
        case SDL_CONTROLLERBUTTONDOWN:
            if (event->cbutton.button == SDL_CONTROLLER_BUTTON_A) {
                mouse_clicked = 1;
                mouse_clicked_pos = mouse_pos;
            }
            break;
        case SDL_CONTROLLERBUTTONUP:
            mouse_clicked = 0;
            any_button_clicked = 0;
            any_option_clicked = 0;
            any_slider_clicked = 0;
            break;
        case SDL_WINDOWEVENT:
            if (event->window.event == SDL_WINDOWEVENT_RESIZED) {
                ClearWidgets();
            }
            break;
    }
    if (global_app.joystick.available) {
        int dir = 0;
        int down = SDL_GameControllerGetAxis(
                       global_app.joystick.device, SDL_CONTROLLER_AXIS_LEFTY
                   ) > 16384;
        down = down ||
               SDL_GameControllerGetButton(
                   global_app.joystick.device, SDL_CONTROLLER_BUTTON_DPAD_DOWN
               );
        int up = SDL_GameControllerGetAxis(
                     global_app.joystick.device, SDL_CONTROLLER_AXIS_LEFTY
                 ) < -16384;
        up = up || SDL_GameControllerGetButton(
                       global_app.joystick.device, SDL_CONTROLLER_BUTTON_DPAD_UP
                   );
        if (down && SDL_GetTicks64() > cooldown_time) {
            dir = 1;
            cooldown_time = SDL_GetTicks64() + 5 * TICK;
        } else if (up && SDL_GetTicks64() > cooldown_time) {
            dir = -1;
            cooldown_time = SDL_GetTicks64() + 5 * TICK;
        }
        size_t now =
            SDL_clamp((long)widget_list.now + dir, 0, widget_list.len - 1);
        if (widget_list.now != now) {
            Mix_PlayChannel(SFX_CHANNEL, switch_sound, 0);
        }
        widget_list.now = now;
        SDL_FRect rect = widget_list.data[widget_list.now];
        mouse_pos = (SDL_FPoint){rect.x + rect.w / 2, rect.y + rect.h / 2};
    }
}

void WidgetBegin() {
    if (global_app.joystick.available && widget_list.len > 0 &&
        widget_list.now == 0) {
        SDL_FRect rect = widget_list.data[widget_list.now];
        mouse_pos = (SDL_FPoint){rect.x + rect.w / 2, rect.y + rect.h / 2};
    }
    normal_text_style.size = global_app.interface_size;
    hovering_text_style.size = global_app.interface_size;
    disabled_text_style.size = global_app.interface_size;
}

void CalcButtonTextSize(char* str, float* w, float* h) {
    CalcSmallTextSize(str, &normal_text_style, w, h);
}

int WidgetButton(float x, float y, char* str, int disabled) {
    float text_w, text_h;
    CalcSmallTextSize(str, &normal_text_style, &text_w, &text_h);
    if (disabled) {
        DrawSmallText(x, y, &disabled_text_style, str);
        return 0;
    }
    SDL_FRect box = {x, y, text_w, text_h};
    if (should_update_widgets) {
        AppendWidget(&box);
    }
    if (SDL_PointInFRect(&mouse_pos, &box)) {
        DrawSmallText(x, y, &hovering_text_style, str);
    } else {
        DrawSmallText(x, y, &normal_text_style, str);
        return 0;
    }
    if (mouse_clicked && SDL_PointInFRect(&mouse_clicked_pos, &box)) {
        if (!any_button_clicked) {
            Mix_PlayChannel(SFX_CHANNEL, click_sound, 0);
        }
        any_button_clicked = 1;
        return 1;
    } else {
        return 0;
    }
}

int WidgetOption(float x, float y, int* data) {
    float size = normal_text_style.size;
    SDL_FRect button_dst = {
        x - 4 * size, y - 3 * size, size * (SMALL_TEXT_WIDTH + 8),
        size * (SMALL_TEXT_HEIGHT + 8)
    };
    SDL_RenderCopyF(
        global_app.renderer, button_texture, &(SDL_Rect){0, 0, 14, 14},
        &button_dst
    );
    if (should_update_widgets) {
        AppendWidget(&button_dst);
    }
    if (SDL_PointInFRect(&mouse_pos, &button_dst)) {
        DrawSmallText(
            x, y, &(TextStyle){.size = size, .color = {0, 0, 0, 128}}, "{0,%d}",
            *data == 1 ? ICON_TICK : ICON_CROSS
        );
    } else {
        DrawSmallText(
            x, y, &(TextStyle){.size = size, .color = {0, 0, 0, 255}}, "{0,%d}",
            *data == 1 ? ICON_TICK : ICON_CROSS
        );
    }
    if (mouse_clicked && SDL_PointInFRect(&mouse_clicked_pos, &button_dst) &&
        !any_option_clicked) {
        any_option_clicked = 1;
        *data = !*data;
        Mix_PlayChannel(SFX_CHANNEL, click_sound, 0);
    }
    return *data;
}

int WidgetSlider(float x, float y, float w, float h, SliderData* data) {
    assert(data != NULL);
    data->now = SDL_min(data->max, SDL_max(data->min, data->now));
    SDL_Rect slider_src[] = {{24, 3, 6, 6}, {37, 3, 10, 6}, {30, 3, 6, 6}};
    SDL_FRect slider_dst[] = {
        {x, y, h, h}, {x + h, y, w - 2 * h, h}, {x + w - h, y, h, h}
    };
    for (int i = 0; i < 3; ++i) {
        SDL_RenderCopyF(
            global_app.renderer, slider_texture, &slider_src[i], &slider_dst[i]
        );
    }
    SDL_FRect button_dst = {
        x + w * (data->now - data->min) / (data->max - data->min) - 7 * h / 12,
        y - 5 * h / 12, 7 * h / 6, 11 * h / 6
    };
    SDL_RenderCopyF(
        global_app.renderer, slider_texture, &(SDL_Rect){15, 1, 7, 11},
        &button_dst
    );
    SDL_FRect box = {x, y, w, h};
    if (should_update_widgets) {
        AppendWidget(&box);
    }
    if (global_app.joystick.available && SDL_PointInFRect(&mouse_pos, &box)) {
        data->is_dragging = 1;
    } else if (!mouse_clicked) {
        data->is_dragging = 0;
        return data->now;
    } else if (any_slider_clicked && !data->is_dragging) {
        data->is_dragging = 0;
        return data->now;
    } else if (mouse_clicked &&
               (SDL_PointInFRect(&mouse_clicked_pos, &button_dst) ||
                SDL_PointInFRect(&mouse_clicked_pos, &box))) {
        data->is_dragging = 1;
    }
    if (data->is_dragging && !global_app.joystick.available) {
        data->now = data->min + (data->max - data->min) * (mouse_pos.x - x) / w;
        data->now = SDL_min(data->max, SDL_max(data->min, data->now));
        any_slider_clicked = 1;
    }
    if (data->is_dragging && global_app.joystick.available) {
        int dir = 0;
        int right =
            SDL_GameControllerGetAxis(
                global_app.joystick.device, SDL_CONTROLLER_AXIS_TRIGGERRIGHT
            ) > 16384;
        int left =
            SDL_GameControllerGetAxis(
                global_app.joystick.device, SDL_CONTROLLER_AXIS_TRIGGERLEFT
            ) > 16384;
        if (right && SDL_GetTicks64() > slider_cooldown_time) {
            dir = 1;
            slider_cooldown_time = SDL_GetTicks64() + 10 * TICK;
            Mix_PlayChannel(SFX_CHANNEL, switch_sound, 0);
        } else if (left && SDL_GetTicks64() > slider_cooldown_time) {
            dir = -1;
            slider_cooldown_time = SDL_GetTicks64() + 10 * TICK;
            Mix_PlayChannel(SFX_CHANNEL, switch_sound, 0);
        }
        data->now = data->now + dir * (data->max - data->min) * 0.1;
        data->now = SDL_min(data->max, SDL_max(data->min, data->now));
    }
    return data->now;
}

void WidgetEnd() {
    should_update_widgets = 0;
}
