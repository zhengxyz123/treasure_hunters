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

#include "base.h"
#include "../map.h"
#include "player.h"
#include <stdarg.h>
#include <stdlib.h>

#define GRAVITY_Y 240

void InitEntitySystem() {
    InitPlayerTexture();
}

void QuitEntitySystem() {
    FreePlayerTexture();
}

EntityList CreateEntityList() {
    EntityListNode* node = calloc(1, sizeof(EntityListNode));
    node->next = NULL;
    return node;
}

void FreeEntityList(EntityList list) {
    EntityListNode* next = NULL;
    for (EntityListNode* node = list->next; node;) {
        next = node->next;
        free(node);
        node = next;
    }
}

void AddEntityToList(EntityList list, Entity* entity) {
    EntityListNode* node = calloc(1, sizeof(EntityListNode));
    node->data = entity;
    node->next = list->next;
    list->next = node;
}

void RemoveEntityInList(EntityList list, Entity* entity) {
    EntityListNode* prev = list->next;
    for (EntityListNode* node = prev->next; node; node = node->next) {
        if (node->data == entity) {
            prev->next = node->next;
            free(node);
            return;
        }
        prev = node;
    }
}

/*
  The helper function to find the X coordinate of the closest left or right wall
  (according to the velocity direction) to prevent collision.
*/
float GetWallX(Map* map, float vx, SDL_FRect* bbox) {
    if (vx > 0) {
        // entity is running right and stuck
        for (int x = bbox->x; x > bbox->x - bbox->w; --x) {
            if (MapIsEmpty(map, &(SDL_FRect){x, bbox->y, bbox->w, bbox->h})) {
                return x;
            }
        }
    } else if (vx < 0) {
        // entity is running left and stuck
        for (int x = bbox->x - 1; x < bbox->x + bbox->w; ++x) {
            if (MapIsEmpty(map, &(SDL_FRect){x, bbox->y, bbox->w, bbox->h})) {
                return x;
            }
        }
    }
    return bbox->x;
}

/*
  The helper function to find the Y coordinate of the closest ground or ceiling
  surface (according to the velocity direction) to prevent collision.
*/
float GetGroundOrCeilingY(Map* map, float vy, SDL_FRect* bbox) {
    if (vy > 0) {
        // entity is falling and stuck
        for (int y = bbox->y + 1; y > bbox->y - bbox->h; --y) {
            if (MapIsEmpty(map, &(SDL_FRect){bbox->x, y, bbox->w, bbox->h})) {
                return y;
            }
        }
    } else if (vy < 0) {
        // entity is jumping and stuck
        for (int y = bbox->y - 1; y < bbox->y + bbox->h; ++y) {
            if (MapIsEmpty(map, &(SDL_FRect){bbox->x, y, bbox->w, bbox->h})) {
                return y;
            }
        }
    }
    return bbox->y;
}

void TickEntityList(EntityList list, float dt) {
    for (EntityListNode* node = list->next; node; node = node->next) {
        if (node->data->should_delete) {
            RemoveEntityInList(list, node->data);
            continue;
        }
        Entity* entity = node->data;
        SDL_FRect bbox =
            (SDL_FRect){entity->pos.x, entity->pos.y - entity->bbox.h,
                        entity->bbox.w, entity->bbox.h};
        SDL_FRect test_bbox = bbox;
        // simulate gravity and deal with collision
        if (!entity->no_gravity_effect) {
            Vector2f gravity = {0, GRAVITY_Y};
            entity->velocity.y += gravity.y * dt;
        }
        test_bbox.x += entity->velocity.x * dt;
        if (MapIsEmpty(entity->map, &test_bbox)) {
            bbox.x = test_bbox.x;
        } else {
            bbox.x = GetWallX(entity->map, entity->velocity.x, &test_bbox);
            entity->velocity.x *= -entity->elastic_collision_factor.x;
            test_bbox.x = bbox.x;
        }
        test_bbox.y += entity->velocity.y * dt;
        if (MapIsEmpty(entity->map, &test_bbox)) {
            bbox.y = test_bbox.y;
        } else {
            bbox.y = GetGroundOrCeilingY(
                entity->map, entity->velocity.y, &test_bbox
            );
            entity->velocity.y *= -entity->elastic_collision_factor.y;
        }
        entity->pos.x = bbox.x;
        entity->pos.y = bbox.y + entity->bbox.h;
        switch (entity->type) {
        case ENTITY_TYPE_PLAYER:
            TickPlayer(entity, dt);
            break;
        }
    }
}

void HandleEntityEvent(EntityList list, SDL_Event* event) {
    for (EntityListNode* node = list->next; node; node = node->next) {
        if (node->data->event_handler) {
            node->data->event_handler(node->data, event);
        }
    }
}

void FreeEntity(Entity* entity) {
    switch (entity->type) {
    case ENTITY_TYPE_PLAYER:
        FreePlayerEntity(entity);
        break;
    }
}

void DrawEntity(Entity* entity) {
    switch (entity->type) {
    case ENTITY_TYPE_PLAYER:
        DrawPlayerEntity(entity);
        break;
    }
}
