local ui = require "ui"

local saved_widgets = {}

local function value2str(widget, propname, name)
    local value
    if propname then        
        value = widget[propname]
    else
        value = widget
    end
    local t = type(value)
    local result = ""
    if t == "string" then
        result = "'"..value:gsub("'", "\'").."'"
    elseif propname and widget[propname.."2str"] then
        result = widget[propname.."2str"](widget)
    elseif t == "table" then
        result = "{ "        
        if propname == "items" then
            return items2str(value)
        else 
            local k, v
            while true do
                k,v = next(value, k)
                if k ~= nil then
                    result = result.."["..(type(k) == "number" and k or value2str(k)).."] = "..value2str(v)..","
                else
                    break
                end
            end
        end
        result = result:sub(1, -2).." }"
    elseif t == "nil" then
        return "nil"
    elseif t == "boolean" then
        return tostring(value)
    elseif t == "number" then
        return '"'..value..'"'
    elseif t:find("Item$") then
        return name..".items["..value.index.."]"
    else
        result = "'"..tostring(value).."'"
    end
    return result
end

local function parsenode(node)
    local result = ""
    for item in each(node.items or node.subitems) do
        result = result..value2str(item.text)..", "
        if item.count > 0 then
            local previtem = result:gsub("('.-'), $", "[%1] = ")
            result = previtem.."{ "..parsenode(item).." } "
        end
    end
    return result
end

function items2str(items)
    local result = ""
    local k, v
    if #items > 0 and not items[1].expand then
        for k, v in ipairs(items) do
            result = result.."["..k.."] = "..value2str(v.text)..","        
        end
    else
        k = 1
        local firstnode = each(items)()
        local tree = firstnode and firstnode.parent or false
        if tree then
            result = parsenode(tree)
        end
    end
    return "{"..result:sub(1, -2).." }"
end


local function write_properties(file, widget, ancestor, name)
    for member in pairs(ancestor) do
        local propname = member:match("^set_(.*)")
        if propname ~= nil then
            if widget ~= formWindow or propname ~= "visible" then
                local value = value2str(widget, propname, name)
                if propname:usearch("text") then
                    value = value:gsub("^'(.*)'$", function(str) return "[["..str.."]]" end)
                    file:write(name.."."..propname.." = "..value.."\n")
                elseif value ~= nil and propname ~= "items" then
                    file:write(name.."."..propname.." = "..value.."\n")
                end
            end
        end
    end
end

local function dump_icons(file, widget)
    local result = ""
    for i = 1, widget.count do
        local icon = widget.icons[i]
        if icon then
            result = result..widget.name..".items["..i.."]:loadicon((embed or sys or _ENV).File('"..icon:gsub(file.path:gsub("\\", "/"), "").."'))\n"
        end
    end
    return result
end

local function save(file, name, widget)
    if saved_widgets[name] then
        return
    end

    local name = name or formWindow.name
    local widget = widget or formWindow
    local t = widget.parent and widget.parent.type or "Window"

    if t == "Groupbox" or t == "Panel" then
       save(file, widget.parent.name, widget.parent)
    end

    local ancestor = super(widget)
    if widget.module then
      file:write("\nrequire '"..widget.module.."'")
    end
    file:write("\nlocal "..name.." = ui."..widget.type.."(")
    if widget == formWindow then
        file:write('rtbuilder, "'..widget.title..'", rtbuilder and "dialog" or "'..widget._style..'", '..widget.width..", "..widget.height..")\n")
        file:write(name.."._transparency = "..widget._transparency.."\n")
    elseif type(widget) ~= "Tracker" then
        if widget.items ~= nil then
            file:write(widget.parent.name..', {}, '..widget.x..", "..widget.y..', '..widget.width..", "..widget.height..")\n")
            file:write(name..".items = "..value2str(widget, "items").."\n")
        elseif widget.text ~= nil or widget.img ~= nil then
            if widget.parent and type(widget.parent) == "TabItem" then
                file:write(widget.parent.owner.name..'.items['..widget.parent.index..']'..', [['..widget.text..']], '..widget.x..", "..widget.y..', '..widget.width..", "..widget.height..")\n")
            else
                file:write(widget.parent.name..', [['..(widget.text or widget.img)..']], '..widget.x..", "..widget.y..', '..widget.width..", "..widget.height..")\n")
            end
        else
            file:write(widget.parent.name..', '..widget.x..", "..widget.y..', '..widget.width..", "..widget.height..")\n")
        end
    end
    file:write(name..".name = '"..name.."'\n")
	file:write(name..".type = '"..widget.type.."'\n")
    if widget.onStartDump then
        widget:onStartDump(file)
    end
    if widget.icons ~= nil then
        file:write(name..".icons = "..value2str(widget, "icons").."\n")
        file:write(dump_icons(file, widget))
    elseif widget.loadicon then
        file:write(name..':loadicon('..(widget.icon and '[['..widget.icon..']]' or "")..')\n')
        file:writeln(name..'.icon = '..(widget.icon and '[['..widget.icon..']]' or "nil"))
    end
    write_properties(file, widget, ancestor, name)
    write_properties(file, widget, inspector.registry[widget.type], name)
    if widget.onEndDump then
        widget:onEndDump(file)
    end
    saved_widgets[name] = true
end

function saveWindow()
    local file
    saved_widgets = {}
	file = filename and sys.File(filename) or ui.savedialog("Save Window as...", false, "Lua files (*.lua, *.wlua)|*.lua;*.wlua|All files (*.*)|*.*") 
	if file == nil or ((not filename) and file.exists and ui.confirm(file.name.." already exists. Continue and overwrite its content ?") ~= "yes") then
		return false
	end
    tracker:stop()
    file:open("write")
    file:write('local ui = require "ui"')
    save(file, formWindow.name, formWindow)
    local widgets = ", {"
    table.sort(Widgets, function(a,b) local t = type(a); return t == "Groupbox" or t == "Tab" or t == "Panel" end)
    for name, w in pairs(Widgets) do
        widgets = widgets.." ['"..name.."'] = "..name..","
        save(file, name, w)
    end
    file:writeln("return "..formWindow.name..widgets.." }")
    file:close()
    filename = file.fullpath
    modified = false
    return true
end

function loadWindow(file)
    if not file then
        file = ui.opendialog("Open Window...", false)--, "Lua files (*.lua, *.wlua)|*.lua;*.wlua|All files (*.*)|*.*")
    	if not file then
		    return nil
	    end 
    end   
    filename = file.fullpath
    local ui_module = {}
    setmetatable(ui_module, { __index = function(self, widget) 
        if inspector.registry[widget] == nil then
            inspector.load(widget);
        end
        return inspector.registry[widget]
    end })
    local func, err = loadfile(file.fullpath, "bt", { print = print, rtbuilder = mainWindow, File = function(f) return file.path..f end, require = function() return ui_module end })
    if not err then
        local result, win, list = pcall(func)
        if result and (type(win) == "Window") then
            if formWindow ~= nil then
                formWindow:onClose()
            end
            sleep()  
            formWindow = win
            Widgets = list
            loadWidgets()
            return file.fullpath
        end
    end
    ui.error("The provided file is not a valid RTBuilder Window")
end
