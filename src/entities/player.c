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

#define PLAYER_RUN_VELOCITY 120
#define PLAYER_JUMP_VELOCITY (-150)
#define CaptainImageGrid(x, y) RectFromImageGrid(504, 320, 8, 9, x, y)

SDL_Texture* captain_texture = NULL;
SDL_Rect idle_with_sword_animation_clip[] = {
    CaptainImageGrid(2, 3), CaptainImageGrid(3, 3), CaptainImageGrid(4, 3),
    CaptainImageGrid(5, 3), CaptainImageGrid(6, 3)
};
Animation* idle_with_sword_animation = NULL;
SDL_Rect run_with_sword_animation_clip[] = {
    CaptainImageGrid(7, 3), CaptainImageGrid(8, 3), CaptainImageGrid(0, 4),
    CaptainImageGrid(1, 4), CaptainImageGrid(2, 4), CaptainImageGrid(3, 4)
};
Animation* run_with_sword_animation = NULL;
SDL_Rect jump_with_sword_texture_rect[] = {
    CaptainImageGrid(4, 4), CaptainImageGrid(5, 4), CaptainImageGrid(6, 4)
};
SDL_Rect fall_with_sword_texture_rect = CaptainImageGrid(7, 4);
SDL_Rect ground_with_sword_texture_rect[] = {
    CaptainImageGrid(8, 4), CaptainImageGrid(0, 5)
};
Animation* attack_with_seord_animation_list[3] = {};
SDL_Rect attack1_with_seord_animation_rect[] = {
    CaptainImageGrid(5, 5), CaptainImageGrid(6, 5), CaptainImageGrid(7, 5)
};
SDL_Rect attack2_with_seord_animation_rect[] = {
    CaptainImageGrid(8, 5), CaptainImageGrid(0, 6), CaptainImageGrid(1, 6)
};
SDL_Rect attack3_with_seord_animation_rect[] = {
    CaptainImageGrid(2, 6), CaptainImageGrid(3, 6), CaptainImageGrid(4, 6)
};

SDL_Rect idle_without_sword_animation_clip[] = {
    CaptainImageGrid(0, 0), CaptainImageGrid(1, 0), CaptainImageGrid(2, 0),
    CaptainImageGrid(3, 0), CaptainImageGrid(4, 0)
};
Animation* idle_without_sword_animation = NULL;
SDL_Rect run_without_sword_animation_clip[] = {
    CaptainImageGrid(5, 0), CaptainImageGrid(6, 0), CaptainImageGrid(7, 0),
    CaptainImageGrid(8, 0), CaptainImageGrid(0, 1), CaptainImageGrid(1, 1),
};
Animation* run_without_sword_animation = NULL;
SDL_Rect jump_without_sword_texture_rect[] = {
    CaptainImageGrid(2, 1), CaptainImageGrid(3, 1), CaptainImageGrid(4, 1)
};
SDL_Rect fall_without_sword_texture_rect = CaptainImageGrid(5, 1);
SDL_Rect ground_without_sword_texture_rect[] = {
    CaptainImageGrid(6, 1), CaptainImageGrid(7, 1)
};

int is_attack_key_pressed = 0;
int is_jump_key_pressed = 0;

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
    attack_with_seord_animation_list[0] = CreateAnimation(
        captain_texture, 0.1, attack1_with_seord_animation_rect,
        SDL_arraysize(attack1_with_seord_animation_rect)
    );
    attack_with_seord_animation_list[1] = CreateAnimation(
        captain_texture, 0.1, attack2_with_seord_animation_rect,
        SDL_arraysize(attack2_with_seord_animation_rect)
    );
    attack_with_seord_animation_list[2] = CreateAnimation(
        captain_texture, 0.1, attack3_with_seord_animation_rect,
        SDL_arraysize(attack3_with_seord_animation_rect)
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
    FreeAnimation(attack_with_seord_animation_list[0]);
    FreeAnimation(attack_with_seord_animation_list[1]);
    FreeAnimation(attack_with_seord_animation_list[2]);
    FreeAnimation(idle_without_sword_animation);
    FreeAnimation(run_without_sword_animation);
}

