local defaultsize = 8
editor.fontsize = defaultsize+1
output.fontsize = defaultsize
console.fontsize = defaultsize

editor.fontname = "Fira Code Retina"
output.fontname = "DejaVu Sans Mono"
console.fontname = output.fontname

filetree.iconfontname = editor.fontname

singleinstance = not mac

unhidewindow = { -- allow unhiding of GUI windows
  -- 1 - show if hidden, 0 - ignore, 2 -- hide if shown
  ConsoleWindowClass = 0,
  ['Auto-Suggest Dropdown'] = 2,
  SysShadow = 2,
  -- IUP library window
  ['GDI+ Hook Window Class'] = 2,
  -- ignore the following windows when "showing all"
  IME = 0,
  wxDisplayHiddenWindow = 0,
  wxTimerHiddenWindow = 0,
  -- window added when Microsoft.Windows.Common-Controls is enabled in the manifest
  tooltips_class32 = 0,
  ['MSCTFIME UI'] = 0,
  -- GLUT/opengl/SDL applications (for example, moai or love2d)
  GLUT = 1, FREEGLUT = 1, SDL_app = 1,
}
