local ui = require "ui"
require "widgets"


inspector = ui.Window(mainWindow, "Widget properties", "fixed", 210, 500)
inspector.x = mainWindow.x + mainWindow.width + 30
inspector.y = mainWindow.y + 30 + mainWindow.height

inspector.properties = require "inspector.properties"
inspector.registry = {}

function inspector:onTrack(tracked)
  if tracked and not tracked.virtual then
    local props = {"x", "y", "width", "height"}
    local tracked = tracked or self.widget or formWindow
    local panel = inspector.panels[tracked.type]
      for prop in each(props) do
          panel.widgets[prop].text = tracker.widget and tracker[prop] or tracked[prop]
      end
    end
end

function inspector:create_panel(widget, reject)
    local panel = ui.Panel(inspector)
    panel.align = "all"
    panel.widgets = {}
    panel:hide()
    local members = { "name"}
    local reject = reject or {}

    local function list(obj)
        local c = 0
        for member in pairs(obj) do
            local property = member:match("^set_(.*)")
            if property ~= nil and property:sub(1,1) ~= "_" and (widget.inspector[property] ~= nil or (not property:match("font.+"))) then
                if not reject[property] then       
                    members[#members+1] = property
                    reject[property] = true
                end
            end
        end
        table.sort(members)
    end
    list(super(widget))
    list(widget)
    panel.pos = 12
    for property in each(members) do
        panel.pos = panel.pos + 4
        ui.Label(panel, property, 10, panel.pos)
        local func =  widget.inspector[property] or inspector.properties[property] or inspector.properties.entry
        if func then
            panel.widgets[property] = func(panel, widget, property)
            panel.pos = panel.pos + 22
        end
    end
    panel.height = panel.pos + 10
    return panel
end

inspector.panels = {}

function inspector.load(widget)
    if inspector.registry[widget] == nil then
        local file = sys.File(sys.currentdir.."/inspector/"..widget..".wlua")
        if file.exists then
            local func, err = loadfile(file.fullpath)
            if not func then
                ui.error(err)
            else
                func()
            end
        else
            error("Inspector for '"..widget..'" not found')
        end
    end
end

function inspector.Update(tracked)
    if type(tracked) == "string" then
        inspector.load(tracked)
        return
    end
    local panel = inspector.panels[tracked.type]
    inspector:loadicon(sys.File(embed and arg[-1] or arg[0]).path.."/resources/"..tracked.type..".ico")
    for widget in each(panel.widgets) do
        if widget.update then
            widget.update(tracked)
        end
    end
    inspector.title = tracked.name.." properties"
    inspector.height = panel.pos + 10
    panel:show()
    if tracked ~= formWindow then
        tracker:start(tracked.container and tracked.container or tracked)
    end
    if inspector.visible == false then
        inspector:show()
    end
    tracker.onTrack = inspector.onTrack
end

function inspector:register(widget, properties, reject)
    local properties = properties or {}
    local name = getmetatable(getmetatable(widget).__type).__typename
    for property, func in pairs(properties) do
        inspector.properties:register(property, func)
    end
    widget.inspector = widget.inspector or {}
    local panel = inspector:create_panel(widget, reject)
    inspector.registry[name] = widget
    widget.type = name
    inspector.panels[name] = panel
end

function inspector.create(name, ...)
    local widget = inspector.registry[name](...)
    widget.type = name
    return widget
end

return inspector