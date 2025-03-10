# Treasure Hunters

A game in the early stages of development.

## Build the Executable File

First clone the whole repo:

```bash
git clone git@github.com:zhengxyz123/treasure_hunters.git
cd treasure_hunters/
```

### Linux
The `SDL2` and `SDL2_image` package should be installed:

```bash
# Arch Linux
sudo pacman -S sdl2-compat sdl2_image
# Debian
sudo apt install libsdl2-dev libsdl2-image-dev
```

Then, run:

```bash
cmake -B build -D CMAKE_BUILD_TYPE="MinSizeRel"
cd build/
make
```

### Windows With Visual Studio
You should download and unzip 

1. `SDL2-devel-2.*.*-VC.zip` from [SDL Release](https://github.com/libsdl-org/SDL/releases)
2. `SDL2_image-devel-2.*.*-VC.zip` from [SDL_image Release](https://github.com/libsdl-org/SDL_image/releases)

Then, run:

```bash
cmake -B build/ -D CMAKE_BUILD_TYPE="MinSizeRel" -D CMAKE_PREFIX_PATH="/path/to/SDL2/;/path/to/SDL2_image"
```

It will generate a `treasure_hunters.sln` under `build/` directory.
