---------------------------------------------------------------------------------
--   "load" application state GUI finalization
--
--   This procedure is called when the state is leave
---------------------------------------------------------------------------------
function ( self )
    -- play menu sound
    local music = nebula.usr.soundlibrary
    if music then
        music:stopallsounds()
    end
end
