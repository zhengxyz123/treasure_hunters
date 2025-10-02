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

#ifndef _TH_TEXT_TTF_H_
#define _TH_TEXT_TTF_H_

#include <SDL.h>

typedef enum {
    FONTFACE_NOTOCJK_JP,
    FONTFACE_NOTOCJK_KR,
    FONTFACE_NOTOCJK_SC,
    FONTFACE_NOTOCJK_TC,
    FONTFACE_NOTOCJK_HK
} FontFaceIndex;

typedef struct {
    int align;
    int anchor;
    int size;
    int style;
    SDL_Color color;
} FontConfig;

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
