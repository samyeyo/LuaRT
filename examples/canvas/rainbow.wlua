local ui = require "ui"
require "canvas"

-- create a simple Window
local win = ui.Window("Canvas Rainbow sample", "fixed", 500, 400)

-- create a Canvas
local canvas = ui.Canvas(win)
canvas.align = "all"

-- create a linear gradient
local gradient = canvas:LinearGradient { [0] = 0xE30940FF, [0.33] = 0xE7D702FF, [0.66] = 0x0FA895FF, [1] = 0x1373E8FF }
gradient.start = { 0, 0 }
gradient.stop = { canvas.width, 0 }

local pos = 0
local dir = 4

function canvas:onPaint()
  gradient.start = {pos, 0}
  self:fillrect(0, 0, canvas.width, canvas.height, gradient)
  if pos > canvas.width-200 then
    dir = -4
  elseif pos < 30 then
    dir = 4
  end
  pos = pos + dir
end

-- update user interface
ui.run(win):wait()