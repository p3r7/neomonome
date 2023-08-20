-- neomonome.
-- @eigen

package.cpath = package.cpath .. ";" .. norns.state.lib .. "?.so"
neomonome = require 'neomonome'


-- ------------------------------------------------------------------------
-- state

tint = {255, 191, 0}

g = nil
local has_nm_grid = false


-- ------------------------------------------------------------------------
-- script lifecycle

function grid_connect_maybe(_g)
  g = grid.connect()
  if g.device ~= nil and util.string_starts(g.name, "neo-monome ") then
    print("has nm grid")
    has_nm_grid = true
  end
end

function grid_register_new()
  print("new grid!!!")
  if not has_grid then
    print("register grid!!!")
    grid_register_new()
  end
end

grid.add = grid_connect_maybe

function init()
  screen.aa(1)
  screen.line_width(1)

  grid_connect_maybe()
end


-- ------------------------------------------------------------------------
-- ux

function enc(n, d)
  tint[n] = util.clamp(tint[n] + d, 0, 255)

  if has_nm_grid then
    neomonome.set_color(g.device.dev, tint[1], tint[2], tint[3])
  end
end