Entity* CreatePlayerEntity(Map* map, float x, float y) {
    Entity* player = calloc(1, sizeof(Entity));
    player->type = ENTITY_TYPE_PLAYER;
    player->status = ENTITY_STATUS_IDLE;
    player->pos = (Vector2f){x, y};
    player->velocity = (Vector2f){0, 0};
    player->no_gravity_effect = 0;
    player->elastic_collision_factor = (Vector2f){0, 0};
    player->bbox = (SDL_FRect){16, 4, 16, 28};
    player->hitbox = (SDL_FRect){24, 11, 15, 7};
    player->map = map;
    player->event_handler = HandlePlayerEvent;
    PlayerUserData* data = calloc(1, sizeof(PlayerUserData));
    data->facing_right = 1;
    data->with_sword = 1;
    data->running_direction = 0;
    data->ground_cooldown_time = 0;
    player->userdata = data;
    attack_with_seord_animation_list[0]->userdata = player;
    attack_with_seord_animation_list[0]->on_animation_end =
        OnPlayerAttackAnimationEnd;
    attack_with_seord_animation_list[1]->userdata = player;
    attack_with_seord_animation_list[1]->on_animation_end =
        OnPlayerAttackAnimationEnd;
    attack_with_seord_animation_list[2]->userdata = player;
    attack_with_seord_animation_list[2]->on_animation_end =
        OnPlayerAttackAnimationEnd;
    return player;
}

void OnPlayerAttackAnimationEnd(void* userdata, Animation* anim) {
    Entity* player = (Entity*)userdata;
    assert(player->type == ENTITY_TYPE_PLAYER);
    PlayerUserData* data = (PlayerUserData*)player->userdata;
    if (player->velocity.x != 0) {
        player->status = ENTITY_STATUS_RUN;
    } else {
        player->status = ENTITY_STATUS_IDLE;
    }
    player->is_attacking = 0;
    data->last_attack_time = 0.5;
}

