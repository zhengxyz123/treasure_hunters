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

#include "global.h"
#include "resources/assets.h"
#include "resources/loader.h"
#include "scenes/setting_menu.h"
#include "scenes/start_menu.h"
#include "scenes/subscene.h"
#include "setting.h"
#include "ui/text.h"
#include <time.h>
#if defined(__WIN32__)
    #include <Windows.h>
#endif

#define CUTE_TILED_IMPLEMENTATION
#include "../extern/cute_tiled.h"

GameApp global_app = {
    .status = GAMESTATUS_NORMAL,
    .joystick = {.available = 0, .which = INT_MIN},
    .should_quit = 0,
    .window_focused = 1
};
Setting global_setting = {
    .fullscreen = 0,
    .music_volume = 64,
    .sfx_volume = 64,
    .mute_when_unfocused = 1
};

/* main function for Linux */
int main(int argc, char* argv[]) {
    srand((unsigned)time(NULL));

    /* initialise SDL, SDL_image and SDL_mixer */
    if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
        SDL_LogError(
            SDL_LOG_CATEGORY_ERROR, "SDL_Init(): %s\n", SDL_GetError()
        );
        return 1;
    }
    SDL_SetHint(SDL_HINT_APP_NAME, "Treasure Hunters");
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
    Mix_OpenAudio(48000, AUDIO_F32SYS, 2, 2048);

    /* create the window, renderer and set the window icon */
    global_app.window = SDL_CreateWindow(
        "Treasure Hunters", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640,
        480, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE
    );
    if (global_app.window == NULL) {
        SDL_LogError(
            SDL_LOG_CATEGORY_ERROR, "SDL_CreateWindow(): %s\n", SDL_GetError()
        );
        return 1;
    }
    SDL_Surface* icon_image =
        LoadSurface(icon_png_content, sizeof(icon_png_content));
    SDL_SetWindowIcon(global_app.window, icon_image);
    SDL_SetWindowMinimumSize(global_app.window, 640, 480);
    global_app.renderer = SDL_CreateRenderer(
        global_app.window, -1,
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
    );
    SDL_RenderSetVSync(global_app.renderer, 1);
    SDL_SetRenderDrawBlendMode(global_app.renderer, SDL_BLENDMODE_BLEND);

    /* setup the global_app and global_setting */
    global_app.argc = argc;
    global_app.argv = argv;
    global_app.exec_path = (char*)calloc(PATH_MAX, sizeof(char));
    char* base_path = SDL_GetBasePath();
    strcpy(global_app.exec_path, base_path);
    SDL_free(base_path);
    InitSetting();
    SDL_SetWindowFullscreen(global_app.window, global_setting.fullscreen);
    Mix_Volume(
        MUSIC_CHANNEL,
        global_setting.mute_when_unfocused ? 0 : global_setting.music_volume
    );
    Mix_Volume(
        SFX_CHANNEL,
        global_setting.mute_when_unfocused ? 0 : global_setting.sfx_volume
    );

    /* setup scenes */
    scene_array[START_SCENE] = &start_scene;
    scene_array[SETTING_SCENE] = &setting_scene;
    InitSceneManager();
    InitSubscene();
    InitWidgetSystem();
    InitTextSystem();

    /* game loop */
    unsigned long prev_tick = SDL_GetTicks64();
    while (!global_app.should_quit) {
        if (SDL_GetTicks64() - prev_tick < TICK) {
            SDL_Delay(TICK - (SDL_GetTicks64() - prev_tick));
        }
        prev_tick = SDL_GetTicks64();
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_WINDOWEVENT:
                    if (event.window.event == SDL_WINDOWEVENT_FOCUS_LOST) {
                        global_app.window_focused = 0;
                        if (global_setting.mute_when_unfocused) {
                            Mix_Volume(MUSIC_CHANNEL, 0);
                            Mix_Volume(SFX_CHANNEL, 0);
                        }
                    } else if (event.window.event ==
                               SDL_WINDOWEVENT_FOCUS_GAINED) {
                        global_app.window_focused = 1;
                        if (global_setting.mute_when_unfocused) {
                            Mix_Volume(
                                MUSIC_CHANNEL, global_setting.music_volume
                            );
                            Mix_Volume(SFX_CHANNEL, global_setting.sfx_volume);
                        }
                    } else if (event.window.event == SDL_WINDOWEVENT_RESIZED) {
                        ClearWidgets();
                    }
                    break;
                case SDL_CONTROLLERDEVICEADDED:
                    if (!global_app.joystick.available) {
                        global_app.joystick.device =
                            SDL_GameControllerOpen(event.cdevice.which);
                        if (global_app.joystick.device != NULL) {
                            global_app.joystick.available = 1;
                            SDL_Joystick* joystick =
                                SDL_GameControllerGetJoystick(
                                    global_app.joystick.device
                                );
                            global_app.joystick.which =
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
                    if (event.jdevice.which == global_app.joystick.which) {
                        global_app.joystick.available = 0;
                        SDL_GameControllerClose(global_app.joystick.device);
                        global_app.joystick.device = NULL;
                        SDL_ShowCursor(SDL_ENABLE);
                    }
                    break;
                case SDL_QUIT:
                    global_app.should_quit = 1;
                    break;
            }
            HandleWidgetEvent(&event);
            HandleSceneEvent(&event);
        }
        SDL_RenderClear(global_app.renderer);
        TickScene();
        SDL_RenderPresent(global_app.renderer);
    }

    /* cleanup */
    if (global_app.joystick.device != NULL) {
        SDL_GameControllerClose(global_app.joystick.device);
    }
    SaveSetting();
    QuitTextSystem();
    QuitWidgetSystem();
    FreeSubscene();
    FreeSceneManager();
    free(global_app.exec_path);
    SDL_FreeSurface(icon_image);
    SDL_DestroyRenderer(global_app.renderer);
    SDL_DestroyWindow(global_app.window);
    IMG_Quit();
    Mix_CloseAudio();
    Mix_Quit();
    SDL_Quit();
    return 0;
}

#if defined(__WIN32__)
/* main function for Windows */
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
