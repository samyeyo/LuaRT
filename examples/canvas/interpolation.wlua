local ui = require "ui"
require "canvas"

local win = ui.Window("Canvas - Image interpolation example", "fixed", 480, 360)

local c = ui.Canvas(win, 0, 0, 0, 300)
c.align = "top"

local cb = ui.Combobox(win, false, {"nearest", "linear"})
cb:center()
cb.selected = cb.items[1]
cb.y = 326

local lbl = ui.Label(win, "Image interpolation mode :")
lbl:center()
lbl.y = 308

local scale = 0.5
local delta = 0.001
local img = c:Image(sys.File(arg[0]).path.."pixelart.png")

-- Scale the Canvas content and draw the image
function c:onPaint()
	self:begin()
	self:clear()
	img:draw((c.width-img.width*scale)/2, (c.height-img.height*scale)/2, scale, 1, cb.text)
	scale = scale + delta
	if scale < 0.5 or scale > 2 then
		delta = -delta
	end
	self:flip()
end

ui.run(win):wait()

