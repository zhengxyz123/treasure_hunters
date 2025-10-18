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

#include "../global.h"
#include "frametimer.h"
#include "image.h"

extern GameApp game_app;

Sprite* CreateTextureSprite(SDL_Texture* texture) {
    Sprite* sprite = (Sprite*)calloc(1, sizeof(Sprite));
    sprite->type = SPRITE_TYPE_TEXTURE;
    sprite->image.texture = texture;
    int w, h;
    SDL_QueryTexture(texture, NULL, NULL, &w, &h);
    sprite->area = (SDL_FRect){0, 0, w, h};
    sprite->center = (SDL_FPoint){w / 2.0, h / 2.0};
    sprite->angle = 0.0;
    sprite->color = (SDL_Color){255, 255, 255};
    sprite->flip = SDL_FLIP_NONE;
    return sprite;
}

Sprite* CreateAnimationSprite(Animation* animation) {
    Sprite* sprite = (Sprite*)calloc(1, sizeof(Sprite));
    sprite->type = SPRITE_TYPE_ANIMATION;
    sprite->image.animation = animation;
    int w = animation->clip[0].area.w;
    int h = animation->clip[0].area.h;
    sprite->area = (SDL_FRect){0, 0, w, h};
    sprite->center = (SDL_FPoint){w / 2.0, h / 2.0};
    sprite->angle = 0.0;
    sprite->color = (SDL_Color){255, 255, 255};
    sprite->flip = SDL_FLIP_NONE;
    return sprite;
}

void FreeSprite(Sprite* sprite) {
    if (sprite->type == SPRITE_TYPE_TEXTURE) {
        SDL_DestroyTexture(sprite->image.texture);
    } else {
        FreeAnimation(sprite->image.animation);
    }
    free(sprite);
}

void SetSpritePosition(Sprite* sprite, float x, float y) {
    sprite->area.x = x;
    sprite->area.y = y;
}

void SetSpriteSize(Sprite* sprite, float w, float h) {
    sprite->area.w = w;
    sprite->area.h = h;
}

void DrawSprite(Sprite* sprite) {
    if (sprite->type == SPRITE_TYPE_TEXTURE) {
        SDL_SetTextureColorMod(
            sprite->image.texture, sprite->color.r, sprite->color.g,
            sprite->color.b
        );
        SDL_SetTextureAlphaMod(sprite->image.texture, sprite->color.a);
        SDL_RenderCopyExF(
            game_app.renderer, sprite->image.texture, NULL, &sprite->area,
            sprite->angle, &sprite->center, sprite->flip
        );
    } else {
        Animation* animation = sprite->image.animation;
        SDL_SetTextureColorMod(
            animation->texture, sprite->color.r, sprite->color.g,
            sprite->color.b
        );
        SDL_SetTextureAlphaMod(animation->texture, sprite->color.a);
        if (animation->paused) {
            animation->dt = 0;
            goto draw;
        }
        animation->dt += frametimer_delta_time(game_app.timer);
        if (animation->dt > animation->clip[animation->now_clip].duration) {
            animation->now_clip = animation->now_clip + 1 > animation->count - 1
                                    ? 0
                                    : animation->now_clip + 1;
            animation->dt = 0;
        }
    draw:
        SDL_RenderCopyExF(
            game_app.renderer, animation->texture,
            &animation->clip[animation->now_clip].area, &sprite->area,
            sprite->angle, &sprite->center, sprite->flip
        );
    }
}
