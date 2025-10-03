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

#include <SDL.h>

#if defined(__LINUX__)
    #define PATH_SEP '/'
    #include <linux/limits.h>
#elif defined(__WIN32__)
    #define PATH_SEP '\\'
    #include <windef.h>
    #define PATH_MAX MAX_PATH
#endif

#ifndef PATH_MAX
    #define PATH_MAX 4096
#endif

#include "frametimer.h"
#include "resources/respack.h"

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
    int should_quit;
    float interface_size;
    frametimer_t* timer;
    GameStatus status;
    struct {
        SDL_GameController* device;
        int available;
        int which;
    } joystick;
    SDL_Window* window;
    int window_focused;
    SDL_Renderer* renderer;
    Respack* assets_pack;
} GameApp;

#endif
