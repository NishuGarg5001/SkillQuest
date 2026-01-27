This project is my implementation of a text-based adventure in SDL3.
Project direction/implementation may change drastically in future.
Currently only has some minimally functioning menus, minimal support for mining skill.

Pre-requisites:
C++ Compiler installed compatible with at least version C++20
SDL3 Library installed
SDL3_ttf Library installed (Library extention to SDL3 required for rendering text)

Compile main.cpp using
g++ -std=c++20 main.cpp -o main.exe -LC:<path to libraries SDL3 and SDL3_ttf> -lSDL3 -lSDL3_ttf -mwindows

-mwindows is to suppress cmd terminal opening alongside game screen.