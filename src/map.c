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

#include "map.h"
#include "entities/player.h"
#include "global.h"
#include "resource/loader.h"
#include <stdlib.h>

extern GameApp game_app;

SDL_Texture* terrains_texture = NULL;

void InitMapSystem() {
    terrains_texture = LoadTexture("maps/tilesets/terrains.png");
}

void QuitMapSystem() {
    SDL_DestroyTexture(terrains_texture);
}

SDL_Texture*
GetTextureRegionFromGID(Map* map, int gid, int* flip, SDL_Rect* rect) {
    for (Tileset* tileset = map->tilemap->tilesets; tileset;
         tileset = tileset->next) {
        if (tileset->firstgid <= gid &&
            tileset->firstgid + tileset->tilecount > gid) {
            int hflip, vflip, dflip, flip_temp = SDL_FLIP_NONE;
            cute_tiled_get_flags(gid, &hflip, &vflip, &dflip);
            if (hflip) {
                flip_temp |= SDL_FLIP_HORIZONTAL;
            }
            if (vflip) {
                flip_temp |= SDL_FLIP_VERTICAL;
            }
            *flip = flip_temp;
            int local_id = cute_tiled_unset_flags(gid) - tileset->firstgid;
            int dw = tileset->imagewidth / tileset->tilewidth;
            rect->x = local_id % dw * tileset->tilewidth;
            rect->y = local_id / dw * tileset->tileheight;
            rect->w = tileset->tilewidth;
            rect->h = tileset->tileheight;
            if (strcmp(tileset->image.ptr, "terrains.png") == 0) {
                return terrains_texture;
            }
        }
    }
    return NULL;
}

void CreateCollisionRectList(Map* map) {
    struct {
        int gid;
        int has_damage;
        int damage;
        int has_collision;
        SDL_Rect collision;
    } special_tile_cache[16];
    int special_tile_count = 0;
    for (Tileset* tileset = map->tilemap->tilesets; tileset;
         tileset = tileset->next) {
        for (TileDescriptor* info = tileset->tiles; info; info = info->next) {
            special_tile_cache[special_tile_count].gid =
                tileset->firstgid + info->tile_index;
            for (int i = 0; i < info->property_count; ++i) {
                TilemapProperty prop = info->properties[i];
                if (strcmp(prop.name.ptr, "has_damage") == 0 &&
                    prop.type == CUTE_TILED_PROPERTY_BOOL) {
                    special_tile_cache[special_tile_count].has_damage =
                        prop.data.boolean;
                } else if (strcmp(prop.name.ptr, "damage") == 0 &&
                           prop.type == CUTE_TILED_PROPERTY_INT) {
                    special_tile_cache[special_tile_count].damage =
                        prop.data.integer;
                }
            }
            if (info->objectgroup->objects) {
                TilemapObject* obj = info->objectgroup->objects;
                special_tile_cache[special_tile_count].has_collision = 1;
                special_tile_cache[special_tile_count].collision =
                    (SDL_Rect){obj->x, obj->y, obj->width, obj->height};
            }
            ++special_tile_count;
        }
    }
    map->collision_list = calloc(1, sizeof(CollisionRectNode));
    map->collision_list->next = NULL;
    for (TilemapLayer* layer = map->tilemap->layers; layer;
         layer = layer->next) {
        if (strncmp(layer->name.ptr, "middle", 6) != 0) {
            continue;
        }
        for (int i = 0; i < layer->data_count; ++i) {
            if (layer->data[i] == 0) {
                continue;
            }
            int x = i % layer->width;
            int y = i / layer->width;
            SDL_Rect dstrect = {
                x * map->tilemap->tilewidth, y * map->tilemap->tileheight,
                map->tilemap->tilewidth, map->tilemap->tileheight
            };
            CollisionRectNode* node = calloc(1, sizeof(CollisionRectNode));
            node->has_damage = 0;
            for (int now = 0; now < special_tile_count; ++now) {
                if (layer->data[i] == special_tile_cache[now].gid) {
                    if (special_tile_cache[now].has_damage) {
                        node->has_damage = 1;
                        node->damage = special_tile_cache[now].damage;
                    }
                    if (special_tile_cache[now].has_collision) {
                        SDL_Rect rect = special_tile_cache[now].collision;
                        dstrect.x += rect.x;
                        dstrect.y += rect.y;
                        dstrect.w = rect.w;
                        dstrect.h = rect.h;
                    }
                }
            }
            node->rect = dstrect;
            node->next = map->collision_list->next;
            map->collision_list->next = node;
        }
    }
}

