---------------------------------------------------------------------------------
--   "editor" application state GUI initialization
--
--   This procedure is called when the state is entered
---------------------------------------------------------------------------------
function ( self )

    rw = lookup('/gui/editorRootWindow')
    if (not rw) then

        guiroot = nebula.sys.servers.gui:getrootpath()
        luaSel(guiroot)

        -- Create template label for message window
        templateLabel = nebula.sys.servers.kernelserver:new('nguifadeouttextlabel', 'message_template_label') 
        templateLabel:setfont('courier15')
        templateLabel:setcolor(1.0,1.0,1.0,1.0)
        templateLabel:setalignment('left')

        -- Create game root window  
        nebula.sys.servers.gui:setrootwindow('')
        rootWindow = nebula.sys.servers.gui:newwindownamed('nguiwindow', true, 'editorRootWindow')
        rootWindow:setrect( 0.0, 1.0, 0.0, 1.0 )
        
        -- Select resolution multiplier
        screenWidth = 1.0
        screenHeight = 1.0
        wTitle, wType, scrPosX, scrPosY, screenWidth, screenHeight, vertSync = nebula.sys.servers.gfx:getdisplaymode()
        -- Constant GUI size (variable relative size)
        resMulX = 1.0 / screenWidth
        resMulY = 1.0 / screenHeight
        -- If resolution lower than 1024x768 (800x600 or 640x480), do constant relative size instead
        if ( screenWidth < 1024 and screenHeight / screenWidth == 0.75) then
            resMulX = 1.0 / 1024.0
            resMulY = 1.0 / 768.0
        end

        pushcwd(rootWindow)
        
            -- Create editor message window
            nebula.sys.servers.gui:setrootwindow('')
            messageWindow = nebula.sys.servers.kernelserver:new('nguitimedmessagewindow', 'MessageWindow')
            messageWindow:show()
            messageWindow:setrect( 0.05, 0.95, 0.6, 0.95 )
            messageWindow:setmessagefadeouttime( 5 )
            messageWindow:settemplatetextlabel(templateLabel:getfullname())

            nebula.app.conjurer.appstates.editor:setonscreenlogupdateinterval( 0.25 )
            
        popcwd()

        popcwd()

    end

    -- Time counters
    lastUpdateTime = 0
    lastLogTime = 0
    elapsedLogTime = 0
    estimatedMeanTime = 0

    -- Set current root window
    nebula.sys.servers.gui:setrootwindow('editorRootWindow')
end
