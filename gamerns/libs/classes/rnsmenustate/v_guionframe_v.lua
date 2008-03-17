---------------------------------------------------------------------------------
--   This procedure is called every frame
---------------------------------------------------------------------------------
function ( self )

    local guiroot = nebula.sys.servers.gui:getrootpath()
    
---------------------------------------------------------------------------------
---------------------------------------------------------------------------------
    function UpdateLayer( layer, delta )
        x0, x1, y0, y1 = layer:getrect()
        
        y0 = y0 - delta
        
        if y0 < -1.0 then
            y0 = 1.0
        end
        
        y1 = y0 + 1.0
        layer:setrect( x0, x1, y0, y1 )
    end

---------------------------------------------------------------------------------
---------------------------------------------------------------------------------
    function UpdateIpList()
        local network = lookup( '/sys/servers/network' )
        
        if self.iplist and network then
            local BUTTONS = 5
            local number = network:getnumservers()
            local servers = network:getservers()
            
            local oldIndex = self.min_ipindex
            
            if not self.min_ipindex then
                self.min_ipindex = 0
            end
            
            self.min_ipindex = math.min( self.min_ipindex, (number-1) - (BUTTONS-1)  )
            self.min_ipindex = math.max( self.min_ipindex, 0 )
            
            local maxnumber = math.min( self.min_ipindex + (BUTTONS-1), number - 1 )
            
            if self.ipselected then
                if self.ipselected < 1 or self.ipselected > BUTTONS then
                    self.ipselected = nil
                end
            end
            
            if oldIndex ~= self.min_ipindex then
                self.ipselected = nil
            end
            
            local ind_btn = 1
            
            if number > 0 then
                for i = self.min_ipindex, maxnumber do
                    self.iplist[ ind_btn ].label:settext( tostring( servers[ i ] ) )
                    self.iplist[ ind_btn ].button:enable()
                    if self.ipselected and ind_btn == self.ipselected then
                        self.iplist[ ind_btn ].button:setdefaultbrush('ipbtn_h')
                        self.iplist[ ind_btn ].button:setdefaultbrush('ipbtn_h')
                        self.iplist[ ind_btn ].button:setpressedbrush('ipbtn_h')
                        self.iplist[ ind_btn ].button:sethighlightbrush('ipbtn_h')
                    else
                        self.iplist[ ind_btn ].button:setdefaultbrush('ipbtn_n')
                        self.iplist[ ind_btn ].button:setdefaultbrush('ipbtn_n')
                        self.iplist[ ind_btn ].button:setpressedbrush('ipbtn_n')
                        self.iplist[ ind_btn ].button:sethighlightbrush('ipbtn_n')
                    end
                    ind_btn = ind_btn + 1
                end
            end
            
            for i = ind_btn, BUTTONS do
                if self.ipselected and ind_btn == self.ipselected then
                    self.ipselected = nil
                end
                self.iplist[ i ].label:settext( "< empty >" )
                self.iplist[ i ].button:disable()
            end
        
            button = lookup(guiroot .. '/menuRootWindow/buttonsWindow/IpUpButton')
            if button then
                if self.min_ipindex > 0 then
                    button:enable()
                else
                    button:disable()
                end
            end

            button = lookup(guiroot .. '/menuRootWindow/buttonsWindow/IpDownButton')
            if button then
                if number > 0 and maxnumber < (number - 1) then
                    button:enable()
                else
                    button:disable()
                end
            end
        end
        
        local button = lookup(guiroot .. '/menuRootWindow/buttonsWindow/JoinOkButton')
        if button then
            if self.ipselected then
                button:enable()
            else
                button:disable()
            end
        end
    end
    
---------------------------------------------------------------------------------
---------------------------------------------------------------------------------

    local delta
    if lookup( '/app/conjurer' ) then
        app = nebula.app.conjurer
    else
        app = nebula.app.renaissance
    end
    local delta = app:getframetime()
    if delta > 0.5 then
        delta = 0.5
    end
    
    self.lasttime = newtime
    
    local x0, x1, y0, y1
    
    local drops
    
    drops = lookup( guiroot .. '/menuRootWindow/drops01Layer')
    if drops then
        UpdateLayer( drops, 0.01*delta )
    end
    
    drops = lookup( guiroot .. '/menuRootWindow/drops01bLayer')
    if drops then
        UpdateLayer( drops, 0.01*delta )
    end
    
    drops = lookup( guiroot .. '/menuRootWindow/drops01LayerBlur')
    if drops then
        UpdateLayer( drops, 0.01*delta )
    end
    
    drops = lookup( guiroot .. '/menuRootWindow/drops01bLayerBlur')
    if drops then
        UpdateLayer( drops, 0.01*delta )
    end
    
    drops = lookup( guiroot .. '/menuRootWindow/drops02Layer')
    if drops then
        UpdateLayer( drops, 0.03*delta )
    end
    
    drops = lookup( guiroot .. '/menuRootWindow/drops02bLayer')
    if drops then
        UpdateLayer( drops, 0.03*delta )
    end

    drops = lookup( guiroot .. '/menuRootWindow/drops02LayerBlur')
    if drops then
        UpdateLayer( drops, 0.03*delta )
    end
    
    drops = lookup( guiroot .. '/menuRootWindow/drops02bLayerBlur')
    if drops then
        UpdateLayer( drops, 0.03*delta )
    end

    local guiserver = lookup( '/sys/servers/gui' )
    
    local skin = guiserver:getskin()
    if skin then
        local transp = 0.5 + 0.5 * math.sin( 0.3 * os.clock() )

    --  ZOMBIE REMOVE        
        --skin:setbrushcolor( 'rns_drops01', 1.0, 1.0, 1.0, transp )
        --skin:setbrushcolor( 'rns_drops02', 1.0, 1.0, 1.0, transp )
        --skin:setbrushcolor( 'rns_drops01b', 1.0, 1.0, 1.0, 1.0 - transp )
        --skin:setbrushcolor( 'rns_drops02b', 1.0, 1.0, 1.0, 1.0 - transp )
        skin:setbrushcolor( 'rns_menubg00', 1.0, 1.0, 1.0, transp )
        skin:setbrushcolor( 'rns_bottom', 1.0, 1.0, 1.0, 1.0 - transp )
        skin:setbrushcolor( 'rns_bottomb', 1.0, 1.0, 1.0, transp )
    end

    UpdateIpList()
end