#if defined(__PSP__)
void DrawMap(Map* map, TilemapLayerGroup group)
#else
void DrawMapTotexture(Map* map, TilemapLayerGroup group)
#endif
{
    for (TilemapLayer* layer = map->tilemap->layers; layer;
         layer = layer->next) {
        switch (group) {
        case TILEMAP_LAYERGROUP_FRONT:
            if (strncmp(layer->name.ptr, "front", 5) != 0) {
                continue;
                ;
            }
            break;
        case TILEMAP_LAYERGROUP_MIDDLE:
            if (strncmp(layer->name.ptr, "middle", 6) != 0) {
                continue;
            }
            break;
        case TILEMAP_LAYERGROUP_BACK:
            if (strncmp(layer->name.ptr, "back", 4) != 0) {
                continue;
            }
            break;
        }
        for (int i = 0; i < layer->data_count; ++i) {
            if (layer->data[i] == 0) {
                continue;
            }
            int x = i % layer->width;
            int y = i / layer->width;
            int flip;
            SDL_Rect srcrect;
            SDL_Rect dstrect = {
                x * map->tilemap->tilewidth, y * map->tilemap->tileheight,
                map->tilemap->tilewidth, map->tilemap->tileheight
            };
#if defined(__PSP__)
            dstrect.x += map->draw_offset.x;
            dstrect.y += map->draw_offset.y;
            dstrect.w *= map->draw_scale;
            dstrect.h *= map->draw_scale;
            if (!SDL_HasIntersection(&dstrect, &(SDL_Rect){0, 0, 480, 272})) {
                continue;
            }
#endif
            SDL_Texture* texture =
                GetTextureRegionFromGID(map, layer->data[i], &flip, &srcrect);
            SDL_RenderCopyEx(
                game_app.renderer, texture, &srcrect, &dstrect, 0, NULL, flip
            );
        }
    }
}

Map* LoadMapFromMem(void* content, size_t size) {
    Map* map = calloc(1, sizeof(Map));
    map->entity_list = CreateEntityList();
    map->tilemap = cute_tiled_load_map_from_memory(content, size, NULL);
    map->draw_scale = 1;
    map->draw_offset = (SDL_Point){0, 0};
    CreateCollisionRectList(map);
    for (TilemapLayer* layer = map->tilemap->layers; layer;
         layer = layer->next) {
        if (strcmp(layer->type.ptr, "objectgroup") == 0) {
            for (TilemapObject* obj = layer->objects; obj; obj = obj->next) {
                if (strcmp(obj->type.ptr, "EntityPosition") == 0 &&
                    strcmp(obj->name.ptr, "player_init") == 0) {
                    Entity* player = CreatePLayerEntity(map, obj->x, obj->y);
                    AddEntityToList(map->entity_list, player);
                }
            }
        }
    }
#if !defined(__PSP__)
    Uint32 format = 0;
    SDL_QueryTexture(terrains_texture, &format, NULL, NULL, NULL);
    map->texture.front = SDL_CreateTexture(
        game_app.renderer, 0,
        SDL_TEXTUREACCESS_STATIC | SDL_TEXTUREACCESS_TARGET,
        map->tilemap->width * map->tilemap->tilewidth,
        map->tilemap->height * map->tilemap->tileheight
    );
    SDL_SetTextureBlendMode(map->texture.front, SDL_BLENDMODE_BLEND);
    map->texture.middle = SDL_CreateTexture(
        game_app.renderer, 0,
        SDL_TEXTUREACCESS_STATIC | SDL_TEXTUREACCESS_TARGET,
        map->tilemap->width * map->tilemap->tilewidth,
        map->tilemap->height * map->tilemap->tileheight
    );
    SDL_SetTextureBlendMode(map->texture.middle, SDL_BLENDMODE_BLEND);
    map->texture.back = SDL_CreateTexture(
        game_app.renderer, 0, SDL_TEXTUREACCESS_TARGET,
        map->tilemap->width * map->tilemap->tilewidth,
        map->tilemap->height * map->tilemap->tileheight
    );
    SDL_SetTextureBlendMode(map->texture.back, SDL_BLENDMODE_BLEND);

    SDL_SetRenderTarget(game_app.renderer, map->texture.back);
    DrawMapTotexture(map, TILEMAP_LAYERGROUP_BACK);
    SDL_SetRenderTarget(game_app.renderer, NULL);
    SDL_SetRenderTarget(game_app.renderer, map->texture.middle);
    DrawMapTotexture(map, TILEMAP_LAYERGROUP_MIDDLE);
    SDL_SetRenderTarget(game_app.renderer, NULL);
    SDL_SetRenderTarget(game_app.renderer, map->texture.front);
    DrawMapTotexture(map, TILEMAP_LAYERGROUP_FRONT);
    SDL_SetRenderTarget(game_app.renderer, NULL);
#endif
    return map;
}

