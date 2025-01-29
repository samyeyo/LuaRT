local ui = require "ui"

local menu = ui.Menu()
local Widget

-- create a menu item 
menu:add("Set icon...").onClick = function(self)
    local win = ui.Window("Load icon...", "fixed", 300, 200)
    win:center()
    local gb = ui.Groupbox(win, "", 10, 4, 160, 188)
    local icon = ui.Picture(gb, Widget.icon or "", 0, 0, 16, 16)
    icon:center()
    
    local icofile
    
    local okbtn = ui.Button(win, "Ok", 184, 36, 110, 26)
    okbtn.enabled = false
    
    function okbtn:onClick(self)
        tracker:stop()
        tracker:start(Widget)
        Widget.icon = icofile
        Widget:loadicon(icofile)
        win:hide()
    end

    local delbtn = ui.Button(gb, "Remove icon", 24, 154, 110, 26)
    
    function delbtn:onClick()
        icon:hide()
        self:hide()
        okbtn.enabled = true
        icofile = nil
    end

    delbtn.visible = Widget.icon
    
    ui.Button(win, "Cancel", 184, 158, 110, 26).onClick = function(self)
        win:hide()
    end
    
    ui.Button(win, "Select icon...", 184, 10, 110, 26).onClick = function(self)
        local file = ui.opendialog("Select icon...", false, "ICO files (*.ico)|*.ico")
        if file then
            icofile = file.fullpath
            okbtn.enabled = true
            icon:load(icofile, 16, 16)
            icon:center()
            delbtn:show()
        end
    end    
    formWindow:showmodal(win)
end

menu:add("Autosize").onClick = function(self)
    tracker:stop()
    Widget.align = nil
    Widget:autosize()
    tracker:start(Widget)
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