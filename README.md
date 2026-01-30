This project is my implementation of a text-based adventure in SDL3.
Project direction/implementation may change drastically in future.
Currently only has some minimally functioning menus, minimal support for mining skill.

Pre-requisites:
C++ Compiler installed compatible with at least version C++20
SDL3 Library installed
SDL3_ttf Library installed (Library extention to SDL3 required for rendering text)
SDL3_image Library installed (Library extention to SDL3 required for rendering textures from images)

Compile main.cpp using
g++ -std=c++20 main.cpp -o main.exe -L<path to libraries> -lSDL3 -lSDL3_image -lSDL3_ttf -mwindows
For example, in my case, all my libraries were installed msys64 ucrt64:
g++ -std=c++20 main.cpp -o main.exe -LC:/msys64/ucrt64/lib/ -lSDL3 -lSDL3_image -lSDL3_ttf -mwindows

-mwindows is to suppress cmd terminal opening alongside game screen.