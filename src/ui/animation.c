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

#include "animation.h"
#include "frametimer.h"

extern GameApp global_app;

Animation* CreateAnimation(
    SDL_Texture* texture, float duration, SDL_Rect* rect, int count
) {
    Animation* animation = (Animation*)calloc(1, sizeof(Animation));
    animation->count = count;
    animation->paused = 0;
    animation->now_clip = 0;
    animation->dt = 0;
    animation->texture = texture;
    animation->clip = (AnimationClip*)calloc(count, sizeof(AnimationClip));
    for (int i = 0; i < count; ++i) {
        animation->clip[i].duration = duration;
        animation->clip[i].area = rect[i];
    }
    return animation;
}

void DrawAnimationEx(
    Animation* animation, float x, float y, float scale, double angle,
    SDL_FPoint* center, SDL_RendererFlip flip
) {
    if (animation->paused) {
        return;
    }
    animation->dt += frametimer_delta_time(global_app.timer);
    if (animation->dt > animation->clip[animation->now_clip].duration) {
        animation->now_clip = animation->now_clip + 1 > animation->count - 1
                                ? 0
                                : animation->now_clip + 1;
        animation->dt = 0;
    }
    SDL_FRect dstrect = {
        x, y, animation->clip[animation->now_clip].area.w * scale,
        animation->clip[animation->now_clip].area.h * scale
    };
    SDL_RenderCopyExF(
        global_app.renderer, animation->texture,
        &animation->clip[animation->now_clip].area, &dstrect, angle, center,
        flip
    );
}

void DrawAnimation(Animation* animation, float x, float y, float scale) {
    DrawAnimationEx(animation, x, y, scale, 0.0, NULL, SDL_FLIP_NONE);
}

void FreeAnimation(Animation* animation) {
    free(animation->clip);
    free(animation);
}
