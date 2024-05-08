local ui = require "ui"
require "canvas"

ui.theme = "dark"

-- create a simple Window
local win = ui.Window("Gradient balls demo", 500, 400)
win.bgcolor = 0xFF0000
-- create a Canvas
local canvas = ui.Canvas(win)
canvas.align = "all"

-- Ball object definition
local Ball = Object{}

-- Ball constructor
function Ball:constructor()
    local gradient = canvas:LinearGradient { [0] = 0xFFCC00FF, [0.66] = 0xFF9900FF, [0.33] = 0xCC7A00FF, [1] = 0x000000FF }
    gradient.start = { 0, 0 }
    self.gradient = gradient
    self.x = math.random(canvas.width)
    self.y = math.random(canvas.height)
    self.dx = math.random(1, 15)
    self.dy = math.random(1, 15)
    self.radius = 3
end

-- Updates Ball position and draws it
function Ball:update()
    self.x = self.x + self.dx
    self.y = self.y + self.dy
    self.gradient.stop = { 0, canvas.height }
    canvas:fillcircle(self.x, self.y, self.radius, self.gradient)
    if self.x < self.radius or self.x > canvas.width-self.radius*0.5 then
        self.dx = -self.dx
    end
    if self.y < self.radius or self.y > canvas.height-self.radius*0.5 then
        self.dy = -self.dy
    end
end

-- table that will contain all Ball objects
local balls = {}

-- Fill the table with 500 Ball objects
for i=1, 500 do
    balls[i] = Ball()
end

-- Canvas:onPaint() event : updates all Balls
function canvas:onPaint()
    canvas:clear()
    for i = 1, 500 do
        balls[i]:update()
    end
end

ui.run(win):wait()