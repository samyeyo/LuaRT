local ui = require "ui"

local win = ui.Window("Zoom example", 512, 380)
local img = ui.Picture(win, "")
local button = ui.Button(win, "Replay")
local factor = 0
win.bgcolor = 0xFFFFFF

function button:onClick()
    self:hide()
    factor = 0
end

function win:onResize()
    img:center()
    button:center()
    button.y = img.y+img.height+20
end

img:load(sys.File(arg[0]).path.."\\LuaRT.png")
win:center()
button:hide()
img:center()
win:show()

repeat
    if factor < 1 then 
        factor = factor + 0.02
        img:resize(factor)
        if factor >= 1 then
            win:onResize()
            button:show()
        end
    end
    sleep()
until win.visible == false
