local ui = require "ui"

local Window = Object(ui.Window)

Window.inspector = {}

local transparency = ui.Window.get_transparency

function Window.inspector.transparency(panel, obj, property)
    local widget = ui.Entry(panel, "", 100, panel.pos-3, 100, 23)
    widget.onSelect = function (self)
        if string.match(widget.text, "%d") ~= nil then
            formWindow._transparency = self.text
        else 
            sys.beep()
            widget.text = formWindow._transparency
        end
    end
    widget.update = function(tracked)
        widget.text = formWindow._transparency
    end
    return widget
end

function Window:get_transparency()
    return self._transparency
end

function Window:set_transparency(transparency)
    self._transparency = transparency
end

function Window:transparency2str()
    return nil
end

local function Window_style(panel, obj, property)
    local widget = ui.Combobox(panel, false, {"dialog", "single", "fixed", "float", "raw"}, 100, panel.pos-3)
    widget.onSelect = function (self, item)
        formWindow._style = item.text
    end
    widget.update = function(tracked)
        widget.selected = widget.items[formWindow._style]
    end
    return widget
end

function Window:get_style()
    return self._style
end

function Window:set_style(style)
    self._style = style
end

function Window:style2str()
    return nil
end

return inspector:register(Window, {
    style = Window_style,
    fullscreen = inspector.properties.boolean,
    topmost = inspector.properties.boolean,
}, {
    traytooltip = true,
    menu = true
})