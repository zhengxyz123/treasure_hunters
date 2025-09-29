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

#ifndef _TH_TEXT_BITMAP_H_
#define _TH_TEXT_BITMAP_H_

#include <SDL.h>

#define BIG_TEXT_WIDTH 10
#define BIG_TEXT_HEIGHT 11
#define SMALL_TEXT_WIDTH 8
#define SMALL_TEXT_HEIGHT 8

typedef struct {
    float size;
    int char_space;
    int line_space;
    int align;
    int anchor;
    SDL_Color color;
    int has_shadow;
    SDL_FPoint shadow_offset;
    SDL_Color shadow_color;
} BitmapTextStyle;

typedef enum {
    ICON_LEFT,
    ICON_RIGHT,
    ICON_DOWN,
    ICON_UP,
    ICON_CROSS,
    ICON_TICK,
    ICON_INFO
} IconID;

void InitBitmapText();
void QuitDitmapText();
void CalcBigBitmapTextSize(
    char* str, BitmapTextStyle* style, float* w, float* h
);
float CalcBigBitmapTextWidthOneLine(char* str, BitmapTextStyle* style);
void DrawBigBitmapText(
    float x, float y, BitmapTextStyle* style, const char* format, ...
);
void CalcSmallBitmapTextSize(
    char* str, BitmapTextStyle* style, float* w, float* h
);
float CalcSmalbitmaplTextWidthOneLine(char* str, BitmapTextStyle* style);
void DrawSmallBitmapText(
    float x, float y, BitmapTextStyle* style, const char* format, ...
);

#endif
