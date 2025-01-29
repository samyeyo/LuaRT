<div align="center">

![rtc][title] 

[![Made with LuaRT](https://badgen.net/badge/Made%20with/LuaRT/yellow)](https://www.luart.org/)
![Windows Vista+](https://badgen.net/badge/Windows/Vista%20and%20later/blue?icon=windows)
[![MIT License](https://badgen.net/badge/License/MIT/green)](#)

Build standalone Windows executables from your Lua scripts.

[Features](#features) |
[Installation](#installation) |
[Usage](#usage) |
[Documentation](https://www.luart.org/doc/toolchain/rtc.html) |
[Links](#links) |
[License](#license)
</div>

## Features

- Standalone tool : no Makefile, no C compiler needed
- Compile from command line or using a GUI frontend
- Build Windows native executable (.exe) from your Lua 5.4.7 scripts
- Windows desktop or console applications
- Static executables (without `lua54.dll` dependency)
- Dynamic executables (with `lua54.dll` dependency)
- Embed any files with your executable, even Lua binary modules with seamless loading using `require()`
- Loads DLL dependencies in embedded files transparently
- Access embedded files seamlessly from your Lua scripts
- Deploy your applications easily without the need to install Lua

## Installation

#### Using latest release package

The easiest way to install is to download the latest binary release from the Github repository.
Just add the directory where you have unpacked the release package to your Windows system PATH variable.

If you use the release package, you don't need any other dependencies to compile Lua scripts.

#### Build rtc
  
To build **rtc**, you will need to install the LuaRT programming framework sources before to proceed.
Go to the ```src\``` directory in the LuaRT folder and type "**make rtc**" in a command prompt.
It should produce a "**rtc.exe**" and "**wrtc.exe**"executable. 

## Usage

#### rtc command line options
  
```
usage:	rtc.exe [-s][-c][-w][-i icon][-o output] [-lmodname] [directory] main.lua
	-s		create static executable (without LUA54.DLL dependency)
	-c		create executable for console (default)
	-w		create executable for Windows desktop
	-i icon		set executable icon (expects an .ico file)
	-o output	set executable name to 'output'
	-lmodname	link the LuaRT binary module 'modname.dll'
	directory	the content of the directory to be embedded in the executable
	main.lua   	the Lua script to be executed
```
  
The specified optional directory will then be embedded within the executable with all its content archived in the ZIP format, bundled with the generated executable.
As an alternative, you can use **wrtc.exe**, the GUI frontend which is more usert friendly.

> **Warning**
> Compiled dynamic executable depends on the provided `lua54.dll` library. Do not use any other library or your application will throw an error or may crash.

#### Accessing embedded files from your LuaRT application
  
To access embedded files from your LuaRT application, just use the global "**embed**" module. It will return a Zip instance, already open for read access, that contains the directory content provided on the command line during compilation with rtc :

```lua
-- extract all the embedded content
embed:extractall("c:/installdir/")
```

If no embedded content exists, "**embed**" will be set to a **nil** value. You should check that the current script is compiled embed table before using it.
See the [LuaRT Zip object documentation](https://www.luart.org/doc/compression/Zip.html) for more information.
  
#### Requiring Lua modules from embedded files

To require a LuaRT script file in the embedded files, use **require** with the name of the module. You can require Lua modules, and binary modules (DLL) without the need to extract first (please note that in-memory binary modules loading works only with dynamic executables and may not work for some modules).

```lua
-- require for the english.lua module, that must have been previously embedded with rtc 
local english = require "english"
print(english.hello)
```
  
## Links
  
- [rtc Documentation](https://www.luart.org/doc/toolchain/rtc.html)
- [rtc Tutorial](https://www.luart.org/doc/tutorial/rtc.html)
- [LuaRT Homepage](https://www.luart.org/)
- [LuaRT Community](https://community.luart.org/)

## License
  
LuaRT and rtc are copyright (c) 2025 Samir Tine.
rtc is open source, released under the MIT License.
See full copyright notice in the LICENSE file.

[title]: rtc.png
