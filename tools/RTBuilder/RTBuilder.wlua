--
-- RTBuilder launch script using wluart.exe interpreter
--

local dir = sys.currentdir

-- Set Lua PATH and CPATH to find sources and needed binary modules
package.path = package.path..";"..dir.."/src/?.wlua;"..dir.."/src/?.lua;"
package.cpath = package.cpath..";"..dir.."/widgets/?.dll;"..dir.."/tracker/?.dll;"

-- Run the main.wlua script
require 'main'