<div align="center">

![LuaRT][title] 

[![Codacy Badge](https://api.codacy.com/project/badge/Grade/03c5cb71ac944498a3e4da9026255480)](https://app.codacy.com/gh/samyeyo/LuaRT?utm_source=github.com&utm_medium=referral&utm_content=samyeyo/LuaRT&utm_campaign=Badge_Grade_Settings)
[![Lua VM 5.4.4](https://badgen.net/badge/Lua%20VM/5.4/yellow)](https://www.lua.org/)
![Windows](https://badgen.net/badge/Windows/Vista%20and%20later/blue?icon=windows)
[![LuaRT license](https://badgen.net/badge/License/MIT/green)](#license)

Lua multipurpose programming framework to develop Windows applications

[Features](#small_blue_diamondfeatures) |
[Installation](#small_blue_diamondinstallation) |
[Links](#small_blue_diamondlinks) |
[License](#small_blue_diamondlicense)

</div>
   
## :small_blue_diamond:Features

#### Lua for Windows, with batteries included
- Multipurpose programming framework with optimized Lua runtime library for Windows.
- Build Windows desktop or console applications with Lua.
- Lightweight with no other dependencies
- Develop in Lua, C programming knowledge is not needed
- Batteries included : UTF8 strings, sockets, GUI, files, ZIP compression...

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
LuaRT should run on Windows Vista, Windows 7, Windows 8, Windows 10 and Windows 11.

First clone or download latest LuaRT GitHub repository to the directory of your choice.
Go to the ```\src``` directory and type ```make```:

```
cd LuaRT\src
make
```
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

> By default, LuaRT is configured to build LuaRT for x86 (32 bits). To switch to 64 bits, set the PLATFORM value to x64 in the Makefile

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
  
- :house_with_garden:[LuaRT Homepage](http://www.luart.org/index.html)
- :speech_balloon:[LuaRT Community](http://community.luart.org)
- :book:[LuaRT Documentation](http://www.luart.org/doc/index.html)
  
## :small_blue_diamond:License
  
LuaRT is copyright (c) 2022 Samir Tine.
LuaRT is open source, released under the MIT License.
See full copyright notice in the LICENSE.txt file.

[title]: examples/LuaRT.png
