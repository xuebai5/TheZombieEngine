---------------------------------------------------------------------------------
---------------------------------------------------------------------------------
function ( self, show )
    if self and self.weaponPanel then
        if show and 
            ( self.weaponPanel.isShowing or self.weaponPanel.isWaiting ) then
            return
        end
        
        if not show and self.weaponPanel.isHiding then
            return
        end
        
        self.weaponPanel.isShowing = show
        self.weaponPanel.isHiding = not show
        self.weaponPanel.isWaiting = false
        self.weaponPanel.stateTime = 0
    end
end