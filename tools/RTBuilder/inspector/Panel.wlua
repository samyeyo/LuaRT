local ui = require "ui"

local Panel = Object(ui.Panel)

return inspector:register(Panel, { border = inspector.properties.boolean })