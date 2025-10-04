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

#include "entities/base.h"
#include "global.h"
#include "map.h"
#include "resources/loader.h"
#include "scenes/setting_menu.h"
#include "scenes/start_menu.h"
#include "scenes/world.h"
#include "setting.h"
#include "translation.h"
#include "ui/ui.h"
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#if defined(__WIN32__)
    #include <Windows.h>
#endif

#define CUTE_TILED_IMPLEMENTATION
#include "cute_tiled.h"
#define FRAMETIMER_IMPLEMENTATION
#include "frametimer.h"

GameApp game_app = {
    .status = GAMESTATUS_NORMAL,
    .joystick = {.available = 0, .which = INT_MIN},
    .should_quit = 0,
    .window_focused = 1
};
Setting game_setting = {
#if !defined(__PSP__) && !defined(__vita__)
    .fullscreen = 0,
#endif
    .music_volume = 64,
    .sfx_volume = 64,
#if defined(__PSP__) || defined(__vita__)
    .mute_all = 0
#else
    .mute_when_unfocused = 1
#endif
};

// main function for Linux
int main(int argc, char* argv[]) {
    srand((unsigned)time(NULL));

    // initialise SDL, SDL_image, SDL_mixer and SDL_ttf
    if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
        SDL_LogError(
            SDL_LOG_CATEGORY_ERROR, "SDL_Init(): %s\n", SDL_GetError()
        );
        return 1;
    }
    SDL_SetHint(SDL_HINT_APP_NAME, "Treasure Hunters");
    SDL_SetHint(SDL_HINT_AUDIO_DEVICE_APP_NAME, "Treasure Hunters");
    SDL_SetHint(SDL_HINT_RENDER_VSYNC, "1");
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "nearest");
    if (IMG_Init(IMG_INIT_PNG) != IMG_INIT_PNG) {
        SDL_LogError(
            SDL_LOG_CATEGORY_ERROR, "IMG_Init(): %s\n", SDL_GetError()
        );
        return 1;
    }
    if (Mix_Init(MIX_INIT_OGG) != MIX_INIT_OGG) {
        SDL_LogError(
            SDL_LOG_CATEGORY_ERROR, "Mix_Init(): %s\n", SDL_GetError()
        );
        return 1;
    }
    Mix_OpenAudio(48000, AUDIO_S16SYS, 2, 2048);
#if !defined(TH_FALLBACK_TO_BITMAP_FONT)
    if (TTF_Init() != 0) {
        SDL_LogError(
            SDL_LOG_CATEGORY_ERROR, "TTF_Init(): %s\n", SDL_GetError()
        );
    }
#endif

    // setup global_app
    game_app.argc = argc;
    game_app.argv = argv;
    game_app.exec_path = (char*)calloc(PATH_MAX, sizeof(char));
    char* base_path = SDL_GetBasePath();
    strcpy(game_app.exec_path, base_path);
    SDL_free(base_path);
    char* rpkg_path = calloc(PATH_MAX, sizeof(char));
    strcpy(rpkg_path, game_app.exec_path);
    strcat(rpkg_path, "assets.rpkg");
    game_app.assets_pack = LoadRespack(rpkg_path);
    if (!game_app.assets_pack) {
        SDL_LogError(
            SDL_LOG_CATEGORY_ERROR, "%s is not a resource pack", rpkg_path
        );
        char* message = calloc(PATH_MAX + 24, sizeof(char));
        sprintf(message, "%s is not a resource pack", rpkg_path);
        SDL_ShowSimpleMessageBox(
            SDL_MESSAGEBOX_ERROR, "Treasure Hunter", message, NULL
        );
        free(rpkg_path);
        free(message);
        free(game_app.exec_path);
        return 1;
    }
    free(rpkg_path);

    // create the window, renderer and set the window icon
#if defined(__PSP__)
    game_app.window = SDL_CreateWindow(
        "Treasure Hunters", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
        480, 272, 0
    );
#else
    game_app.window = SDL_CreateWindow(
        "Treasure Hunters", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 966,
        544, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE
    );
#endif
    if (game_app.window == NULL) {
        SDL_LogError(
            SDL_LOG_CATEGORY_ERROR, "SDL_CreateWindow(): %s\n", SDL_GetError()
        );
        return 1;
    }
#if !defined(__PSP__) && !defined(__vita__)
    SDL_Surface* icon_image = LoadSurface("images/icon.png");
    SDL_SetWindowIcon(game_app.window, icon_image);
    SDL_SetWindowMinimumSize(game_app.window, 640, 480);
#endif
    game_app.renderer = SDL_CreateRenderer(
        game_app.window, -1,
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC |
            SDL_RENDERER_TARGETTEXTURE
    );
    SDL_RenderSetVSync(game_app.renderer, 1);
    SDL_SetRenderDrawBlendMode(game_app.renderer, SDL_BLENDMODE_BLEND);

    // restore previous settings
    InitSetting();
#if defined(__PSP__) || defined(__vita__)
    Mix_Volume(
        MUSIC_CHANNEL, game_setting.mute_all ? 0 : game_setting.music_volume
    );
    Mix_Volume(
        SFX_CHANNEL, game_setting.mute_all ? 0 : game_setting.sfx_volume
    );
