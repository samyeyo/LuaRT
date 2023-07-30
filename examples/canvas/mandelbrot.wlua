local ui = require "ui"
require "canvas"

local win = ui.Window("Canvas - Mandelbrot fractal example", "fixed", 512, 360)
local c = ui.Canvas(win)
c.align = "all"
c.bgcolor = 0x000000FF

win:center()

local max_iterations = 64

-- Uses internal timer for 30fps drawing
function c:onPaint()
  self:clear()	
  local row = 0
  local height = self.height
  local width = self.width
  
  while row < height do
    local col = 0

    while col < width do
      local c_re = (col - width / 2.0) * 4.0 / width
      local c_im = (row - height / 2.0) * 4.0 / width
      
      local x = 0
      local y = 0
      local iteration = 0

      while (x * x + y * y <= 4) and (iteration < max_iterations) do
        local x_new = x * x - y * y + c_re
        y = 2 * x * y + c_im
        x = x_new
        iteration = iteration + 1
      end

      if iteration < max_iterations then
        self:point(col, row, ((iteration*3 << 24) | (iteration*2 << 16) | (iteration << 8) | 255))
      else
        self:point(col, row, 0)
      end
      col = col + 1
    end
    row = row + 1
  end
end

ui.run(win)