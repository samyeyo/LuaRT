--
--  LuaRT balls.lua example
--  Shows a bouncing ball
--  Adapted from cos00kun (https://community.luart.org)
--

local ui = require "ui"
local picture= {}
local alpha= {}
local speed= {}
local balls= 6
local win = ui.Window("Bouncing balls", "fixed", 640, 480)

math.randomseed(sys.clock())

function alpha_Random(_i, _j)
	return math.rad(math.random(_i, _j))
end
 
function control(_x, _y, _i)
	if _x <= 32 then
		alpha[_i]= math.rad(math.random(0,180))
	end

	if _x >= win.width-32 then
		alpha[_i]= math.rad(math.random(180,360))
	end
	
	if _y <= 32 then
		alpha[_i]= math.rad(math.random(90,270))
	end
	
	if _y >= win.height-32 then
		alpha[_i]= math.rad(math.random(270,360))
	end

end

for i=1, balls do  
  alpha[i]= alpha_Random(0, 360)
  picture[i]= ui.Picture(win, sys.File(arg[0]).path.."\\ball.png", math.random(200, 850), math.random(100, 500))
end		

win:center()
win:show()

repeat
  for i=1, balls do
    picture[i].x = picture[i].x + 12*math.sin(alpha[i])
    picture[i].y = picture[i].y - 12*math.cos(alpha[i])
    control(picture[i].x, picture[i].y, i)
  end
  ui.update()
until not win.visible