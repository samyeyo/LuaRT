
local version = 5.4

local function exePath(self, version)
    local version = tostring(version or ""):gsub('%.','')
    local mainpath = ide:GetRootPath()
    return (ide.osname == "Windows" and mainpath..[[bin\luart.exe]]), ide.config.path['lua'..version] ~= nil
end

return {
    name = "Console",
    description = ("console interpreter with debugger"),
    api = {"baselib"},
    luaversion = version,
    fexepath = exePath,
    frun = function(self,wfilename,rundebug)
    local exe, iscustom = self:fexepath(version or "")
    local filepath = ide:GetShortFilePath(wfilename:GetFullPath())

    if rundebug then
        ide:GetDebugger():SetOptions({runstart = ide.config.debugger.runonstart == true})
        -- update arg to point to the proper file
        rundebug = ('if arg then arg[0] = [[%s]] end '):format(wfilename:GetFullPath())..rundebug

        local tmpfile = wx.wxFileName()
        tmpfile:AssignTempFileName(".")
        filepath = ide:GetShortFilePath(tmpfile:GetFullPath())

        local ok, err = FileWrite(filepath, rundebug)
        if not ok then
        ide:Print(("Can't open temporary file '%s' for writing: %s."):format(filepath, err))
        return
        end
    end
    local params = self:GetCommandLineArg("lua")
    if params ~= nil then
        params = params:gsub("[^%s]+", function(param) return '\\"'..param..'\\"' end)
    end
    local cmd = '"'..exe..'" -e "sys.cmd(\'\\"\\"'..exe:gsub("\\", "/")..'\\" \\"'..(filepath):gsub("\\", "/")..'\\" '..(params and params..'\\"' or '')..[[', false)"]]
    local envcpath = "LUA_CPATH"
    local envlpath = "LUA_PATH"
    if version then
        local env = "PATH_"..string.gsub(version, '%.', '_')
        if os.getenv("LUA_C"..env) then envcpath = "LUA_C"..env end
        if os.getenv("LUA_"..env) then envlpath = "LUA_"..env end
    end

    local cpath = os.getenv(envcpath)
    local luart_path = ide.frame.bottomnotebook.shellbox.getvalue([[sys.registry.read('HKEY_CURRENT_USER', 'Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\LuaRT', 'InstallLocation', false)]])
    if #luart_path == 0 then
        luart_path = ide:GetRootPath().."/../.."
    end
    cpath = cpath..";"..luart_path.."/modules/?/?.dll"

    if rundebug and cpath and not iscustom then
        -- prepend osclibs as the libraries may be needed for debugging,
        -- but only if no path.lua is set as it may conflict with system libs
        wx.wxSetEnv(envcpath, ide.osclibs..';'..cpath)
    end
    if version and cpath then
        -- adjust references to /clibs/ folders to point to version-specific ones
        local clibs = string.format('/clibs%s/', version):gsub('%.','')
        if not cpath:find(clibs, 1, true) then cpath = cpath:gsub('/clibs/', clibs) end        
        wx.wxSetEnv(envcpath, cpath)
    end

    local lpath = version and (not iscustom) and os.getenv(envlpath)
    if lpath then
        -- add oslibs libraries when LUA_PATH_5_x variables are set to allow debugging to work
        wx.wxSetEnv(envlpath, lpath..';'..ide.oslibs)
    end

    -- CommandLineRun(cmd,wdir,tooutput,nohide,stringcallback,uid,endcallback)
    local pid = CommandLineRun(cmd,self:fworkdir(wfilename),true,false,nil,nil,
        function() if rundebug then wx.wxRemoveFile(filepath) end end)

    if (rundebug or version) and cpath then wx.wxSetEnv(envcpath, cpath) end
    if lpath then wx.wxSetEnv(envlpath, lpath) end
        return pid
    end,
    hasdebugger = true,
    scratchextloop = false,
    unhideanywindow = true,
    takeparameters = true,
    skipcompile = true
}


