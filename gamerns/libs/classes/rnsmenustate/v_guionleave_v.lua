---------------------------------------------------------------------------------
--   This procedure is called when the state is leave
---------------------------------------------------------------------------------
function ( self )
    local guiserver = lookup( '/sys/servers/gui' )

    local button
    button = lookup(guiroot .. '/menuRootWindow/buttonsWindow/StartButton')
    if button then
        unpin( button )
    end
    
    button = lookup(guiroot .. '/menuRootWindow/buttonsWindow/JoinButton')
    if button then
        unpin( button )
    end
    
    button = lookup(guiroot .. '/menuRootWindow/buttonsWindow/ExitButton')
    if button then
        unpin( button )
    end
    
    button = lookup(guiroot .. '/menuRootWindow/buttonsWindow/CancelButton')
    if button then
        unpin( button )
    end

    button = lookup(guiroot .. '/menuRootWindow/buttonsWindow/JoinOkButton')
    if button then
        unpin( button )
    end
    
    button = lookup(guiroot .. '/menuRootWindow/buttonsWindow/ipUpButton')
    if button then
        unpin( button )
    end
    
    button = lookup(guiroot .. '/menuRootWindow/buttonsWindow/ipDownButton')
    if button then
        unpin( button )
    end
    
    if not self.startGame then
        local network = lookup( '/sys/servers/network' )
        if network then
            delete( network )
        end
    end
        
    -- clear ip list
    if self.iplist then
        for i = 1,5 do
            if self.iplist[ i ] then
                delete( self.iplist[ i ].label )
                delete( self.iplist[ i ].button )
            end
        end
        
        self.iplist = nil
    end
    
    -- quit menu sound
    local music = nebula.usr.soundlibrary
    if music then
        music:stopallsounds()
    end
    
    unpin( self )
end