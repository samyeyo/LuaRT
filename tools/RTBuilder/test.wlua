local ui = require "ui"
local Window = ui.Window(rtbuilder, "Window", rtbuilder and "dialog" or "dialog", 640, 400)
Window.name = 'Window'
Window.type = 'Window'
Window:loadicon()
Window.icon = nil
Window.x = "446"
Window.traytooltip = ''
Window.topmost = false
Window.allowdrop = false
Window.cursor = 'arrow'
Window.fontstyle = { ['thin'] = true,['underline'] = false,['italic'] = false,['heavy'] = false,['strike'] = false,['bold'] = false }
Window.fontsize = "9"
Window.font = 'Segoe UI'
Window.title = 'Window'
Window.width = "640"
Window.transparency = "255"
Window.bgcolor = "15790320"
Window.align = nil
Window.fullscreen = false
Window.y = "237"
Window.menu = nil
Window.height = "400"
Window.enabled = true
Window.style = 'dialog'

local Tab1 = ui.Tab(Window, {}, 48, 48, 300, 200)
Tab1.items = {[1] = 'New item',[2] = 'New item' }
Tab1.name = 'Tab1'
Tab1.type = 'Tab'
Tab1.icons = { }
Tab1.x = "48"
Tab1.fontsize = "9"
Tab1.allowdrop = false
Tab1.cursor = 'arrow'
Tab1.style = 'text'
Tab1.width = "300"
Tab1.tooltip = ''
Tab1.align = nil
Tab1.fontstyle = { ['thin'] = true,['underline'] = false,['italic'] = false,['heavy'] = false,['strike'] = false,['bold'] = false }
Tab1.font = 'Segoe UI'
Tab1.y = "48"
Tab1.visible = true
Tab1.selected = Tab1.items[2]
Tab1.enabled = true
Tab1.height = "200"

local Groupbox1 = ui.Groupbox(Tab1.items[1], [[Groupbox1]], 69, 26, 160, 121)
Groupbox1.name = 'Groupbox1'
Groupbox1.type = 'Groupbox'
Groupbox1.x = "69"
Groupbox1.fontsize = "9"
Groupbox1.fontstyle = { ['thin'] = true,['underline'] = false,['italic'] = false,['heavy'] = false,['strike'] = false,['bold'] = false }
Groupbox1.cursor = 'arrow'
Groupbox1.width = "160"
Groupbox1.align = nil
Groupbox1.tooltip = ''
Groupbox1.y = "26"
Groupbox1.visible = true
Groupbox1.height = "121"
Groupbox1.text = [[Groupbox1]]
Groupbox1.enabled = true
Groupbox1.allowdrop = false
Groupbox1.font = 'Segoe UI'

local Checkbox1 = ui.Checkbox(Groupbox1, [[Checkbox1]], 36, 44, 78, 20)
Checkbox1.name = 'Checkbox1'
Checkbox1.type = 'Checkbox'
Checkbox1.x = "36"
Checkbox1.fontsize = "9"
Checkbox1.cursor = 'arrow'
Checkbox1.width = "78"
Checkbox1.align = nil
Checkbox1.tooltip = ''
Checkbox1.font = 'Segoe UI'
Checkbox1.checked = false
Checkbox1.y = "44"
Checkbox1.fontstyle = { ['thin'] = true,['underline'] = false,['italic'] = false,['heavy'] = false,['strike'] = false,['bold'] = false }
Checkbox1.visible = true
Checkbox1.height = "20"
Checkbox1.text = [[Checkbox1]]
Checkbox1.enabled = true
Checkbox1.allowdrop = false
return Window, { ['Tab1'] = Tab1, ['Checkbox1'] = Checkbox1, ['Groupbox1'] = Groupbox1, }
