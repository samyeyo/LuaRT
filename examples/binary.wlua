--
--  luaRT binary.lua example
--  A binary file viewer
--  Author: Samir Tine
--

local ui = require "ui"

-- create a simple window 
local win = ui.Window("Binary file viewer sample", "fixed", 560, 460)
local edit = ui.Edit(win, "")
edit.align = "all"
edit.cursor = "arrow"

-- set edit widget properties
edit.font = "Lucida Console"
edit.fontsize = 10
edit.wordwrap = true
edit.richtext = false
edit.readonly = true
edit.bgcolor = 0x000030
edit.color = 0xAAAAAA

win:show()

function clean(buff)
  local result = ""
  for char in each(tostring(buff)) do
    if char:byte(1) < 16 then
      result = result.."."
    else
      result = result..char
    end
  end
  if #buff < 16 then
    result = string.rep("   ", 16-#buff)..result
  end
  return result
end

-- Read data task by 16 bytes chunks
local function readdata(file)
  while true do
    local data = file:read(16)
    if #data == 0 then
      break
    end
    edit:append(string.gsub(data:encode("hex"), "%w%w", function (byte) return byte.." " end).."  "..clean(data).."\n")
    coroutine.yield()
  end
end

local file = ui.opendialog("Select a file to view its binary content", false, "All files (*.*)|*.*")
file:open("read", "binary")
 
-- use a coroutine to keep loading the file while updating the ui (for big files)
local task = coroutine.create(readdata)

-- update user interface
repeat
	ui.update()
  coroutine.resume(task, file)
until not win.visible