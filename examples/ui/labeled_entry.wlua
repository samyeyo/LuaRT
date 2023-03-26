local ui = require "ui"

-- create a new widget based on ui.Entry
local LabeledEntry = Object(ui.Entry)

-- create a constructor
function LabeledEntry:constructor(parent, text, x, y, width, height)
	self.label = ui.Label(parent, text, x, y)
  -- call the Entry constructor using self
	ui.Entry.constructor(self, parent, "", self.label.x + self.label.width + 6, self.label.y - 2, width or 120, height)
end

-- How to use this new custom widget
local win = ui.Window("LabeledEntry widget", "fixed", 300, 100)
local entry = LabeledEntry(win, "Enter your name :", 40, 30)

function entry:onSelect()
	ui.info("Welcome "..entry.text)
end

win:show()

repeat
	ui.update()
until not win.visible
