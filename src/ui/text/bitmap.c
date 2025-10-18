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
  Draw bitmap fonts.
*/

#include "../../global.h"
#include "../../resource/loader.h"
#include "text.h"
#include <assert.h>
#include <stdio.h>

extern GameApp game_app;

SDL_Texture* big_text_texture = NULL;
SDL_Texture* small_text_texture = NULL;
SDL_Texture* input_prompt_texture = NULL;

void InitBitmapText() {
    big_text_texture = LoadTexture("images/ui/big_text.png");
    small_text_texture = LoadTexture("images/ui/small_text.png");
    input_prompt_texture = LoadTexture("images/ui/input_prompt.png");
}

void QuitDitmapText() {
    SDL_DestroyTexture(big_text_texture);
    SDL_DestroyTexture(small_text_texture);
    SDL_DestroyTexture(input_prompt_texture);
}

void CalcBigBitmapTextSize(
    char* str, BitmapTextStyle* style, float* w, float* h
) {
    float line_w = 0;
    float w_temp = 0;
    float h_temp = style->size * BIG_TEXT_HEIGHT + style->line_space;
    for (int i = 0; str[i] != '\0'; ++i) {
        if (str[i] == '\n') {
            h_temp += style->size * BIG_TEXT_HEIGHT + style->line_space;
            if (line_w > w_temp) {
                w_temp = line_w;
            }
            line_w = 0;
        } else {
            line_w += style->size * BIG_TEXT_WIDTH + style->char_space;
        }
    }
    if (line_w > w_temp) {
        w_temp = line_w;
    }
    if (w) {
        *w = w_temp;
    }
    if (h) {
        *h = h_temp;
    }
}

float CalcBigBitmapTextWidthOneLine(char* str, BitmapTextStyle* style) {
    float w = 0;
    for (int i = 0; str[i] != '\n' && str[i] != '\0'; ++i) {
        w += style->size * BIG_TEXT_WIDTH + style->char_space;
    }
    return w;
}

void DrawBigBitmapText(
    float x, float y, BitmapTextStyle* style, const char* format, ...
) {
    va_list args;
    va_start(args, format);
    char* str;
#if defined(__GNUC__) && !defined(__MINGW32__) && !defined(__PSP__) &&         \
    !defined(__vita__)
    vasprintf(&str, format, args);
#else
    str = (char*)calloc(1024, sizeof(char));
    vsprintf(str, format, args);
#endif
    va_end(args);
    float text_w, text_h;
    float dx = 0, line_w = CalcBigBitmapTextWidthOneLine(str, style);
    CalcBigBitmapTextSize(str, style, &text_w, &text_h);
    if (style->anchor & TEXT_ANCHOR_X_CENTER) {
        x -= text_w / 2;
    } else if (style->anchor & TEXT_ANCHOR_X_RIGHT) {
        x -= text_w;
    }
    if (style->anchor & TEXT_ANCHOR_Y_CENTER) {
        y -= text_h / 2;
    } else if (style->anchor & TEXT_ANCHOR_Y_BOTTOM) {
        y -= text_h;
    }
    if (style->align & TEXT_ALIGN_CENTER) {
        dx = (text_w - line_w) / 2;
    } else if (style->align & TEXT_ALIGN_RIGHT) {
        dx = text_w - line_w;
    }

    SDL_Rect text_src = {0, 0, BIG_TEXT_WIDTH, BIG_TEXT_HEIGHT};
    SDL_FRect text_dst = {
        x + dx, y, BIG_TEXT_WIDTH * style->size, BIG_TEXT_HEIGHT * style->size
    };
    for (int i = 0; str[i] != '\0'; ++i) {
        if (str[i] == '\n') {
            line_w = CalcBigBitmapTextWidthOneLine(str + i + 1, style);
            if (style->align & TEXT_ALIGN_CENTER) {
                dx = (text_w - line_w) / 2;
            } else if (style->align & TEXT_ALIGN_RIGHT) {
                dx = text_w - line_w;
            }
            text_dst.x = x + dx;
            text_dst.y += BIG_TEXT_HEIGHT * style->size + style->line_space;
            continue;
        } else if (str[i] >= '0' && str[i] <= '9') {
            text_src.x = (str[i] - '0') % 6 * BIG_TEXT_WIDTH;
            text_src.y = (str[i] - '0') / 6 * BIG_TEXT_HEIGHT;
        } else if (str[i] >= 'A' && str[i] <= 'Z') {
            text_src.x = (str[i] - 'A' + 10) % 6 * BIG_TEXT_WIDTH;
            text_src.y = (str[i] - 'A' + 10) / 6 * BIG_TEXT_HEIGHT;
        } else if (str[i] >= 'a' && str[i] <= 'z') {
            text_src.x = (str[i] - 'a' + 10) % 6 * BIG_TEXT_WIDTH;
            text_src.y = (str[i] - 'a' + 10) / 6 * BIG_TEXT_HEIGHT;
        } else {
            text_dst.x += BIG_TEXT_WIDTH * style->size;
            continue;
        }
        SDL_RenderCopyF(
            game_app.renderer, big_text_texture, &text_src, &text_dst
        );
        text_dst.x += BIG_TEXT_WIDTH * style->size + style->char_space;
    }
    free(str);
}

