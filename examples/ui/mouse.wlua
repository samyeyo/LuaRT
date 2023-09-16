local ui = require("ui")
require "canvas"

local win = ui.Window("Mouse button down/up example",800,600)
local canvas = ui.Canvas(win)

canvas.align = "all"
canvas.cursor = "cross"

local draw = false
local px, py, lastx, lasty

function canvas:onPaint()
    if draw then
        self:line(lastx or px, lasty or py, px, py, 0x8c1affff, 3)
        lastx = px
        lasty = py

    end
end

function canvas:onHover(x, y)
    px = x
    py = y
end

function canvas:onMouseDown(button)
    draw = button == "left"
end

function canvas:onMouseUp(button, x, y)
    draw = button ~= "left"
    if not draw then
        lastx = nil
        lasty = nil
    end
end

ui.run(win):wait()