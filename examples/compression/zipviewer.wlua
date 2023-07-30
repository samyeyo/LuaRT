local ui = require "ui"
local compression = require "compression"

local win = ui.Window("Zip file viewer", "fixed", 250, 300)
local list = ui.List(win, {}, 0, 40, 250, 260)
list.style = "icons"
local button = ui.Button(win, "Open ZIP file", 80)

local zip_archive
local toremove = {}

function button:onClick()
    local file = ui.opendialog("Select a ZIP archive file", false, "ZIP archive files (*.zip)|*.zip")
    if file ~= nil and compression.isZip(file) then
        list:clear()
        zip_archive = compression.Zip(file, "read")
        for entry, isdir in each(zip_archive) do
            local dir = isdir and "\\" or ""
            list:add(entry):loadicon(entry..dir)
        end
    end
end

function list:onDoubleClick(item)
    local file = zip_archive:extract(item.text, sys.env.Temp)
    sys.cmd('start "ZipViewer" "'..file.fullpath..'"')
    toremove[#toremove+1] = file.fullpath
end


ui.run(win)

for fname in each(toremove) do
    sys.File(fname):remove()
end