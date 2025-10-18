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

#include "background.h"
#include "../global.h"
#include "../image/image.h"
#include "../resource/loader.h"

extern GameApp game_app;

SDL_Texture* background_texture = NULL;
SDL_Texture* small_cloud_texture[3] = {NULL, NULL, NULL};
float small_cloud_x = 20.0;
int small_cloud_index = 0;
SDL_Texture* big_cloud_texture = NULL;
float big_cloud_x = 0;
SDL_Texture* water_reflect_big_texture = NULL;
SDL_Rect water_reflect_big_animation_clip[] = {
    RectFromImageGrid(170, 40, 4, 1, 0, 0),
    RectFromImageGrid(170, 40, 4, 1, 0, 1),
    RectFromImageGrid(170, 40, 4, 1, 0, 2),
    RectFromImageGrid(170, 40, 4, 1, 0, 3)
};
Animation* water_reflect_big_animation = NULL;
SDL_Texture* water_reflect_medium_texture = NULL;
SDL_Rect water_reflect_medium_animation_clip[] = {
    RectFromImageGrid(53, 12, 4, 1, 0, 0),
    RectFromImageGrid(53, 12, 4, 1, 0, 1),
    RectFromImageGrid(53, 12, 4, 1, 0, 2),
    RectFromImageGrid(53, 12, 4, 1, 0, 3),
};
Animation* water_reflect_medium_animation = NULL;
SDL_Texture* water_reflect_small_texture = NULL;
SDL_Rect water_reflect_small_animation_clip[] = {
    RectFromImageGrid(35, 12, 4, 1, 0, 0),
    RectFromImageGrid(35, 12, 4, 1, 0, 1),
    RectFromImageGrid(35, 12, 4, 1, 0, 2),
    RectFromImageGrid(35, 12, 4, 1, 0, 3),
};
Animation* water_reflect_small_animation = NULL;

void InitBackground() {
    background_texture = LoadTexture("images/background/background_sky.png");
    small_cloud_texture[0] = LoadTexture("images/background/small_cloud1.png");
    small_cloud_texture[1] = LoadTexture("images/background/small_cloud2.png");
    small_cloud_texture[2] = LoadTexture("images/background/small_cloud3.png");
    big_cloud_texture = LoadTexture("images/background/big_cloud.png");
    water_reflect_big_texture =
        LoadTexture("images/background/water_reflect_big.png");
    water_reflect_big_animation = CreateAnimation(
        water_reflect_big_texture, 0.2, water_reflect_big_animation_clip,
        SDL_arraysize(water_reflect_big_animation_clip)
    );
    water_reflect_medium_texture =
        LoadTexture("images/background/water_reflect_medium.png");
    water_reflect_medium_animation = CreateAnimation(
        water_reflect_medium_texture, 0.2, water_reflect_medium_animation_clip,
        SDL_arraysize(water_reflect_medium_animation_clip)
    );
    water_reflect_small_texture =
        LoadTexture("images/background/water_reflect_small.png");
    water_reflect_small_animation = CreateAnimation(
        water_reflect_small_texture, 0.2, water_reflect_small_animation_clip,
        SDL_arraysize(water_reflect_small_animation_clip)
    );
}

void QuitBackground() {
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

void DrawBackground(float dt) {
    int win_w, win_h, img_w, img_h;
    SDL_GetWindowSize(game_app.window, &win_w, &win_h);
    SDL_RenderCopyF(game_app.renderer, background_texture, NULL, NULL);
    // draw small clouds
    SDL_QueryTexture(
        small_cloud_texture[small_cloud_index], NULL, NULL, &img_w, &img_h
    );
    float small_cloud_scale = 0.31 * win_h / (img_h + 1);
    small_cloud_x += 120 * dt;
    SDL_FRect small_cloud_dst = {
        small_cloud_x, 0.31 * win_h - img_h * small_cloud_scale,
        img_w * small_cloud_scale, img_h * small_cloud_scale
    };
    SDL_RenderCopyF(
        game_app.renderer, small_cloud_texture[small_cloud_index], NULL,
        &small_cloud_dst
    );
    if (small_cloud_x > win_w) {
        small_cloud_index = rand() % 3;
        SDL_QueryTexture(
            small_cloud_texture[small_cloud_index], NULL, NULL, &img_w, NULL
        );
        small_cloud_x = -img_w * small_cloud_scale - rand() % 50 - 50;
    }
    // draw big cloud
    SDL_QueryTexture(big_cloud_texture, NULL, NULL, &img_w, &img_h);
    float big_cloud_scale = 0.67 * win_h / (img_h + 1);
    big_cloud_x += 30 * dt;
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
            game_app.renderer, big_cloud_texture, NULL, &big_cloud_dst
        );
    }
    // draw water reflects
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
