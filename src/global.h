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

#ifndef _TH_GLOBAL_H_
#define _TH_GLOBAL_H_

#if defined(WIN32) || defined(_WIN32) || defined(__MINGW32__)
    #include <SDL.h>
    #include <SDL_image.h>
    #include <SDL_mixer.h>
#else
    #include <SDL2/SDL.h>
    #include <SDL2/SDL_image.h>
    #include <SDL2/SDL_mixer.h>
#endif

#if defined(__LINUX__)
    #define PATH_SEP '/'
    #include <linux/limits.h>
#elif defined(__WIN32__)
    #define PATH_SEP '\\'
    #include <windef.h>
    #define PATH_MAX MAX_PATH
#endif

#define TICK 17 /* about 60 FPS */

#define MUSIC_CHANNEL 0
#define SFX_CHANNEL 1

typedef enum {
    GAMESTATUS_NORMAL,
    GAMESTATUS_KEY_BINDING,
} GameStatus;

typedef struct {
    int argc;
    char** argv;
    char* exec_path;
    GameStatus status;
    struct {
        SDL_GameController* device;
        int available;
        int which;
    } joystick;
    int should_quit;
    SDL_Window* window;
    SDL_Renderer* renderer;
} GameApp;

#endif
