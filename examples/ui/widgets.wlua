-- LuaRT example that shows widgets from ui module
local ui = require "ui"

-- create a window
local win = ui.Window("LuaRT ui widgets demo", 320, 200)
local label = ui.Label(win, "Select a widget : ", 40, 75)
local combobox = ui.Combobox(win, {"Button", "Label", "Entry", "Picture", "Edit", "Checkbox", "Radiobutton", "Groupbox", "Calendar", "Progressbar", "Panel", "List", "Combobox", "Tree", "Tab"}, label.x + label.width + 8, label.y - 4, 100)
local demo_win = ui.Window("Widget", "fixed", 480, 360)
demo_win.x = win.x + win.width + 6;
demo_win.y = win.y

function combobox:onSelect(item)
    if ui[item.text] ~= nil then
        ui.remove(demo_win.widget)
        if item.text == "Picture" then
            demo_win.widget = ui[item.text](demo_win, sys.File(arg[0]).path.."LuaRT.png")
        elseif item.text == "Progressbar" then
            demo_win.widget = ui[item.text](demo_win)
            demo_win.widget.position = 75
        elseif item.index > 11 then
            demo_win.widget = ui[item.text](demo_win, {"Item 1", "Item 2", "Item 3"})
        elseif item.index == 11 then
            demo_win.widget = ui[item.text](demo_win)
            demo_win.widget.border = true
            demo_win.widget.bgcolor = 0xFFFFFF
        else    
            demo_win.widget = ui[item.text](demo_win, item.text ~= "Calendar" and item.text or nil)
        end
        demo_win.widget.x = demo_win.width/2-demo_win.widget.width/2
        demo_win.widget.y = demo_win.height/2-demo_win.widget.height/2
    end
end

demo_win:show()

function demo_win:onClose()
    return not win.visible
end

ui.run(win):wait()