local ui = require "ui"
require "canvas"

local Canvas = Object(ui.Canvas)

function Canvas:onPaint()
    self:begin()
    self:clear()
    self:flip()
end

Canvas.inspector = {}

Canvas.module = "canvas"

function Canvas.inspector.fontstyle(panel, obj, property)
    local widget = ui.Combobox(panel, false, {"normal", "italic", "oblique"}, 100, panel.pos-3)
    widget.onSelect = function (self, item)
        tracker.widget.fontstyle = item.text
    end
    widget.update = function(tracked)
        widget.selected = widget.items[tracked.fontstyle]
    end
    return widget
end

function Canvas.inspector.font(panel, obj, property)
    local widget = ui.Button(panel, "", 99, panel.pos-3)
    widget.textalign = "left"

    widget.update = function(tracked)
        local tracked = tracked or tracker.widget
        tracker:stop()
        widget.text = tracked.font.." "..tracked.fontsize
        widget.width = 102
        if tracked ~= formWindow then
            tracker:start(tracked)
        end
    end

    widget.onClick = function(self)
        local result = {ui.fontdialog()}
        if #result > 0 then
            local tracked = tracker.widget
            if #result[1] > 0 and result[1] then
                tracked.font = result[1]
            end
            widget.update(tracked)
        end
    end
    panel.pos = panel.pos + 2
    return widget
end

function Canvas.inspector.bgcolor(panel, obj, property)
    local widget = ui.Entry(panel, "0xFFFFFFFF", 120, panel.pos-2, 80)
    widget.icon = ui.Panel(panel, 100, panel.pos+2, 16, 16)
    widget.icon.border = true
    widget.textalign = "center"
    widget.icon.cursor = "hand"
    widget.update = function(tracked)
        local color = string.format("0x%.8X", tracked[property])
        widget.icon.bgcolor = tracked[property] >> 8
        widget.text = color
    end
    widget.onSelect = function(self)
        if not tonumber(widget.text) then
            sys.beep()
            self.update(widget.icon)
        else
            local tracked = tracker.widget
            tracked[property] = widget.text
            self.update(tracked)
        end
    end
    widget.icon.onClick = function (self)
        local tracked = tracker.widget       
        local lastvalue = tracked[property]
        local color = ui.colordialog(lastvalue)
        if color and color ~= lastvalue then
            tracked[property] = (color << 8) + 0xFF
            widget.update(tracked)
        end
    end
    return widget
end

function Canvas.inspector.fontweight(panel, obj, property)
    widget = ui.Entry(panel, "", 100, panel.pos-3, 100, 23)
    widget.onSelect = function (self)
        local func = function()
            tracker.widget.fontweight = widget.text
        end
        if not pcall(func) then
            ui.error("The fontweight entered '"..widget.text.."' is invalid")
            widget.text = tracker.fontweight
        end
    end
    widget.update = function(tracked)
        widget.text = tracked.fontweight
    end
    return widget
end

function Canvas.inspector.color(panel, obj, property)
    return Canvas.inspector.bgcolor(panel, obj, property)
end


return inspector:register(Canvas, {}, { sync = true })