#else
    SDL_SetWindowFullscreen(game_app.window, game_setting.fullscreen);
    Mix_Volume(MUSIC_CHANNEL, game_setting.music_volume);
    Mix_Volume(SFX_CHANNEL, game_setting.sfx_volume);
#endif

    /* setup game engine */
    scene_array[START_SCENE] = &start_scene;
    scene_array[SETTING_SCENE] = &setting_scene;
    scene_array[WORLD_SCENE] = &world_scene;
    InitTranslation();
    InitMapSystem();
    InitEntitySystem();
    InitSceneSystem();
    InitUISystem();
    TranslationSetLanguage("en_us");

    // game loop
    game_app.timer = frametimer_create(NULL);
    frametimer_lock_rate(game_app.timer, 60);
    while (!game_app.should_quit) {
        float dt = frametimer_update(game_app.timer);
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
#if !defined(__PSP__) && !defined(__vita__)
            case SDL_WINDOWEVENT:
                if (event.window.event == SDL_WINDOWEVENT_FOCUS_LOST) {
                    game_app.window_focused = 0;
                    if (game_setting.mute_when_unfocused) {
                        Mix_Volume(MUSIC_CHANNEL, 0);
                        Mix_Volume(SFX_CHANNEL, 0);
                    }
                } else if (event.window.event == SDL_WINDOWEVENT_FOCUS_GAINED) {
                    game_app.window_focused = 1;
                    if (game_setting.mute_when_unfocused) {
                        Mix_Volume(MUSIC_CHANNEL, game_setting.music_volume);
                        Mix_Volume(SFX_CHANNEL, game_setting.sfx_volume);
                    }
                } else if (event.window.event == SDL_WINDOWEVENT_RESIZED) {
                    ClearWidgets();
                }
                break;
#endif
            case SDL_CONTROLLERDEVICEADDED:
                if (!game_app.joystick.available) {
                    game_app.joystick.device =
                        SDL_GameControllerOpen(event.cdevice.which);
                    if (game_app.joystick.device != NULL) {
                        game_app.joystick.available = 1;
                        SDL_Joystick* joystick = SDL_GameControllerGetJoystick(
                            game_app.joystick.device
                        );
                        game_app.joystick.which =
                            SDL_JoystickInstanceID(joystick);
                        SDL_ShowCursor(SDL_DISABLE);
                    } else {
                        SDL_LogError(
                            SDL_LOG_CATEGORY_ERROR,
                            "SDL_GameControllerOpen(): %s", SDL_GetError()
                        );
                    }
                }
                break;
            case SDL_CONTROLLERDEVICEREMOVED:
                if (event.jdevice.which == game_app.joystick.which) {
                    game_app.joystick.available = 0;
                    SDL_GameControllerClose(game_app.joystick.device);
                    game_app.joystick.device = NULL;
                    SDL_ShowCursor(SDL_ENABLE);
                }
                break;
            case SDL_QUIT:
                game_app.should_quit = 1;
                break;
            }
            HandleWidgetEvent(&event);
            HandleSceneEvent(&event);
        }
        SDL_RenderClear(game_app.renderer);
        TickWidgets(dt);
        TickScene(dt);
        SDL_RenderPresent(game_app.renderer);
    }

    // cleanup
    frametimer_destroy(game_app.timer);
    if (game_app.joystick.device != NULL) {
        SDL_GameControllerClose(game_app.joystick.device);
    }
    SaveSetting();
    QuitTranslation();
    QuitMapSystem();
    QuitEntitySystem();
    QuitSceneSystem();
    QuitUISystem();
    FreeRespack(game_app.assets_pack);
    free(game_app.exec_path);
#if !defined(__PSP__) && !defined(__vita__)
    SDL_FreeSurface(icon_image);
#endif
    SDL_DestroyRenderer(game_app.renderer);
    SDL_DestroyWindow(game_app.window);
    IMG_Quit();
    Mix_CloseAudio();
    Mix_Quit();
#if defined(TH_FALLBACK_TO_BITMAP_FONT)
    TTF_Quit();
#endif
    SDL_Quit();
    return 0;
}

#if defined(__WIN32__)
// main function for Windows
int WinMain(
    HINSTANCE hInstance, HINSTANCE hPrevInstnce, LPSTR lpCmdLine, int nShowCmd
) {
    int argc;
    LPWSTR* argvw = CommandLineToArgvW(lpCmdLine, &argc);
    char** argv = (char**)calloc(argc, sizeof(char*));
    for (int i = 0; i < argc; ++i) {
        int len =
            WideCharToMultiByte(CP_UTF8, 0, argvw[i], -1, NULL, 0, NULL, NULL);
        argv[i] = (char*)calloc(len, sizeof(char));
        WideCharToMultiByte(CP_UTF8, 0, argvw[i], -1, argv[i], len, NULL, NULL);
    }
    int result = main(argc, argv);
    for (int i = 0; i < argc, ++i) {
        free(argv[i]);
    }
    free(argv);
    LocalFree(argvw);
    return result;
}
#endif
