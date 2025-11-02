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

#ifndef TH_ENTITIES_BASE_H_
#define TH_ENTITIES_BASE_H_

#include "../global.h"
#include <SDL.h>

struct Map;
typedef struct Map Map;

typedef enum EntityType {
    ENTITY_TYPE_PLAYER
} EntityType;

typedef enum EntityStatus {
    ENTITY_STATUS_IDLE,
    ENTITY_STATUS_RUN,
    ENTITY_STATUS_JUMP,
    ENTITY_STATUS_FLY,
    ENTITY_STATUS_FALL,
    ENTITY_STATUS_GROUND,
    ENTITY_STATUS_ATTACK,
    ENTITY_STATUS_HURT
} EntityStatus;

typedef struct Entity {
    EntityType type;
    EntityStatus status;
    Vector2f pos;
    int no_gravity_effect;
    Vector2f velocity;
    Vector2f elastic_collision_factor;
    SDL_FRect bbox;
    int health;
    int damage;
    int is_attacking;
    SDL_FRect hitbox;
    int can_interact;
    char* interact_text_id;
    struct {
        float cooldown_time;
        float immortal_time;
    } take_damage;
    int should_delete;
    Map* map;
    void (*event_handler)(struct Entity*, SDL_Event*);
    void* userdata;
} Entity;

typedef struct EntityListNode {
    Entity* data;
    struct EntityListNode* next;
} EntityListNode;

typedef EntityListNode* EntityList;

#define ForEachEntity(element, list)                                           \
    for (EntityListNode* element = list ? (list)->next : NULL; element;        \
         element = (element)->next)

void InitEntitySystem();
void QuitEntitySystem();
EntityList CreateEntityList();
void FreeEntityList(EntityList list);
void AddEntityToList(EntityList list, Entity* entity);
void RemoveEntityInList(EntityList list, Entity* entity);
void TickEntityList(EntityList list, float dt);
void HandleEntityEvent(EntityList list, SDL_Event* event);
void FreeEntity(Entity* entity);
void DrawEntity(Entity* entity);

#endif
