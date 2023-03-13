# Spock

this game engine uses (almost) entirely flecs for handling data and reading/writing data.

spock is a game engine being developed for a game called synther, it's specifically tailored for it, some code inside the engine may include code that you will not use.

***This engine is still undergoing many changes, and it will take while to develop a stable and non changing API. ENUMs, class names, struct names, the internal structure of the engine as a whole all could change so please keep that in mind when using***

Current feature list:
- basic sound, can play mp3 and wav files
- simple UI system, which currently includes button and 
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

## Dependencies
All the libraries that spock engine is dependent on are all built within
the project. But when compiling, spock engine's dependencies rely on python scripts.
Installing 'jinja2' may be necessary if not already installed.

if your lazy:
```console
pip install jinja2
```
