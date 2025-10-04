# Treasure Hunters

A 2D platform game.

## Build the Executable File

Python3 and [Tiled](https://www.mapeditor.org/) should be installed to generate the resource pack.

First clone the whole repo:

```bash
git clone git@github.com:zhengxyz123/treasure_hunters.git
cd treasure_hunters/
```

### Linux

The `SDL2`, `SDL2_image`, `SDL2_mixer` and `SDL2_ttf` package should be installed:

```bash
# Arch Linux
sudo pacman -S sdl2-compat sdl2_image sdl2_mixer sdl2_ttf
# Debian
sudo apt install libsdl2-dev libsdl2-image-dev libsdl2-mixer-dev libsdl2-ttf-dev
```

Build using `cmake`:

```bash
cmake -B build -D CMAKE_BUILD_TYPE="Release"
cd build/
make
```

### PSP

You should [install PSPDEV](https://pspdev.github.io/installation.html) first.

Build using `cmake`:

```bash
psp-cmake -B build -D CMAKE_BUILD_TYPE="Release"
cd build/
make
```

## PS Vita

**Vita support is experimental!**

You should install [Vita SDK](https://github.com/vitasdk/vdpm) first.

Build using `cmake` with `-D BUILD_VITA=1`:

```
cmake -B build -D CMAKE_BUILD_TYPE="Release" -D BUILD_VITA=1
cd build/
make
```

### Windows With Visual Studio

You should download and unzip

1. `SDL2-devel-2.*.*-VC.zip` from [SDL release](https://github.com/libsdl-org/SDL/releases)
2. `SDL2_image-devel-2.*.*-VC.zip` from [SDL_image release](https://github.com/libsdl-org/SDL_image/releases)
3. `SDL2_mixer-devel-2.*.*-VC.zip` from [SDL_mixer release](https://github.com/libsdl-org/SDL_mixer/releases)
4. `SDL2_ttf-devel-2.*.*-VC.zip ` from [SDL_ttf release](https://github.com/libsdl-org/SDL_ttf/releases)

Build using `cmake` with `-D CMAKE_PREFIX_PATH=...`:

```bash
cmake -B build/
      -D CMAKE_BUILD_TYPE="Release"
      -D CMAKE_PREFIX_PATH="/path/to/SDL2/cmake/;/path/to/SDL2_image/cmake;/path/to/SDL2_mixer/cmake;/path/to/SDL2_ttf/cmake"
```

It will generate a `treasure_hunters.sln` under `build/` directory.
