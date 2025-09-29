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
#include "../resources/loader.h"
#include "frametimer.h"
#include "text/text.h"
#include "text/bitmap.h"
#include <assert.h>
#include <stdlib.h>

extern GameApp game_app;

struct {
    struct {
        size_t size;
        size_t len;
        size_t now;
        SDL_FRect aabb;
        SDL_FRect* data;
    } widget_list;
    int should_update_widgets;
    int mouse_in_rect;
    SDL_FPoint mouse_pos;
    SDL_FPoint mouse_clicked_pos;
    int mouse_clicked;
    int any_button_clicked;
    int any_option_clicked;
    int any_slider_clicked;
    float joystick_cooldown_time;
    float slider_cooldown_time;
    int is_hovering;
} ctx;

SDL_Texture* button_texture = NULL;
SDL_Texture* slider_texture = NULL;
Mix_Chunk* click_sound = NULL;
Mix_Chunk* switch_sound = NULL;

BitmapTextStyle normal_text_style = {
    .size = 1.0,
    .anchor = TEXT_ANCHOR_X_LEFT | TEXT_ANCHOR_Y_TOP,
    .color = {0, 0, 0, 255},
    .has_shadow = 1,
    .shadow_offset = {1, 1},
    .shadow_color = {85, 85, 85, 255}
};
BitmapTextStyle hovering_text_style = {
    .size = 1.0,
    .align = TEXT_ALIGN_LEFT,
    .anchor = TEXT_ANCHOR_X_LEFT | TEXT_ANCHOR_Y_TOP,
    .color = {17, 160, 54, 255},
    .has_shadow = 1,
    .shadow_offset = {1, 1},
    .shadow_color = {4, 40, 13, 255}
};
BitmapTextStyle disabled_text_style = {
    .size = 1.0,
    .anchor = TEXT_ANCHOR_X_LEFT | TEXT_ANCHOR_Y_TOP,
    .color = {170, 170, 170, 255},
    .has_shadow = 1,
    .shadow_offset = {1, 1},
    .shadow_color = {42, 42, 42, 255}
};

void InitWidget() {
    ctx.widget_list.size = 16;
    ctx.widget_list.data =
        (SDL_FRect*)calloc(ctx.widget_list.size, sizeof(SDL_FRect));
    ctx.should_update_widgets = 1;
    button_texture = LoadTexture("images/ui/yellow_panel.png");
    slider_texture = LoadTexture("images/ui/slider.png");
    click_sound = LoadSound("sounds/click.ogg");
    switch_sound = LoadSound("sounds/switch.ogg");
}

void QuitWidget() {
    free(ctx.widget_list.data);
    SDL_DestroyTexture(button_texture);
    SDL_DestroyTexture(slider_texture);
    Mix_FreeChunk(click_sound);
    Mix_FreeChunk(switch_sound);
}

void ClearWidgets() {
    ctx.widget_list.len = 0;
    ctx.widget_list.now = 0;
    ctx.widget_list.aabb = (SDL_FRect){0, 0, 0, 0};
    ctx.should_update_widgets = 1;
}

void ClearStates() {
    ctx.is_hovering = 0;
}

