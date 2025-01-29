local ui = require "ui"
require "tracker"
require "dump"


local VERSION = require "version"

Widgets = {}
onContext = {}

filename = nil
modified = false
formWindow = nil
mainWindow = ui.Window("RTBuilder "..VERSION, "single", 800, 110)
widgetsTab = ui.Tab(mainWindow, {}, 6, 6, 790, 80)
mainWindow.menu = ui.Menu()

package.path = package.path..sys.currentdir..'/?/init.lua;'
package.cpath = package.cpath..sys.currentdir..'/widgets/?.dll;'

for entry in sys.Directory(sys.currentdir.."/context"):list("*.lua") do
    onContext[entry.name:gsub("%.lua", "")] = loadfile(entry.fullpath)()
end

function Tracker(widget)
    local t = ui.Tracker(widget)
    t.onContext = function ()
        local func = onContext[t.widget and t.widget.type or "Window"] or false
        if func then
            formWindow:popup(func(t.widget))
        end
    end
    t.onDelete = function(self)
        local w = tracker.widget
        self:stop()
        formWindow:onClick()
        Widgets[w.name] = nil
        ui.remove(w)
        modified = true
    end
    return t 
end

function CloseWindow(save)
    local save = (save == nil and true) or false
    if formWindow ~= nil then
        formWindow.tracker:stop()
        ui.remove(formWindow.tracker)
        formWindow.tracker = nil
        if save then
           return saveWindow()
        end
        formWindow.visible = false
		ui.remove(formWindow)
		inspector:hide()
        formWindow = nil
        filename = nil
        mainWindow:UpdateTitle()
        mainWindow:show()
    end
end

function onClose(self)
    if self ~= nil then
        if not filename then
            if modified then
                result = ui.confirm("The current Window is about to be closed.\nDo you want to save before continuing ?")
                if result == "cancel" then
                    return false
                elseif result == "yes" then
                    saveWindow()  
                end 
            end
        elseif modified then
            saveWindow()
        end
        CloseWindow(false)
    end
    return true
end    

function mainWindow:onClose()
   return onClose(formWindow)
end

function mainWindow:UpdateTitle()
    mainWindow.title = filename == nil and "RTBuilder "..VERSION or "RTBuilder "..VERSION.." - "..sys.File(filename).name
end

function init_formwindow(win)
    win.onClick = function(self) if tracker.widget then tracker:stop(); end inspector.Update(self) end
    win.onMove = function(self) inspector:onTrack(self) end
    win.onClose = onClose
    win.onContext = function (self)
        local func = onContext.Window or false
        if func then
            win:popup(func(self))
        end
    end
    win.onMinimize = function(self) mainWindow:minimize() end  
    win.onRestore = function(self) mainWindow:restore() end  
    set_shortcuts(win)
    win.tracker = Tracker(win)
    tracker = win.tracker
end

----------------------------- Window menu
local WindowMenu = ui.Menu()
mainWindow.menu:add("&Window").submenu = WindowMenu

-- Window => New 
local WindowNew = WindowMenu:add("&New\tCtrl+N")

function WindowNew:onClick()
    if formWindow ~= nil then
        if not formWindow:onClose() then
            return
        end
    end
    if formWindow == nil then
        inspector.Update("Window")
        formWindow = inspector.registry.Window("Window")
        Widgets = {}
        formWindow._style = "dialog"
        formWindow._transparency = 255
        formWindow.name = "Window"
        formWindow:center()
        formWindow.y = mainWindow.y + mainWindow.height + 70
        formWindow.height = 400
        formWindow:show()           
        init_formwindow(formWindow)   
        mainWindow:tofront()
        inspector.Update(formWindow)
    end
end

----------------------------- Settings menu
local SettingsMenu = ui.Menu()
mainWindow.menu:add("&Settings").submenu = SettingsMenu

-- Settings => Change theme 
local ThemeChange = SettingsMenu:add("Switch to "..(ui.theme == "light" and "dark" or "light").." &theme")

function ThemeChange:onClick()
	local old = ui.theme
	local theme =  old == "light" and "dark" or "light"
	ui.theme = theme
	self.text = "Switch to "..old.." theme"
    if formWindow then
        tracker:stop()
        formWindow:onClick()
    end
end

local function do_openfile(f)
    if formWindow ~= nil then
        if not formWindow:onClose() then
            return
        end
    end  
    sleep()
    local result = loadWindow(f)
    if result then
        filename = result
        mainWindow:UpdateTitle(filename)
        formWindow.tracker = Tracker(formWindow)
        set_shortcuts(formWindow)
        formWindow.onClick = function(self) tracker:stop(); inspector.Update(self) end
        formWindow.onMove = function(self) inspector.Update(self) end
        formWindow.onClose = onClose
        tracker = formWindow.tracker
        formWindow._transparency = formWindow.transparency
        formWindow.transprency = 254
        formWindow:show()        
        mainWindow:tofront()
        inspector.Update(formWindow)       
    end
end

-- Window => Open 
local WindowOpen = WindowMenu:add("&Open...\tCtrl+O")
function WindowOpen:onClick()
    do_openfile()
end

-- Window => Save
local WindowSave = WindowMenu:add("&Save\tCtrl+S")
function WindowSave:onClick()
	saveWindow()
    mainWindow:UpdateTitle()
end

-- Window => Save as
local WindowSaveAs = WindowMenu:add("S&ave as...")
function WindowSaveAs:onClick()
    local fname = filename
    filename = nil
    if saveWindow() then
        mainWindow:UpdateTitle()
    else
        filename = fname
    end
end

-- Window => Close
local WindowClose = WindowMenu:add("&Close")
function WindowClose:onClick()
    if formWindow ~= nil then
        onClose(formWindow)
    end
end

-- Window => Exit
local WindowExit = WindowMenu:add("E&xit")
function WindowExit:onClick()
    if formWindow ~= nil then
        formWindow:onClose()
        if formWindow ~= nil then
            return
        end
    end
	mainWindow:hide()
end

function mainWindow:onMinimize()
    widgetsTab.width = mainWindow.width-10
    if formWindow then
        formWindow:minimize()
    end
end

function mainWindow:onRestore()
    widgetsTab.width = mainWindow.width-10
    if formWindow then
        formWindow:restore()
    end
end

------------------- Keyboard shortcuts
set_shortcuts =  function(win)
    win:shortcut("n", WindowNew.onClick, true)
    win:shortcut("o", WindowOpen.onClick, true)
    win:shortcut("s", WindowSave.onClick, true)
end

------------------- Main window default position
mainWindow:center()
mainWindow.y = 60
set_shortcuts(mainWindow)

inspector = require "inspector"
set_shortcuts(inspector)
require "widgets"

mainWindow:show()

if #arg > 0 then
    local openfile = sys.File(arg[1])
    if openfile.exists then
        do_openfile(openfile)
    end
end

if formWindow == nil then
    ------------------- Create a new empty Window
    WindowNew:onClick()
end

-- Task to update menu items state
async(function()
    while true do
        WindowClose.enabled = formWindow
        WindowSaveAs.enabled = formWindow
        WindowSave.enabled = formWindow
        sleep(250)
    end
end)

ui.run(mainWindow):wait()