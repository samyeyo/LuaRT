local ui = require "ui"

local Button = Object(ui.Button)

return inspector:register(Button, {
    hastext = inspector.properties.boolean
})