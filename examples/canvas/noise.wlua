local ui = require "ui"
require "canvas"
require "console"

-- create a simple Canvas 
local win = ui.Window("Canvas.point sample", 320, 240)
local Canvas = ui.Canvas(win)
Canvas.align = "all"
Canvas.bgcolor = 0x000000FF

win:show()
  
function Canvas:onPaint()
  local width = Canvas.width
  local height = Canvas.height
  local pixels = {}
  for x = 0, width do
    for y = 0, height do
      local rgb = math.random(0xFF)
      pixels[#pixels+1] = string.pack("BBBB", rgb, rgb, rgb, 0xFF)
    end
  end
  Canvas:map(table.concat(pixels))
end

while win.visible do
  ui.update()
end