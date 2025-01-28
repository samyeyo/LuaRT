--
--  LuaRT word.lua example
--  MS Word automation using sys.COM object
--  Requirement : MS Word must be installed

local word = sys.COM("Word.Application")
local doc = word.Documents:Add()
local sel = word.Selection

sel.Paragraphs.Alignment = 1
sel.Font.Name = "Calibri"
sel.Font.Size = "72"
sel.Font.Color = 0x800000
sel:TypeText("Lua")
sel.Font.Color = 0x4589F0
sel:TypeText("RT")
doc:SaveAs(sys.currentdir.."\\test.doc")
word:Quit()

-- Open the document using MS Word
require("sysutils").shellexec("open", sys.currentdir.."\\test.doc")