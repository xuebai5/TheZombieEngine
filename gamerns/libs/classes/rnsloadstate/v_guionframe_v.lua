---------------------------------------------------------------------------------
--   "load" application state GUI frame call
---------------------------------------------------------------------------------
function ( self )
    local percent = self:getpercent()
    
    local guiroot = nebula.sys.servers.gui:getrootpath()
    
    local loadBar = lookup( guiroot .. '/loadRootWindow/LoadBar')
    if loadBar then
        loadBar:setfillstate( percent / 100 )
    end
end
