---------------------------------------------------------------------------------
--   This procedure is called when the state is entered
---------------------------------------------------------------------------------
function ( self )
    local buttonSound = -1
    local menuMusic = -1

    -- get sound index
    local music = nebula.usr.soundlibrary
    if music then
        menuMusic = music:getsoundindex( "Menu_Music" )
        buttonSound = music:getsoundindex( "Menu_Enter" )
    end
    
    -- Select resolution multiplier
    resMulX = 1.0 / 1024.0
    resMulY = 1.0 / 768.0
        
    local gamePort = "Orochimaru"
    local guiserver = lookup( '/sys/servers/gui' )
    local guiroot = guiserver:getrootpath()
    
---------------------------------------------------------------------------------
---------------------------------------------------------------------------------
    local ipButtonPressed = function (self)
        self.state.ipselected = self.ipnumber

        local music = nebula.usr.soundlibrary
        if music and ( buttonSound ~= -1 ) then
            music:play( buttonSound )
        end
    end
    
---------------------------------------------------------------------------------
---------------------------------------------------------------------------------
    local startGamePressed = function (self)
        local network = lookup( '/sys/servers/network' )
        
        if not network then
            network = newthunk( 'nnetworkmanagermulti', '/sys/servers/network' )
        end

        if network then
            network:setserverport( gamePort )
            network:startserver()
            
            local app
            if lookup('/app/conjurer') then
                app = nebula.app.conjurer
            else
                app = nebula.app.renaissance
            end
            
            state = app.appstates.load
            state:setnextstate( 'game' )
            
            app:setstate( 'load' )
        end
        
        self.state.startGame = true

        local music = nebula.usr.soundlibrary
        if music and ( buttonSound ~= -1 ) then
            music:play( buttonSound )
        end
    end
    
---------------------------------------------------------------------------------
---------------------------------------------------------------------------------
    local joinPressed = function (self)
        local button, layer
        
        layer = lookup(guiroot .. '/menuRootWindow/ipsLayer')
        if layer then
            layer:show()
        end
        
        button = lookup(guiroot .. '/menuRootWindow/buttonsWindow/StartButton')
        if button then
            button:disable()
        end
        
        button = lookup(guiroot .. '/menuRootWindow/buttonsWindow/JoinButton')
        if button then
            button:disable()
        end
        
        button = lookup(guiroot .. '/menuRootWindow/buttonsWindow/CancelJoinButton')
        if button then
            button:show()
        end
        
        button = lookup(guiroot .. '/menuRootWindow/buttonsWindow/JoinOkButton')
        if button then
            button:show()
        end
        
        button = lookup(guiroot .. '/menuRootWindow/buttonsWindow/IpUpButton')
        if button then
            button:show()
        end
        
        button = lookup(guiroot .. '/menuRootWindow/buttonsWindow/IpDownButton')
        if button then
            button:show()
        end
        
        -- create ip list
        self.state.iplist = {}
        
        local x0,x1,y0, y1
        
        for i = 1,5 do
            self.state.iplist[ i ] = {}
            
            x0 = 101 * resMulX
            x1 = x0 + 198 * resMulX
            y0 = ( 458 + 40 * ( i - 1 ) ) *resMulY
            y1 = y0 + 39 * resMulY
                
            pushcwd( lookup(guiroot .. '/menuRootWindow/buttonsWindow') )
                local button = newthunk('nguibutton', 'IpButton' .. i )
                button:setrect( x0, x1, y0, y1 )
                button:setdefaultbrush('ipbtn_n')
                button:setpressedbrush('ipbtn_n')
                button:sethighlightbrush('ipbtn_n')
                button:setdisabledbrush('ipbtn_n')
                button:disable()

                pin( button )
                button.state = self.state
                button.ipnumber = i
                button.pressedFunction = ipButtonPressed
                button:setcommand([[nthis:pressedFunction()]])

                self.state.iplist[ i ].button = button
            popcwd() --buttons window

            pushcwd( lookup(guiroot .. '/menuRootWindow') )
                local msgWindow = newthunk('nguitextlabel', 'IpLabel' .. i )
                msgWindow:show()
                msgWindow:setrect( x0, x1, y0, y1 )
                msgWindow:setwordbreak(false)
                msgWindow:setfont('courier15')
                msgWindow:setcolor(0.8,1.0,0.8,1.0)
                msgWindow:setalignment('center')
                msgWindow:settext('< empty >')
                
                self.state.iplist[ i ].label = msgWindow
            popcwd() --menuRootWindow
        end
        
        local network = newthunk( 'nnetworkmanagermulti', '/sys/servers/network' )
        if network then
            network:startsearching( gamePort )
        end
        
        local music = nebula.usr.soundlibrary
        if music and ( buttonSound ~= -1 ) then
            music:play( buttonSound )
        end
        
        self.state.ipselected = nil
        self.state.min_ipindex = 0
    end
    
