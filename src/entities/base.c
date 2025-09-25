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
#include "player.h"
#include <stdarg.h>
#include <stdlib.h>

#define ForEachEntity(element, list)                                           \
    for (EntityListNode* element = list ? (list)->next : NULL; element;        \
         element = (element)->next)

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

void DestroyEntity(Entity* entity) {
    switch (entity->type) {
    case ENTITY_TYPE_PLAYER:
        DestroyPlayerEntity(entity);
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
