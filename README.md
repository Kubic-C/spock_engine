# Spock

this game engine uses (almost) entirely flecs for handling data and reading/writing data.

spock is a game engine being developed for a game called synther

Current feature list:
- simple UI system, which currently includes button and text
- simple physics world, using box2d
- basic window handling and manipulation

Undergoing heavy construction

## Compiling
use CMake to generate the projects files for your OS.
Use GCC or CLANG, or a MinGW build if your on windows

note: if your going to use Clang, you will have problems
using valgrind as for whatever reason valgrind has problems with 
the debugging symbols that Clang generates.
