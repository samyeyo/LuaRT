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