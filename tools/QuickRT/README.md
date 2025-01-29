<div align="center">

![QuickRT][logo] 

[![Made with LuaRT](https://badgen.net/badge/Made%20with/LuaRT/yellow)](https://www.luart.org/)
![Windows](https://badgen.net/badge/Windows/Vista%20and%20later/blue?icon=windows)
[![QuickRT license](https://badgen.net/badge/License/MIT/green)](#)
[![Twitter Follow](https://img.shields.io/twitter/follow/__LuaRT__?style=social)](https://www.twitter.com/__LuaRT__)

QuickRT is a Lua console REPL to easily prototype, test, and learn Lua programming with LuaRT, the Windows programming framework.

[Features](#features) |
[Getting started](#getting-started) |
[Installation](#installation) 

![Demo][demo] 
</div>

## Features

- Lua 5.4.7 interactive REPL (Read-Print-Eval-Loop)
- Lua colored syntax highlighting as you type
- Multiline editing with indentation.
- Autocompletion.
- Pretty printing of results
- Command history.
- LuaRT documentation included.
- UTF8 characters support.

## Installation

### Running QuickRT from sources
[LuaRT](https://www.luart.org) must have been previously installed. Doubleclick on the quickrt.lua file in the Windows explorer to launch QuickRT.

Alternatively, open a LuaRT console and go to the directory where you've downloaded QuickRT source files.
Then type the following command :

```batch
luart quickrt.lua
```
### Running QuickRT from release package
No need to previously install LuaRT in this case.
Go to the folder where you have extracted the release package and doubleclick on **QuickRT.exe** in the Windows explorer.

## Getting started

To get you started with QuickRT, type the following commands :

```lua
help("topic")      -- search help for "topic"
clear()            -- clears the screen
a                  -- shows the value of variable 'a'
5+5                -- shows the result of 5+5
print("hello")     -- calls a function and shows its result
pr                 -- type "pr" then press the TAB key to use autocompletion (should find 'print')
sys.exit()         -- exits QuickRT
```

You can use last entered commands by pressing the &#8593; and &#8595; keys.
You can also quit QuickRT with CTRL+C.

[demo]: contrib/QuickRT.webp
[logo]: contrib/QuickRT.png
