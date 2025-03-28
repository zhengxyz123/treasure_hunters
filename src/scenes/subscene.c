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

#include "subscene.h"
#include "../resources/assets.h"
#include "../resources/loader.h"
#include "../ui/animation.h"

extern GameApp global_app;

// For DrawBackground()
SDL_Texture* background_texture = NULL;
SDL_Texture* small_cloud_texture[3] = {NULL};
float small_cloud_x = 20.0;
int small_cloud_index = 0;
SDL_Texture* big_cloud_texture = NULL;
float big_cloud_x = 0;
SDL_Texture* water_reflect_big_texture = NULL;
SDL_Rect water_reflect_big_animation_clip[] = {
    {0, 0, 170, 10}, {0, 10, 170, 10}, {0, 20, 170, 10}, {0, 30, 170, 10}
};
Animation* water_reflect_big_animation = NULL;
SDL_Texture* water_reflect_medium_texture = NULL;
SDL_Rect water_reflect_medium_animation_clip[] = {
    {0, 0, 53, 3}, {0, 3, 53, 3}, {0, 6, 53, 3}, {0, 9, 53, 3}
};
Animation* water_reflect_medium_animation = NULL;
SDL_Texture* water_reflect_small_texture = NULL;
SDL_Rect water_reflect_small_animation_clip[] = {
    {0, 0, 35, 3}, {0, 3, 35, 3}, {0, 6, 35, 3}, {0, 9, 35, 3}
};
Animation* water_reflect_small_animation = NULL;

void InitSubscene() {
    // For DrawBackground()
    background_texture = LoadTexture(
        background_sky_png_content, sizeof(background_sky_png_content)
    );
    small_cloud_texture[0] =
        LoadTexture(small_cloud1_png_content, sizeof(small_cloud1_png_content));
    small_cloud_texture[1] =
        LoadTexture(small_cloud2_png_content, sizeof(small_cloud2_png_content));
    small_cloud_texture[2] =
        LoadTexture(small_cloud3_png_content, sizeof(small_cloud3_png_content));
    big_cloud_texture =
        LoadTexture(big_cloud_png_content, sizeof(big_cloud_png_content));
    water_reflect_big_texture = LoadTexture(
        water_reflect_big_png_content, sizeof(water_reflect_big_png_content)
    );
    water_reflect_big_animation = CreateAnimation(
        water_reflect_big_texture, 10 * TICK, water_reflect_big_animation_clip,
        4
    );
    water_reflect_medium_texture = LoadTexture(
        water_reflect_medium_png_content,
        sizeof(water_reflect_medium_png_content)
    );
    water_reflect_medium_animation = CreateAnimation(
        water_reflect_medium_texture, 10 * TICK,
        water_reflect_medium_animation_clip, 4
    );
    water_reflect_small_texture = LoadTexture(
        water_reflect_small_png_content, sizeof(water_reflect_small_png_content)
    );
    water_reflect_small_animation = CreateAnimation(
        water_reflect_small_texture, 10 * TICK,
        water_reflect_small_animation_clip, 4
    );
}

void DrawBackground() {
    int win_w, win_h, img_w, img_h;
    SDL_GetWindowSize(global_app.window, &win_w, &win_h);
    SDL_RenderCopyF(global_app.renderer, background_texture, NULL, NULL);
    // Draw small clouds
    SDL_QueryTexture(
        small_cloud_texture[small_cloud_index], NULL, NULL, &img_w, &img_h
    );
    float small_cloud_scale = 0.31 * win_h / (img_h + 1);
    small_cloud_x += 2;
    SDL_FRect small_cloud_dst = {
        small_cloud_x, 0.31 * win_h - img_h * small_cloud_scale,
        img_w * small_cloud_scale, img_h * small_cloud_scale
    };
    SDL_RenderCopyF(
        global_app.renderer, small_cloud_texture[small_cloud_index], NULL,
        &small_cloud_dst
    );
    if (small_cloud_x > win_w) {
        small_cloud_index = rand() % 3;
        SDL_QueryTexture(
            small_cloud_texture[small_cloud_index], NULL, NULL, &img_w, NULL
        );
        small_cloud_x = -img_w * small_cloud_scale - rand() % 50 - 50;
    }
    // Draw big cloud
    SDL_QueryTexture(big_cloud_texture, NULL, NULL, &img_w, &img_h);
    float big_cloud_scale = 0.67 * win_h / (img_h + 1);
    big_cloud_x += 0.5;
    while (big_cloud_x > -img_w * big_cloud_scale) {
        big_cloud_x -= img_w * big_cloud_scale;
    }
    SDL_FRect big_cloud_dst = {
        big_cloud_x, 0.675 * win_h - img_h * big_cloud_scale,
        img_w * big_cloud_scale, img_h * big_cloud_scale
    };
    for (float x = big_cloud_x; x < win_w; x += img_w * big_cloud_scale) {
        big_cloud_dst.x = x;
        SDL_RenderCopyF(
            global_app.renderer, big_cloud_texture, NULL, &big_cloud_dst
        );
    }
    // Draw water reflects
    float anime_w[] = {
        water_reflect_big_animation_clip[0].w,
        water_reflect_medium_animation_clip[0].w,
        water_reflect_small_animation_clip[0].w
    };
    float water_reflect_scale =
        win_w / (anime_w[0] + anime_w[1] + anime_w[2] + win_w / 20.0);
    DrawAnimation(
        water_reflect_medium_animation, win_w / 20.0, 0.69 * win_h,
        water_reflect_scale
    );
    DrawAnimation(
        water_reflect_big_animation,
        win_w / 2.0 +
            (anime_w[1] - anime_w[2] - anime_w[0]) * water_reflect_scale / 2.0,
        0.69 * win_h, water_reflect_scale
    );
    DrawAnimation(
        water_reflect_small_animation,
        0.95 * win_w - anime_w[2] * water_reflect_scale, 0.69 * win_h,
        water_reflect_scale
    );
}

void FreeSubscene() {
    // For DrawBackground()
    SDL_DestroyTexture(background_texture);
    for (int i = 0; i < SDL_arraysize(small_cloud_texture); ++i) {
        SDL_DestroyTexture(small_cloud_texture[i]);
    }
    SDL_DestroyTexture(big_cloud_texture);
    FreeAnimation(water_reflect_big_animation);
    FreeAnimation(water_reflect_medium_animation);
    FreeAnimation(water_reflect_small_animation);
    SDL_DestroyTexture(water_reflect_big_texture);
    SDL_DestroyTexture(water_reflect_medium_texture);
    SDL_DestroyTexture(water_reflect_small_texture);
}
