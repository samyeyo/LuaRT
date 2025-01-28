local ui = require("ui")
require "canvas"

local win = ui.Window("Whiteboard example")
local canvas = ui.Canvas(win)

canvas.align = "all"
canvas.cursor = "cross"

function canvas:onHover(x, y, button)
    self:begin()
    if button.left then
        if self.lastx then
            self:line(x, y, self.lastx, self.lasty, 0x8c1affff, 2)
        end
        self.lastx = x
        self.lasty = y
    else
        self.lastx = nil
        self.lasty = nil
    end
    self:flip()
end

ui.run(win):wait()