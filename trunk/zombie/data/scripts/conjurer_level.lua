--------------------------------------------------------------------------------
--   data/scripts/conjurer_level.lua
--
--   Helper functions for level management
---------------------------------------------------------------------------------
function OnConjurerNewLevel()

end

--------------------------------------------------------------------------------
--   Called when a level is loaded
--------------------------------------------------------------------------------
function OnConjurerLoadLevel()
    level = nebula.sys.servers.levelmanager:getcurrentlevelobject()
    filename = 'wc:levels/' .. level:getname() .. '/preset.n2'
    f = nebula.sys.servers.file2
    if (f:fileexists( filename )) then
        nebula.app.conjurer.appviewportui.viewport0:loadstate( filename )
    end
end


--------------------------------------------------------------------------------
--   save currrent state of the editor in the level folder
--------------------------------------------------------------------------------
function savepreset()
    level = nebula.sys.servers.levelmanager:getcurrentlevelobject()
    filename = 'wc:levels/' .. level:getname() .. '/preset.n2'
    nebula.app.conjurer.appviewportui.viewport0:savestateas( filename )
end
