# Terri-Fried

Terri-Fried is a multi-platform C++ game made in 72 hours for Ludum Dare 46. It is currently available on PC, Web, PS Vita, PSP, Wii, GameCube, and Nintendo DS. Builds for each platform can be downloaded [here](https://polymars.itch.io/terri-fried).

## Screenshots
![](https://img.itch.zone/aW1hZ2UvNjIwMzc4LzMyOTcwNzkucG5n/347x500/tOVUPR.png) ![](https://img.itch.zone/aW1hZ2UvNjIwMzc4LzMyOTcwODAucG5n/347x500/7WajOY.png)![](https://img.itch.zone/aW1hZ2UvNjIwMzc4LzMyOTc3NTMucG5n/347x500/OQvCg8.png) ![](https://img.itch.zone/aW1hZ2UvNjIwMzc4LzMyOTcwODMucG5n/347x500/sQhgXc.png)


## Background
After participating in Ludum Dare 46, I challenged myself to port my entry, a simple platformer where you launch an egg between platforms to avoid falling in lava, to as many consoles as I could using various homebrew libraries. The original code was written in 72 hours so it isn't perfect, but I decided to provide the source regardless to hopefully help anyone interested in making games for any of these platforms. Feel free to ask any questions if you need anything to be clarified!


## Compiling
### raylib (Windows)
After installing [raylib](https://github.com/raysan5/raylib/releases), open the command prompt in the ``/raylib`` directory and type ``make``.
### PS Vita
Compiling for PS Vita requires vitasdk. Then, open the command prompt in the ``/psvita`` directory and type ``cmake .&& make``.
### Nintendo DS
NightFox's Lib does not need to be downloaded as it is already integrated into this repository, but you'll need to install the libnds libraries with [devkitPro](https://devkitpro.org/wiki/Getting_Started). Then, run the ``compile.bat`` script in ``/nds``.
### Wii
Compiling for Wii requires the libogc libraries that can be installed with [devkitPro](https://devkitpro.org/wiki/Getting_Started). You will also need [GRRLIB](https://github.com/GRRLIB/GRRLIB). Then, open the command prompt in the ``/wii`` directory and type ``make``.
### GameCube
Compiling for GameCube requires the same libogc libraries from [devkitPro](https://devkitpro.org/wiki/Getting_Started), but it requires a different fork of GRRLIB that can be found [here](https://github.com/capz/GRRLIB). Then, open the command prompt in the ``/gamecube`` directory and type ``make``.
## Contributing
Feel free to create a fork of this repository if you want to port the game to additional platforms! The code can also be used for any other noncommercial purposes.
