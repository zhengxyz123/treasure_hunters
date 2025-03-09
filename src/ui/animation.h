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

#ifndef _TH_ANIMATION_H_
#define _TH_ANIMATION_H_

#include "../global.h"

typedef struct {
    int duration;
    SDL_Rect area;
} AnimationClip;

typedef struct {
    int count;
    int paused;
    unsigned long prev_tick;
    unsigned long now_clip;
    SDL_Texture* texture;
    AnimationClip* clip;
} Animation;

Animation*
CreateAnimation(SDL_Texture* texture, int duration, SDL_Rect* rect, int count);
void DrawAnimationEx(
    Animation* animation, float x, float y, float scale, double angle,
    SDL_FPoint* center, SDL_RendererFlip flip
);
void DrawAnimation(Animation* animation, float x, float y, float scale);
void FreeAnimation(Animation* animation);

#endif
