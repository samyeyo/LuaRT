--
--  LuaRT shortcut.lua example
--  Create a shortcut file (.lnk) using sys.COM Object
--

-- create a Shell COM Object
local shell = sys.COM("WScript.Shell")

-- create a shortcut file named "LuaRT Documentation", in the current directory
local shortcut = shell:CreateShortcut("LuaRT Documentation.lnk")

-- set its target path as the LuaRT website address
shortcut.TargetPath = "https:\\luart.org"

-- and save the shortcut
shortcut:Save()

-- execute the shortcut, an browser window should open at LuaRT website
sys.cmd('"LuaRT Documentation.lnk"')