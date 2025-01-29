local ui = require "ui"

local menu = ui.Menu()
local Widget

-- create a menu item 
menu:add("Edit Combobox items...").onClick = function(self)
    local win = ui.Window("Combobox items editor", "fixed", 400, 260)
    Widget.icons = Widget.icons or {}
    local oldicons = Widget.icons
    win:center()
    local olditems = {}
    local items = Widget.items
    for i=1, Widget.count do
        olditems[i] = items[i].text
    end
    local list = ui.List(win, olditems, 10, 10, 170, 210)
    for i = 1, Widget.count do
        if oldicons[i] then
            list.items[i]:loadicon(oldicons[i])
        end
    end
    if list.count > 0 then
        list.selected = list.items[1]
    end
    
    list.style = "icons"
    
    ui.Button(win, "Add item...", 56, 228, 80).onClick = function(self)
        list.selected = list:add("New item")
    end
    ui.Button(win, "OK", 206, 228, 80).onClick = function(self)
        Widget:clear()
        for i=1, list.count do
            Widget:add(list.items[i].text):loadicon(Widget.icons[i])
        end
        inspector.Update(Widget)
        win:hide()
    end
    ui.Button(win, "Cancel", 290, 228, 80).onClick = function(self)
        if ui.confirm("All modifications made to the Combobox will not be saved.\nAre you sure to continue ?") == "yes" then
            Widget.items = olditems
            Widget.icons = oldicons
            for i=1, Widget.count do
                Widget.items[i]:loadicon(oldicons[i])
            end
            win:hide()
        end
    end
    
    local gb = ui.Groupbox(win, "", 190, 4, 200, 180)
    gb:hide()
    local label = ui.Label(gb, "Item text:", 10, 22)
    local entry = ui.Entry(gb, "NewItem", label.x + label.width+6, label.y-4, 124)
    local label2 = ui.Label(gb, "Item icon:", 10, 62)
    local panel = ui.Panel(gb, label2.x + label2.width+6, label2.y, 16, 16)
    local ico = ui.Picture(panel, "setup/img/logo.ico")
    panel.cursor = "hand"
    ico.cursor = "hand"
    ico.align = "all"
    local icobtn = ui.Button(gb, "Remove icon", panel.x+30, label2.y-5, 90)
    local label3 = ui.Label(gb, "Item position :", 10, 100)
    local entry2 = ui.Entry(gb, "1", label3.x + label3.width+6, label3.y-3, 30)
    entry2.enabled = false

    function entry:onChange()
        local sel = list.selected
        sel.text = self.text
    end
    
    ui.Button(gb, "Delete Item", 60, 148, 80).onClick = function(self)
        local pos = tonumber(entry2.text)
        local sel = list.selected
        if sel then
            Widget.icons[pos] = nil
            for i=pos, list.count do
                Widget.icons[i] = Widget.icons[i+1] or nil
            end        
            list:remove(sel)
            if list.count > 0 then
                list.selected = list.items[pos-1]
            else
                gb:hide()
            end        
        end  
    end
    
    ui.Button(gb, "\xe2\x96\xb4", entry2.x + entry2.width + 2, entry2.y-1, 14, 12).onClick = function(self)
        local pos = tonumber(entry2.text)
        if pos > 1 then
            local new = Widget.icons[pos]
            local old = Widget.icons[pos-1]
            Widget.icons[pos] = old
            Widget.icons[pos-1] = new
            list.selected.index = pos-1
            list:onSelect(list.selected)
        end
    end
    ui.Button(gb, "\xe2\x96\xbe", entry2.x + entry2.width + 2, entry2.y+10, 14, 12).onClick = function(self)
        local pos = tonumber(entry2.text)
        if pos < list.count then
            local new = Widget.icons[pos]
            local old = Widget.icons[pos+1]
            Widget.icons[pos] = old
            Widget.icons[pos+1] = new
            list.selected.index = pos+1
            list:onSelect(list.selected)
        end
    end    

    function list:onSelect(item)
        gb:show()
        entry.text = item.text
        entry2.text = item.index
        if Widget.icons[item.index] == nil then
            ico:hide()
            panel.border = true
            icobtn:hide()
        else
            ico:show()
            panel.border = false
            icobtn:show()
            ico:load(Widget.icons[item.index])
        end
    end

    function ico:onClick()
        local file = ui.opendialog("Load icon...", false, "Icon files (*.ico)|*.ico|All files (*.*)|*.*")
        if file then
            local sel = list.selected
            sel:loadicon(file)
            Widget.icons[sel.index] = file.fullpath:gsub("\\", "/")
            ico:load(file.fullpath)
            ico:show()
            icobtn:show()
            Widget.style = "icons"
            panel.border = false
        end
    end

    function icobtn:onClick()
        local sel = list.selected
        sel:loadicon()
        ico:hide()
        panel.border = true
        self:hide()
        Widget.icons[sel.index] = nil
    end

    function win:onHide()
        ui.remove(win)
        return true
    end

    panel.onClick = ico.onClick

    formWindow:showmodal(win)
end

menu:add("Center").onClick = function(self)
    tracker:stop()
    Widget:center()
    tracker:start(Widget)
end

menu:add("Clear all items").onClick = function(self)
    if Widget.count and ui.confirm("Are you sure to remove all Combobox items ?") == "yes" then
        Widget:clear()
        Widget.icons = {}
        inspector.panels.Combobox.widgets.selected.update(Widget)
    end
end

return function(self)
    Widget = self
    return menu
end