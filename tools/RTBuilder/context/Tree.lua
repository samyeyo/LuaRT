local ui = require "ui"

local menu = ui.Menu()
local Widget, tree

-- create a menu item 
menu:add("Edit Tree items...").onClick = function(self)
    local function getcurindex(item)
        local item = item or tree.selected
        local count = 0
        while type(item) ~= "Tree" do
            count = count + 1
            if item.previous == nil then
                item = item.parent
            else 
                item = item.previous
                count = count + item.count
            end
        end
        return count
    end
    
    local function assign(from, to)
        for item in each(from.items or from.subitems) do
            local ico = Widget.icons[item.index]
            local new = to:add(item.text)
            new:loadicon(ico)
            if item.count > 0 then
                assign(item, new)          
            end
        end
    end
    
    local win = ui.Window("Tree items editor", "fixed", 490, 300)
    Widget.icons = Widget.icons or {}
    local oldicons = Widget.icons
    win:center()
    tree = ui.Tree(win, {}, 10, 10, 260, 250)
    assign(Widget, tree)
    if tree.count > 0 then
        tree.selected = tree.items[1]
    end
    
    tree.style = "icons"
    
    ui.Button(win, "Add root item...", 74, 268, 120).onClick = function(self)
        local new = tree:add("New item")
        tree.selected = new
        new:show()
    end
    ui.Button(win, "OK", 300, 268, 80).onClick = function(self)
        Widget:clear()
        assign(tree, Widget)
        if next(Widget.icons) then
            Widget.style = "icons"
        end
        inspector.Update(Widget)
        win:hide()
    end
    ui.Button(win, "Cancel", 390, 268, 80).onClick = function(self)
        if ui.confirm("All modifications made to the Tree will not be saved.\nAre you sure to continue ?") == "yes" then
            Widget.icons = oldicons
            win:hide()
        end
    end
    
    local gb = ui.Groupbox(win, "", 280, 4, 200, 180)
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
    
    function entry:onChange()
        local sel = tree.selected
        sel.text = self.text
    end
    
    ui.Button(gb, "Delete Item", 60, 148, 80).onClick = function(self)
        local sel = tree.selected
        local idx = sel.index
        local newicons = {}
        if idx <= tree.count then
            local newicons = {}
            Widget.icons[idx] = nil
            for k, v in pairs(Widget.icons) do
                if k >= idx then
                    newicons[k-1] = v
                else
                    newicons[k] = v
                end
            end
            Widget.icons = newicons
        end
        tree:remove(sel)
        if tree.count == 0 then
            gb:hide()
        end
    end
    ui.Button(gb, "Add new subitem", 46, label2.y+30, 110).onClick = function(self)    
        local new = tree.selected:add("New sub item")
        tree.selected = new.parent
        new:show()
        local idx = new.index
        if idx < tree.count then
            local newicons = {}
            for k, v in pairs(Widget.icons) do
                if k >= idx then
                    newicons[k+1] = v
                else
                    newicons[k] = v
                end
            end
            Widget.icons = newicons
        end
    end
    
    function win:onKey(key)
        if key == "VK_UP" then
            local prev = tree.selected.previous
            if type(prev) ~= "Tree" then
                if prev == nil then
                    tree.selected = tree.selected.parent
                elseif prev.count > 0 then
                    for item in each(prev.subitems) do
                        if item.next == nil then
                            tree.selected = item
                            return;
                        end
                    end
                    tree.selected = tree.selected.parent
                else
                    tree.selected = prev
                end
            end
        elseif key == "VK_DOWN" then
            local sel = tree.selected
            if sel.count > 0 then
                for item in each(sel.subitems) do
                    if item.prev == nil then
                        tree.selected = item
                        break;
                    end
                end
            else
                local next = sel.next or sel.parent.next
                if type(next) ~= "Tree" then
                    tree.selected = next or tree.selected
                end
            end
        end
    end
    
    function tree:onChange(item, action)
        if action == "edited" then
            entry.text = item.text
        end
    end
    
    function tree:onSelect(item)
        gb:show()
        entry.text = item.text
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
        local sel = tree.selected
        if sel and file then
            local idx = sel.index
            Widget.icons[idx] = file.fullpath:gsub("\\", "/")
            sel:loadicon(file)
            ico:load(file.fullpath)
            ico:show()
            icobtn:show()
            panel.border = false
        end
    end
    
    function icobtn:onClick()
        local sel = tree.selected
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

menu:add("Sort items in alphabetical order\xe2\x96\xb4").onClick = function(self)
    Widget:sort("ascend")
end

menu:add("Sort items in inverse order\xe2\x96\xbe").onClick = function(self)
    Widget:sort("descend")
end

menu:add("Clear all items").onClick = function(self)
    if ui.confirm("Are you sure to remove all Tree items ?") == "yes" then
        Widget:clear()
    end
end

return function(self)
    Widget = self
    return menu
end