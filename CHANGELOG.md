## LuaRT v1.7.0 (Jan 05 2024)

#### Highlights 
- This release introduces RTBuilder, a new GUI RAD tool to design LuaRT desktop interfaces using `ui` module
- Lots of bugs have been fixed, especially for the `ui` module, the `rtc` compiler and the LuaRT Studio IDE

#### LuaRT toolchain
- Updated: MSVC builds now use better optimizations for faster execution
- Fixed: `rtcheck` won't propose to update for an inferior version now (Fixes #167)

#### rtc
- New: `rtc` now accepts mutiple files (the first one will be considered the main script)
- Updated: `-l` option now search for modules in the current directory too
- Fixed: Errors thrown by any compiled executables now mention source file and error line (Fixes #5)

#### LuaRT Studio
- Updated: LuaRT Studio is now using the LuaRT 1.7.0 toolchain
- Fixed: Debugger will now pause once a breakpoint is hit inside an `ui` event function, when using a `Task` to update the GUI
- Fixed: Output panel bug with Unicode input/output fixed
- Fixed: require of modules using `init.lua` from compiled executable is now working as expected
- Fixed: Setting project arguments won't add a blank space to the first argument anymore

#### Examples
- New: `balls_demo.wlua` example for `canvas` module

#### `sys` module
- Fixed: `Task:cancel()` won't try to close an already closed Task
- Fixed: `sys.cmd()` now returns `true` in LuaRT Studio as expected (Fixes #155)
- Fixed: `File:write()` method now flush data to disk when the file is a standard stream (Fixes #165)

#### `ui` module
- New: `Window` constructor now takes a first optional argument to provide a parent Window
- New: `Window:restore()` method
- New: `Window.topmost` property
- New: `Window.onMaximize()`, `Window.onMinimize()` and `Window.onRestore()` events
- New: `Tree.items` property can now be indexed by integer value, returning the `TreeItem` in numerical order from the first `TreeItem` to the last one
- New: `Tree.items` property is now iterable with the `each()` iterator
- New: `TreeItem.index` property to get the numerical order of the `TreeItem` in the `Tree`
- Updated: Setting `Entry.text` property now moves the caret at the start Updated: Setting the `Calendar.date` property from a `string` is now possible
- Fixed: `Treeitem: Loadicon()` won't fail to load a new icon after having removed the previous one (Fixes #160)
- Fixed: Providing a filename without extension in `ui.savedialog()` now returns correct filename when using multiple filters (Fixes #166)
- Fixed: `Window` keyboard shortcuts now execute binded Lua functions as expected (Fixes #162)
- Fixed: After `Progressbar` construction, `Progressbar.themed` returns `true` now as expected (Fixes #154)
- Fixed: `Canvas.fontstyle` now returns a string value as expected (Fixes #164)
- Fixed: Third party widgets now use initial size/position when the `align` property is set to `nil` (Fixes #158)
- Fixed: `Widget:center()` method now work as expected inside a `Groupbox` (Fixes #163 and #153)
- Fixed: `Window.height` returns now the correct value and a `Window.menu` property is set (Fixes #161)
- Fixed: Setting `Edit.font` property now preserve the current foreground color (Fixes #159)

## LuaRT v1.6.0 (Nov 26 2023)

#### Highlights 
- This release focuses on fixing many bugs in the `ui` module, to prepare for the launch of the LuaRT GUI builder
- It also offers many suggestions proposed by LuaRT users
- New: `ini` module to convert from/to Windows INI configuration file from/to Lua table
- New: ``Panel`` widget that can contain other child widgets

#### LuaRT toolchain
- Updated: Using ``require()`` to load ``.wlua`` files is now possible
- New: New tool `rtcheck.exe` to check/install LuaRT updates
- Fixed: `rtc` compiled executables with embedded modules can have now multiple running instances (Fixes #124)
- Fixed: `QuickRT` now pretty prints correctly `Object` values (Fixes #9).
- Fixed: `wluart.exe` don't crash anymore in case of error (Fixes #121)
- Fixed: `wluart.exe` errors are now shown, without the empty "Runtime error" message box (Fixes #123)

#### LuaRT Studio
- Updated: LuaRT Studio is now using the LuaRT 1.6.0 toolchain
- New: New `Check for LuaRT update` command in `File` menu
- New: New toolbar icon for the "Analyze" command
- Updated: `Help` menu now contains links related to LuaRT webiste
- Fixed: Debugging a script that contains a call to the `Task:wait()` method won't crash the program anymore (Fixes #20)
- Fixed: Clicking on the "stop" button will now really stop the running program (Fixes #19)

#### QuickRT
- Updated: QuickRT now uses LuaRT 1.6.0 runtime (Lua 5.4.6 VM)
- Fixed: interacting with `ui` widgets and windows won't hang the GUI anymore
- Fixed: Pretty printing of LuaRT `Object` is now fixed

#### LuaRT documentation
- Updated: now the documentation contains all new LuaRT 1.6.0 changes, and a lot of fixes
- Fixed: ``Canvas`` documentation for transformations methods are now published as expected

#### Examples
- Updated: `net\download.lua` hides now the cursor during downloading
- Updated: `ui\notepad.wlua` can now open *.rtf files
- Fixed: `net\server.wlua` now shows text as expected
- Fixed: Syntax highlighting fixed in `ui\zoom.wlua`

#### `sys` module
- Updated: Faster `Task` switching in internal scheduler providing more responsiveness
- Updated: `sys.cmd()` now has a third argument to specify whether the launched process is a standalone one or a child process
- Fixed: Setting a number `COM` property value don't cause an error anymore when using a string (Fixes #113)
- Fixed: ``COM`` objects don't crash once garbage collection/program exit (Fixes #116)
- Fixed: ``Datetime:interval()`` now returns a to...from interval as expected (Fixes #114)

#### `console` module
- Fixed: `console.reset()` now resets cursor height too

#### `net` module
- Fixed: `Http` object can now send new requests without crashing (Fixes #149)

#### `ui` module
- New: ``Panel`` widget that can contain other child widgets
- New: `Edit.rtf` property to set Rich Text Format mode without loosing content
- Updated: `Edit.richtext` property now set/get the current Edit content in Rich Text Format
- Updated: Setting ``Edit.text`` property won't scroll the content to the bottom anymore
- Updated: Autosized widgets now conforms more with Windows UI standards
- Updated: `Widget.align` now remembers last widget position and size
- Updated: `Widget.bgcolor` property now returns `nil` if bgcolor was not previously set, meaning the widget uses the parent background color
- Updated: `List.selected`, `Tree.selected` and `Combobox.selected` can now be set to `nil` to unselect current selection
- Fixed: Examples using `Edit.color` instead of `Edit.fgcolor` since LuaRT 1.5.0 are now fixed
- Fixed: ``Label.bgcolor`` and ``Label.fgcolor`` cannot be set when ``Label`` is inside a ``TabItem`` (Fixes #119)
- Fixed: ``ui.run()`` ``Task`` won't run indefinitely anymore in some cases
- Fixed: ``Groupbox`` child widgets events now provides the correct `self` (Fixes #125)
- Fixed: `Tab.cursor` property don't set cursor for `TabItems`, only on the tabsheet part (Fixes #134)
- Fixed: `Tab` don't align TabItems correctly when setting the `Tab.align` property (Fixes #128)
- Fixed: Widget background color may not be drawn correctly (Fixes #127)
- Fixed: Clicking on a `List` blank space unselects the current selected item (Fixes #143)
- Fixed: Deleting a `Treeitem` may crash the program (Fixes #137)
- Fixed: `Entry` is still focused when ENTER/RETURN key is pressed (Fixes #135)
- Fixed: `Groupbox` does not dispatch the `onClick()` event to its child widgets as expected (Fixes #147)
- Fixed: `Window:maximize()` does not work on first display (Fixes #142)
- Fixed: `Picture:load()` don't adjust image to `Picture` alignment (Fixes #132)
- Fixed: `Label.textalign` returns `"center"` instead of `"right"` when text alignement was previously set to right (Fixes #139)
- Fixed: Widgets property `tooltip` returns incorrect or empty string (Fixes #138)
- Fixed: Centering a Widget with the `center()` method in a `Window` with a status bar resulting in a vertical position too far up (Fixes #146)
- Fixed: `Picture:load()` don't adjust image to `Picture` alignment (Fixes #132)
- Fixed: `Progressbar.fgcolor` and `Progressbar.bgcolor` return wrong `0x000000` colors in non themed mode (Fixes #131)
- Fixed: `ui.opendialog()` and `ui.savedialog()` should not change current directory (Fixes #133) 
- Fixed: Clicking on a `MenuItem` without an `onClick` event handler should not throw a runtime error (Fixes #145)
- Fixed: `ListItem:loadicon()` `TabItem:loadicon()` `ComboItem:loadicon()` and `TreeItem:loadicon()` don't remove icon when called with a `nil` value (Fixes #129)
- Fixed: Setting `Edit.text` property don't work when `Edit.richedit` is set to `true` (Fixes #130)
- Fixed: `Edit:load()` and `Edit:save()` methods don't closes the file (Fixes #148)

#### `webview` module
- New: `Webview.acceleratorkeys` property 
- Updated: ``Webview:eval()`` now returns a `Task` that will be finished once the Javascript engine has returned the result
- Updated: `Webview:onResult()` event removed due to the `eval()` rework
- Updated: `hostfromfolder()` now allows local resource access when using `loadstring()`
- Fixed: Compiling ``Webview`` module won't throw library missing errors anymore

#### `json` module
- New: `json.load()` and `json.save()` to load/save lua table from/to a JSON file
- Fixed : `json.encode()` failed memory allocation fixed (Fixes #117)
- Fixed : `json.encode()` now encodes Lua `"null"` values to JSON `null` (Fixes #118)

## LuaRT v1.5.2 Bugfix (Sept 16 2023)

#### LuaRT installer
- Fixed: now the LuaRT installer window will show over other windows.

#### LuaRT toolchain
- Updated: `rtc` now prevents access to embedded content from generated executables.
- Fixed: LuaRT can now be built with `mingw-w64` compiler again (Fixes #105)

#### LuaRT documentation
- Updated: now the documentation contains all new LuaRT 1.5.0 (and later) changes 
- Fixed: Checking for a Widget instance won't throw a "((null)) instance expected, found..." anymore

#### LuaRT C API
- New: `lua_gettask()` function to get the current executing Task.

#### `sys` module
- Fixed: Getting COM enumeration property value don't crash anymore (Fixes #104)

#### Asynchronous programming
- Fixed: Calling `sleep()` from a compiled script don't cause a fatal error anymore (Fixes #103)

#### `ui` module
- New: `Progressbar.themed` property (as a side effect the `isthemed` constructor parameter is no more used).
- New: `Button.textalign`, and `Entry.textalign` properties.
- New: example `filelist.wlua` that implements an explorer-like List widget.
- Updated: `ui.run()` function now returns a `Task` object that will update the GUI asynchronously.
- Updated: `bgcolor` and `fgcolor` properties now returns the default color instead of `nil`.
- Updated: `Edit.selection.color` renamed to `Edit.selection.fgcolor` for consistency.
- Updated: `Edit.color` renamed to `Edit.fgcolor` for consistency.
- Fixed: `ui.colordialog()` and `ui.fontdialog()` now return a RGB color value as expected (Fixes #111)
- Fixed: `Tab.onMouseUp()` and `Tab.onMouseDown()` events are now fired when the mouse is over the entire Tab (Fixes #108)
- Fixed: Setting `List.selected` property now ensures the selected item is visible (Fixes #107)
- Fixed: `Label.bgcolor` now renders the Label background correctly (Fixes #102)
- Fixed: `Picture` constructor now uses the width and height parameters as expected (Fixes #106)
- Fixed: `Picture` transparency bug fixed when inside a Tab (Fixes #100)
- Fixed: `List` now redraws correctly when resized (Fixes #110)
- Fixed: `Label.textalign` property now behave as expected (Fixes #99)

## LuaRT v1.5.1 Bugfix (Aug 10 2023)

#### LuaRT installer
- Fixed: now the LuaRT installer will launch even if a previous LuaRT distribution is found

#### Asynchronous programming
- Fixed: await/async/sleep from a compiled script is now possible (Fixes #95)

#### `ui` module
- New: `onMouseDown()` and `onMouseUp()` events for `Window` and any other widgets
- New: example `mouse.wlua` to illustrate the new mouse events
- Fixed: calling `sleep()` from an ui event handler now works as expected (Fixes #96)
- Fixed: `Window.onCreate()` event is now fired as expected (Fixes #97)

## LuaRT v1.5.0 (Aug 03 2023)

#### Asynchronous programming with LuaRT
- Asynchronous programming with LuaRT is now available with a thin layer around Lua coroutines, with an integrated scheduler
- New global functions `async()`, `await()`, and `waitall()` functions
- New global function `sleep()` that replaces `sys.sleep()`
- New `sys.Task` object

#### LuaRT codebase 
- Visual C++ compiler support is now complete, thus becoming the default target compiler for LuaRT
- LuaRT release package is built using `cl.exe` version 19.35.32217.1 (Visual Studio 2022). See build instructions in `README.md`
- Mingw-w64 compiler support still available, but is now deprecated (might be removed later)
- LuaRT won't use AVX2 instructions set by default anymore (causes crash on CPU that don't provide it)

#### Lua VM
- Updated: LuaRT is now using Lua 5.4.6

#### LuaRT C API
- New: `lua_pushtask()` and `lua_sleep()` to create `Task` object and to `sleep` from C
- Fixed: `luaL_checkFilename()` now checks for a File instance

### QuickRT
- Better support of `ui` module (Window/widget can now be created and manipulated interactively from the command prompt) (suggestion [#72](https://github.com/samyeyo/LuaRT/issues/72))

### `sys` module	
- New: `sys.COM` objects can now manage enumeration properties by getting/setting string values
- New: `sys.COM` constructor can now use an already running instance of a COM object
- New: `sys.Task` object that encapsulates a Lua coroutine with scheduling capabilites
- Updated: `Pipe.read()` and `Pipe.readerror()` methods are now asynchronous and returns a `Task` object instance
- Removed: `sys.sleep()` function have been replaced by the new global `sleep()` function
- Fixed: `File.open()` with "append" mode don't recreate a new file instead of appending to it anymore (Fixes [#79](https://github.com/samyeyo/LuaRT/issues/79))
- Fixed: Errors now uses current locale encoding when displayed (Fixes [#89](https://github.com/samyeyo/LuaRT/issues/89))

### `console` module
- Fixed: `console.fullscreen` now works as expected on x64 (Fixes [#90](https://github.com/samyeyo/LuaRT/issues/90))

### `net` module	
- Total rework of the `net` module to use asynchronous operations
- `Http` object uses asynchronous Tasks for GET, POST, PUT, and DELETE HTTP requests
- `Http` object now supports headers, cookies and proxy connections (suggestion [#63](https://github.com/samyeyo/LuaRT/issues/63))
- `Http.sheme`, `Http.status`, `Http.statuscode`, `Http.mime`, `Http.open()` members have been removed
- New `Http.put()`, `Http.delete()`, `Http.cookies`, `Http.proxy()` members
- `Ftp` object now uses asynchronous Tasks to perform operations
- `Ftp` object now supports proxy connection and logging
- `Ftp.open()` and `Ftp.active` members have been removed
- New `Ftp.proxy()` method
- `Socket` uses now asynchronous Tasks for non blocking sockets, for `Socket.recv()`, `Socket.send()`, `Socket.connect()` and `Socket.accept()`
- `Socket.sendall()` method have been removed
- `net` modules examples have been updated accordingly

### `sqlite` module
- New binary module for LuaRT that provides SQLite3 database support
- New `Database`object to execute SQL commands or queries and iterate over the results seamlessly

### `canvas` module
- Fixed crash on some integrated Intel GPU without Direct2D support (Fixes [#76](https://github.com/samyeyo/LuaRT/issues/76))
- `Canvas.print()` prints now antialiased text.
- New `Canvas.rotate()`, `Canvas.translate()`, `Canvas.scale()`, `Canvas.skew()`, `Canvas.identity()` methods

### `audio` module
- Fixed crash on CPU that don't have AVX2 instruction set (Fixes [#77](https://github.com/samyeyo/LuaRT/issues/77))

### `ui` module
- New: Added support for RTL (right to left) layout, with layout autodetection and `ui.rtl` property to force a RTL or LTR layout
- New `ui.run()` function to the specified main Window asynchronously, while processing events, and returning only when this Window have been closed
- Updated `ui.update()` that don't use the `delay` argument anymore.
- Fixed iterating `Tree.items` with `each()` (Fixes [#91](https://github.com/samyeyo/LuaRT/issues/91))
- Fixed `Edit.append()` that may crash when appending lot of lines (Fixes [#88](https://github.com/samyeyo/LuaRT/issues/88))
- Fixed `Window.onMove()` event that was not properly fired (Fixes [#87](https://github.com/samyeyo/LuaRT/issues/87))
- Fixed `Groupbox` not redrawing correctly (Fixes #80 and #81)
- Fixed `Edit.onCaret()` event that was not fired (Fixes #75)
- Fixed `Edit.selection.visible` that was not hiding selection as expected (Fixes #74)
- Fixed `Edit.load()` to support BOM and prevent crash for big files (Fixes #73)

## LuaRT v1.4.0 (Apr 21 2023)

#### LuaRT codebase
- Rework of the LuaRT codebase, now separated between the core framework and the other modules
- The core framework includes the LuaRT runtime and the toolchain.
- The core framework source code is in the `src\core\` folder
- Complementary modules source code is in the `src\modules\` folder
- Updated build system to build all or part of LuaRT
- Examples are now sorted by modules

#### Lua VM
- Updated: LuaRT is now using Lua 5.4.5

#### LuaRT C API
- New: Macros for easy registering modules/objects functions and properties
- New: `luaL_getlasterror()` to push last Windows error message on Lua stack
- Updated: `luaL_checkscinstance()` now returns a more explicit error message
- Fixed: Headers can now be included from C++

#### LuaRT Studio v1.4.0
- Updated: LuaRT Studio now uses the LuaRT 1.4.0 toolchain

#### rtc v1.4.0
- New: `-l` option to embed binary modules in the executable
- Updated : `wrtc` has a new field to embed modules

### `embed` module	
- Fixed: New mechanism to embed files when compiling LuaRT scripts to executables, compatible with x86 and x64 (Fixes #66 and #67)

### `sys` module	
- New: `sys.fsentry()` function that returns a `Directory` or `File` object by name
- Fixed: `sys.error` property no longer returns strange characters in certain locales
	
### `ui` module
- New: Window have now an `onKey()` event, thrown when the user press a key
- New: Widgets have now an `onClick()` event (except Radiobutton and Checkbox)
- New: Window.fullscreen property, to switch the Window to full screen
- New: Widgets now have `tofront()` and `toback()` methods to control their Z position
- Updated: Window `"single"` style no longer allows the Window to be maximized
- Updated: Default `Entry.height` increased due to Segoe UI font
- Fixed: Closing Window with status bar don't make it showing again when closing (Fixes #68)
- Fixed: Custom widgets events don't crash anymore in case of Lua error (Fixes #65)
- Fixed: `Tab.selected` now really selects the specifed TabItem (Fixes #64)

### `canvas` module
- New binary module `canvas` that provides a new `ui` widget as a drawing surface, using Direct2D
- You can now draw text, lines, circles, rectangles, images, with colors, gradients and transparency
- New documentation and examples for the `canvas` module

### `audio` module
- New binary module `audio` for audio playback
- You can now play music and sounds encoded in WAV, MP3, FLAC, and OGG Vorbis
- New documentation and examples for the `audio` module

### `Webview` module
- Webview module is now part of the LuaRT codebase

#### Other
- Updated: Installer button now include platform version (x86/x64)


## LuaRT v1.3.2 Bugfix (Feb 19 2023)

#### LuaRT Studio
- Fixed `print()` function prints only first argument in Lua interpreter panel (Fixes #15)

#### `ui` module
- Fixed Widgets inside a `Groupbox` don't display background and foreground colors correctly (Fixes #62)
- Fixed Background colors of widgets may not display correctly (Fixes #61)
- Fixed ``Groupbox`` widget displays incorrectly in TabItem (Fixes #60)
- Fixed ``Button.onClick()`` event handler is called twice (Fixes #59)
- Fixed Widgets ``bgcolor`` property returns wrong RGB color value (Fixes #58)
- Fixed ``Window.align`` property returns ``"all"`` instead of ``nil`` (Fixes #57)
- Fixed ``Edit.bgcolor`` property returns wrong value (Fixes #56)
- Fixed ``Combobox:onClick()`` event not thrown (Fixes #55)
- Fixed Clicking on Window keep the current widget focused (Fixes #54)
- Fixed Fast ``Checkbox`` mouse click don't throw ``onClick()`` event (Fixes #53)
- Fixed ``Edit`` selection is not visible when focus is lost (Fixes #52)
- Fixed ``tree:remove()`` called with string argument throws an error (Fixes #51)
- Fixed ``Combobox:remove()`` don't check for value type (Fixes #50)
- Fixed ``Combobox:clear()`` don't clear at all items (Fixes #49)
- Fixed ``Combobox:onChange()`` event not thrown when selection change

## LuaRT v1.3.1 Bugfix (Feb 04 2023)

#### LuaRT Studio
- Fixed running script fails due to spaces in path (Fixes #13) 
- Fixed running with arguments fails (Fixes #14)

#### `ui` module
- Fixed Combobox:clear() don't clear at all items (Fixes #49)
- Fixed Combobox:remove(), Listbox:remove(), Tree:remove(), Tab:remove() don't check for value type (Fixes #50)
- Fixed Combobox:onChange() event not thrown when selection change

## LuaRT v1.3.0 (Jan 28 2023)
 
#### General / Highlights
- New: compiled Lua scripts can now require for embedded DLL binary modules **without extraction** (may not work for all binary modules)
- New: binary modules inside the `\modules` folder can now be required using a folder structure, while still using ``luaopen_modulename()`` function
- Updated ``luart.exe``and ``wluart.exe`` command line arguments (-e and script can now be specified together : the -e statement will be executed first)
- String module now uses non-encoded strings by default, as standard Lua (UTF8 functions are now available, see ``string`` module below)
- Updated default desktop application icon 

#### LuaRT installer
- Now detects already installed LuaRT distribution 

#### rtc
- Updated ``wrtc`` so that it don't show a message box after compilation anymore
- Updated to use the UTF8 string functions

#### QuickRT
- Updated to use the new UTF8 string functions

#### LuaRT Studio
- LuaRT toolchain updated to v1.3.0 
- Updated examples to LuaRT v1.3.0
- New integrated Lua code formatter/beautifier <kbd>Alt</kbd> + <kbd>F</kbd> (uses Stylua)
- New Variables pane shown during debugging that list all current scope variables
- New debugging view that shows Symbols and Variables pane during debugging
- New UTF8/UNICODE complete support in Output pane
- New LuaRT ``modules\`` folder autodetection
- New keyboard shortcut to copy line(s) down <kbd>Alt</kbd> + <kbd>Shift</kbd> + <kbd>&#8595;</kbd> and up <kbd>Alt</kbd> + <kbd>Shift</kbd> + <kbd>&#8593;</kbd>
- New keyboard shortcut to move line(s) down <kbd>Alt</kbd> + <kbd>&#8595;</kbd> and up <kbd>Alt</kbd> + <kbd>&#8593;</kbd>
- Changed comment keyboard shortcut to <kbd>Ctrl</kbd> + <kbd>:</kbd>
- Changed replace keyboard shortcut to <kbd>Ctrl</kbd> + <kbd>H</kbd>
- Changed replace in files keyboard shortcut to <kbd>Ctrl</kbd> + <kbd>Shift</kbd> + <kbd>H</kbd>
- Updated "Local console" pane to "Lua interpreter" pane
- Updated "Remote console" pane to "Debugging console" pane
- Updated breakpoint color to a more consensual one (red !)
- Updated wrtc to show message in Output panel after compilation succeeded instead of showing a message box
- Symbols pane now shows only symbols for the current opened file
- Project view now shows EXE and DLL files
- Removed Watch pane (replaced by the new Variables Pane)
- Fixed Lua scripts cannot be executed in the IDE 
- Fixed "Show console when running option" has no effect
- Fixed document tab may contain more than one '*' after file change
- Fixed Symbols pane showing global variables multiple times
- Fixed Symbols pane showing a non transparent icon for anonymous functions
- Fixed Output pane don't support reading from console input 
- Fixed LuaRT Console output module breaking Output pane redirection 
- Fixed binary modules in LuaRT "modules\\" folder not found during debugging session

#### Examples
- Examples have been updated to use the new UTF8 string functions

#### Objects
- Fixed ``super()`` global function do not returns inherited parent object (Fixes #42)
- ``super()`` supports only method ancestor calls using the "." dot notation, specifying the self instance explicitly

#### LuaRT C API
- Fixed ``luart.h`` C++ inclusion that was throwing errors regarding `typename` keyword (Fixes #41)
- New ``lua_super()`` function in LuaRT C API to push on the stack the ancestor of the provided indexed Object/instance on the stack

### ``console`` module
- Fixed bad standard input redirection with ``luart.exe`` and ``wluart.exe`` (Fixes #39)
- Fixed ``console.readchar()`` running forever when interpreter input is redirected (Fixes #40)

#### `string` module
- Updated UTF8 functions in ``string`` module, now prefixed with a ``u`` to preserve compatibility with standard Lua

#### `net` module
- New ``net.getmime()`` function to retrieve MIME type from URL, filename or ``sys.File`` instance

#### `sys` module
- New ``Directory:copy()`` method to copy a folder recursively
- Fixed buffer overflow when reading long sized UTF8 character from ``sys.File`` instance (Fixes #43)
- Fixed ``Directory:removeall()`` that can be very slow on Windows 10 and later (Fixes #44)

#### `compression` module
- New ``Zip:open("delete")`` mode
- New ``Zip.remove()`` method to delete one or more entries in ZIP archive.
- Updated dependencies zip to 0.2.6 and miniz to 2.2.0 

#### `ui` module
- New ``"single"`` Window style (standard Window that cannot be resized)
- Fixed widgets ``align`` property not working correctly
- Fixed Parent align property don't propagate to childs widgets (Fixes #46)

---

## LuaRT v1.2.0 (Dec 11 2022)
 
#### General
- New ``modules\`` folder in the root directory to put your binary/Lua modules
- Fixed *lots of* modules memory leaks (string iterator, ``embed`` module, ``COM`` module, ``ui`` module, ``Zip`` object)

#### LuaRT installer
- Updated installer with the new Progressbar widget and other cosmetics changes
- Fixed uninstaller from deleting installation folder even if it's not empty anymore (Implements #36)

#### LuaRT Studio
- Updated LuaRT Studio with LuaRT 1.2.0 runtime
- Fixed LuaRT-Studio x64 program crash when starting a new debug session

#### LuaRT C API
- Include files are now in the ``include\`` folder in the root directory
- Library files are now in the ``lib\``
- New `lua_registerevent()` and `lua_getevent()` for custom widgets events
- New `lua_widgetdestructor()` to be called during custom Widget destruction
- New `lua_widgetproc()` to register a custom Widget messages procedure
- ``luart.h`` can now be included in C++ files easily
- New header ``Widget.h`` for builtin and custom Widgets
- Fixed compilation warnings when using GCC 12+

- `lua_widgetfinalize()` renamed to `lua_widgetconstructor()`

- Fixed `lua_checkcinstance()` error when using a non instance value
- Fixed `lua_registerwidget()` not setting Widget properties properly

#### `sys` module
- New ``Directory.isempty`` property to check if a directory is empty

- Fixed crash when getting a ``COM`` object
- Fixed ``COM`` methods that might being called twice

#### `console` module
- Fixed console to be not closeable anymore when using desktop applications (Fixes #32)

#### `ui` module
- New ``Progressbar`` widget
- New `onClick()` event for all Widgets
- New rounded corners for ``"raw"`` windows on Windows 11
- New ``Widget.align`` property (alignment now persists even if the size of the parent Widget changes)

- Removed ``Widget:align()`` method (replaced by the new ``align`` property)
- Updated ` Window:status()` to convert each argument to string
- Updated ``ui.info()``, ``ui.error()``, ``ui.confirm()`` and ``ui.warn()`` to convert first argument to string
- Updated ``ui.update()`` to use registered events for custom Widgets
- Updated ``ui.List`` selection now spans to the entire row
- Updated ``Edit:append()`` to scroll down automatically

- Fixed changing a widget font attribute alter other widget fonts (Fixes #18)
- Fixed ``Widget:center()`` not taking in count the presence of a Window statusbar
- Fixed ``ui.Tab`` not drawn properly and background color when parent window background color changes
- Fixed ``Label.fgcolor`` and ``Label.bgcolor`` properties ineffective when parent is a ``TabItem`` (Fixes #35)
- Fixed Window statusbar not always showing on top of other widgets (Fixes #31)
- Fixed ``Tree``, ``List``, ``Combobox`` and ``Tab`` that didn't check for a ``table`` argument in constructor
- Fixed readonly ``Edit`` scrollbars that coouldn't be used
- Fixed ``Picture`` constructor not accepting ``string`` or a ``File`` instance (Fixes #30)
- Fixed ``Window`` finalizer not being called
- Fixed taskbar showing ugly-resized Window icon when application is compiled with rtc -i option (Fixes #34)
- Fixed ``Widget:center()`` not taking in count the presence of the window statusbar
- Fixed selected ``TreeItem`` not staying higlighted

16/10/2022 -------- LuaRT 1.0.2 (bugfix)

----------- General
[ NEW ]			New examples img_viewer.wlua and binary.wlua

----------- QuickRT
[ BUG ]			Fixed nil variable names with underscore character "_" outputs no result 

----------- LuaRT Studio
[ BUG ]			Fixed runtime error when compiling to executable without a project set 

---------- net module
[ BUG ] 		Http:get() method might return wrong results

----------- ui module
[ UPDATE ]		Segoe UI font is now used by default, as recommended since Windows Vista
[ BUG ]			Fixed Runtime error when compiling to executable without a project set 
[ BUG ]			Fixed Groupbox painted with white background color when redrawn

08/10/2022 -------- LuaRT 1.0.1 (bugfix)

----------- General
[ BUG ]			Fixed GCC 10+ warnings during compilation
[ BUG ]			Fixed build error because of "bin\" and "bin\std" folders missing

---------- rtc
[ BUG ] 		Lua compiled scripts silently crash after execution

----------- ui module
[ BUG ]			Fixed strange characters with IME input when using Entry and Combobox widgets

25/09/2022 -------- LuaRT 1.0.0

----------- General
[ NEW ]			LuaRT is now open sourced, under the MIT License
[ NEW ]			LuaRT binary distribution now includes LuaRT Studio and QuickRT
[ NEW ]			New examples : syntax.wlua, iconbutton.wlua, balls.wlua, dir.lua

----------- rtc
[ NEW ]			New rtc -i option to change executable icon 
[ NEW ]			New wrtc.exe GUI frontend for rtc

----------- OOP
[ NEW ]			New mixins implementation for Object() declarations
[ NEW ]			New super() global function to get inherited object
[ NEW ]			New Object.iterator() to iterate over an object instance
[ UPDATED ]		rawset() and rawget() global functions cannot operate on objects/instances anymore

----------- zip module
[ UPDATED ]		Zip object iteration now returns only the entry name
[ BUG ]			Fixed Zip:write() archive corruption when writing directories

----------- ui module
[ NEW ]			New Edit.selection.visible property to disable scrolling/visibility when manipulating the selection
[ NEW ]			New Button.hastext property 
[ NEW ]			New readonly option in Combobox constructor 
[ NEW ]			New Window.bgcolor, Label.bgcolor, Label.fgcolor properties
[ NEW ]			New Window:loadtrayicon() method, Window.traytooltip property and tray icon events
[ UPDATED ]		Tree:add(), Treeitem:add(), List:add(), Tab:add() and Combobox:add() now accepts more than one string
[ UPDATED ]		Menu:add() now has an optional parameter for a submenu, as Menu:insert()
[ UPDATED ]		Window:loadicon(), MenuItem:loadicon() and button:loadicon() called without argument removes the icon
[ BUG ]			Fixed edit.color and edit.bgcolor property (using RGB value and not BGR value)
[ BUG ]			Fixed application crash on status bar mouse click
[ BUG ]			Fixed runtime error once a MenuItem, without onClick event set, is clicked
[ BUG ]			Fixed widgets not redrawing once their position has changed
[ BUG ]			Fixed Groupbox childs widget not receiving events notification
[ BUG ]			Fixed TAB and ARROWS keys widget navigation on a Window
[ BUG ]			Fixed crash with Window.parent property 

12/06/2022 -------- LuaRT 0.9.9
[ NEW ]			LuaRT is now available for x64 and x86 Windows systems
[ NEW ]			New global _ARCH variable that holds LuaRT interpreter architecture ("x64" or "x86")
[ NEW ]			New sys.COM object added to interact with Microsoft Component Object Model
[ NEW ]			New examples illustrating the new sys.COM object : json.lua, speech.lua, shortcut.lua
[ NEW ]			New LuaRT interpreter option "-e" to execute a Lua statement from the command line

[ UPDATED ]		LuaRT interpreter now accepts UTF8/UNICODE characters as arguments
[ UPDATED ]		Global _VERSION variable now holds full LuaRT version string
[ UPDATED ]		Now qrcode.wlua example can now save the generated QRCODE 

[ BUG ]			ComboItem:remove(), ListItem:remove(), TabItem:remove() are now working as expected
[ BUG ]			Window shortcuts are now calling the binded function correctly
[ BUG ]			TabItem:remove() now gives focus to the next remaining TabItem
[ BUG ]			Path registry key is no more corrupted during LuaRT installation
[ BUG ]			ComboItem, TabItem, TreeItem, ListItem and MenuItem objects are no more available from ui module

```LuaRT beta releases
26/05/2022 -------- LuaRT 0.9.8
[ NEW ]			New 'compatibility' LUA54.DLL runtime in bin\std, with better Lua 5.4 standard library compatibility (including io and os modules)
				This runtime library provides better compatibility with Lua ecosystem (mobdebug, LuaSocket,...)				
[ NEW ]			LuaRT now uses generational garbage collector by default
[ NEW ]			New sys.tempdir() function to create temporary directories

[ UPDATED ]		sys.Directory constructor now uses the current directory by default
[ UPDATED ]		File:copy() now returns a File object representing the copied file
[ UPDATED ]		Buffer:contains() now returns only one value (sequence position or false)
[ UPDATED ]		Edit.textlength property removed 
[ UPDATED ]		Edit.lines property now returns empty string instead of nil when line is empty
[ UPDATED ]		"embed" module is now defined globally if embedded content is detected inside executable
[ UPDATED ]		ui.Edit:onChange() event don't fire any other Edit:onChange() during its call

[ BUG ]			console required by a desktop application don't close entire application now
[ BUG ]			ui.List now has better horizontal visibility
[ BUG ]			Widget:loadicon() now load other Widget's icon correctly
[ BUG ]			console.fullscreen property now works correctly with wluart.exe interpreter
[ BUG ]			GroupBox child widgets are now throwing events to child widgets correctly
[ BUG ]			Fixed random crashes when using ui module when IME is activated on Windows

20/03/2022 -------- LuaRT 0.9.7
[ NEW ]		LuaRT script to executable compiler rtc.exe
[ NEW ]		New geo.lua example
[ NEW ]		New geo.lua example
[ NEW ]		New qrcode.wlua example
[ NEW ]		New labeled_entry.wlua example
[ NEW ]		console.cursor property to get/set cursor visibility

[ UPDATE ]	luart.exe now don't include the ui module, use wluart.exe instead for GUI applications

[ BUG ]		sys.registry.delete() now delete key and all subkeys/values if no specific value is provided, as expected
[ BUG ]		GUI applications now open a console Window as expected when requiring for the console module
[ BUG ]		console.fullscreen now work better with Windows Terminal
[ BUG ]		File.accessed, File.created, File.modified now return nil when file do not exists
[ BUG ]		console.readchar() now return correct value when pressing "à" key on french, belgian, and canadian keyboards
[ BUG ]		High CPU usage when calling ui.update() fixed
[ BUG ]		Combobox now display selected item correctly
[ BUG ]		Setting MenuItem.text don't corrupt the MenuItem.text property anymore
[ BUG ]		Last item added to the List was not always visible
[ BUG ]		Horizontal List's scrollbar is now shown only if the item width is lower than List width
[ BUG ]		Directory.fullpath is now valid if the Directory name is a drive letter (C:\, D:...)
[ BUG ]		TreeItem:loadicon() now works if the Tree widget has been previously cleared with Tree:clear()
[ BUG ]		Zip:write() and Zip:extract() don't return false anymore if the operation succeeded
[ BUG ]		Cipher.constructor() now throws the right error message if incorrect mode is provided

12/03/2022 -------- LuaRT 0.9.6
[ NEW ]		LuaRT now uses Lua 5.4.4 VM
[ NEW ]		New ui.Tree Widget supporting icons and new theme for Windows 10/11
[ NEW ]		New TreeItem Object
[ NEW ]		New documentation for ui.Tree and ui.Picture widgets
[ NEW ]		New Picture:load(), Picture:save(), Picture:resize() methods  
[ NEW ]		New Picture:onClick() and Picture:onDoubleClick() events
[ NEW ]		New Widget:center() method 
[ NEW ]		New Tab.style property
[ NEW ]		New Label:onDoubleClick() event
[ NEW ]		New fireworks.lua example from QB64 version
[ NEW ]		New zoom.wlua example
[ NEW ]		New imgconv.wlua example

[ UPDATE ]	console.locate() now accepts float numbers
[ UPDATE ]	console:readln() provides a better user experience (support now arrows, history, HOME,...)
[ UPDATE ]	Widget:show() now shows the Widget and bring it on top
[ UPDATE ] 	examples updated for new ui.Tree widget
[ UPDATE ] 	ui.opendialog() and ui.savedialog() now use the selected file extension if omitted by user
[ UPDATE ] 	Combobox and List don't loose their icons anymore when switching between "text" and "icons" styles
[ UPDATE ]	Ftp:upload() now expect a string or a File object
[ UPDATE ]	sys.cmd() now has an optional parameter to show/hide the executed command window
[ UPDATE ]	Zip object iterator now returns an entry string and a isdir boolean value

[ BUG ]		*Lots of* documentation fixes
[ BUG ]		luart.exe and wluart.exe now shows the right LuaRT version
[ BUG ]		Fixed: Window:show() now should show the Window on top
[ BUG ]		error message in console are now followed by a newline
[ BUG ]		global _VERSION variable is now uptodate
[ BUG ]		Combobox.text property is now back
[ BUG ]		example widget.lua : Picture widget now load LuaRT.png correctly
[ BUG ]		Various GUI application crashes fixed
[ BUG ]		sys.registry.read() now fixed prior to Windows 10
[ BUG ]		LuaRT install script don't crash anymore prior to Windows 10
[ BUG ]		console.readchar() fixed for unicode char 'à' (0x00E0)
[ BUG ] 	console.fullscreen now don't show scrollbars when set to true

23/01/2022 -------- LuaRT 0.9.5
[ NEW ]		New LuaRT GUI installer written in LuaRT
[ NEW ]		New widget ui.Picture supporting JPEG, TIFF, GIF, PNG, BMP and ICO formats
[ NEW ]		New registry functions : sys.registry.read(), sys.registry.write(), sys.registry.delete()
[ NEW ]		Zip.file property returns the Zip filename
[ NEW ]		zip.isvalid() checks if the specified zip file is valid
[ NEW ]		ui.remove() removes a widget
[ NEW ] 	List:onDoubleClick() event
[ NEW ] 	New console.font and console.fontsize property
[ NEW ]		New Directory.name property

[ UPDATE ]	File:copy() now uses the same filename by default
[ UPDATE ]	zip module now have two member : Zip Object constructor and isvalid() method
[ UPDATE ]	Zip:extract() can now extract zip entries preserving their path if needed
[ UPDATE ]	Widget:loadicon() now accept string or File object
[ UPDATE ]	Widget:loadicon() method can now extract an icon from an EXE or DLL or retrieve the default associated icon
[ UPDATE ]	Widget.font property can now accept a font file (".ttf", ".fon"...) 
[ UPDATE ]	Tab.items, List.items, Combo.items properties are now indexable by strings
[ UPDATE ]	File.name property replaces now File.filename
[ UPDATE ]	Setting/getting LuaRT modules fields is now possible as in standard Lua
[ UPDATE ]	Widget:loadicon() now uses default system associated icon if not found in the specified file
[ UPDATE ]  Zip constructor now accepts File objects 
[ UPDATE ]  notepad.wlua now changes font, fontsize and fontstyle when setting font

[ BUG ]		ui.msg, ui.info, ui.error, ui.warn() now ensure the foreground window is from ui module
[ BUG ]		Button:loadicon() is now *really* available
[ BUG ]		Combo:onSelect() and Combo:onChange() crash fixed when altering item.text property
[ BUG ]		Sizing and position properties now fixed when using non integer values
[ BUG ]		Windows have now their default background color (back to gray) !
[ BUG ]		Now widgets transparency and Windows theme are more respected (TabItems, Labels, Checkboxes...)
[ BUG ]		ui.opendialog() and ui.savedialog() now use the provided title
[ BUG ]		List column now resizes correctly with the List
[ BUG ]		List can now show items correctly
[ BUG ]		Widget align() method now uses correct parent size
[ BUG ]		Tabs now draws correctly
[ BUG ]		Console.fullscreen removes the vertical scrollbar correctly when set to true

27/12/2021 -------- LuaRT 0.9.4
[ NEW ]		Tab and List onContext(), onHover() and onLeave() events
[ NEW ] 	Combobox, List and Tab remove() method
[ NEW ]		ComboItem, ListItem and TabItem are now iterable
[ NEW ]		ComboItem, ListItem and TabItem owner property
[ NEW ] 	ComboItem, ListItem and TabItem remove() method
[ NEW ] 	Combobox.text property 
[ NEW ] 	Combobox, Tab and Listbox constructors now expects table with strings items
[ NEW ] 	List:sort() method 
[ NEW ]		List.style and Combobox.style property
[ NEW ]		MenuItems, Button, List, Combobox, Tab objects now support icons
[ NEW ]		MenuItems.onClick event
[ NEW ]		Widget.cursor property to change control's cursor when hovering with the mouse
[ NEW ]		Widget.font is now a read/write property
[ NEW ]		Widget.fontsize property
[ NEW ]		Widget.fontstyle property
[ NEW ]		Window:loadicon(), Button:loadicon() and MenuItem:loadicon() methods
[ NEW ]		Item object for List, Tab and Combobox members
[ NEW ]		Tab:insert(), List:insert() and Combobox:insert() methods
[ NEW ]		Entry:searchup() and Edit:searchdown() method
[ NEW ]		Entry:add() method
[ NEW ]		Edit.selection property
[ NEW ]		Edit:load() and Edit:save() method, supporting RTF files
[ NEW ]		New ui.colordialog() function
[ NEW ]		New Window style "raw"
[ NEW ]		The long awaited system function sys.Beep() is now implemented
[ NEW ]		Widget.parent property
[ NEW ]		Widget.onCreate event
[ NEW ]		ui.mousepos() function
[ NEW ]		Window:showmodal() method
[ NEW ]		Window:shortcut() method
[ NEW ]		Window:popup() method
[ NEW ]		Window.menu property
[ NEW ]		File.name to get filename (replace previous File.filename propoerty) and Directory.name to get directory name (the last part of the directory path)

[ UPDATE ]	Rework of ui objects event system for a new faster implementation and more responsiveness
[ UPDATE ]	Total rework of Menu and MenuItem objects
[ UPDATE ]	ui.dirdialog() now return a Directory value
[ UPDATE ]	ui.opendialog() now allow multiple files selection
[ UPDATE ]	ui.dialogfont() updated
[ UPDATE ]	Tab.selected, Combobox.selected and List.selected properties now get/set an Item object
[ UPDATE ]	Menu.items, Tab.items, List.items and Combobox.items are now iterable
[ UPDATE ] 	List.items, Combobox.items and Tab.items are now proxy table to Item objects
[ UPDATE ] 	Menu.items is now a proxy table to MenuItem objects
[ UPDATE ] 	Tab.pages renamed to Tab.items accordingly to List.items and Combobox.items
[ UPDATE ]  ui.fontdialog() now returns choosen font name, size, styles and color.
[ UPDATE ] 	Notepad.wlua example rewrite, now supporting large files, RTF file type
[ UPDATE ]  ui.update() now has an optional delay argument
[ UPDATE ]	wluart.exe has now a new icon

[ BUG ]		Objects inheriting from Widgets are now fixed
[ BUG ]		Inherited Objects use of properties inside constructor is now fixed
[ BUG ]		Tab.pages text property fixed
[ BUG ]		ui.savedialog and ui.loaddialog filter bug fixed
[ BUG ]		Setting Tab.selected, Listbox.selected and Combobox.selected properties now throws an onSelect event
[ BUG ]		Edit object now has a border
[ BUG ]		Stack bug fixed after Window:onClose() event
[ BUG ]		Fixed error message when occuring with get/set property
[ BUG ]		Changing font of a Widget now autosizes the Widget to take count the change
[ BUG ]		Widgets fontsize property now get/set the font height in points not in pixels
[ BUG ]		Window.constructor() bug fixed, now using the given width and height arguments
[ BUG ]		Window:align() bug fixed
[ BUG ]		Widget.x and Widget.y property bug fixed
[ BUG ]		Widget.width and Widget.height property bug fixed
[ BUG ]		Widget.enabled property now redraws the Widget
[ BUG ]		Resized window now update properly its status bar
[ BUG ]		sys.clipboard get property now fixed
[ BUG ]		console.read bug with DEL key fixed
[ BUG ]		File:open() uses "binary" encoding now, if not provided
[ BUG ]		Richedit DLL now unloads correctly
[ BUG ]		Pipe constructor now hides the created process
[ BUG ]		Windows with "fixed" styles maximize button now hidden on Windows 11
[ BUG ]		Now ui.Window() shows correctly on the Windows Taskbar
[ BUG ]		ui.opendialog() and ui.savedialog() not return nil if user have canceled as expected

04/07/2021 -------- LuaRT 0.9.3 beta

[ NEW ]		Total rework of Zip module with support of Zip64 specification
[ NEW ]		Zip:isdirectory() method
[ NEW ]		Zip.iszip64() method
[ NEW ] 	Zip:write() / Zip:read() / Zip:extract() now more integrated with File, Directory objects
[ NEW ] 	crypto.Cipher object supporting now AES128, AES192, AES256, DES, 2DES, 3DES, RC2, RC4
[ NEW ] 	Support encryption/decryption in CBC, CFB, ECB, and CTS modes
[ NEW ]		crypto.hash() function supporting MD5, SHA1, SHA256, SHA384, SHA512
[ NEW ]		Buffer:contains() method
[ NEW ]		File.directory property
[ NEW ]		Global _VERSION variable is now set to "luaRT x.x"
[ NEW ]		New example "help.lua" with auto-generated table from documentation

[ BUG ]		Buffer equality fixed and optimized
[ BUG ]		File:open() - User provided encoding now prevail over BOM file detection
[ BUG ]		Directory:make() now can create folders on another drive
[ BUG ]		Various memory leaks fixed
[ BUG ]		Datetime:interval() now use "months" instead of "month"
[ BUG ]		*Lots of* documentation fixes
[ BUG ]		sys.cmd() now don't impact standard streams

07/03/2021 -------- LuaRT 0.9.2 beta1

[ NEW ]		net.Ftp object
[ NEW ]		net.Http object
[ NEW ]		net.adapters iterator
[ NEW ]		net.urlparse() function
[ NEW ]		net.ip and net.publicip properties
[ NEW ]		Socket.port, Socket.ip, Socket.family properties
[ NEW ]		Socket:sendall() method
[ NEW ]		Zip:extractall() method
[ NEW ]		Http object to perform GET/POST requests
[ NEW ]		ui.dirdialog() to open a dialog and choose directory
[ NEW ]		New example sendmail.lua
[ NEW ]		New example widgets.lua
[ NEW ]		LuaRT Community now hosted on http://community.luart.org

[ UPDATE ]	crypto module has been refactored
[ UPDATE ]	net.resolve() and net.reverse() now supports IPv6
[ UPDATE ]	Socket constructor now supports IPv6
[ UPDATE ]	Socket:start_tls() have been renamed to Socket:starttls() for more consistency
[ UPDATE ]	Socket:bind() have been removed
[ UPDATE ]	Buffer:encode() has been renamed to Buffer:decode() (actualy Buffer:encode does not encode, but decode)
[ UPDATE ]	Zip:extract() can now extract in a specified path
[ UPDATE ]	Zip:extract() can now extract entire directories
[ UPDATE ]	console.readln() now accepts a string (written to the console before reading for input)
[ UPDATE ]	console.fullscreen is now a read/write property 
[ UPDATE ]	console.echo now enable/disable echoing, with the possibility to set the echoed character.
[ UPDATE ]	Examples lang.lua and guess.lua have been updated to use the new console color names
[ UPDATE ]	Documentation : various fixes and rewrites, better SEO, navigation links in modules section

[ BUG ]		global arg table fixed (arg[0] was the luart.exe interpreter)
[ BUG ]		console.read(n) reads more than n characters when console.echo is true
[ BUG ]		Fixed ui onClick() event not leaving the clicked control
[ BUG ]		Fixed type() function not returning "nil" for nil argument as it should
[ BUG ]		Function console.clear now uses the provided color argument as it should
[ BUG ]		Directory:make() is now *really* constructing the path
[ BUG ]		console.stderr and console.stdout now check for "write" File open mode before assignment
[ BUG ]		Fixed console.fullscreen() hanging minimized when calling console.fullscreen(false) twice
[ BUG ]		File:writeln() now writes CRLF "\r\n" (default Windows EOL)

07/02/2021 -------- LuaRT 0.9.1 beta1

[ NEW ]		Added Socket:shutdown()
[ NEW ]		Added Socket.canread, Socket.canwrite, Socket.failed properties
[ NEW ]		Added net.select()
[ NEW ]		Added console.x and console.y properties
[ NEW ]		Added console.readmouse() function
[ NEW ]		Strings are now iterable with each() function
[ NEW ]		Buffers are now iterable with each() function

[ UPDATE ]	lua54.dll is no more embedded in luart.exe and wluart.exe (generate lots of false positive with antivirus)
[ UPDATE ]	console colors naming update 
[ UPDATE ]	console.write() and console.writeln() now write all arguments to console.stdout using tostring()
			and separate each arguments with a space character
[ UPDATE ]	console.writecolor() major change : first rgument is the color, then write all arguments to console.stdout 
			using tostring() and separate each arguments with a space character
[ UPDATE ]	Fixed false positive antivirus detection using crypt module 
[ UPDATE ]	console.open() removed : now console module detects and open a console if none is available (wluart.exe)
[ UPDATE ] 	Documentation : various fixes
[ UPDATE ] 	Documentation : console module documentation now available

[ BUG ]		Fixed application hanging when clicking on ui.Window status bar
[ BUG ]		Fixed wrong index in string.byte()
[ BUG ]		File:SaveAs() function fixed in examples\notepad.wlua
[ BUG ]		Throws error when File:read() on an File opened in "write" or "append" mode
[ BUG ]		Throws error when File:write() on an File opened in "read" mode
[ BUG ]		File:read() and File:readln() now return nil instead of "" when reading past File.eof
[ BUG ]		console.stdin/out/err:close() not reaffecting correctly standard File
[ BUG ]		console.locate coordinates uses now 1 based coordinates
[ BUG ]		Fixed Socket.constructor throwing strange error message
[ BUG ]		Fixed modeless ui.opendialog, ui.savedialog
[ BUG ]		Fixed modeless ui.msg, ui.warn, ui.info, ui.confirm

17/01/2021 -------- LuaRT 0.9 beta1