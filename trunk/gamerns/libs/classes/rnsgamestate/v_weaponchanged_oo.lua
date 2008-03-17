---------------------------------------------------------------------------------
---------------------------------------------------------------------------------
function ( self, changedPlayer, changedWeapon )
    local rnsentitymanager = nebula.sys.servers.rnsentitymanager
    if rnsentitymanager then
        local player = rnsentitymanager:getlocalplayer()
        if player then
            local weapon = player:getcurrentweapon()
            if weapon then
                if weapon:getid() == changedWeapon:getid() and 
                    player:getid() == changedPlayer:getid() then
                    self:ToggleWeaponPanel( true )
                end
            end
        end
    end
end
