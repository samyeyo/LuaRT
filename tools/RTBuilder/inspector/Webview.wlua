local ui
 = require "ui"
require "webview"

local Webview = Object(ui.Webview)

Webview.module = "webview"

function Webview:constructor(parent, url, x, y, width, height)
	ui.Webview.constructor(self, parent, url or "about:blank", x or 10, y or 10, width, height)
	self.values = {}
	self.name = findname("Webview")
	self.mt = getmetatable(self)
	self.newindex = self.mt.__newindex
	self.mt.__newindex = function(t, property, val) self.values[property] = val end
	setmetatable(self, self.mt)
  end

function Webview:onMessage(str)
    if str == '"click"' then
		WidgetOnClick(self)
	end
end

function Webview:set_url(url)
	super(self).set_url(self, url)
end

function Webview:onLoaded()
	self:eval([[
		const handleClick = () => {
			window.chrome.webview.postMessage('click');
		}
		document.addEventListener('click', handleClick);
		
	]])
end

function Webview:onReady()
	self.mt.__newindex = self.newindex
	for k, v in pairs(self.values) do
		self[k] = v		
	end
	self:onLoaded()
	inspector.Update(self)
  inspector:onTrack(self)
end

function Webview:onStartDump(file)
	file:write("function "..self.name.."_init()\n")
end

function Webview:onEndDump(file)
	file:write("end\n"..self.name.."_init()\nfunction "..self.name..":onReady()\n\t"..self.name.."_init()\nend\n")
end

return inspector:register(Webview, { 
	acceleratorkeys = inspector.properties.boolean,
	contextmenu = inspector.properties.boolean,
	devtools = inspector.properties.boolean,
	statusbar = inspector.properties.boolean
})