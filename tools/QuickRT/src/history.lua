local history = {}

local file = sys.File(sys.File(arg[-1]).path.."history.conf")

function history.load()
    local t = {}
    if not file.exists then
        return t
    end
    for line in file:open().lines do
        t[#t+1] = line
    end
    file:close()
    return t
end

function history.save(list)
    file:open("write")
    local start = (#list > 100) and #list-100 or 1
    for i=start, 100 do
        local line = list[i]
        if line == nil then
            break
        end
        if #line > 0 then
            file:writeln(line)
        end
    end
    file:close()
end

return history