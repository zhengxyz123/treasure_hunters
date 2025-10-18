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
   Draw TTF font. If defined TH_FALLBACK_TO_BITMAP_FONT, it will fallback to
   draw bitmap fonts.
*/

#include "../../global.h"
#include "text.h"
#include <SDL_ttf.h>
#include <stdarg.h>

#if !defined(TH_FALLBACK_TO_BITMAP_FONT)
    #include "../../resource/respack.h"
#endif

extern GameApp game_app;

struct {
    void* mem;
    size_t mem_size;
    SDL_RWops* src;
    TTF_Font* font;
} font;
FontConfig font_config;
int max_wrapped_width = 512;

void InitTTFText() {
#if defined(TH_FALLBACK_TO_BITMAP_FONT)
    return;
#else
    font.mem = GetRespackItem(
        game_app.assets_pack, "fonts/NotoSansMonoCJK.ttc", &font.mem_size
    );
    font.src = SDL_RWFromMem(font.mem, font.mem_size);
    font.font = TTF_OpenFontIndexRW(font.src, 1, 32, FONTFACE_NOTOCJK_JP);
    font_config.color = (SDL_Color){0, 0, 0, 255};
    SDL_RendererInfo info;
    if (SDL_GetRendererInfo(game_app.renderer, &info) == 0) {
        max_wrapped_width = info.max_texture_width;
    }
#endif
}

void QuitTTFText() {
#if defined(TH_FALLBACK_TO_BITMAP_FONT)
    return;
#else
    TTF_CloseFont(font.font);
    free(font.mem);
#endif
}

void ReloadFont(long index) {
#if defined(TH_FALLBACK_TO_BITMAP_FONT)
    return;
#else
    TTF_CloseFont(font.font);
    font.src = SDL_RWFromMem(font.mem, font.mem_size);
    font.font = TTF_OpenFontIndexRW(font.src, 1, font_config.size, index);
    TTF_SetFontKerning(font.font, 1);
    TTF_SetFontStyle(font.font, font_config.style);
    TTF_SetFontWrappedAlign(font.font, font_config.align);
#endif
}

void SetFontAlign(int align) {
    font_config.align = align;
#if !defined(TH_FALLBACK_TO_BITMAP_FONT)
    TTF_SetFontWrappedAlign(font.font, align);
#endif
}

void SetFontAnchor(int anchor) {
    font_config.anchor = anchor;
}

void SetFontSize(int size) {
    font_config.size = size;
#if !defined(TH_FALLBACK_TO_BITMAP_FONT)
    TTF_SetFontSize(font.font, size);
#endif
}

void SetFontStyle(int style) {
    font_config.style = style;
#if !defined(TH_FALLBACK_TO_BITMAP_FONT)
    TTF_SetFontStyle(font.font, style);
#endif
}

void SetFontColor(int r, int g, int b, int a) {
    font_config.color = (SDL_Color){r, g, b, a};
}

void GetCurrentFontConfig(FontConfig* config) {
    if (config) {
        *config = font_config;
    }
}

int MeasureTextSize(char* str, int* w, int* h) {
#if defined(TH_FALLBACK_TO_BITMAP_FONT)
    BitmapTextStyle style = {.size = font_config.size};
    float temp_w, temp_h;
    CalcSmallBitmapTextSize(str, &style, &temp_w, &temp_h);
    if (w) {
        *w = temp_w;
    }
    if (h) {
        *h = 1.5 * temp_h;
    }
    return 1;
#else
    return TTF_SizeUTF8(font.font, str, w, h);
#endif
}

void DrawTextWrappedV(int x, int y, int max_width, char* format, va_list args) {
    char* str;
#if defined(__GNUC__) && !defined(__MINGW32__) && !defined(__PSP__) &&         \
    !defined(__vita__)
    vasprintf(&str, format, args);
#else
    str = (char*)calloc(1024, sizeof(char));
    vsprintf(str, format, args);
#endif
#if defined(TH_FALLBACK_TO_BITMAP_FONT)
    int align = 0;
    switch (font_config.align) {
    case TTF_WRAPPED_ALIGN_CENTER:
        align = TEXT_ALIGN_CENTER;
        break;
    case TTF_WRAPPED_ALIGN_LEFT:
        align = TEXT_ALIGN_LEFT;
        break;
    case TTF_WRAPPED_ALIGN_RIGHT:
        align = TEXT_ALIGN_RIGHT;
        break;
    }
    BitmapTextStyle style = {
        .size = font_config.size,
        .align = align,
        .anchor = font_config.anchor,
        .color = font_config.color
    };
    DrawSmallBitmapText(x, y, &style, str);
#else
    if (max_width > max_wrapped_width) {
        max_width = max_wrapped_width;
    }
    int text_w, text_h;
    MeasureTextSize(str, &text_w, &text_h);
    if (font_config.anchor & TEXT_ANCHOR_X_CENTER) {
        x -= text_w / 2;
    } else if (font_config.anchor & TEXT_ANCHOR_X_RIGHT) {
        x -= text_w;
    }
    if (font_config.anchor & TEXT_ANCHOR_Y_CENTER) {
        y -= text_h / 2;
    } else if (font_config.anchor & TEXT_ANCHOR_Y_BOTTOM) {
        y -= text_h;
    }
    SDL_Surface* surface = TTF_RenderUTF8_Blended_Wrapped(
        font.font, str, font_config.color, max_width
    );
    SDL_Texture* texture =
        SDL_CreateTextureFromSurface(game_app.renderer, surface);
    SDL_RenderCopy(
        game_app.renderer, texture, NULL,
        &(SDL_Rect){x, y, surface->w, surface->h}
    );
    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
#endif
    free(str);
}

void DrawTextWrapped(float x, float y, int max_width, char* format, ...) {
    va_list args;
    va_start(args, format);
    DrawTextWrappedV(x, y, max_width, format, args);
    va_end(args);
}

void DrawText(float x, float y, char* format, ...) {
    va_list args;
    va_start(args, format);
    DrawTextWrappedV(x, y, 9999, format, args);
    va_end(args);
}
