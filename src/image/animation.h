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

#ifndef TH_IMAGE_ANIMATION_H_
#define TH_IMAGE_ANIMATION_H_

#include <SDL.h>

typedef struct AnimationClip {
    float duration;
    SDL_Rect area;
} AnimationClip;

typedef struct Animation {
    int count;
    int paused;
    int now_clip;
    float dt;
    SDL_Texture* texture;
    AnimationClip* clip;
} Animation;

Animation* CreateAnimation(
    SDL_Texture* texture, float duration, SDL_Rect* rect, int count
);
void FreeAnimation(Animation* animation);
void DrawAnimationEx(
    Animation* animation, float x, float y, float scale, double angle,
    SDL_FPoint* center, SDL_RendererFlip flip
);
void DrawAnimation(Animation* animation, float x, float y, float scale);

#endif
