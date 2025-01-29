local ui = require "ui"

local menu = ui.Menu()
local Widget


menu:add("Select today").onClick = function(self)
    Widget.selected = sys.Datetime()
    inspector.panels.Calendar.widgets.selected.update(Widget)
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