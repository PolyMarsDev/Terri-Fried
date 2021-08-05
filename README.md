# Terri-Fried

Terri-Fried is a multi-platform C++ game made in 72 hours for Ludum Dare 46. It is currently available on PC, Web, PS Vita, PSP, Wii, GameCube, and Nintendo DS. Builds for each platform can be downloaded [here](https://polymars.itch.io/terri-fried).

## Screenshots
![](https://img.itch.zone/aW1hZ2UvNjIwMzc4LzMyOTcwNzkucG5n/347x500/tOVUPR.png) ![](https://img.itch.zone/aW1hZ2UvNjIwMzc4LzMyOTcwODAucG5n/347x500/7WajOY.png)![](https://img.itch.zone/aW1hZ2UvNjIwMzc4LzMyOTc3NTMucG5n/347x500/OQvCg8.png) ![](https://img.itch.zone/aW1hZ2UvNjIwMzc4LzMyOTcwODMucG5n/347x500/sQhgXc.png)


## Background
After participating in Ludum Dare 46, I challenged myself to port my entry to as many consoles as I could using various homebrew libraries. The game is a simple platformer where you launch an egg between platforms to avoid falling in lava. The code was written in 72 hours so it isn't perfect, but I decided to provide the source regardless to hopefully help anyone interested in making games for these platforms. Feel free to ask any questions if you need anything to be clarified!


## Compiling
### Windows
#### raylib 
After installing [raylib](https://github.com/raysan5/raylib/releases), execute ``make`` in the ``/raylib`` directory.
#### SDL2 
Tested with msys2. Install the `SDL2`, `SDL2_image`, `SDL2_mixer` and `SDL2_ttf` packages with `pacman`, then execute `make` in the windows/sdl2 folder. Finally, copy the resources folder to the output executable location.
### PS Vita
Compiling for PS Vita requires vitasdk. Then, execute ``cmake .&& make`` in the ``/psvita`` directory.
### Nintendo DS
NightFox's Lib does not need to be downloaded as it is already integrated into this repository, but you'll need to install the libnds libraries with [devkitPro](https://devkitpro.org/wiki/Getting_Started). Then, run the ``compile.bat`` script in ``/nds``.
### Wii
Compiling for Wii requires the libogc libraries that can be installed with [devkitPro](https://devkitpro.org/wiki/Getting_Started). You will also need [GRRLIB](https://github.com/GRRLIB/GRRLIB). Then, execute ``make`` in the ``/wii`` directory.
### GameCube
Compiling for GameCube requires the same libogc libraries from [devkitPro](https://devkitpro.org/wiki/Getting_Started) but a different fork of GRRLIB that can be found [here](https://github.com/capz/GRRLIB). Then, execute ``make`` in the ``/gamecube`` directory.
### Original Xbox
Clone [this fork of nxdk](https://github.com/dracc/nxdk/tree/xgu) directly next to the Terri-Fried folder, then [install the nxdk prerequisites](https://github.com/XboxDev/nxdk/wiki/Getting-Started). Follow the instructions [here](https://github.com/Voxel9/xbox-xgu-examples#quick-guide) to fix the XGU headers in nxdk. This is a temporary fix for some duplicate symbol linker errors.  
Now `cd` to the `Terri-fried/xbox` folder and run `make`. An XBE file should be built in the `bin` folder. Copy the contents of the `bin` folder to your Xbox and run `default.xbe`.
### Nintendo Switch
Requires devkitpro, devkitARM, libNX and the following packages to be installed via dkp-pacman:  
`switch-pkg-config`, `switch-SDL2`, `switch-SDL2_mixer`, `switch-SDL2_image` `switch-SDL2_ttf`  
Once installed, `cd` to `Terri-Fried/switch` and run `make`. Copy the resulting NRO file over to your Switch's microSD card in the `switch` folder and launch via nx-hbmenu.
### Wii U
Requires devkitpro, devkitPPC, [wut](https://github.com/devkitPro/wut) and the following packages to be installed with dkp-pacman:  
`wiiu-pkg-config`, `wiiu-sdl2`, `wiiu-sdl2_mixer`, `wiiu-sdl2_image` `wiiu-sdl2_ttf`  
Once installed `cd` to `Terri-Fried/wiiu` and run `make`. Create a folder on your SD card with any name in the directory `/wiiu/apps` and copy the file(s) `Terri-Fried.rpx`, (`icon.png` and `meta.xml` *optional*) into that folder. You will also need to create a folder on the root of your SD card named `Terri-Fried`.
## Contributing
Feel free to create a fork of this repository if you want to port the game to additional platforms! The code can also be used for any other noncommercial purposes.
