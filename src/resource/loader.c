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
#include "respack.h"
#include <SDL_image.h>

extern GameApp game_app;

SDL_Surface* LoadSurfaceFromMem(void* content, size_t size) {
    SDL_RWops* raw_image = SDL_RWFromConstMem(content, size);
    return IMG_Load_RW(raw_image, 1);
}

SDL_Surface* LoadSurface(char* filename) {
    size_t size;
    void* content = GetRespackItem(game_app.assets_pack, filename, &size);
    if (size == 0) {
        return NULL;
    }
    SDL_Surface* surface = LoadSurfaceFromMem(content, size);
    free(content);
    return surface;
}

SDL_Texture* LoadTextureFromMem(void* content, size_t size) {
    SDL_RWops* raw_image = SDL_RWFromConstMem(content, size);
    return IMG_LoadTexture_RW(game_app.renderer, raw_image, 1);
}

SDL_Texture* LoadTexture(char* filename) {
    size_t size;
    void* content = GetRespackItem(game_app.assets_pack, filename, &size);
    if (size == 0) {
        return NULL;
    }
    SDL_Texture* texture = LoadTextureFromMem(content, size);
    free(content);
    return texture;
}

Mix_Chunk* LoadSoundFromMem(void* content, size_t size) {
    SDL_RWops* raw_music = SDL_RWFromConstMem(content, size);
    return Mix_LoadWAV_RW(raw_music, 1);
}

Mix_Chunk* LoadSound(char* filename) {
    size_t size;
    void* content = GetRespackItem(game_app.assets_pack, filename, &size);
    if (size == 0) {
        return NULL;
    }
    Mix_Chunk* chunk = LoadSoundFromMem(content, size);
    free(content);
    return chunk;
}
