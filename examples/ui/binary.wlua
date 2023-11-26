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
edit.fgcolor = 0xAAAAAA

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

local file = ui.opendialog("Select a file to view its binary content", false, "All files (*.*)|*.*")
file:open("read", "binary")

-- use a Task to keep loading the file while updating the ui (for big files)
async(function ()
  while true do
    for i = 1, 64 do
      local data = file:read(16)
      if #data == 0 then
        return
      end
      edit:append(string.gsub(data:encode("hex"), "%w%w", function (byte) return byte.." " end).."  "..clean(data).."\n")
    end
    sleep()
  end  
end)

-- update user interface Task concurrently
ui.run(win):wait()