-- LuaRT Notepad example

local ui = require "ui"

-- Main Window object
local win = ui.Window("Untitled - LuaRT Notepad")
win:loadicon(sys.env.windir.."\\notepad.exe")	

-- Edit object
local Edit = ui.Edit(win, "", 0, 0)
Edit.font = "Consolas"
Edit.fontsize = 8

-- global state variables
encoding = "UTF8" 	-- current edited file encoding
title = "Untitled"	-- current file title
file = nil			-- current File object

-- helper function to update status bar
local function update_status()
	if (Edit.modified) then
		win.title = title.."* - LuaRT Notepad"
	else
		win.title = title.." - LuaRT Notepad"
	end
	win:status("Encoding : "..encoding, "Total lines : "..#Edit.lines, "Line : "..Edit.line, "Position : "..Edit.column)
end

----------------------------- Window main menu
win.menu = ui.Menu()

----------------------------- File menu
local FileMenu = ui.Menu()
win.menu:add("&File").submenu = FileMenu

-- File => New 
local FileNew = FileMenu:add("&New\tCtrl+N")
function FileNew:onClick()
	if Edit.modified then
		if ui.confirm("Current file is not saved. Any changes will be discarded. Continue without saving ?") ~= "yes" then
			return
		end
	end
	Edit.text = ""
	title = "Untitled"
	Edit.modified = false
	update_status()
end

-- File => Open 
local FileOpen = FileMenu:add("&Open...\tCtrl+O")
function FileOpen:onClick()
	if not Edit.modified or (Edit.modified and ui.confirm("Current file is not saved. Any changes will be discarded. Continue without saving ?") == "yes") then
		file = ui.opendialog("Open file...")
		if file ~= nil then
			file:open()
			local isRTF = false
			if file.extension == ".rtf" then 
				encoding = "UTF8"
				isRTF = true
			else
				encoding = file.encoding:capitalize()
				if encoding == "Binary" then 
					encoding = "ASCII"
				end
			end
			file:close()
			Edit:load(file, isRTF)
			title = file.name
			Edit.modified = false
			update_status()
		end
	end
end

-- File => Save
local FileSave = FileMenu:add("&Save\tCtrl+S")
function FileSave:onClick()
	if file == nil then
		file = ui.savedialog("Save as...", false, "All files (*.*)|*.*|Text files (*.txt)|*.txt")
		if file ~= nil and file.exists and ui.confirm(file.fullpath.." already exists. Continue and overwrite its content ?") ~= "yes" then
		 return
		end
	end
	if file ~= nil then
		Edit:save(file)
		title = file.name
		Edit.modified = false
		update_status()
	end
end

-- File => Save
local FileSaveAs = FileMenu:add("S&ave as...")
function FileSaveAs:onClick()
	file = nil
	FileSave:onClick()
end

-- File => Exit
local FileExit = FileMenu:add("E&xit")
function FileExit:onClick()
	if Edit.modified and ui.confirm("Current file is not saved. Any changes will be discarded. Continue without saving ?") ~= "yes" then
		return false
	end
	win:hide()
end

----------------------------- Edit menu
local EditMenu = ui.Menu("Undo\tCtrl+Z", "Redo\tCtrl+Y", "Cut...\tCtrl+X", "Copy\tCtrl+C", "Paste\tCtrl+V", "", "Select All\tCtrl+A")
win.menu:add("&Edit").submenu = EditMenu

-- EditMenu => single onClick event
function EditMenu:onClick(item)
	-- retrieve Edit method from clicked MenuItem
	local method = item.text:match("(%w+)"):lower()
	if method == "select" then
		Edit.selection.from = 1
		Edit.selection.to = 0
	else
		-- call this method
		Edit[method](Edit)
	end
end

----------------------------- View menu
local ViewMenu = ui.Menu("Word wrap")
win.menu:add("&View").submenu = ViewMenu

-- ViewMenu => single onClick event
function ViewMenu:onClick(idx)
	ViewMenu.items[1].checked = not ViewMenu.items[1].checked
	Edit.wordwrap = ViewMenu.items[1].checked
end

----------------------------- Settings menu
local SettingsMenu = ui.Menu("Change font...")
win.menu:add("&Settings").submenu = SettingsMenu

-- SettingMenu => single onClick event
function SettingsMenu:onClick(idx)
	local font, size, style = ui.fontdialog(Edit)
	if font ~= nil then
		Edit.font = font
		Edit.fontsize = size
		Edit.fontstyle = style
	end
	Edit.modified = false
end

-- Realign Edit size when the Window is resized
Edit.align = "all"

-- Main Window onClose Event => call MenuItem FileExit onClick() method
function win:onClose()
	return FileExit:onClick()
end

-- Edit onChange event : updates status bar and EditMenu items
function Edit:onChange()
	update_status()
	EditMenu.items[1].enabled = Edit.canundo
	EditMenu.items[2].enabled = Edit.canredo
	EditMenu.items[3].enabled = #Edit.selection > 0
	EditMenu.items[4].enabled = #Edit.selection > 0
	EditMenu.items[6].enabled = sys.clipboard or false
end

-- Edit onCaret event : updates status bar
function Edit:onCaret()
	update_status()
end

Edit.onSelect = Edit.onChange

-- Edit on right click event : shows EditMenu as a popup menu
function Edit:onContext()
	win:popup(EditMenu)
end

------------------- Window keyboard shortcuts

-- Binds shortcut CTRL+o with FileOpen:onClick() method
win:shortcut("o", FileOpen.onClick, true)
-- Binds shortcut CTRL+s with FileSave:onClick() method
win:shortcut("s", FileSave.onClick, true)
-- Binds shortcut CTRL+n with FileNew:onClick() method
win:shortcut("n", FileNew.onClick, true)

-- update status bar and Edit menuitems
Edit:onChange()

-- shows the main Window and set focus to the Edit object
win:show()

---------------------------- Main loop
while win.visible do
	ui.update()
end