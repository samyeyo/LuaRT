local ui = require "ui"

local Checkbox = Object(ui.Checkbox)

return inspector:register(Checkbox, {
    checked = inspector.properties.boolean
})