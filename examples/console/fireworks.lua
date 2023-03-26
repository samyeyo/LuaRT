-- ASCII Fireworks for LuaRT
-- adapted from QB64 forum example by Onyx, from https://community.luart.org
-- thanks to original authors : Bplus, Pete and TempodiBasic

local console = require "console"

console.fullscreen = true
console.fontsize = 22

local nR = 14
local msg = "LuaRT ASCII Fireworks from QB64 forum. Adapted from Bplus Inspired by Pete, TempodiBasic"
local Rocket = Object {}

console.cursor = false
console.fullscreen = true

local colmax = console.width
local rowmax = console.height
local colors = { "black", "blue", "green", "cyan", "red", "purple", "yellow", "white", "gray",
"lightblue", "lightgreen", "lightcyan", "lightred", "lightpurple", "lightyellow", "brightwhite"}
local r = {}

function Rocket:constructor()
    self.x = math.random(colmax-20) + 10
    self.y = rowmax-3    
    self.bang = math.random(rowmax - 10)    
    self.age = 0   
    self.c = math.random(15)+1
end

function Rocket:draw(idx)
    if self.y > self.bang then
        console.locate(self.x, self.y)
        console.writecolor("lightred", "^")
        self.y = self.y - 1
    else
        self.age = self.age + 1
        if self.age > 15 then
            r[idx] = Rocket()
        else
            for j = 1,12 do
                local xx = self.x +  self.age * math.cos(j * math.pi / 6)
                local yy = self.y + 0.5*self.age * math.sin(j * math.pi / 6)
                if xx > 0 and xx < colmax and yy > 0 and yy < rowmax then
                    console.locate(xx, yy)
                    console.writecolor(colors[self.c], "*")
                end
            end
        end
    end
end

local lc = 0
local p = 0
local rocs = 0

for i=1, nR do
    r[i] = Rocket()
end

while not console.keyhit do
    console.clear()
    lc = lc + 1
    if lc % 3 == 0 then
        p = (p + 1) % #msg    
    end    
    console.locate(0.4*colmax,2)
    console.writecolor("red", msg:sub(p+1, math.floor(0.5*colmax)+1))
    rocs = rocs + 1
    if rocs > nR then
        rocs = nR
    end
    for i = 1, rocs do
        r[i]:draw(i)
    end
    sys.sleep(1)
end

console.clear()

console.cursor = true