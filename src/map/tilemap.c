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

#include "tilemap.h"
#include "../resources/loader.h"
#include <stdlib.h>

extern GameApp global_app;

SDL_Texture* tilesets_texture = NULL;

void InitMapSystem() {
    tilesets_texture = LoadTexture("images/tilesets.png");
}

void QuitMapSystem() {
    SDL_DestroyTexture(tilesets_texture);
}

SDL_Texture* GetTextureRegionFromGID(Tilemap* map, int gid, SDL_Rect* rect) {
    for (Tileset* tileset = map->tilemap->tilesets; tileset;
         tileset = tileset->next) {
        if (tileset->firstgid <= gid &&
            tileset->firstgid + tileset->tilecount > gid) {
            int lid = gid - tileset->firstgid;
            int dw = tileset->imagewidth / tileset->tilewidth;
            rect->x = lid % dw * tileset->tilewidth;
            rect->y = lid / dw * tileset->tileheight;
            rect->w = tileset->tilewidth;
            rect->h = tileset->tileheight;
            if (strcmp(tileset->image.ptr, "tilesets.png") == 0) {
                return tilesets_texture;
            }
        }
    }
    return NULL;
}

/*
  `TilemapDrawLayer` is a global function for PSP and `DrawTilemapTotexture` is
  a local function for other platform.
*/

#if defined(__PSP__)
void TilemapDrawLayer(Tilemap* map, TilemapLayerGroup group, SDL_Point* offset)
#else
void DrawTilemapTotexture(Tilemap* map, TilemapLayerGroup group)
#endif
{
    TilemapLayer* layer = map->tilemap->layers;
    int found = 0;
    for (; layer; layer = layer->next) {
        switch (group) {
        case TILEMAP_LAYERGROUP_FRONT:
            if (strncmp(layer->name.ptr, "front", 5) == 0) {
                found = 1;
                goto next;
            }
            break;
        case TILEMAP_LAYERGROUP_MIDDLE:
            if (strncmp(layer->name.ptr, "middle", 6) == 0) {
                found = 1;
                goto next;
            }
            break;
        case TILEMAP_LAYERGROUP_BACK:
            if (strncmp(layer->name.ptr, "back", 4) == 0) {
                found = 1;
                goto next;
            }
            break;
        }
    }
next:
    if (!found) {
        return;
    }
    for (; layer; layer = layer->next) {
        switch (group) {
        case TILEMAP_LAYERGROUP_FRONT:
            if (strncmp(layer->name.ptr, "front", 5) != 0) {
                return;
            }
            break;
        case TILEMAP_LAYERGROUP_MIDDLE:
            if (strncmp(layer->name.ptr, "middle", 6) != 0) {
                return;
            }
            break;
        case TILEMAP_LAYERGROUP_BACK:
            if (strncmp(layer->name.ptr, "back", 4) != 0) {
                return;
            }
            break;
        }
        for (int i = 0; i < layer->data_count; ++i) {
            if (layer->data[i] == 0) {
                continue;
            }
            int x = i % layer->width;
            int y = i / layer->width;
            SDL_Rect srcrect;
            SDL_Rect dstrect = {
                x * map->tilemap->tilewidth, y * map->tilemap->tileheight,
                map->tilemap->tilewidth, map->tilemap->tileheight
            };
#if defined(__PSP__)
            dstrect.x -= offset->x;
            dstrect.y -= offset->y;
            if (!SDL_HasIntersection(&dstrect, &(SDL_Rect){0, 0, 480, 272})) {
                continue;
            }
#endif
            SDL_Texture* texture =
                GetTextureRegionFromGID(map, layer->data[i], &srcrect);
            SDL_RenderCopy(global_app.renderer, texture, &srcrect, &dstrect);
        }
    }
}

Tilemap* LoadTilemap_Mem(void* content, size_t size) {
    Tilemap* map = calloc(1, sizeof(Tilemap));
    map->tilemap = cute_tiled_load_map_from_memory(content, size, NULL);
#if !defined(__PSP__)
    Uint32 format = 0;
    SDL_QueryTexture(tilesets_texture, &format, NULL, NULL, NULL);
    map->texture.front = SDL_CreateTexture(
        global_app.renderer, 0,
        SDL_TEXTUREACCESS_STATIC | SDL_TEXTUREACCESS_TARGET,
        map->tilemap->width * map->tilemap->tilewidth,
        map->tilemap->height * map->tilemap->tileheight
    );
    SDL_SetTextureBlendMode(map->texture.front, SDL_BLENDMODE_BLEND);
    map->texture.middle = SDL_CreateTexture(
        global_app.renderer, 0,
        SDL_TEXTUREACCESS_STATIC | SDL_TEXTUREACCESS_TARGET,
        map->tilemap->width * map->tilemap->tilewidth,
        map->tilemap->height * map->tilemap->tileheight
    );
    SDL_SetTextureBlendMode(map->texture.middle, SDL_BLENDMODE_BLEND);
    map->texture.back = SDL_CreateTexture(
        global_app.renderer, 0, SDL_TEXTUREACCESS_TARGET,
        map->tilemap->width * map->tilemap->tilewidth,
        map->tilemap->height * map->tilemap->tileheight
    );
    SDL_SetTextureBlendMode(map->texture.back, SDL_BLENDMODE_BLEND);

    SDL_SetRenderTarget(global_app.renderer, map->texture.back);
    DrawTilemapTotexture(map, TILEMAP_LAYERGROUP_BACK);
    SDL_SetRenderTarget(global_app.renderer, NULL);
    SDL_SetRenderTarget(global_app.renderer, map->texture.middle);
    DrawTilemapTotexture(map, TILEMAP_LAYERGROUP_MIDDLE);
    SDL_SetRenderTarget(global_app.renderer, NULL);
    SDL_SetRenderTarget(global_app.renderer, map->texture.front);
    DrawTilemapTotexture(map, TILEMAP_LAYERGROUP_FRONT);
    SDL_SetRenderTarget(global_app.renderer, NULL);
#endif
    return map;
}

Tilemap* LoadTilemap(char* filename) {
    size_t size;
    void* content = RespackGetItem(global_app.assets_pack, filename, &size);
    if (size == 0) {
        return NULL;
    }
    Tilemap* map = LoadTilemap_Mem(content, size);
    free(content);
    return map;
}

void FreeTilemap(Tilemap* map) {
    cute_tiled_free_map(map->tilemap);
#if !defined(__PSP__)
    SDL_DestroyTexture(map->texture.front);
    SDL_DestroyTexture(map->texture.middle);
    SDL_DestroyTexture(map->texture.back);
#endif
}