void TickPlayer(Entity* player, float dt) {
    assert(player->type == ENTITY_TYPE_PLAYER);
    PlayerUserData* data = (PlayerUserData*)player->userdata;
    // set map drawing offset
    int win_w, win_h;
    SDL_GetWindowSize(game_app.window, &win_w, &win_h);
    Map* map = player->map;
    map->draw_scale = 0.15 * win_h / map->tilemap->tileheight;
    if (map->draw_scale < 1) {
        map->draw_scale = 1;
    }
    map->draw_offset.x = win_w / 2.0 - player->pos.x * map->draw_scale;
    player->map->draw_offset.y = win_h / 1.5 - player->pos.y * map->draw_scale;
    if (map->draw_offset.x > 0) {
        map->draw_offset.x = 0;
    } else if (map->draw_offset.x < win_w - map->tilemap->width *
                                                map->tilemap->tilewidth *
                                                map->draw_scale) {
        map->draw_offset.x = win_w - map->tilemap->width *
                                         map->tilemap->tilewidth *
                                         map->draw_scale;
    }
    if (map->draw_offset.y > 0) {
        map->draw_offset.y = 0;
    } else if (map->draw_offset.y < win_h - map->tilemap->height *
                                                map->tilemap->tileheight *
                                                map->draw_scale) {
        map->draw_offset.y = win_h - map->tilemap->height *
                                         map->tilemap->tileheight *
                                         map->draw_scale;
    }
    // set status according to velocity
    if (player->velocity.y >= 0.5) {
        player->status = ENTITY_STATUS_FALL;
        player->is_attacking = 0;
    }
    if (player->status == ENTITY_STATUS_GROUND) {
        data->ground_cooldown_time -= dt;
        if (data->ground_cooldown_time <= 0) {
            player->status = ENTITY_STATUS_IDLE;
        }
    }
    if (player->velocity.y == 0 && player->status == ENTITY_STATUS_FALL) {
        player->status = ENTITY_STATUS_GROUND;
        data->ground_cooldown_time = 0.12;
    }
    // change attack variants
    if (data->last_attack_time >= 0) {
        data->last_attack_time -= dt;
    }
    int left = data->running_direction == 1;
    int right = data->running_direction == 2;
    if (game_app.joystick.available) {
        left = left || SDL_GameControllerGetAxis(
                           game_app.joystick.device, SDL_CONTROLLER_AXIS_LEFTX
                       ) < -16384;
        left = left ||
               SDL_GameControllerGetButton(
                   game_app.joystick.device, SDL_CONTROLLER_BUTTON_DPAD_LEFT
               );
        right = right || SDL_GameControllerGetAxis(
                             game_app.joystick.device, SDL_CONTROLLER_AXIS_LEFTX
                         ) > 16384;
        right = right ||
                SDL_GameControllerGetButton(
                    game_app.joystick.device, SDL_CONTROLLER_BUTTON_DPAD_RIGHT
                );
    }
    if (left && right) {
        // do nothing
    } else if (left) {
        if (player->status == ENTITY_STATUS_IDLE &&
            player->status != ENTITY_STATUS_GROUND &&
            player->status != ENTITY_STATUS_ATTACK) {
            player->status = ENTITY_STATUS_RUN;
        }
        player->bbox = (SDL_FRect){24, 4, 16, 28};
        player->velocity.x = -PLAYER_RUN_VELOCITY;
        data->facing_right = 0;
    } else if (right) {
        if (player->status == ENTITY_STATUS_IDLE &&
            player->status != ENTITY_STATUS_GROUND &&
            player->status != ENTITY_STATUS_ATTACK) {
            player->status = ENTITY_STATUS_RUN;
        }
        player->bbox = (SDL_FRect){16, 4, 16, 28};
        player->velocity.x = PLAYER_RUN_VELOCITY;
        data->facing_right = 1;
    } else {
        if (player->status == ENTITY_STATUS_RUN) {
            player->status = ENTITY_STATUS_IDLE;
        }
        player->velocity.x = 0;
    }
    if (data->facing_right) {
        if (data->attack_variant == 0) {
            player->hitbox = (SDL_FRect){25, 11, 15, 7};
        } else {
            player->hitbox = (SDL_FRect){27, 19, 9, 26};
        }
    } else {
        if (data->attack_variant == 0) {
            player->hitbox = (SDL_FRect){-24, 11, 15, 7};
        } else {
            player->hitbox = (SDL_FRect){-20, 19, 9, 26};
        }
    }
}

void HandlePlayerEvent(Entity* player, SDL_Event* event) {
    assert(player->type == ENTITY_TYPE_PLAYER);
    PlayerUserData* data = (PlayerUserData*)player->userdata;
    switch (event->type) {
    case SDL_KEYDOWN:
        if (event->key.keysym.sym == SDLK_LEFT) {
            data->running_direction = 1;
        } else if (event->key.keysym.sym == SDLK_RIGHT) {
            data->running_direction = 2;
        } else if (event->key.keysym.sym == SDLK_x) {
            if (player->status != ENTITY_STATUS_JUMP &&
                player->status != ENTITY_STATUS_FALL && !is_jump_key_pressed) {
                player->status = ENTITY_STATUS_JUMP;
                player->velocity.y = PLAYER_JUMP_VELOCITY;
                is_jump_key_pressed = 1;
            }
        } else if (event->key.keysym.sym == SDLK_z) {
            if (player->status != ENTITY_STATUS_ATTACK &&
                player->status != ENTITY_STATUS_JUMP &&
                player->status != ENTITY_STATUS_FALL &&
                !is_attack_key_pressed) {
                player->status = ENTITY_STATUS_ATTACK;
                player->is_attacking = 1;
                is_attack_key_pressed = 1;
                if (data->last_attack_time > 0) {
                    ++data->attack_variant;
                    if (data->attack_variant == 3) {
                        data->attack_variant = 0;
                    }
                }
                if (data->last_attack_time < 0) {
                    data->attack_variant = 0;
                }
            }
        }
        break;
    case SDL_KEYUP:
        if (event->key.keysym.sym == SDLK_LEFT ||
            event->key.keysym.sym == SDLK_RIGHT) {
            data->running_direction = 0;
        } else if (event->key.keysym.sym == SDLK_x) {
            is_jump_key_pressed = 0;
        } else if (event->key.keysym.sym == SDLK_z) {
            is_attack_key_pressed = 0;
        }
        break;
    case SDL_CONTROLLERBUTTONDOWN:
        if (event->cbutton.button == SDL_CONTROLLER_BUTTON_A) {
            if (player->status != ENTITY_STATUS_JUMP &&
                player->status != ENTITY_STATUS_FALL) {
                player->status = ENTITY_STATUS_JUMP;
                player->velocity.y = PLAYER_JUMP_VELOCITY;
            }
        } else if (event->cbutton.button == SDL_CONTROLLER_BUTTON_X) {
            if (player->status != ENTITY_STATUS_ATTACK &&
                player->status != ENTITY_STATUS_JUMP &&
                player->status != ENTITY_STATUS_FALL) {
                player->status = ENTITY_STATUS_ATTACK;
                player->is_attacking = 1;
                if (data->last_attack_time > 0) {
                    ++data->attack_variant;
                    if (data->attack_variant == 3) {
                        data->attack_variant = 0;
                    }
                }
                if (data->last_attack_time < 0) {
                    data->attack_variant = 0;
                }
            }
        }
    }
}