---------------------------------------------------------------------------------
---------------------------------------------------------------------------------
    local exitPressed = function (self)
        self.state:exit()

        local music = nebula.usr.soundlibrary
        if music and ( buttonSound ~= -1 ) then
            music:play( buttonSound )
        end
    end
    
---------------------------------------------------------------------------------
---------------------------------------------------------------------------------
    local cancelJoinPressed = function (self)
        local button, layer
        
        layer = lookup(guiroot .. '/menuRootWindow/ipsLayer')
        if layer then
            layer:hide()
        end
        
        button = lookup(guiroot .. '/menuRootWindow/buttonsWindow/StartButton')
        if button then
            button:enable()
        end
        
        button = lookup(guiroot .. '/menuRootWindow/buttonsWindow/JoinButton')
        if button then
            button:enable()
        end
        
        button = lookup(guiroot .. '/menuRootWindow/buttonsWindow/CancelJoinButton')
        if button then
            button:hide()
        end
        
        button = lookup(guiroot .. '/menuRootWindow/buttonsWindow/JoinOkButton')
        if button then
            button:hide()
        end

        button = lookup(guiroot .. '/menuRootWindow/buttonsWindow/IpUpButton')
        if button then
            button:hide()
        end

        button = lookup(guiroot .. '/menuRootWindow/buttonsWindow/IpDownButton')
        if button then
            button:hide()
        end

        -- clear ip list
        if self.state.iplist then
            for i = 1,5 do
                if self.state.iplist[ i ] then
                    delete( self.state.iplist[ i ].label )
                    delete( self.state.iplist[ i ].button )
                end
            end
            
            self.state.iplist = nil
        end
        
        self.state.ipselected = nil
        self.state.min_ipindex = 0

        delete( "/sys/servers/network" )

        local music = nebula.usr.soundlibrary
        if music and ( buttonSound ~= -1 ) then
            music:play( buttonSound )
        end
    end
    
---------------------------------------------------------------------------------
---------------------------------------------------------------------------------
    local joinOkPressed = function (self)
        local network = lookup( '/sys/servers/network' )
        if self.state.ipselected and network then
            local app
            if (lookup( '/app/conjurer' )) then
                app = nebula.app.conjurer
            else
                app = nebula.app.renaissance
            end
            
            local state = app.appstates.load
            state:setnextstate( 'game' )
            
            state = app.appstates.game
            local host = self.state.iplist[ self.state.ipselected ].label:gettext()
            state:initconnectionwith( host, gamePort )
            
            app:setstate( 'load' )
            
            self.state.startGame = true
        end

        local music = nebula.usr.soundlibrary
        if music and ( buttonSound ~= -1 ) then
            music:play( buttonSound )
        end
    end
    
---------------------------------------------------------------------------------
---------------------------------------------------------------------------------
    local ipUpPressed = function (self)
        self.state.min_ipindex = self.state.min_ipindex - 1
        if self.state.ipselected then
            self.state.ipselected = self.state.ipselected + 1
        end
    end
    
---------------------------------------------------------------------------------
---------------------------------------------------------------------------------
    local ipDownPressed = function (self)
        self.state.min_ipindex = self.state.min_ipindex + 1
        if self.state.ipselected then
            self.state.ipselected = self.state.ipselected - 1
        end
    end
    
