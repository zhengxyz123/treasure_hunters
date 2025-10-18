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
#include "../image/image.h"
#include "../map.h"
#include "../resource/loader.h"
#include <assert.h>
#include <math.h>

#define PLAYER_RUN_VELOCITY 100
#define PLAYER_JUMP_VELOCITY (-150)

SDL_Texture* captain_texture = NULL;
SDL_Rect idle_with_sword_animation_clip[] = {
    RectFromImageGrid(504, 320, 8, 9, 2, 3),
    RectFromImageGrid(504, 320, 8, 9, 3, 3),
    RectFromImageGrid(504, 320, 8, 9, 4, 3),
    RectFromImageGrid(504, 320, 8, 9, 5, 3),
    RectFromImageGrid(504, 320, 8, 9, 6, 3)
};
Animation* idle_with_sword_animation = NULL;
SDL_Rect run_with_sword_animation_clip[] = {
    RectFromImageGrid(504, 320, 8, 9, 7, 3),
    RectFromImageGrid(504, 320, 8, 9, 8, 3),
    RectFromImageGrid(504, 320, 8, 9, 0, 4),
    RectFromImageGrid(504, 320, 8, 9, 1, 4),
    RectFromImageGrid(504, 320, 8, 9, 2, 4),
    RectFromImageGrid(504, 320, 8, 9, 3, 4)
};
Animation* run_with_sword_animation = NULL;
SDL_Rect jump_with_sword_texture_rect[] = {
    RectFromImageGrid(504, 320, 8, 9, 4, 4),
    RectFromImageGrid(504, 320, 8, 9, 5, 4),
    RectFromImageGrid(504, 320, 8, 9, 6, 4)
};
SDL_Rect fall_with_sword_texture_rect = RectFromImageGrid(504, 320, 8, 9, 7, 4);

SDL_Rect idle_without_sword_animation_clip[] = {
    RectFromImageGrid(504, 320, 8, 9, 0, 0),
    RectFromImageGrid(504, 320, 8, 9, 1, 0),
    RectFromImageGrid(504, 320, 8, 9, 2, 0),
    RectFromImageGrid(504, 320, 8, 9, 3, 0),
    RectFromImageGrid(504, 320, 8, 9, 4, 0)
};
Animation* idle_without_sword_animation = NULL;
SDL_Rect run_without_sword_animation_clip[] = {
    RectFromImageGrid(504, 320, 8, 9, 5, 0),
    RectFromImageGrid(504, 320, 8, 9, 6, 0),
    RectFromImageGrid(504, 320, 8, 9, 7, 0),
    RectFromImageGrid(504, 320, 8, 9, 8, 0),
    RectFromImageGrid(504, 320, 8, 9, 0, 1),
    RectFromImageGrid(504, 320, 8, 9, 1, 1),
};
Animation* run_without_sword_animation = NULL;
SDL_Rect jump_without_sword_texture_rect[] = {
    RectFromImageGrid(504, 320, 8, 9, 2, 1),
    RectFromImageGrid(504, 320, 8, 9, 3, 1),
    RectFromImageGrid(504, 320, 8, 9, 4, 1)
};
SDL_Rect fall_without_sword_texture_rect =
    RectFromImageGrid(504, 320, 8, 9, 5, 1);

void InitPlayerTexture() {
    captain_texture = LoadTexture("images/characters/captain.png");
    idle_with_sword_animation = CreateAnimation(
        captain_texture, 0.1, idle_with_sword_animation_clip,
        SDL_arraysize(idle_with_sword_animation_clip)
    );
    run_with_sword_animation = CreateAnimation(
        captain_texture, 0.1, run_with_sword_animation_clip,
        SDL_arraysize(run_with_sword_animation_clip)
    );
    idle_without_sword_animation = CreateAnimation(
        captain_texture, 0.1, idle_without_sword_animation_clip,
        SDL_arraysize(idle_without_sword_animation_clip)
    );
    run_without_sword_animation = CreateAnimation(
        captain_texture, 0.1, run_without_sword_animation_clip,
        SDL_arraysize(run_without_sword_animation_clip)
    );
}

void FreePlayerTexture() {
    SDL_DestroyTexture(captain_texture);
    FreeAnimation(idle_with_sword_animation);
    FreeAnimation(run_with_sword_animation);
    FreeAnimation(idle_without_sword_animation);
    FreeAnimation(run_without_sword_animation);
}

