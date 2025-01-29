local ui = require "ui"

local menu = ui.Menu()
local Widget

-- create a menu item 
menu:add("Edit lines...").onClick = function(self)
    local win = ui.Window("Lines editor", "fixed", 400, 260)
    win:center()
    local edit = ui.Edit(win, "")
    edit.rtf = Widget.rtf
    if edit.rtf then
        edit.richtext = Widget.richtext
    else
        edit.font = Widget.font
        edit.fontsize = Widget.fontsize
        edit.fontstyle = Widget.fontstyle
        edit.text = edit.text
    end
    edit.wordwrap = Widget.wordwrap
    edit.align = "top"
    edit.height = 230
    
    ui.Button(win, "Ok", 220, 232, 80).onClick = function(self)
        if edit.rtf then
            Widget.richtext = edit.richtext
        else
            Widget.text = edit.text
        end
        win:hide()
    end
    
    ui.Button(win, "Cancel", 310, 232, 80).onClick = function(self)
        win:hide()
    end
    
    ui.Button(win, "Load from file...", 10, 232, 120).onClick = function(self)
        local file = ui.opendialog("Load lines from file...", false, "All files (*.*)|*.*|Text files (*.txt)|*.txt")
        if file then
            if not edit:load(file) then
                edit.rtf = not edit.rtf
                edit:load(file)
            end
        end
    end    
    formWindow:showmodal(win)
end

menu:add("Center").onClick = function(self)
    tracker:stop()
    Widget:center()
    tracker:start(Widget)
end

return function(self)
    Widget = self
    return menu
end