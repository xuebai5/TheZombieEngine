---------------------------------------------------------------------------------
--   "game" application state GUI initialization
--
--   This procedure is called when the state is entered
---------------------------------------------------------------------------------
function ( self, activate )
    local buttonSound = -1
    
    -- get sound index
    local music = nebula.usr.soundlibrary
    if music then
        buttonSound = music:getsoundindex( "IMP_054_G_Impact_Impact_MetalHard" )
    end
    
    local guiserver = lookup( '/sys/servers/gui' )

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
    if activate then
        if lookup(guiroot .. '/menuRootWindow') then
            local button
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
            
            button = lookup(guiroot .. '/menuRootWindow/buttonsWindow/ExitButton')
            if button then
                pin( button )
                button:show()
                button:enable()
                button.state = self
                button.pressedFunction = exitPressed
                button:setcommand([[nthis:pressedFunction()]])
            end
            
            guiserver:setrootwindow('menuRootWindow')
            
            local skin = guiserver:getskin()
            if skin then
            --  ZOMBIE REMOVE
                --skin:setbrushcolor( 'rns_drops01', 1.0, 1.0, 1.0, 0.0 )
                --skin:setbrushcolor( 'rns_drops02', 1.0, 1.0, 1.0, 0.0 )
                --skin:setbrushcolor( 'rns_drops01b', 1.0, 1.0, 1.0, 0.0 )
                --skin:setbrushcolor( 'rns_drops02b', 1.0, 1.0, 1.0, 0.0 )
                skin:setbrushcolor( 'rns_menubg00', 1.0, 1.0, 1.0, 0.0 )
                skin:setbrushcolor( 'rns_bottom', 1.0, 1.0, 1.0, 0.0 )
                skin:setbrushcolor( 'rns_bottomb', 1.0, 1.0, 1.0, 0.0 )
                
                local layer = lookup(guiroot .. '/menuRootWindow/ipsLayer')
                if layer then
                    layer:hide()
                end
            end
        end
    else
        if lookup(guiroot .. '/gameRootWindow') then
            guiserver:setrootwindow('gameRootWindow')
        end
    end
end