Entity* CreatePLayerEntity(Map* map, float x, float y) {
    Entity* player = calloc(1, sizeof(Entity));
    player->type = ENTITY_TYPE_PLAYER;
    player->status = ENTITY_STATUS_IDLE;
    player->pos = (SDL_FPoint){x, y};
    player->velocity = (Vector2f){0, 0};
    player->bbox = (SDL_FRect){12, 4, 24, 28};
    player->map = map;
    player->event_handler = HandlePlayerEvent;
    PlayerUserData* data = calloc(1, sizeof(PlayerUserData));
    data->facing_right = 1;
    data->with_sword = 1;
    player->userdata = data;
    return player;
}

void TickPlayer(Entity* player, float dt) {
    assert(player->type == ENTITY_TYPE_PLAYER);
    PlayerUserData* data = (PlayerUserData*)player->userdata;
    int win_w, win_h;
    SDL_GetWindowSize(game_app.window, &win_w, &win_h);
    player->map->draw_scale = 0.15 * win_h / player->map->tilemap->tileheight;
    if (player->map->draw_scale < 1) {
        player->map->draw_scale = 1;
    }
    player->map->draw_offset.x =
        win_w / 2.0 - player->pos.x * player->map->draw_scale;
    player->map->draw_offset.y =
        win_h / 1.5 - player->pos.y * player->map->draw_scale;
    if (player->velocity.y >= 0.5) {
        player->status = ENTITY_STATUS_FALL;
    }
    if (player->velocity.y == 0 && player->status == ENTITY_STATUS_FALL) {
        if (fabs(player->velocity.x) > 40) {
            player->status = ENTITY_STATUS_RUN;
        } else {
            player->status = ENTITY_STATUS_IDLE;
        }
    }
    if (game_app.joystick.available) {
        int left = SDL_GameControllerGetAxis(
                       game_app.joystick.device, SDL_CONTROLLER_AXIS_LEFTX
                   ) < -16384;
        left = left ||
               SDL_GameControllerGetButton(
                   game_app.joystick.device, SDL_CONTROLLER_BUTTON_DPAD_LEFT
               );
        int right = SDL_GameControllerGetAxis(
                        game_app.joystick.device, SDL_CONTROLLER_AXIS_LEFTX
                    ) > 16384;
        right = right ||
                SDL_GameControllerGetButton(
                    game_app.joystick.device, SDL_CONTROLLER_BUTTON_DPAD_RIGHT
                );
        if (left && right) {
            // do nothing
        } else if (left) {
            if (player->status == ENTITY_STATUS_IDLE) {
                player->status = ENTITY_STATUS_RUN;
            }
            player->bbox = (SDL_FRect){20, 4, 24, 28};
            player->velocity.x = -PLAYER_RUN_VELOCITY;
            data->facing_right = 0;
        } else if (right) {
            if (player->status == ENTITY_STATUS_IDLE) {
                player->status = ENTITY_STATUS_RUN;
            }
            player->bbox = (SDL_FRect){12, 4, 24, 28};
            player->velocity.x = PLAYER_RUN_VELOCITY;
            data->facing_right = 1;
        } else {
            if (player->status == ENTITY_STATUS_RUN) {
                player->status = ENTITY_STATUS_IDLE;
            }
            player->velocity.x = 0;
        }
    }
}

