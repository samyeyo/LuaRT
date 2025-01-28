--
--  LuaRT json.lua example
--  Decode a JSON string using a sys.COM Object
--

if _ARCH == "x64" then
   error("This example can only be run on x86 Windows")
end

local function decodeJSON(str)
    -- create a ScriptControl COM Object
    local script = sys.COM("MSScriptControl.ScriptControl")
    -- Set script language to JScript
    script.Language = "JScript"
    -- Add a JScript function
    script:AddCode('function decode() { return '..JSON..'; } ')
    -- Execute that function adn return the result (a sys.COM Object )
    return script:Run("decode")
end

JSON = [[{ 
        "searchResponse":{ 
            "myname": "Sam",
           "element":[ 
              { 
                 "accType":"R",
                 "accountNumber":"1111111",
                 "accountStatus":"A",
                 "taxId":"#54XDD6",
                 "result": true
              }
           ]
        }
     }]]

local json = decodeJSON(JSON)

-- Should print true
print(json.searchResponse.element[0].result)