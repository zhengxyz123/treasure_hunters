# Treasure Hunters

A game in the early stages of development.

## Build the Executable File

First clone the whole repo:

```bash
git clone git@github.com:zhengxyz123/treasure_hunters.git
cd treasure_hunters/
```

### Linux
The `SDL2`, `SDL2_image` and `SDL2_mixer` package should be installed:

```bash
# Arch Linux
sudo pacman -S sdl2-compat sdl2_image sdl2_mixer
# Debian
sudo apt install libsdl2-dev libsdl2-image-dev libsdl2-mixer-dev
```

Then, run:

```bash
cmake -B build -D CMAKE_BUILD_TYPE="MinSizeRel" -D CMAKE_EXPORT_COMPILE_COMMANDS=1
cd build/
make
```

### Windows With Visual Studio
You should download and unzip

1. `SDL2-devel-2.*.*-VC.zip` from [SDL release](https://github.com/libsdl-org/SDL/releases)
2. `SDL2_image-devel-2.*.*-VC.zip` from [SDL_image release](https://github.com/libsdl-org/SDL_image/releases)
3. `SDL2_mixer-devel-2.*.*-VC.zip` from [SDL_mixer release](https://github.com/libsdl-org/SDL_mixer/releases)

Then, run:

```bash
cmake -B build/
      -D CMAKE_BUILD_TYPE="MinSizeRel"
      -D CMAKE_EXPORT_COMPILE_COMMANDS=1
      -D CMAKE_PREFIX_PATH="/path/to/SDL2/cmake/;/path/to/SDL2_image/cmake;/path/to/SDL2_mixer/cmake"
```

It will generate a `treasure_hunters.sln` under `build/` directory.