void AppendWidget(SDL_FRect* rect) {
    if (ctx.widget_list.len + 1 >= ctx.widget_list.size) {
        ctx.widget_list.size *= 2;
        ctx.widget_list.data = (SDL_FRect*)realloc(
            ctx.widget_list.data, ctx.widget_list.size * sizeof(SDL_FRect)
        );
    }
    ctx.widget_list.data[ctx.widget_list.len++] = *rect;
    ctx.widget_list.aabb = ctx.widget_list.data[0];
    if (ctx.widget_list.len == 1) {
        return;
    }
    ctx.widget_list.aabb.w += ctx.widget_list.aabb.x;
    ctx.widget_list.aabb.h += ctx.widget_list.aabb.y;
    for (size_t i = 0; i < ctx.widget_list.len; ++i) {
        SDL_FRect rect = ctx.widget_list.data[i];
        ctx.widget_list.aabb.x = SDL_min(ctx.widget_list.aabb.x, rect.x);
        ctx.widget_list.aabb.y = SDL_min(ctx.widget_list.aabb.y, rect.y);
        ctx.widget_list.aabb.w =
            SDL_max(ctx.widget_list.aabb.w, rect.x + rect.w);
        ctx.widget_list.aabb.h =
            SDL_max(ctx.widget_list.aabb.h, rect.y + rect.h);
    }
    ctx.widget_list.aabb.w -= ctx.widget_list.aabb.x;
    ctx.widget_list.aabb.h -= ctx.widget_list.aabb.y;
}

void UpdateWidgetList() {
    if (!SDL_PointInFRect(&ctx.mouse_pos, &ctx.widget_list.aabb)) {
        ctx.mouse_in_rect = 0;
        return;
    }
    for (size_t i = 0; i < ctx.widget_list.len; ++i) {
        if (SDL_PointInFRect(&ctx.mouse_pos, &ctx.widget_list.data[i])) {
            if (!ctx.mouse_in_rect) {
                Mix_PlayChannel(SFX_CHANNEL, switch_sound, 0);
                ctx.mouse_in_rect = 1;
            }
            ctx.widget_list.now = i;
            return;
        }
    }
    ctx.mouse_in_rect = 0;
}

void HandleWidgetEvent(SDL_Event* event) {
    switch (event->type) {
    case SDL_MOUSEMOTION:
        if (!game_app.joystick.available) {
            ctx.mouse_pos = (SDL_FPoint){event->motion.x, event->motion.y};
            UpdateWidgetList();
        }
        break;
    case SDL_MOUSEBUTTONDOWN:
        if (!game_app.joystick.available) {
            if (event->button.button == SDL_BUTTON(1)) {
                ctx.mouse_clicked = 1;
            }
            ctx.mouse_clicked_pos =
                (SDL_FPoint){event->button.x, event->button.y};
        }
        break;
    case SDL_MOUSEBUTTONUP:
        if (!game_app.joystick.available) {
            ctx.mouse_clicked = 0;
            ctx.any_button_clicked = 0;
            ctx.any_option_clicked = 0;
            ctx.any_slider_clicked = 0;
        }
        break;
    case SDL_CONTROLLERBUTTONDOWN:
        if (event->cbutton.button == SDL_CONTROLLER_BUTTON_A) {
            ctx.mouse_clicked = 1;
            ctx.mouse_clicked_pos = ctx.mouse_pos;
        }
        break;
    case SDL_CONTROLLERBUTTONUP:
        ctx.mouse_clicked = 0;
        ctx.any_button_clicked = 0;
        ctx.any_option_clicked = 0;
        ctx.any_slider_clicked = 0;
        break;
    case SDL_WINDOWEVENT:
        if (event->window.event == SDL_WINDOWEVENT_RESIZED) {
            ClearWidgets();
        }
        break;
    }
}

void TickWidgets(float dt) {
    if (!game_app.joystick.available || ctx.widget_list.len == 0) {
        return;
    }
    int dir = 0;
    int down = SDL_GameControllerGetAxis(
                   game_app.joystick.device, SDL_CONTROLLER_AXIS_LEFTY
               ) > 16384;
    down = down || SDL_GameControllerGetButton(
                       game_app.joystick.device, SDL_CONTROLLER_BUTTON_DPAD_DOWN
                   );
    int up = SDL_GameControllerGetAxis(
                 game_app.joystick.device, SDL_CONTROLLER_AXIS_LEFTY
             ) < -16384;
    up = up || SDL_GameControllerGetButton(
                   game_app.joystick.device, SDL_CONTROLLER_BUTTON_DPAD_UP
               );
    ctx.joystick_cooldown_time -= dt;
    if (down && ctx.joystick_cooldown_time < 0) {
        dir = 1;
        ctx.joystick_cooldown_time = 0.12;
    } else if (up && ctx.joystick_cooldown_time < 0) {
        dir = -1;
        ctx.joystick_cooldown_time = 0.12;
    }
    size_t now =
        SDL_clamp((long)ctx.widget_list.now + dir, 0, ctx.widget_list.len - 1);
    if (ctx.widget_list.now != now) {
        Mix_PlayChannel(SFX_CHANNEL, switch_sound, 0);
    }
    ctx.widget_list.now = now;
    SDL_FRect rect = ctx.widget_list.data[ctx.widget_list.now];
    ctx.mouse_pos = (SDL_FPoint){rect.x + rect.w / 2, rect.y + rect.h / 2};
}

