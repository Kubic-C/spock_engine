# Spock

spock is a game engine being developed for a game called synther, it's specifically tailored for it, you may find some features ... unconventional.

Current feature list:
- basic sound, can play mp3 and wav files
- simple UI system, which currently includes button and text, textures can be used in place of the background.
- simple physics world (in heavy development)
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

You will not be able to use MSVC due to their incredible ignorance
of keeping only one standard of C: C89.

## Dependencies
All the libraries that spock engine is dependent on are all built within
the project. But when compiling, spock engine's dependencies rely on python scripts.
Installing 'jinja2' may be necessary if not already installed.

if your lazy:
```console
pip install jinja2
```
