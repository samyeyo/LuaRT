local ui = require "ui"

local Edit = Object(ui.Edit)


local function Edit_text(panel, obj, property)
    local widget = ui.Button(panel, "Edit lines...", 99, panel.pos-3, 103, 26)
    widget.onClick = function(self)
       onContext.Edit(tracker.widget).items[1].onClick()
    end
    panel.pos = panel.pos + 1
    return widget
end

return inspector:register(Edit, {
    readonly = inspector.properties.boolean,
    wordwrap = inspector.properties.boolean,
    text = Edit_text,
    rtf = inspector.properties.boolean
}, { column = true, line = true, caret = true, modified = true, richtext = true })