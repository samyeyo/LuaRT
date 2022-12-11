<div align="center">

![LuaRT][title] 

[![Lua VM 5.4.4](https://badgen.net/badge/Lua%20VM/5.4/yellow)](https://www.lua.org/)
![Windows](https://badgen.net/badge/Windows/Vista%20and%20later/blue?icon=windows)
[![LuaRT license](https://badgen.net/badge/License/MIT/green)](#license)
[![Codacy Badge](https://app.codacy.com/project/badge/Grade/af54881b3d764f5ea210a5419fb96086)](https://www.codacy.com/gh/samyeyo/LuaRT/dashboard?utm_source=github.com&amp;utm_medium=referral&amp;utm_content=samyeyo/LuaRT&amp;utm_campaign=Badge_Grade)  
[![Twitter Follow](https://img.shields.io/twitter/follow/__LuaRT__?style=social)](https://www.twitter.com/__LuaRT__)

Lua multipurpose programming framework to develop Windows applications

![Banner][banner] 

[Features](#small_blue_diamondfeatures) |
[Installation](#small_blue_diamondinstallation) |
[Documentation](https://www.luart.org/doc/index.html) |
[Links](#small_blue_diamondlinks) |
[License](#small_blue_diamondlicense)

</div>
   
## :small_blue_diamond:Features

#### Lua for Windows, with batteries included
- Multipurpose programming framework with optimized Lua runtime library for x86 and x64 Windows.
- Build Windows desktop or console applications with Lua.
- Lightweight with no other dependencies
- Develop in Lua, C programming knowledge is not needed
- Batteries included : UTF8 strings, sockets, GUI, files, ZIP, compression...

#### Complete development environment 
- rtc: a Lua script to executable compiler
- QuickRT: a powerful Lua REPL
- LuaRT Studio: a Lua/LuaRT IDE for Windows to develop and debug Lua desktop/console applications
  
## :small_blue_diamond:Installation

#### Method 1 : Release package :package:

The preferred way to install LuaRT is to download the latest release package available on GitHub, and run the setup executable.
It will install the LuaRT binaries, create the Windows Start menu shortcuts for the IDE and REPL, and update the PATH system variable. 
It's the easiest and fastest way to start developing with LuaRT.
  
#### Method 2 : Building from sources :gear:

All you need to build LuaRT from sources is a valid installation of the Mingw-w64 GCC compiler, feedback is welcome for other C compilers.
The Mingw-w64 GCC 8.1.0 is used to produce the LuaRT release. Compilation have been successfully reported for Mingw-w64 GCC 11.2.0 and Mingw-w64 GCC 12.2.0 (ie the latest Mingw-w64 10.0.0 runtime).
LuaRT should run on Windows Vista, Windows 7, Windows 8, Windows 10 and Windows 11.

First clone the LuaRT repository (or manualy download the repository but don't forget submodules in tools folder) :
```
git clone --recurse-submodules https://github.com/samyeyo/LuaRT.git
```

Then go to the ```\src``` directory and type one of the following commands : ```make```, or ``make -j4`` to speed up the compilation on multicore CPU:

- `make` : Build LuaRT library and executable using the default x64 platform
- `make -j4` : Speed up building with multithreaded compilation
- `make PLATFORM=x86`: Build LuaRT library and executable using the x86 platform
- `make debug`: Build debug versions of LuaRT library and executable using the default x64 platform
- `make PLATFORM=x86 debug`: Build debug versions of LuaRT library and executable using the default x86 platform
- `make clean` : Clean all the generated binaries

If everything went right, it will produce in the ```\bin``` directory:
- ```lua54.dll``` : the LuaRT shared library, ABI compatible with the standard lua54.dll
- ```luart.exe``` : the LuaRT console interpreter
- ```wluart.exe```: the desktop LuaRT interpreter
- ```luart-static.exe```: the LuaRT console interpreter, without ```lua54.dll``` dependency
- ```wluart-static.exe```: the desktop LuaRT interpreter, without ```lua54.dll``` dependency
- ```rtc.exe``` : the Lua script to executable compiler
- ```wrtc.exe``` : the GUI front-end for rtc

Don't forget to add the ```\bin\``` directory to the system PATH (set it accordingly to your LuaRT path):

```
SET PATH=%PATH%;"C:\LuaRT\bin"
```

> **Note**

> By default, LuaRT is configured to build LuaRT for x64 (64 bits). To switch to 32 bits, set the PLATFORM value to x86 in the Makefile

## :small_blue_diamond:Usage

```
luart.exe [-e "statement"] | filename [arg1 arg2...]
wluart.exe [-e "statement"] | filename [arg1 arg2...]

-e "statement"
Executes the Lua statement in double quotes and exits.

filename [arg1 arg2...]
Loads and executes the Lua script in "filename", with optional arguments (each will be available in the global table arg in Lua).
```
For more details, check the LuaRT documentation.
  
## :small_blue_diamond:Links
  
- :house_with_garden: [LuaRT Homepage](http://www.luart.org/index.html)
- :speech_balloon: [LuaRT Community](http://community.luart.org)
- :book: [LuaRT Documentation](http://www.luart.org/doc/index.html)
  
## :small_blue_diamond:License
  
LuaRT is copyright (c) 2022 Samir Tine.
LuaRT is open source, released under the MIT License.
See full copyright notice in the LICENSE.txt file.

[title]: examples/LuaRT.png
[banner]: https://luart.org/img/features.png
