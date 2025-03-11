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

#include "loader.h"
#include "../global.h"

extern GameApp global_app;

SDL_Surface* LoadSurface(const unsigned char* content, int size) {
    SDL_RWops* raw_image = SDL_RWFromConstMem(content, size);
    SDL_Surface* surface = IMG_LoadPNG_RW(raw_image);
    SDL_RWclose(raw_image);
    return surface;
}

SDL_Texture* LoadTexture(const unsigned char* content, int size) {
    SDL_RWops* raw_image = SDL_RWFromConstMem(content, size);
    SDL_Texture* texture =
        IMG_LoadTexture_RW(global_app.renderer, raw_image, 1);
    return texture;
}

Mix_Chunk* LoadSound(const unsigned char* content, int size) {
    SDL_RWops* raw_music = SDL_RWFromConstMem(content, size);
    Mix_Chunk* chunk = Mix_LoadWAV_RW(raw_music, 1);
    return chunk;
}
