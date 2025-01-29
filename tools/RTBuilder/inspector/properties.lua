local ui = require "ui"

local properties = {}

local cursors_names = {}
local cursors_icons = {}
local i = 0

cursors_names[0] = "none"
for entry in each(sys.Directory(sys.currentdir.."/resources/cursors/ico/"):list("*.ico")) do
    i = i + 1
    cursors_names[i] = entry.name:gsub(entry.extension, "")
    cursors_icons[i] = entry.fullpath
end

local function autosize(tracked)
    if tracked.autosize ~= nil and tracked.align == nil then
        tracker:stop()
        tracked:autosize()
        tracker:start(tracked)
    end
end

function properties.items(panel, obj, property)
    local widget = ui.Button(panel, "Edit items...", 99, panel.pos-3, 103, 26)
    widget.onClick = function(self)
       onContext[tracker.widget.type](tracker.widget).items[1].onClick()
    end
    panel.pos = panel.pos + 1
    return widget
end

function properties.selected(panel, obj, property)
    local widget = ui.Combobox(panel, false, {}, 100, panel.pos-3)
    widget.style = "icons"

    widget.onSelect = function (self, item)
        local tracked = tracker.widget
        if tracked then
            tracked.selected = item.text ~= "nil" and tracked.items[item.index] or nil
        end
    end

    widget.update = function(tracked)
        local sel = tracked.selected
        widget:clear()
        widget.enabled = tracked.count > 0
        if widget.enabled then
            for i=1,tracked.count do
                local item = tracked.items[i].text
                if tracked.icons then
                    widget:add(item):loadicon(tracked.icons[i])
                end
            end
            widget:add("nil")
            widget.selected = widget.items[sel and sel.index or "nil"]
        end
    end
    return widget
end

function properties.cursor(panel, obj, property)
    local widget = ui.Combobox(panel, false, cursors_names, 100, panel.pos-3)
    widget.style = "icons"
    for i = 1, widget.count do
        widget.items[i]:loadicon(cursors_icons[i])
    end

    widget.onSelect = function (self, item)
        local tracked = tracker.widget or formWindow
        tracked.cursor = item.text
    end
    widget.update = function(tracked)
        widget.selected = widget.items[tracked.cursor]
    end    
    return widget
end

function properties.bgcolor(panel, obj, property)
    local widget = ui.Entry(panel, 0xF0F0F0, 120, panel.pos-2, 80)
    widget.icon = ui.Panel(panel, 100, panel.pos+2, 16, 16)
    widget.icon.border = true
    widget.textalign = "center"
    widget.icon.cursor = "hand"
    widget.update = function(tracked)
        local color = string.format("0x%.6X", tracked[property])
        widget.icon.bgcolor = color
        widget.text = color
    end
    widget.onSelect = function(self)
        if not tonumber(widget.text) then
            sys.beep()
            self.update(widget.icon)
        else
            local tracked = tracker.widget or formWindow
            tracked[property] = widget.text
            self.update(tracked)
        end
    end
    widget.icon.onClick = function (self)
        local tracked = tracker.widget or formWindow       
        local lastvalue = tracked[property]
        local color = ui.colordialog(lastvalue)
        if color and color ~= lastvalue then
           tracked[property] = color
           widget.update(tracked, color)
        end
    end
    return widget
end

function properties.fgcolor(panel, obj, property)
    return properties.bgcolor(panel, obj, property)
end

function properties.font(panel, obj, property)
    local widget = ui.Button(panel, "", 99, panel.pos-3)
    widget.textalign = "left"

    widget.update = function(tracked)
        local tracked = tracked or tracker.widget
        tracker:stop()
        widget.text = tracked.font..", "..tracked.fontsize
        widget.fontstyle = tracked.fontstyle
        widget.width = 102
        if tracked ~= formWindow then
            tracker:start(tracked)
        end
    end

    widget.onClick = function(self)
        local tracked = tracker.widget or formWindow
        local result = {ui.fontdialog(tracked, tracked.fgcolor or nil)}
        if #result > 0 then
            if #result[1] > 0 and result[1] then
                tracked.font = result[1]
            end
            tracked.fontsize = result[2]
            tracked.fontstyle = result[3]
            widget.update(tracked)
        end
    end
    panel.pos = panel.pos + 2
    return widget
end

function properties.textalign(panel, obj, property)
    local widget = ui.Combobox(panel, false, {'left', 'right', 'center'}, 100, panel.pos-3)    
    widget.onSelect = function (self, item)
        tracker.widget.textalign = item.text
    end
    widget.update = function(tracked)
        widget.selected = widget.items[tracked.textalign]
    end
    return widget
end

function properties.align(panel, obj, property)
    local widget = ui.Combobox(panel, false, {"nil", 'all', 'bottom', 'top', 'right', 'left'}, 100, panel.pos-3)
    widget.onSelect = function (self, item)
        local tracked = tracker.widget or formWindow
        tracker:stop()
        tracked.align = item.text ~= "nil" and item.text or nil
        if tracked ~= formWindow then
            tracker.align = tracked.align
            tracker:start(tracked)
        end
    end
    widget.update = function(tracked)
        local align = tostring(tracked.align)
        local sel = widget.selected
        if (not sel) or sel.text ~= align then
            widget.selected = widget.items[align]
        end
    end    
    return widget
end

function properties.boolean(panel, obj, property)
    local widget = ui.Combobox(panel, false, {"true", "false"}, 100, panel.pos-3)
    widget.onSelect = function (self, item)
        local tracked = tracker.widget or formWindow
        local prop = property
        if property == "enabled" or property == "visible" then
            prop = "_"..property
        end
        tracked[prop] = item.text == "true"
        if property == "hastext" then
            if tracked.hastext == true and tracked.text == "" then
                tracked.text = tracked.name
                autosize(tracked)
            elseif item.text == "false" then
                autosize(tracked)
            end
        end
    end
    widget.update = function(tracked)
        widget.selected = widget.items[tostring(tracked[property])]
    end
    return widget
end

function properties.entry(panel, obj, property)
    local widget = ui.Entry(panel, "test", 100, panel.pos-3, 100, 23)

    widget.onSelect = function (self)
        local tracked = tracker.widget or formWindow
        local lastvalue = tracked[property]
        if lastvalue ~= nil then
            if property == "name" then
                if lastvalue ~= widget.text and Widgets[widget.text] then
                    ui.error("A Widget named '"..widget.text.."' already exists.")
                    widget.text = lastvalue
                    tracked[property] = lastvalue
                else
                    panel.title = widget.text.." properties"
                    tracked.name = widget.text
                    Widgets[widget.text] = tracked
                    Widgets[lastvalue] = nil
                end
            else
                local func = function() 
                    if tracked.type ~= "Window" then
                        tracker:stop()
                        tracked[property] = widget.text;
                        tracker:start(tracked)
                    else
                        tracked[property] = widget.text;
                    end
                    ui.update()
                end
                if not pcall(func) then
                    sys.beep()
                    tracked[property] = lastvalue
                    widget.text = lastvalue
                end
            end
        end
    end
    widget.update = function(tracked)
        widget.text = tostring(tracked[property])
    end
    return widget
end

properties.visible =  properties.boolean
properties.enabled =  properties.boolean
properties.allowdrop =  properties.boolean

function properties:register(property, func)
    if type(func) ~= "function" then
        error("Error trying to register property '"..property.."' with '"..type(func).."' (expecting function)")
    end
    properties[property] = func
end

return properties