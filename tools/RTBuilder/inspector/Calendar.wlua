local ui = require "ui"

local Calendar = Object(ui.Calendar)

Calendar.inspector = {}

function Calendar.inspector.selected(panel, obj, property)
    local widget = ui.Entry(panel, "", 100, panel.pos-3, 100, 23)
    widget.onSelect = function (self, item)
        local func = function()
            tracker.widget.selected = sys.Datetime(widget.text)
        end
        if not pcall(func) then
            ui.error("The date entered '"..widget.text.."' is invalid")
            widget.text = tracker.widget.selected.date
        end
    end
    widget.update = function(tracked)
        widget.text = tracked.selected.date
    end
    return widget
end

return inspector:register(Calendar)