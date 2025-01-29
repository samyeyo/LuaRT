local ui = require "ui"

local menu = ui.Menu()
local Widget

menu:add("Center").onClick = function(self)
    tracker:stop()
    Widget:center()
    tracker:start(Widget)
end

return function(self)
    Widget = self
    return menu
end