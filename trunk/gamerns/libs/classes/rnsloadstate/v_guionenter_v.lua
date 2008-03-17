---------------------------------------------------------------------------------
--   "load" application state GUI initialization
--
--   This procedure is called when the state is entered
---------------------------------------------------------------------------------
function ( self )
    -- Select resolution multiplier
    resMulX = 1.0 / 1024.0
    resMulY = 1.0 / 768.0
    
    local guiroot = nebula.sys.servers.gui:getrootpath()
    
    local win = lookup(guiroot .. '/loadRootWindow')

    if not win then
        luaSel( guiroot )
        
        -- Create game root window    
        nebula.sys.servers.gui:setrootwindow('')
        local rootWindow = nebula.sys.servers.gui:newwindownamed('nguiwindow', true, 'loadRootWindow')
        rootWindow:setrect( 0.0, 1.0, 0.0, 1.0 )
        rootWindow:setdefaultbrush( 'loadscreen' )
        rootWindow:show()
        
        pushcwd(rootWindow)
            -- Create load bar
            loadBar = nebula.sys.servers.kernelserver:new('nguibar', 'LoadBar')
            loadBar:setdefaultbrush('loadbar_n')
            loadBar:setdisabledbrush('loadbar_d')
            loadBar:setrect( 362 * resMulX, ( 361 +  292 ) * resMulX, 
                624 * resMulY, ( 624 + 39 ) * resMulY )
            loadBar:setfillstate(0.0)
        popcwd()--rootWindow
    end
    
    -- Set current root window
    nebula.sys.servers.gui:setrootwindow('loadRootWindow')

    -- play menu sound
    local music = nebula.usr.soundlibrary
    if music then
        music:stopallsounds()
        local menuMusic = music:getsoundindex( "awe2" )
        if menuMusic ~= -1 then
            music:playrepeat( menuMusic, 0 )
        end
    end
end