void HandlePlayerEvent(Entity* player, SDL_Event* event) {
    assert(player->type == ENTITY_TYPE_PLAYER);
    PlayerUserData* data = (PlayerUserData*)player->userdata;
    switch (event->type) {
    case SDL_KEYDOWN:
        if (event->key.keysym.sym == SDLK_LEFT) {
            if (player->status == ENTITY_STATUS_IDLE) {
                player->status = ENTITY_STATUS_RUN;
            }
            player->bbox = (SDL_FRect){20, 4, 24, 28};
            player->velocity.x = -PLAYER_RUN_VELOCITY;
            data->facing_right = 0;
        } else if (event->key.keysym.sym == SDLK_RIGHT) {
            if (player->status == ENTITY_STATUS_IDLE) {
                player->status = ENTITY_STATUS_RUN;
            }
            player->bbox = (SDL_FRect){12, 4, 24, 28};
            player->velocity.x = PLAYER_RUN_VELOCITY;
            data->facing_right = 1;
        } else if (event->key.keysym.sym == SDLK_x) {
            if (player->status != ENTITY_STATUS_JUMP &&
                player->status != ENTITY_STATUS_FALL) {
                player->status = ENTITY_STATUS_JUMP;
                player->velocity.y = PLAYER_JUMP_VELOCITY;
            }
        }
        break;
    case SDL_KEYUP:
        if (event->key.keysym.sym == SDLK_LEFT ||
            event->key.keysym.sym == SDLK_RIGHT) {
            if (player->status == ENTITY_STATUS_RUN) {
                player->status = ENTITY_STATUS_IDLE;
            }
            player->velocity.x = 0;
        }
        break;
    case SDL_CONTROLLERBUTTONDOWN:
        if (event->cbutton.button == SDL_CONTROLLER_BUTTON_A) {
            if (player->status != ENTITY_STATUS_JUMP &&
                player->status != ENTITY_STATUS_FALL) {
                player->status = ENTITY_STATUS_JUMP;
                player->velocity.y = PLAYER_JUMP_VELOCITY;
            }
        }
    }
}

void DrawPlayerEntity(Entity* player) {
    assert(player->type == ENTITY_TYPE_PLAYER);
    PlayerUserData* data = (PlayerUserData*)player->userdata;
    int scale = player->map->draw_scale;
    float x = player->pos.x * scale + player->map->draw_offset.x -
              player->bbox.x * scale;
    float y = player->pos.y * scale + player->map->draw_offset.y -
              (player->bbox.y + player->bbox.h) * scale;
    if (player->status == ENTITY_STATUS_JUMP) {
        int index = 0;
        if (player->velocity.y < -100) {
            index = 0;
        } else if (player->velocity.y < -50) {
            index = 1;
        } else {
            index = 2;
        }
        SDL_Rect* texture_rect = data->with_sword
                                   ? jump_with_sword_texture_rect
                                   : jump_without_sword_texture_rect;
        SDL_RenderCopyEx(
            game_app.renderer, captain_texture, &texture_rect[index],
            &(SDL_Rect){x, y, 56 * scale, 40 * scale}, 0, NULL,
            data->facing_right ? SDL_FLIP_NONE : SDL_FLIP_HORIZONTAL
        );
    } else if (player->status == ENTITY_STATUS_FALL) {
        SDL_Rect* texture_rect = data->with_sword
                                   ? &fall_with_sword_texture_rect
                                   : &fall_without_sword_texture_rect;
        SDL_RenderCopyEx(
            game_app.renderer, captain_texture, texture_rect,
            &(SDL_Rect){x, y, 56 * scale, 40 * scale}, 0, NULL,
            data->facing_right ? SDL_FLIP_NONE : SDL_FLIP_HORIZONTAL
        );
    } else {
        Animation* anim = NULL;
        switch (player->status) {
        case ENTITY_STATUS_IDLE:
            anim = data->with_sword ? idle_with_sword_animation
                                    : idle_without_sword_animation;
            break;
        case ENTITY_STATUS_RUN:
            anim = data->with_sword ? run_with_sword_animation
                                    : run_without_sword_animation;
            break;
        default:
            break;
        }
        DrawAnimationEx(
            anim, x, y, scale, 0, NULL,
            data->facing_right ? SDL_FLIP_NONE : SDL_FLIP_HORIZONTAL
        );
    }
#if !defined(NDEBUG)
    int r = 0, g = 0, b = 0;
    if (player->status == ENTITY_STATUS_IDLE) {
        r = 255;
    } else if (player->status == ENTITY_STATUS_RUN) {
        g = 255;
    } else if (player->status == ENTITY_STATUS_JUMP) {
        b = 255;
    } else if (player->status == ENTITY_STATUS_FALL) {
        r = 255;
        g = 128;
    }
    SDL_SetRenderDrawColor(game_app.renderer, r, g, b, 255);
    SDL_FRect bbox = {
        player->pos.x * scale + player->map->draw_offset.x,
        player->pos.y * scale + player->map->draw_offset.y -
            player->bbox.h * scale,
        player->bbox.w * scale, player->bbox.h * scale
    };
    SDL_RenderDrawRectF(game_app.renderer, &bbox);
#endif
}

void FreePlayerEntity(Entity* player) {
    assert(player->type == ENTITY_TYPE_PLAYER);
    free(player->userdata);
    free(player);
}
