local ui = require "ui"

local Progressbar = Object(ui.Progressbar)

Progressbar.inspector = {}

function Progressbar.inspector.range(panel, obj, property)
    local widget = {}
    widget.min = ui.Entry(panel, "", 100, panel.pos-3, 40, 23)
    ui.Label(panel, "-", 107+widget.min.width, panel.pos)
    widget.max = ui.Entry(panel, "", 120+widget.min.width, panel.pos-3, 40, 23)
    widget.onSelect = function (self)
        local func = function()
            tracker.widget.range = { tonumber(widget.min.text), tonumber(widget.max.text) }
        end
        if not pcall(func) then
            ui.error("The range entered '"..widget.min.text.." - "..widget.max.text.."' is invalid")
            widget.min.text = tracker.widget.range.min
            widget.max.text = tracker.widget.range.max
        end
    end
    widget.update = function(tracked)
        widget.min.text = tracked.range.min
        widget.max.text = tracked.range.max
    end
    widget.min.onSelect = widget.onSelect
    widget.max.onSelect = widget.onSelect
    return widget
end

function Progressbar:range2str()
    local range = self.range
    return "{"..range.min..", "..range.max.."}"
end

return inspector:register(Progressbar, { themed = inspector.properties.boolean})