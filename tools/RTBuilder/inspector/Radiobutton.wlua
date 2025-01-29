local ui = require "ui"

local Radiobutton = Object(ui.Radiobutton)

return inspector:register(Radiobutton, {
    checked = inspector.properties.boolean
})