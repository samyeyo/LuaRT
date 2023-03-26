--
--  luaRT syntax.lua example
--  A syntax higlighting Edit widget
--  Author: Samir Tine
--

local ui = require "ui"

local SyntaxEdit = Object(ui.Edit)


function SyntaxEdit:constructor(...)
  local keyword_color = 0x1570CB
  local keywords = {  ["and"] = true, ["break"] = true, ["do"] = true, ["else"] = true, ["elseif"] = true,
                      ["end"] = true, ["for"] = true, ["function"] = true, ["goto"] = true, ["if"] = true, ["in"] = true, 
                      ["local"] = true, ["not"] = true, ["or"] = true, ["repeat"] = true, ["return"] = true, ["then"] = true,
                      ["until"] = true, ["while"] = true }
                    
  -- keywords property => Get/set a table with keywords
  function self:get_keywords()
    return keywords
  end
  
  function self:set_keywords(keywords_table)
    keywords = keywords_table
  end
  
  -- kwcolor property => Get/set the keywords color
  function self:get_kwcolor()
    return keyword_color
  end
  
  function self:set_kwcolor(c)
    keyword_color = c
  end
  
  -- onChange event : parse and recolorize the keywords in the text 
  function SyntaxEdit:onChange()
    self.selection.visible = false
    local from = self.selection.from
    local to = self.selection.to
    
    self.selection.from = 1
    self.selection.to = 0
    self.selection.color = 0x000000
    
    local start = 1
    local text = self.text:gsub("\r\n", "\n")
    while start < string.len(text) do
      local s, e = text:find("%w+", start)
      if s ~= nil then
        local word = text:sub(s, e)
        start = e+1
        if self.keywords[word] ~= nil then
          self.selection.from = s
          self.selection.to = start
          self.selection.color = keyword_color
        end
      else
        break;
      end
    end
    self.selection.from = from
    self.selection.to = to
    self.selection.visible = true
  end

  super(self).constructor(self, ...)
end

-- create a simple window 
local win = ui.Window("Syntax highlighting sample", "fixed", 520, 490)
local edit = SyntaxEdit(win, "", 10, 10, 500, 440)
local button = ui.Button(win, "Run script", 230, 456) 

-- set edit.properties
edit.font = "Lucida Console"
edit.fontsize = 9
edit.wordwrap = true
edit.text = [[local ui = require "ui"

local win = ui.Window("Zoom example", 512, 380)
local img = ui.Picture(win, "")
local button = ui.Button(win, "Replay")
local factor = 0

function button:onClick()
    self:hide()
    factor = 0
end

function win:onResize()
    img:center()
    button:center()
    button.y = img.y+img.height+20
end

img:load("LuaRT.png")
win:center()
button:hide()

win:show()

repeat
    ui.update()
    if factor < 1 then 
        factor = factor + 0.05
        img:resize(factor)
        img:center()
        if factor >= 1 then
            win:onResize()
            button:show()
        end
    end
until win.visible == false]]

function checkerror(err)
  if err ~= nil then
    err = err:gsub("%[.+%]:", "line ")
    edit.line = err:match("line (%d+):")
    ui.error(err, "Script error")
    return false
  end
  return true
end

function button:onClick()
  local func, err = load(edit.text)
  if checkerror(err) then
    win:hide()
    local success, err = pcall(func)
    win:show()
    checkerror(err)
  end
end

win:show()

-- update user interface
repeat
	ui.update()
until not win.visible