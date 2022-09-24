local ui = require "ui"

-- create a simple Window 
local win = ui.Window("Button.hastext sample", 320, 200)
local button = ui.Button(win, "I'm the button's text", 154, 80)


-- set hastext property to false to hide the button text
button.hastext = false

-- load an icon from shell32.dll
button:loadicon(sys.env.WINDIR.."/system32/shell32.dll", 28)

win:show()

-- button:onClick() event (won't be fired !)
function button:onClick()
	if ui.confirm("You are going to shutdown the computer !!") == "yes" then
		sys.halt("shutdown")
	end
end

-- update user interface
repeat
	ui.update()
until not win.visible