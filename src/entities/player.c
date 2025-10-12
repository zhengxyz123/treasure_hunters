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

#include "player.h"
#include "../map.h"
#include "../resources/loader.h"
#include <assert.h>

SDL_Texture* captain_texture = NULL;

void InitPlayerTexture() {
    captain_texture = LoadTexture("images/characters/captain.png");
}

void FreePlayerTexture() {
    SDL_DestroyTexture(captain_texture);
}

Entity* CreatePLayerEntity(Map* map) {
    Entity* player = calloc(1, sizeof(Entity));
    player->type = ENTITY_TYPE_PLAYER;
    player->map = map;
    PlayerUserData* data = calloc(1, sizeof(PlayerUserData));
    data->dummy = 42;
    player->userdata = data;
    return player;
}

void TickPlayer(Entity* player, float dt) {}

void DrawPlayerEntity(Entity* player) {
    assert(player->type == ENTITY_TYPE_PLAYER);
    // PlayerUserData* data = (PlayerUserData*)player->userdata;
}

void DestroyPlayerEntity(Entity* player) {
    free(player->userdata);
    free(player);
}
