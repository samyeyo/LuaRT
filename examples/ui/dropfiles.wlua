--
-- LuaRT drag and drop example 
--

local ui = require "ui"

local win = ui.Window("Drop files and folders", 300, 200)

-- create a List to receive the dropped files/directories
local filelist = ui.List(win, {})
filelist.allowdrop = true
filelist.align = "all"
filelist.style = "icons"
filelist.border = false

-- onDrop event thrown on a drop operation
function filelist:onDrop(kind, content)
    if kind == "files" then
        -- for each file/directory add a ListItem and sets its icon
        for entry in each(content) do
            filelist:add(entry.name):loadicon(entry.fullpath)
        end
    end
end

ui.run(win):wait()