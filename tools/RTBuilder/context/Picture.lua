local ui = require "ui"

local menu = ui.Menu()
local Widget


menu:add("Load image...").onClick = function(self)
    local file = ui.savedialog("Save picture as...", false, "All files (*.*)|*.*|PNG image files (*.png)|*.png|JPEG image files (*.jpg)|*.jpg|Bitmap image files (*.bmp)|*.bmp|GIF image files (*.gif)|*.gif|ICO image files (*.ico)|*.ico|TIFF image files (*.tiff)|*.tiff")
    if file ~= nil then
        tracker:stop()
        Widget.image = file.fullpath:gsub("\\", "/")
        tracker:start(Widget)
    end
end

menu:add("Restore image size").onClick = function(self)
    if Widget.oldWidth then
        tracker:stop()
        Widget.width = Widget.oldWidth
        Widget.height = Widget.oldHeight
        tracker:start(Widget)
    end
end

menu:add("Center").onClick = function(self)
    tracker:stop()
    Widget:center()
    tracker:start(Widget)
end

return function(self)
    Widget = self
    menu.items[2].enabled = self.oldWidth
    return menu
end