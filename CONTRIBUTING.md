# Developer Contributing Guide

This guide is for developers who want to contribute to LuaRT.
No matter how experienced or inexperienced you are, you can contribute to improving LuaRT by contributing to the code base, the documentation, reporting bugs, or even creating examples.

## Development Environment

To develop LuaRT, including **rtc**, **QuickRT** and the LuaRT toolchain for LuaRT Studio, you need to install the following tools:

- [MinGW-w64 for Windows](https://sourceforge.net/projects/mingw-w64/files/) : version 8.1.0 is the one used to produce LuaRT releases)
- [Git for Windows](https://git-scm.com/download/win)
- A text editor, or any IDE supporting the GCC compiler, or just a terminal to run the `mingw32-make` command

## Get the source code

#### Create a fork of the LuaRT repository
First create a fork of the LuaRT repository (either with your IDE that supports GitHub or with the web interface of the GitHub site).

#### Clone the repository
Run the following command to clone the repository (or use your IDE if it has builtin git support) :

```
git clone --recurse-submodules https://github.com/samyeyo/LuaRT.git
```

Enter the `src\` directory:

```
cd LuaRT\src
```

## Build LuaRT

Simply run the following command:

```
mingw32-make
```

This will build :

- Both `bin\luart.exe` and `bin\wluart.exe` interpreters
- `lua54.dll` and `lib\liblua54.a` libraries
- `bin\rtc.exe` and "bin\wrtc.exe" compilers


## Make a pull request

If you want to submit changes (new functionnality, bug fix...). Create a pull request from your forked repository.
It will then be submitted for review before merging to the main codebase.

## LuaRT folder structure

- `bin/` - The compiled binary files.
- `examples/` - The samples
- `include/` - The include files to use LuaRT C API
- `lib/` - The library `liblua54.a` to be linked with third party binary modules
- `modules/` - The third party Lua/LuaRT binary modules 
- `setup/` - The install Lua scripts and resources
- `src/` - The source code for the lua54.dll library and LuaRT interpreters.
- `tools/` - The LuaRT framework tools (rtc, QuickRT, and LuaRT Studio)
