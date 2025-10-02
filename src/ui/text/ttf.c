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

#include "ttf.h"
#include "../../global.h"
#include "text.h"
#include <SDL_ttf.h>
#include <stdarg.h>

// font texture cache only available for PSP
#if defined(__PSP__)
    #define TH_ENABLE_FONT_TEXTURE_CACHE
#endif
#if defined(TH_ENABLE_FONT_TEXTURE_CACHE)
    #include "../../resources/respack.h"
#endif
#if !defined(TH_TEXTURE_CACHE_SIZE)
    #define TH_TEXTURE_CACHE_SIZE 32
#endif

extern GameApp game_app;

#if defined(TH_ENABLE_FONT_TEXTURE_CACHE)
typedef struct {
    int valid;
    uint32_t hash;
    int align;
    int style;
    int size;
    SDL_Texture* texture;
} CacheEntry;

struct {
    CacheEntry entry[TH_TEXTURE_CACHE_SIZE];
    int size;
    int next_entry;
} texture_cache;
#endif

FontConfig font_config;

char* font_file = NULL;
TTF_Font* font = NULL;
int max_wrapped_width = 512;

int _mbtowc(wchar_t* pwc, const char* s, size_t n);

void InitTTFText() {
    font_file = calloc(PATH_MAX, sizeof(char));
    strcpy(font_file, game_app.exec_path);
    strcat(font_file, "NotoSerifCJK.ttc");
    font = TTF_OpenFontIndex(font_file, 32, FONTFACE_NOTOCJK_JP);
    TTF_SetFontKerning(font, 1);
    SDL_RendererInfo info;
    if (SDL_GetRendererInfo(game_app.renderer, &info) == 0) {
        max_wrapped_width = info.max_texture_width;
    }
    font_config.color = (SDL_Color){0, 0, 0, 255};
#if defined(TH_ENABLE_FONT_TEXTURE_CACHE)
    texture_cache.size = 0;
    texture_cache.next_entry = 0;
    for (int i = 0; i < TH_TEXTURE_CACHE_SIZE; ++i) {
        texture_cache.entry[i].valid = 0;
    }
#endif
}

void QuitTTFText() {
    TTF_CloseFont(font);
    free(font_file);
#if defined(TH_ENABLE_FONT_TEXTURE_CACHE)
    for (int i = 0; i < TH_TEXTURE_CACHE_SIZE; ++i) {
        if (texture_cache.entry[i].valid) {
            SDL_DestroyTexture(texture_cache.entry[i].texture);
        }
    }
#endif
}

void ReloadFont(long index) {
#if defined(TH_ENABLE_FONT_TEXTURE_CACHE)
    for (int i = 0; i < TH_TEXTURE_CACHE_SIZE; ++i) {
        if (texture_cache.entry[i].valid) {
            SDL_DestroyTexture(texture_cache.entry[i].texture);
            texture_cache.entry[i].valid = 0;
        }
    }
    texture_cache.size = 0;
    texture_cache.next_entry = 0;
#endif
    font = TTF_OpenFontIndex(font_file, font_config.size, index);
    TTF_SetFontKerning(font, 1);
    TTF_SetFontStyle(font, font_config.style);
    TTF_SetFontWrappedAlign(font, font_config.align);
}

void SetFontAlign(int align) {
    font_config.align = align;
    TTF_SetFontWrappedAlign(font, align);
}

void SetFontAnchor(int anchor) {
    font_config.anchor = anchor;
}

void SetFontSize(int size) {
    font_config.size = size;
    TTF_SetFontSize(font, size);
}

void SetFontStyle(int style) {
    font_config.style = style;
    TTF_SetFontStyle(font, style);
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
    return TTF_SizeUTF8(font, str, w, h);
}

void DrawTextWrappedV(int x, int y, int max_width, char* format, va_list args) {
    char* str;
#if defined(__GNUC__) && !defined(__MINGW32__) && !defined(__PSP__)
    vasprintf(&str, format, args);
#else
    str = (char*)calloc(1024, sizeof(char));
    vsprintf(str, format, args);
#endif
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
#if defined(TH_ENABLE_FONT_TEXTURE_CACHE)
    for (int i = 0; i < TH_TEXTURE_CACHE_SIZE; ++i) {
        CacheEntry entry = texture_cache.entry[i];
        if (entry.valid) {
            if (entry.hash == fnv1a_32(str, FNV1_32_INIT) &&
                entry.style == font_config.style &&
                entry.align == font_config.align &&
                entry.size == font_config.size) {
                int w, h;
                SDL_QueryTexture(entry.texture, NULL, NULL, &w, &h);
                SDL_SetTextureColorMod(
                    entry.texture, font_config.color.r, font_config.color.g,
                    font_config.color.b
                );
                SDL_SetTextureAlphaMod(entry.texture, font_config.color.a);
                SDL_RenderCopy(
                    game_app.renderer, entry.texture, NULL,
                    &(SDL_Rect){x, y, w, h}
                );
                return;
            }
        }
    }
    if (texture_cache.size == TH_TEXTURE_CACHE_SIZE - 1) {
        CacheEntry* entry = &texture_cache.entry[texture_cache.next_entry];
        SDL_DestroyTexture(entry->texture);
        entry->valid = 0;
        entry->hash = 0;
    } else {
        ++texture_cache.size;
    }
    CacheEntry* entry = &texture_cache.entry[texture_cache.next_entry];
    SDL_Surface* surface = TTF_RenderUTF8_Blended_Wrapped(
        font, str, (SDL_Color){255, 255, 255, 255}, max_width
    );
    entry->valid = 1;
    entry->hash = fnv1a_32(str, FNV1_32_INIT);
    entry->style = font_config.style;
    entry->align = font_config.align;
    entry->size = font_config.size;
    entry->texture = SDL_CreateTextureFromSurface(game_app.renderer, surface);
    texture_cache.next_entry =
        (texture_cache.next_entry + 1) % TH_TEXTURE_CACHE_SIZE;
    SDL_SetTextureColorMod(
        entry->texture, font_config.color.r, font_config.color.g,
        font_config.color.b
    );
    SDL_SetTextureAlphaMod(entry->texture, font_config.color.a);
    SDL_RenderCopy(
        game_app.renderer, entry->texture, NULL,
        &(SDL_Rect){x, y, surface->w, surface->h}
    );
    SDL_FreeSurface(surface);
#else
    SDL_Surface* surface =
        TTF_RenderUTF8_Blended_Wrapped(font, str, font_config.color, max_width);
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