void WidgetBegin() {
    if (game_app.joystick.available && ctx.widget_list.len > 0 &&
        ctx.widget_list.now == 0) {
        SDL_FRect rect = ctx.widget_list.data[ctx.widget_list.now];
        ctx.mouse_pos = (SDL_FPoint){rect.x + rect.w / 2, rect.y + rect.h / 2};
    }
    normal_text_style.size = game_app.interface_size;
    hovering_text_style.size = game_app.interface_size;
    disabled_text_style.size = game_app.interface_size;
}

int WidgetIsHovering() {
    return ctx.is_hovering;
}

void CalcButtonTextSize(char* str, float* w, float* h) {
    CalcSmallBitmapTextSize(str, &normal_text_style, w, h);
}

int WidgetButton(float x, float y, char* str, int disabled) {
    ClearStates();
    // draw widget
    float text_w, text_h;
    CalcSmallBitmapTextSize(str, &normal_text_style, &text_w, &text_h);
    if (disabled) {
        DrawSmallBitmapText(x, y, &disabled_text_style, str);
        return 0;
    }
    // handle events
    SDL_FRect box = {x, y, text_w, text_h};
    if (ctx.should_update_widgets) {
        AppendWidget(&box);
    }
    if (SDL_PointInFRect(&ctx.mouse_pos, &box)) {
        DrawSmallBitmapText(x, y, &hovering_text_style, str);
        ctx.is_hovering = 1;
    } else {
        DrawSmallBitmapText(x, y, &normal_text_style, str);
        return 0;
    }
    if (ctx.mouse_clicked && SDL_PointInFRect(&ctx.mouse_clicked_pos, &box)) {
        if (!ctx.any_button_clicked) {
            Mix_PlayChannel(SFX_CHANNEL, click_sound, 0);
        }
        ctx.any_button_clicked = 1;
        return 1;
    } else {
        return 0;
    }
}

int WidgetOption(float x, float y, int* data) {
    ClearStates();
    // draw widget
    float size = normal_text_style.size;
    SDL_FRect button_dst = {
        x - 4 * size, y - 3 * size, size * (SMALL_TEXT_WIDTH + 8),
        size * (SMALL_TEXT_HEIGHT + 8)
    };
    SDL_RenderCopyF(
        game_app.renderer, button_texture, &(SDL_Rect){0, 0, 14, 14},
        &button_dst
    );
    if (ctx.should_update_widgets) {
        AppendWidget(&button_dst);
    }
    // handle events
    if (SDL_PointInFRect(&ctx.mouse_pos, &button_dst)) {
        DrawSmallBitmapText(
            x, y, &(BitmapTextStyle){.size = size, .color = {0, 0, 0, 128}}, "{0,%d}",
            *data == 1 ? ICON_TICK : ICON_CROSS
        );
        ctx.is_hovering = 1;
    } else {
        DrawSmallBitmapText(
            x, y, &(BitmapTextStyle){.size = size, .color = {0, 0, 0, 255}}, "{0,%d}",
            *data == 1 ? ICON_TICK : ICON_CROSS
        );
    }
    if (ctx.mouse_clicked &&
        SDL_PointInFRect(&ctx.mouse_clicked_pos, &button_dst) &&
        !ctx.any_option_clicked) {
        ctx.any_option_clicked = 1;
        *data = !*data;
        Mix_PlayChannel(SFX_CHANNEL, click_sound, 0);
    }
    return *data;
}