void DrawPlayerEntity(Entity* player) {
    assert(player->type == ENTITY_TYPE_PLAYER);
    PlayerUserData* data = (PlayerUserData*)player->userdata;
    int scale = player->map->draw_scale;
    int x = player->pos.x * scale + player->map->draw_offset.x -
            player->bbox.x * scale;
    int y = player->pos.y * scale + player->map->draw_offset.y -
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
    } else if (player->status == ENTITY_STATUS_GROUND) {
        int index = 0;
        if (data->ground_cooldown_time < 0.1) {
            index = 1;
        }
        SDL_Rect* texture_rect = data->with_sword
                                   ? ground_with_sword_texture_rect
                                   : ground_without_sword_texture_rect;
        SDL_RenderCopyEx(
            game_app.renderer, captain_texture, &texture_rect[index],
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
        case ENTITY_STATUS_ATTACK:
            anim = attack_with_seord_animation_list[data->attack_variant];
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
    switch (player->status) {
    case ENTITY_STATUS_IDLE:
        r = 255;
        break;
    case ENTITY_STATUS_RUN:
        g = 255;
        break;
    case ENTITY_STATUS_JUMP:
        b = 255;
        break;
    case ENTITY_STATUS_FALL:
        r = 255;
        g = 128;
        break;
    case ENTITY_STATUS_GROUND:
        r = 255;
        b = 128;
        break;
    case ENTITY_STATUS_ATTACK:
        g = 255;
        b = 255;
        break;
    default:
        break;
    }
    SDL_SetRenderDrawColor(game_app.renderer, r, g, b, 255);
    SDL_FRect bbox = {
        player->pos.x * scale + player->map->draw_offset.x,
        player->pos.y * scale + player->map->draw_offset.y -
            player->bbox.h * scale,
        player->bbox.w * scale, player->bbox.h * scale
    };
    SDL_RenderDrawRectF(game_app.renderer, &bbox);
    if (player->is_attacking) {
        SDL_SetRenderDrawColor(game_app.renderer, 255, 128, 0, 128);
        SDL_FRect hitbox = {
            player->pos.x * scale + player->map->draw_offset.x +
                player->hitbox.x * scale,
            player->pos.y * scale + player->map->draw_offset.y -
                player->hitbox.y * scale,
            player->hitbox.w * scale, player->hitbox.h * scale
        };
        SDL_RenderFillRectF(game_app.renderer, &hitbox);
    }
#endif
}

void FreePlayerEntity(Entity* player) {
    assert(player->type == ENTITY_TYPE_PLAYER);
    free(player->userdata);
    free(player);
}
