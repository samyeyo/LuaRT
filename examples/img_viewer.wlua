--
--  luaRT img_viewer.lua example
--  An image viewer
--  Author: Samir Tine
--

local ui = require "ui"

local win = ui.Window("Image viewer", "raw", 512, 380)
local img = ui.Picture(win, "")

local dir = ui.dirdialog("Select a directory to view images")

win:center()
win:show()

local function display_img()
  for f in each(dir) do
    if is(f, sys.File) and string.search(".png .jpg .jpeg .gif .tiff", f.extension or "") then
      local time = sys.clock()
      img:load(f)
      win.width = img.width
      win.height = img.height
      img:center()
      win:center()
      while sys.clock()-time < 2000 do
        coroutine.yield()
      end
    end
  end
end

local task = coroutine.create(display_img)

repeat
    ui.update()
    local result, err = coroutine.resume(task)
    if result == false then
      ui.error(err)
    end
until not win.visible or coroutine.status(task) == "dead"
