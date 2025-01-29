local ui = require "ui"

local Picture = Object(ui.Picture)

function Picture:constructor(parent, img, x, y, width, height)
  ui.Picture.constructor(self, parent, "", 128, 128)
  if sys.File(img).exists then
    self.image = img
  else
    self.image = "resources/picture.ico"
    self.oldWidth = nil
    self.oldHeight = nil
  end
end

function Picture:set_image(img)
  if self:load(img) then
    self.img = img
    self.oldWidth = self.width
    self.oldHeight = self.height
  else
    ui.error("Failed to load image '"..img.fullpath.."'")
  end
end

function Picture:image2str()
  return nil
end

return inspector:register(Picture, {}, {image = true})