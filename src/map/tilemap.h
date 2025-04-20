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

#ifndef _TH_TILEMAP_H_
#define _TH_TILEMAP_H_

#include "../global.h"

typedef enum {
    OBJECT_TYPE_POINT,
    OBJECT_TYPE_RECT
} TileMapObjectType;

typedef struct {
    SDL_Texture* texture;
    int first_index;
} TileSet;

typedef struct {
    int is_playing;
    char name[8];
    size_t count;
    SDL_Rect* frame;
} AnimatedTile;

typedef struct {
    TileMapObjectType type;
    char class_name[8];
    char object_name[8];
    unsigned char flag;
    union {
        SDL_Point point;
        SDL_Rect rect;
    } data;
} TileMapObject;

typedef struct {
    SDL_Rect area;
    int* data;
} TileMayLayer;

typedef struct {
    int tile_width;
    int tile_height;
    size_t tileset_count;
    TileSet* tileset;
    size_t animated_tile_count;
    AnimatedTile* animated_tile;
    size_t object_count;
    TileMapObject* object;
    size_t layer_count;
    TileMayLayer* layer;
} TileMap;

void InitMapSystem();
void QuitMapSystem();
TileMap* LoadTileMap(const unsigned char* data, size_t size);
void FreeTileMap(TileMap* map);

#endif
