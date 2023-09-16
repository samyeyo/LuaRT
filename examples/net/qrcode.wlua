--
--  luaRT qrcode.lua example
--  Generate a QR code
--  Author: Onyx from https://community.luart.org
--

local net = require "net" 
local ui = require "ui"

local win = ui.Window("QR Code Generator", 490, 240)
local img = ui.Picture(win, "", 170, 50)
local label = ui.Label(win, "Enter the QR Code content :")
local entry = ui.Entry(win, "https://www.luart.org", label.x + label.width + 4, label.y - 4, 200, 22)
local button = ui.Button(win, "Generate QR code !", entry.x + entry.width + 6, entry.y - 1)

function button:onClick()
    -- Generate a QR Code with the free REST API https://goqr.me/, trademark of DENSO WAVE INCORPORATED
    local _, response = await(net.Http("https://api.qrserver.com"):download("/v1/create-qr-code/?size=150x150&data="..entry.text))
    if not response then
      error(net.error)
    end
    img:load(response.file)
    response.file:remove()
    ui.Button(win, "Save QR Code...", 190, 50+img.height+6).onClick = function (self)
      local f = ui.savedialog("Save QR Code...", false, "PNG image files (*.png)|*.png|All files (*.*)|*.*") or false
      if f then
        img:save(f)
      end
    end
end

ui.run(win):wait()