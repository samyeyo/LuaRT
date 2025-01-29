<div align="center">

[![Title][title]](https://studio.luart.org/)

[![Lua VM 5.4.5](https://badgen.net/badge/Lua%20VM/5.4.7/yellow)](https://www.lua.org/)
![Windows](https://badgen.net/badge/Windows/Vista%20and%20later/blue?icon=windows)
[![LuaRT Studio license](https://badgen.net/badge/License/MIT/green)](#license)
[![Twitter Follow](https://img.shields.io/twitter/follow/__LuaRT__?style=social)](https://www.twitter.com/__LuaRT__)
  
![Demo][demo] 
  
[Features](#features) |
[Installation](#installation) |
[Documentation](#documentation) |
[Usage](#usage) |
[Author](#author) |
[License](#license) |
[Links](#links)

</div>

## Features

LuaRT Studio is a Windows IDE to develop Lua desktop or console applications, based on the LuaRT interpreter. LuaRT Studio can also be used to develop standard Lua applications based on latest Lua 5.4.7 VM.

* Small and portable Lua 5.4.7 IDE for Windows for x86 and x64
* Based on ZeroBrane Studio, from Paul Kulchenko 
* Bundled with a specific LuaRT interpreter, compatible with standard Lua
* Automatic switch between Lua console or desktop application based on file extension (.lua and .wlua respectively)
* Updated UI, using current Windows UI theme, icons for files, tabs, and panels.
* Rework of the "Outline" tab, now called "Symbols" (displays local and global variables, new icons, table expansion...)
* Icons Stack panel and a new Symbol tab
* New Variable panel, that shows during debugging session
* Support for using ttf font files
* New Stylua 0.15.3 linter
* LuaCheck updated to 0.26
* LuaSec updated to 1.2.0
* LuaSocket updated to 3.0-rc1
* Updated mobdebug to support LuaRT objects and Tasks
* New project option to Show/Hide console window.
* Local Lua 5.4.7 console 

## Installation

The IDE can be **installed into and run from any directory**. There are three options to install it:

* Download a [current snapshot of the repository](https://github.com/samyeyo/LuaRT-Studio/). Select the branch you want [x86](https://github.com/samyeyo/LuaRT-Studio/tree/x86) or [x64](https://github.com/samyeyo/LuaRT-Studio/tree/x64)
* Download the [last release](https://github.com/samyeyo/LuaRT-Studio/releases/).
* Clone the repository to access the current development version.

**No compilation is needed** for any of the installation options, although the script to compile "LuaRT Studio.exe" executable is available in the build\ directory.

## Documentation

* No specific documentation available for now, as most of the original ZeroBrane Studio documentation should apply for LuaRT Studio.
* This video shows you how to start with the IDE : [LuaRT Studio - Installation and first steps]()

## Usage

The IDE can be launched by clicking `LuaRT Studio.exe` from the directory that the IDE is installed to. You can also create a shortcut to this executable.

The general command for launching is the following: `"LuaRT Studio.exe" [option] [<project directory>] [<filename>...]`.

* **Open files**: `"LuaRT Studio.exe" <filename> [<filename>...]`.
* **Set project directory** (and optionally open files): `"LuaRT Studio.exe" <project directory> [<filename>...]`.
* **Overwrite default configuration**: `"LuaRT Studio.exe" -cfg "string with configuration settings"`, for example: `zbstudio -cfg "editor.fontsize=12; editor.usetabs=true"`.
* **Load custom configuration file**: `"LuaRT Studio.exe" -cfg <filename>`, for example: `"LuaRT Studio.exe" -cfg cfg/xcode-keys.lua`.

All configuration changes applied from the command line are only effective for the current session.

If you are loading a file, you can also **set the cursor** on a specific line or at a specific position by using `filename:<line>` and `filename:p<pos>` syntax.

In all cases only one instance of the IDE will be allowed to launch by default:
if one instance is already running, the other one won't launch, but the directory and file parameters
passed to the second instance will trigger opening of that directory and file(s) in the already started instance.

## Author

### LuaRT studio

  **LuaRT:** Samir Tine (samir.tine@luart.org)

### ZeroBrane Studio and MobDebug

  **ZeroBrane LLC:** Paul Kulchenko (paul@zerobrane.com)
 
## License

See [LICENSE](LICENSE).

## Links

- [LuaRT Studio Homepage](https://studio.luart.org/)
- [LuaRT Homepage](http://www.luart.org/)
- [LuaRT Community](http://community.luart.org/)
- [LuaRT Documentation](http://www.luart.org/doc)

[demo]: https://studio.luart.org/ide.png
[title]: https://studio.luart.org/logo.png