---------------------------------------------------------------------------------
---------------------------------------------------------------------------------
    function FirstCreate()
        luaSel( guiroot )
        
        -- Create menu root window    
        guiserver:setrootwindow('')
        local rootWindow = guiserver:newwindownamed('nguiwindow', true, 'menuRootWindow')
        rootWindow:setrect( 0.0, 1.0, 0.0, 1.0 )
        rootWindow:setdefaultbrush( 'rns_menubg01' )

        -- Set current root window
        guiserver:setrootwindow('menuRootWindow')
        
        local layer
        
        layer = guiserver:newwindownamed('nguiwindow', true, 'ipsLayer')
        layer:setrect( 100 * resMulX, 300 * resMulX, 439 * resMulY, 658 * resMulY )
        layer:setdefaultbrush( 'rns_iplist' )
        layer:hide()
        
        layer = guiserver:newwindownamed('nguiwindow', true, 'faceLayer')
        layer:setrect( 0.5, 1.0, 0.0, 1.0 )
        layer:setdefaultbrush( 'rns_menubg00' )
        
        -- ZOMBIE REMOVE
        --layer = guiserver:newwindownamed('nguiwindow', true, 'drops01Layer')
        --layer:setrect( 0.5, 1.0, 0.0, 1.0 )
        ---layer:setdefaultbrush( 'rns_drops01' )
        
        --layer = guiserver:newwindownamed('nguiwindow', true, 'drops01bLayer')
        --layer:setrect( 0.5, 1.0, 1.0, 2.0 )
        --layer:setdefaultbrush( 'rns_drops01' )
        --
        --layer = guiserver:newwindownamed('nguiwindow', true, 'drops01LayerBlur')
        --layer:setrect( 0.5, 1.0, 0.0, 1.0 )
        --layer:setdefaultbrush( 'rns_drops01b' )
        --
        --layer = guiserver:newwindownamed('nguiwindow', true, 'drops01bLayerBlur')
        --layer:setrect( 0.5, 1.0, 1.0, 2.0 )
        --layer:setdefaultbrush( 'rns_drops01b' )
        --
        --layer = guiserver:newwindownamed('nguiwindow', true, 'drops02Layer')
        --layer:setrect( 0.5, 1.0, 0.0, 1.0 )
        --layer:setdefaultbrush( 'rns_drops02' )
        --
        --layer = guiserver:newwindownamed('nguiwindow', true, 'drops02bLayer')
        --layer:setrect( 0.5, 1.0, 1.0, 2.0 )
        --layer:setdefaultbrush( 'rns_drops02' )
        --
        --layer = guiserver:newwindownamed('nguiwindow', true, 'drops02LayerBlur')
        --layer:setrect( 0.5, 1.0, 0.0, 1.0 )
        --layer:setdefaultbrush( 'rns_drops02b' )
        --
        --layer = guiserver:newwindownamed('nguiwindow', true, 'drops02bLayerBlur')
        --layer:setrect( 0.5, 1.0, 1.0, 2.0 )
        --layer:setdefaultbrush( 'rns_drops02b' )
        
        --layer = guiserver:newwindownamed('nguiwindow', true, 'crystalLayer')
        --layer:setrect( 1.0 - 149 * resMulX, 1.0, 0.0, 151 * resMulY )
        --layer:setdefaultbrush( 'rns_crystal' )
        
        layer = guiserver:newwindownamed('nguiwindow', true, 'bottomLayer')
        layer:setrect( 1.0 - 512 * resMulX, 1.0, 1.0 - 256 * resMulY, 1.01 )
        layer:setdefaultbrush( 'rns_bottom' )
        
        layer = guiserver:newwindownamed('nguiwindow', true, 'bottombLayer')
        layer:setrect( 1.0 - 512 * resMulX, 1.0, 1.0 - 256 * resMulY, 1.01 )
        layer:setdefaultbrush( 'rns_bottomb' )
        
        local buttonsWindow = guiserver:newwindownamed('nguiwindow', true, 'buttonsWindow')
        buttonsWindow:setrect( 0.0, 1.0, 0.0, 1.0 )
        
        pushcwd(buttonsWindow)
            local x0, y0, button
        
            -- Create start button
            button = newthunk('nguibutton', 'StartButton')
            x0 = 100 * resMulX
            y0 = 209 * resMulY
            button:setrect( x0, x0 + 200 * resMulX, y0, y0 + 50 * resMulY )
            button:setdefaultbrush('startbtn_n')
            button:setpressedbrush('startbtn_p')
            button:sethighlightbrush('startbtn_h')
            button:setdisabledbrush('startbtn_d')
            
            -- Create join button
            button = newthunk('nguibutton', 'JoinButton')
            x0 = 100 * resMulX
            y0 = 273 * resMulY
            button:setrect( x0, x0 + 200 * resMulX, y0, y0 + 50 * resMulY )
            button:setdefaultbrush('joinbtn_n')
            button:setpressedbrush('joinbtn_p')
            button:sethighlightbrush('joinbtn_h')
            button:setdisabledbrush('joinbtn_d')
            
            -- Create exit button
            button = newthunk('nguibutton', 'ExitButton')
            x0 = 100 * resMulX
            y0 = 335 * resMulY
            button:setrect( x0, x0 + 200 * resMulX, y0, y0 + 50 * resMulY )
            button:setdefaultbrush('exitbtn_n')
            button:setpressedbrush('exitbtn_p')
            button:sethighlightbrush('exitbtn_h')
            button:setdisabledbrush('exitbtn_d')

            -- Create cancel join button
            button = newthunk('nguibutton', 'CancelJoinButton')
            x0 = 100 * resMulX
            y0 = 664 * resMulY
            button:setrect( x0, x0 + 105 * resMulX, y0, y0 + 29 * resMulY )
            button:setdefaultbrush('canceljoinbtn_n')
            button:setpressedbrush('canceljoinbtn_p')
            button:sethighlightbrush('canceljoinbtn_h')
            button:setdisabledbrush('canceljoinbtn_d')
            
            -- Create ok join button
            button = newthunk('nguibutton', 'JoinOkButton')
            x0 = 214 * resMulX
            y0 = 664 * resMulY
            button:setrect( x0, x0 + 86 * resMulX, y0, y0 + 29 * resMulY )
            button:setdefaultbrush('joinokbtn_n')
            button:setpressedbrush('joinokbtn_p')
            button:sethighlightbrush('joinokbtn_h')
            button:setdisabledbrush('joinokbtn_d')
            
            -- Create ip up button
            button = newthunk('nguibutton', 'IpUpButton')
            x0 = 300 * resMulX
            y0 = 457 * resMulY
            button:setrect( x0, x0 + 30 * resMulX, y0, y0 + 30 * resMulY )
            button:setdefaultbrush('ipupbtn_n')
            button:setpressedbrush('ipupbtn_p')
            button:sethighlightbrush('ipupbtn_h')
            button:setdisabledbrush('ipupbtn_d')
            
            -- Create ip up button
            button = newthunk('nguibutton', 'IpDownButton')
            x0 = 300 * resMulX
            y0 = 628 * resMulY
            button:setrect( x0, x0 + 30 * resMulX, y0, y0 + 30 * resMulY )
            button:setdefaultbrush('ipdownbtn_n')
            button:setpressedbrush('ipdownbtn_p')
            button:sethighlightbrush('ipdownbtn_h')
            button:setdisabledbrush('ipdownbtn_d')
        popcwd() --buttonsWindow
    end
    