Map* LoadMap(char* filename) {
    size_t size;
    void* content = GetRespackItem(game_app.assets_pack, filename, &size);
    if (size == 0) {
        return NULL;
    }
    Map* map = LoadMapFromMem(content, size);
    free(content);
    return map;
}

void FreeMap(Map* map) {
    cute_tiled_free_map(map->tilemap);
    FreeEntityList(map->entity_list);
    CollisionRectNode* next = NULL;
    for (CollisionRectNode* node = map->collision_list->next; node;) {
        next = node->next;
        free(node);
        node = next;
    }
#if !defined(__PSP__)
    SDL_DestroyTexture(map->texture.front);
    SDL_DestroyTexture(map->texture.middle);
    SDL_DestroyTexture(map->texture.back);
#endif
}

void DrawMapLayer(Map* map, TilemapLayerGroup group) {
#if defined(__PSP__)
    DrawMap(map, group);
#else
    SDL_Texture* texture = NULL;
    switch (group) {
    case TILEMAP_LAYERGROUP_BACK:
        texture = map->texture.back;
        break;
    case TILEMAP_LAYERGROUP_MIDDLE:
        texture = map->texture.middle;
        break;
    case TILEMAP_LAYERGROUP_FRONT:
        texture = map->texture.front;
        break;
    }
    int w, h;
    SDL_QueryTexture(texture, NULL, NULL, &w, &h);
    SDL_RenderCopy(
        game_app.renderer, texture, NULL,
        &(SDL_Rect){map->draw_offset.x, map->draw_offset.y, w * map->draw_scale,
                    h * map->draw_scale}
    );
#endif
    if (group == TILEMAP_LAYERGROUP_MIDDLE) {
        ForEachEntity(entity, map->entity_list) {
            DrawEntity(entity->data);
        }
#if !defined(NDEBUG)
        for (CollisionRectNode* node = map->collision_list->next; node;
             node = node->next) {
            if (node->has_damage) {
                SDL_SetRenderDrawColor(game_app.renderer, 255, 0, 0, 48);
            } else {
                SDL_SetRenderDrawColor(game_app.renderer, 0, 255, 0, 48);
            }
            SDL_Rect rect = {
                node->rect.x * map->draw_scale + map->draw_offset.x,
                node->rect.y * map->draw_scale + map->draw_offset.y,
                node->rect.w * map->draw_scale, node->rect.h * map->draw_scale
            };
            SDL_RenderFillRect(game_app.renderer, &rect);
            SDL_SetRenderDrawColor(game_app.renderer, 0, 0, 0, 255);
            SDL_RenderDrawRect(game_app.renderer, &rect);
        }
#endif
    }
}

int MapIsEmptyEx(Map* map, SDL_FRect* rect, SDL_FRect* union_rect) {
    for (CollisionRectNode* node = map->collision_list->next; node;
         node = node->next) {
        SDL_FRect now = {
            node->rect.x, node->rect.y, node->rect.w, node->rect.h
        };
        if (SDL_HasIntersectionF(rect, &now) && !node->has_damage) {
            if (union_rect) {
                SDL_UnionFRect(rect, &now, union_rect);
            }
            return 0;
        }
    }
    return 1;
}

int MapIsEmpty(Map* map, SDL_FRect* rect) {
    return MapIsEmptyEx(map, rect, NULL);
}

int MapHasDamage(Map* map, SDL_FRect* rect) {
    for (CollisionRectNode* node = map->collision_list->next; node;
         node = node->next) {
        SDL_FRect now = {
            node->rect.x, node->rect.y, node->rect.w, node->rect.h
        };
        if (SDL_HasIntersectionF(rect, &now) && node->has_damage) {
            return node->damage;
        }
    }
    return 0;
}
