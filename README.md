<div align="center">

![LuaRT][title] 

[![Lua VM 5.4.4](https://badgen.net/badge/Lua%20VM/5.4/yellow)](https://www.lua.org/)
![Windows](https://badgen.net/badge/Windows/Vista%20and%20later/blue?icon=windows)
[![LuaRT license](https://badgen.net/badge/License/MIT/green)](#license)

Lua multipurpose programming framework to develop Windows applications

[Features](#features) |
[Installation](#installation) |
[Links](#links) |
[License](#license)

</div>

## Features

#### Lua for Windows, with batteries included
- Multipurpose programming framework with optimized Lua runtime library for Windows.
- Build Windows Desktop or console applications with Lua.
- Lightweight with no other dependencies
- Develop in Lua, C programming knowledge is not needed
- Batteries included : UTF8 strings, sockets, GUI, files, ZIP compression...

#### Complete development environment 
- rtc: a Lua script to executable compiler
- QuickRT: a powerful Lua REPL
- LuaRT Studio: a Lua/LuaRT IDE for Windows to develop and debug Lua desktop/console applications

## Installation

#### Release package

The easiest way to install LuaRT is to download the latest release package available on GitHub, and run the setup executable.

#### Building from sources

LuaRT has been successfully built with Mingw-w64 GCC compiler, feedback is welcome for other C compilers.
LuaRT should run on Windows Vista, Windows 7, Windows 8, Windows 10 and Windows 11.

Open a console prompt, go to the __\src__ directory and type "__make__". If everything went right, it will produce in the __\bin__ directory:

- __lua54.dll__ : the LuaRT shared library, ABI compatible with the standard lua54.dll
- __luart.exe__ : the LuaRT console interpreter
- __wluart.exe__: the desktop LuaRT interpreter
- __luart-static.exe__: the LuaRT console interpreter, without __lua54.dll__ dependency
- __wluart-static.exe__: the desktop LuaRT interpreter, without __lua54.dll__ dependency
- __rtc.exe__ : the Lua script to executable compiler
- __wrtc.exe__ : the GUI front-end for rtc

Don't forget to add the __\bin\\__ directory to the system PATH.

> By default, the Makefile is configured to build LuaRT for x86 (32 bits). 
> To switch to x64 (64 bits), set the PLATFORM value to x64 at the start of the Makefile

#### First run

Open a console prompt, go to the __\example__ directory and type :

```lua
wluart hello.wlua
```

Then a Window message "Hello World !" should pop up.

## Links
  
- [LuaRT Homepage](http://www.luart.org/index.html)
- [LuaRT Community](http://community.luart.org)
- [LuaRT Documentation](http://www.luart.org/doc/index.html)

## License
  
LuaRT is copyright (c) 2022 Samir Tine.
LuaRT is open source, released under the MIT License.
See full copyright notice in the LICENSE.txt file.

[title]: examples/LuaRT.png
