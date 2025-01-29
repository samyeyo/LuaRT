local ui = require "ui"

local menu = ui.Menu()
local Widget

menu:add("Set Window icon...").onClick = function(self)
    local win = ui.Window("Set Window icon...", "fixed", 300, 200)
    win:center()
    local gb = ui.Groupbox(win, "", 10, 4, 160, 188)
    local icon = ui.Picture(gb, Widget.icon or "", 0, 0, 16, 16)
    icon:center()
    
    local icofile
    
    local okbtn = ui.Button(win, "Ok", 184, 36, 110, 26)
    okbtn.enabled = false
    
    function okbtn:onClick(self)
        Widget:loadicon(icofile)
        Widget.icon = icofile
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
            icon:show()
            delbtn:show()
        end
    end    
    formWindow:showmodal(win)
end

menu:add("Show status bar").onClick = function(item)
    Widget._status = not Widget._status
    if Widget._status then
        item.text = "Hide status bar"
        Widget:status("")
    else
        item.text = "Show status bar"
        Widget:status()
    end
end

return function(self)
    Widget = self
    return menu
end