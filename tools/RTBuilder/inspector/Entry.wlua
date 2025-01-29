local ui = require "ui"

local Entry = Object(ui.Entry)

return inspector:register(Entry, {
    masked = inspector.properties.boolean
}, {
    modified = true
})