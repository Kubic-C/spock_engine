# Spock

this game engine uses (almost) entirely flecs for handling data and reading/writing data.

spock is a game engine being developed for a game called synther

Current feature list:
- simple UI system, which currently includes button and text
- simple physics world, using box2d
- basic window handling and manipulation
- a particle system using sprites
- tilebodies and tilemaps
- sprites
- resource manager, allowing simple loading of files for sprites, textures, or other related files

Undergoing heavy construction

## Compiling
use CMake to generate the projects files for your OS.
Use GCC or CLANG, or a MinGW build if your on windows

note: if your going to use Clang, you will have problems
using valgrind as for whatever reason valgrind has problems with 
the debugging symbols that Clang generates.

## Dependencies
All the libraries that spock engine is dependent on are all built within
the project. But when compiling, spock engine's dependencies rely on python scripts.
Installing 'jinja2' may be necessary if not already installed.

if your lazy:
```console
pip install jinja2
```
