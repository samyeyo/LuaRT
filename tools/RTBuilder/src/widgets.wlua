local ui = require "ui"

local tabpos = 22
local tabindex = 0

function findname(name)
    local i = 0
    repeat
        i = i + 1
    until Widgets[name..i] == nil
    return name..i
end

function WidgetOnClick(w)
    tracker:stop();
    local parent = w.parent
    local t = parent.type
    if t == 'Tab' then
        tracker = parent.owner.tracker or Tracker(parent)
    else
        tracker = parent.tracker or Tracker(parent);
    end
    inspector.Update(w)
end

local function widgetOnClick(self)
    local name
    if formWindow == nil then
        ui.warn("To use a "..self.name.." widget, create a new Window first")
        return
    end
    local old = tracker.widget
    local w = old and old.type or type(old)
    local parent
    tracker:stop()
    if w == "Groupbox" or w == "Panel" or w == "Tab" then 
        if w == "Tab" then
            if old.count == 0 then                
                ui.warn("You must first create a Tab element to put a widget on it")
                tracker:start(old)
                return
            end
            parent = old.selected
            parent.owner.tracker = parent.owner.tracker or Tracker(parent)
        else
            parent = old
            parent.tracker = parent.tracker or Tracker(parent)
        end
        tracker = w == "Tab" and parent.owner.tracker or parent.tracker
        parent.WidgetOnClick = WidgetOnClick;
    else 
        parent = old and old.parent or formWindow
        tracker = parent.tracker or parent.owner.tracker
    end
    inspector.Update(self.name)
    local function create(content)
        return inspector.create(self.name, parent, content, 50, 50)
    end
    
    local success, widget
    name = findname(self.name)
    success, widget = pcall(create, name) 
    if not success then
        success, widget = pcall(create, {})
        if not success then
            widget = inspector.create(self.name, parent)
        end
    end
    if old then
        old:toback()
    end
    widget.name = name
    Widgets[widget.name] = widget
    if not widget.isvirtual then
        widget:tofront()
        widget.get_visible = function(self) return self._visible == nil and super(self).get_visible(self) or self._visible end
        widget.set_visible = function(self, val) self._visible = val end
        widget.get_enabled = function(self) return self._enabled == nil and super(self).get_enabled(self) or self._enabled end
        widget.set_enabled = function(self, val) self._enabled = val end
        widget._enabled = true
        widget._visible = true
    end
    widget.onClick = WidgetOnClick
    widget.onContext = function (self) tracker:stop(); inspector.panels[widget.type]:show(); tracker:start(widget); formWindow:popup(onContext[self.type](self)) end
    if widget["get_items"] ~= nil then
        widget.icons = {}
    end
    if widget.type == "Combobox" then
        widget.onSelect = function (self, item)
            if tracker.widget then
                tracker:stop()
                inspector.panels.Combobox.widgets.selected.update(widget)
                sleep(100)
                tracker:start(self)
            end
        end    
    end
    modified = true
    inspector.Update(widget)
end

function loadWidgets()
    for name, w in pairs(Widgets) do
		if not inspector.panels[w.type] then
			inspector.Update(w.type)
		end
        w.onClick = WidgetOnClick
        w._enabled = w.enabled
        w._visible = w.visible        
        w.enabled = true
        w.visible = true
        w.get_visible = function(self) return self._visible == nil and super(self).get_visible(self) or self._visible end
        w.set_visible = function(self, val) self._visible = val end
        w.get_enabled = function(self) return self._enabled == nil and super(self).get_enabled(self) or self._enabled end
        w.set_enabled = function(self, val) self._enabled= val end
        w.onContext = function (self) tracker:stop(); inspector.panels[w.type]:show(); tracker:start(w); formWindow:popup(onContext[self.type](self)) end
        if w.type == "Combobox" then
            w.onSelect = function (self, item)
                if tracker.widget then
                    tracker:stop()
                    inspector.panels.Combobox.widgets.selected.update(w)
                    sleep(100)
                    tracker:start(self)
                end
            end    
        end
    end
    init_formwindow(formWindow)
    modified = false
end

function addwidget(name)
    local name = name:capitalize()
    local widget = ui.Picture(widgetsTab.items[tabindex], "resources/"..name..".ico", tabpos, 10, 24, 24)
    local label = ui.Label(widgetsTab.items[tabindex], name, widget.x, 36)
    widget.name = name
    label.name = name
    label.fontsize = 8
    label.x = (widget.x - label.width/2 + 11)
    widget.cursor = "hand"
    label.cursor = "hand"
    label.onHover = function() if (not label.fontstyle.underline) then label.fontstyle = {underline = true} end end
    widget.onHover = label.onHover
    label.onLeave = function() label.fontstyle = {underline = false} end
    widget.onLeave = label.onLeave
    widget.onClick = widgetOnClick
    label.onClick = widgetOnClick
    tabpos = tabpos + 64
    if label.width > 50 then
        tabpos = tabpos+8
    elseif label.width < 36 then 
        tabpos = tabpos-8
        label.x = label.x-6
        widget.x = widget.x-6
        if label.width < 30 then 
            label.x = label.x-4
            widget.x = widget.x-4
        end
    end
end

local json = require "json"

for tab in each(json.load("config.json").mainWindow.WidgetTabs) do
    for tabname, widgets in pairs(tab) do
        widgetsTab:add(tabname)
        tabindex = tabindex + 1
        tabpos = 22
        for widget in each(widgets) do
            addwidget(widget)
        end
    end
end