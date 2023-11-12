local ui = require "ui"
local audio = require "audio"

local win = ui.Window("Reverb effect example", 320, 200)
local button = ui.Button(win, "Play logon sound")
button:center()

local ch = ui.Checkbox(win, "Reverb effect")
ch:center()
ch.y = button.y + 30

local sound = audio.Sound(sys.env.WINDIR.."\\Media\\Windows Logon.wav")

function button:onClick()
    sound:play()
end

function ch:onClick()
    if ch.checked then
        sound:reverb() -- apply reverb effect with default values
    else
        sound:reverb(nil) -- suppress reverb effect
    end
end

ui.run(win):wait()