---------------------------------------------------------------------------------
---------------------------------------------------------------------------------
    local win = lookup(guiroot .. '/menuRootWindow')

    if not win then
        FirstCreate()
    end
    
    -- Set current root window
    guiserver:setrootwindow('menuRootWindow')
    local button
    
    local layer
    layer = lookup(guiroot .. '/menuRootWindow/ipsLayer')
    if layer then
        layer:hide()
    end
    
    button = lookup(guiroot .. '/menuRootWindow/buttonsWindow/StartButton')
    if button then
        pin( button )
        button:show()
        button:enable()
        button.state = self
        button.pressedFunction = startGamePressed
        button:setcommand([[nthis:pressedFunction()]])
    end
    
    button = lookup(guiroot .. '/menuRootWindow/buttonsWindow/JoinButton')
    if button then
        pin( button )
        button:show()
        button:enable()
        button.state = self
        button.pressedFunction = joinPressed
        button:setcommand([[nthis:pressedFunction()]])
    end
    
    button = lookup(guiroot .. '/menuRootWindow/buttonsWindow/ExitButton')
    if button then
        pin( button )
        button:show()
        button:enable()
        button.state = self
        button.pressedFunction = exitPressed
        button:setcommand([[nthis:pressedFunction()]])
    end

    button = lookup(guiroot .. '/menuRootWindow/buttonsWindow/CancelJoinButton')
    if button then
        pin( button )
        button:hide()
        button.state = self
        button.pressedFunction = cancelJoinPressed
        button:setcommand([[nthis:pressedFunction()]])
    end

    button = lookup(guiroot .. '/menuRootWindow/buttonsWindow/JoinOkButton')
    if button then
        pin( button )
        button:hide()
        button.state = self
        button.pressedFunction = joinOkPressed
        button:setcommand([[nthis:pressedFunction()]])
    end
    
    button = lookup(guiroot .. '/menuRootWindow/buttonsWindow/IpUpButton')
    if button then
        pin( button )
        button:hide()
        button.state = self
        button.pressedFunction = ipUpPressed
        button:setcommand([[nthis:pressedFunction()]])
    end
    
    button = lookup(guiroot .. '/menuRootWindow/buttonsWindow/IpDownButton')
    if button then
        pin( button )
        button:hide()
        button.state = self
        button.pressedFunction = ipDownPressed
        button:setcommand([[nthis:pressedFunction()]])
    end
    
    -- put default transparency
    skin:setbrushcolor( 'rns_menubg01', 1.0, 1.0, 1.0, 1.0 )

    -- play menu sound
    local music = nebula.usr.soundlibrary
    if music then
        music:stopallsounds()
        if menuMusic ~= -1 then
            music:playrepeat( menuMusic, 0 )
        end
    end
    
    pin( self )
    self.ipselected = nil
    self.min_ipindex = 0
    self.startGame = false
end