int WidgetSlider(float x, float y, float w, float h, SliderData* data) {
    assert(data != NULL);
    data->now = SDL_min(data->max, SDL_max(data->min, data->now));
    ClearStates();
    // draw widget
    SDL_Rect slider_src[] = {{24, 3, 6, 6}, {37, 3, 10, 6}, {30, 3, 6, 6}};
    SDL_FRect slider_dst[] = {
        {x, y, h, h}, {x + h, y, w - 2 * h, h}, {x + w - h, y, h, h}
    };
    for (int i = 0; i < 3; ++i) {
        SDL_RenderCopyF(
            game_app.renderer, slider_texture, &slider_src[i], &slider_dst[i]
        );
    }
    SDL_FRect button_dst = {
        x + w * (data->now - data->min) / (data->max - data->min) - 7 * h / 12,
        y - 5 * h / 12, 7 * h / 6, 11 * h / 6
    };
    SDL_RenderCopyF(
        game_app.renderer, slider_texture, &(SDL_Rect){15, 1, 7, 11},
        &button_dst
    );
    // handle events
    SDL_FRect box = {x, y, w, h};
    if (ctx.should_update_widgets) {
        AppendWidget(&box);
    }
    if (SDL_PointInFRect(&ctx.mouse_pos, &box)) {
        ctx.is_hovering = 1;
    }
    if (game_app.joystick.available && SDL_PointInFRect(&ctx.mouse_pos, &box)) {
        data->is_dragging = 1;
    } else if (!ctx.mouse_clicked) {
        data->is_dragging = 0;
        return data->now;
    } else if (ctx.any_slider_clicked && !data->is_dragging) {
        data->is_dragging = 0;
        return data->now;
    } else if (ctx.mouse_clicked &&
               (SDL_PointInFRect(&ctx.mouse_clicked_pos, &button_dst) ||
                SDL_PointInFRect(&ctx.mouse_clicked_pos, &box))) {
        data->is_dragging = 1;
    }
    if (data->is_dragging && !game_app.joystick.available) {
        data->now =
            data->min + (data->max - data->min) * (ctx.mouse_pos.x - x) / w;
        data->now = SDL_min(data->max, SDL_max(data->min, data->now));
        ctx.any_slider_clicked = 1;
    }
    if (data->is_dragging && game_app.joystick.available) {
        int dir = 0;
        int right = SDL_GameControllerGetButton(
            game_app.joystick.device, SDL_CONTROLLER_BUTTON_DPAD_RIGHT
        );
        int left = SDL_GameControllerGetButton(
            game_app.joystick.device, SDL_CONTROLLER_BUTTON_DPAD_LEFT
        );
        ctx.slider_cooldown_time -= frametimer_delta_time(game_app.timer);
        if (right && ctx.slider_cooldown_time < 0) {
            dir = 1;
            ctx.slider_cooldown_time = 0.15;
            if (data->now + dir < data->max) {
                Mix_PlayChannel(SFX_CHANNEL, switch_sound, 0);
            }
        } else if (left && ctx.slider_cooldown_time < 0) {
            dir = -1;
            ctx.slider_cooldown_time = 0.15;
            if (data->now + dir > data->min) {
                Mix_PlayChannel(SFX_CHANNEL, switch_sound, 0);
            }
        }
        data->now = data->now + dir * (data->max - data->min) * 0.1;
        data->now = SDL_min(data->max, SDL_max(data->min, data->now));
    }
    return data->now;
}

void WidgetEnd() {
    ctx.should_update_widgets = 0;
}
