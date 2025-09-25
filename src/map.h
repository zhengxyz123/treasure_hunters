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

#include "entities/base.h"
#include <SDL.h>
#include <cute_tiled.h>

typedef cute_tiled_map_t Tilemap;
typedef cute_tiled_layer_t TilemapLayer;
typedef cute_tiled_object_t TilemapObject;
typedef cute_tiled_property_t TilemapProperty;
typedef cute_tiled_tile_descriptor_t TileDescriptor;
typedef cute_tiled_frame_t TileFrame;
typedef cute_tiled_tileset_t Tileset;

typedef enum {
    TILEMAP_LAYERGROUP_FRONT,
    TILEMAP_LAYERGROUP_MIDDLE,
    TILEMAP_LAYERGROUP_BACK
} TilemapLayerGroup;

typedef struct {
    EntityList entity_list;
    Tilemap* tilemap;
#if !defined(__PSP__)
    struct {
        SDL_Texture* front;
        SDL_Texture* middle;
        SDL_Texture* back;
    } texture;
#endif
} Map;

void InitMapSystem();
void QuitMapSystem();
Map* LoadMap_Mem(void* content, size_t size);
Map* LoadMap(char* filename);
void FreeMap(Map* map);
void MapDrawLayer(Map* map, TilemapLayerGroup group, SDL_Point* offset);
int MapIsEmpty(Map* map, SDL_FRect* rect);
int MapHasDamage(Map* map, SDL_FRect* rect);

#endif