void CalcSmallBitmapTextSize(
    char* str, BitmapTextStyle* style, float* w, float* h
) {
    float line_w = 0;
    float w_temp = 0;
    float h_temp = style->size + style->line_space;
    for (int i = 0; str[i] != '\0'; ++i) {
        if (str[i] == '\n') {
            h_temp += style->size + style->line_space;
            if (line_w > w_temp) {
                w_temp = line_w;
            }
            line_w = 0;
        } else {
            line_w += style->size + style->char_space;
        }
    }
    if (line_w > w_temp) {
        w_temp = line_w;
    }
    if (w) {
        *w = w_temp;
    }
    if (h) {
        *h = h_temp;
    }
}

float CalcSmalBitmapTextWidthOneLine(char* str, BitmapTextStyle* style) {
    float w = 0;
    for (int i = 0; str[i] != '\n' && str[i] != '\0'; ++i) {
        w += style->size + style->char_space;
    }
    return w;
}

void DrawSmallBitmapText(
    float x, float y, BitmapTextStyle* style, const char* format, ...
) {
#if defined(TH_FALLBACK_TO_BITMAP_FONT)
    y += 0.25 * style->size;
#endif
    SDL_SetTextureColorMod(
        small_text_texture, style->color.r, style->color.g, style->color.b
    );
    va_list args;
    va_start(args, format);
    char* str;
#if defined(__GNUC__) && !defined(__MINGW32__) && !defined(__PSP__) &&         \
    !defined(__vita__)
    vasprintf(&str, format, args);
#else
    str = (char*)calloc(1024, sizeof(char));
    vsprintf(str, format, args);
#endif
    va_end(args);
    float text_w, text_h;
    float dx = 0, line_w = CalcSmalBitmapTextWidthOneLine(str, style);
    CalcSmallBitmapTextSize(str, style, &text_w, &text_h);
    if (style->anchor & TEXT_ANCHOR_X_CENTER) {
        x -= text_w / 2;
    } else if (style->anchor & TEXT_ANCHOR_X_RIGHT) {
        x -= text_w;
    }
    if (style->anchor & TEXT_ANCHOR_Y_CENTER) {
        y -= text_h / 2;
    } else if (style->anchor & TEXT_ANCHOR_Y_BOTTOM) {
        y -= text_h;
    }
    if (style->align & TEXT_ALIGN_CENTER) {
        dx = (text_w - line_w) / 2;
    } else if (style->align & TEXT_ALIGN_RIGHT) {
        dx = text_w - line_w;
    }

    SDL_Rect text_src = {0, 0, SMALL_TEXT_WIDTH, SMALL_TEXT_HEIGHT};
    SDL_FRect text_dst = {x + dx, y, style->size, style->size};
    for (int i = 0; str[i] != '\0'; ++i) {
        if (str[i] == '\n') {
            // make a newline
            line_w = CalcSmalBitmapTextWidthOneLine(str + i + 1, style);
            if (style->align & TEXT_ALIGN_CENTER) {
                dx = (text_w - line_w) / 2;
            } else if (style->align & TEXT_ALIGN_RIGHT) {
                dx = text_w - line_w;
            }
            text_dst.x = x + dx;
            text_dst.y += style->size + style->line_space;
            continue;
        } else if (str[i] >= '!' && str[i] <= '~') {
            // draw printable ASCII characters
            text_src.x = (str[i] - ' ') % 16 * SMALL_TEXT_WIDTH;
            text_src.y = (str[i] - ' ') / 16 * SMALL_TEXT_HEIGHT;
            if (style->has_shadow) {
                SDL_SetTextureColorMod(
                    small_text_texture, style->shadow_color.r,
                    style->shadow_color.g, style->shadow_color.b
                );
                SDL_SetTextureAlphaMod(
                    small_text_texture, style->shadow_color.a
                );
                SDL_FRect shadow_dst = {
                    text_dst.x + style->shadow_offset.x * style->size,
                    text_dst.y + style->shadow_offset.y * style->size,
                    text_dst.w, text_dst.h
                };
                SDL_RenderCopyF(
                    game_app.renderer, small_text_texture, &text_src,
                    &shadow_dst
                );
            }
            SDL_SetTextureColorMod(
                small_text_texture, style->color.r, style->color.g,
                style->color.b
            );
            SDL_SetTextureAlphaMod(small_text_texture, style->color.a);
            SDL_RenderCopyF(
                game_app.renderer, small_text_texture, &text_src, &text_dst
            );
        }
        text_dst.x += style->size + style->char_space;
    }
    free(str);
}

void DrawBitmapIcon(float x, float y, int size, SDL_Color color, IconID id) {
    SDL_Rect icon_src = {
        (id + 96) % 16 * SMALL_TEXT_WIDTH, (id + 96) / 16 * SMALL_TEXT_HEIGHT,
        SMALL_TEXT_WIDTH, SMALL_TEXT_HEIGHT
    };
    SDL_SetTextureColorMod(small_text_texture, color.r, color.g, color.b);
    SDL_SetTextureAlphaMod(small_text_texture, color.a);
    SDL_RenderCopy(
        game_app.renderer, small_text_texture, &icon_src,
        &(SDL_Rect){x, y, size, size}
    );
}
