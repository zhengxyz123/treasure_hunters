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

#ifndef TH_UI_TEXT_H_
#define TH_UI_TEXT_H_

#include <SDL.h>

#define TEXT_ALIGN_LEFT 0
#define TEXT_ALIGN_CENTER 2
#define TEXT_ALIGN_RIGHT 4

#define TEXT_ANCHOR_X_LEFT 2
#define TEXT_ANCHOR_X_CENTER 4
#define TEXT_ANCHOR_X_RIGHT 8
#define TEXT_ANCHOR_Y_TOP 16
#define TEXT_ANCHOR_Y_CENTER 32
#define TEXT_ANCHOR_Y_BOTTOM 64

#define BIG_TEXT_WIDTH 10
#define BIG_TEXT_HEIGHT 11
#define SMALL_TEXT_WIDTH 8
#define SMALL_TEXT_HEIGHT 8

typedef struct BitmapTextStyle {
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

typedef enum IconID {
    ICON_LEFT,
    ICON_RIGHT,
    ICON_DOWN,
    ICON_UP,
    ICON_CROSS,
    ICON_TICK,
    ICON_INFO
} IconID;

typedef enum FontFaceIndex {
    FONTFACE_NOTOCJK_JP,
    FONTFACE_NOTOCJK_KR,
    FONTFACE_NOTOCJK_SC,
    FONTFACE_NOTOCJK_TC,
    FONTFACE_NOTOCJK_HK
} FontFaceIndex;

typedef struct FontConfig {
    int align;
    int anchor;
    int size;
    int style;
    SDL_Color color;
} FontConfig;

void InitBitmapText();
void QuitDitmapText();
float CalcBigBitmapTextWidthOneLine(char* str, BitmapTextStyle* style);
void DrawBigBitmapText(
    float x, float y, BitmapTextStyle* style, const char* format, ...
);
void CalcSmallBitmapTextSize(
    char* str, BitmapTextStyle* style, float* w, float* h
);
void DrawSmallBitmapText(
    float x, float y, BitmapTextStyle* style, const char* format, ...
);
void DrawBitmapIcon(float x, float y, int size, SDL_Color color, IconID id);

void InitTTFText();
void QuitTTFText();
void ReloadFont(long index);
void SetFontAlign(int align);
void SetFontAnchor(int anchor);
void SetFontSize(int size);
void SetFontStyle(int style);
void SetFontColor(int r, int g, int b, int a);
void GetCurrentFontConfig(FontConfig* style);
int MeasureTextSize(char* str, int* w, int* h);
void DrawTextWrapped(float x, float y, int max_width, char* format, ...);
void DrawText(float x, float y, char* format, ...);

#endif
