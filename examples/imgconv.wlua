local ui = require "ui"

local win = ui.Window("Picture conversion example", 512, 380)
win:status("")

local img = ui.Picture(win, "")
img:load("LuaRT.png")
img:center()

local button = ui.Button(win, "Save picture to...")
button:center()
button.y = img.y + img.height + 30

function button:onClick()
    local file = ui.savedialog("Save picture as...", false, "All files (*.*)|*.*|PNG image files (*.png)|*.png|JPEG image files (*.jpg)|*.jpg|Bitmap image files (*.bmp)|*.bmp|GIF image files (*.gif)|*.gif|ICO image files (*.ico)|*.ico|TIFF image files (*.tiff)|*.tiff")
    if file ~= nil and (not file.exists or (file.exists and ui.confirm(file.fullpath.." already exists. Continue and overwrite its content ?") == "yes")) then
        if img:save(file) then
            win:status("Picture saved as "..file.fullpath)
        end
    end
end

win:center()

win:show()


repeat
    ui.update()
until win.visible == false
