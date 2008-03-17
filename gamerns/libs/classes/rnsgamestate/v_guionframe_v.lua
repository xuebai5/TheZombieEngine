---------------------------------------------------------------------------------
--   "game" application state GUI
--
--   This procedure is called per frame
---------------------------------------------------------------------------------
function ( self )

    local guiroot = nebula.sys.servers.gui:getrootpath()
    
---------------------------------------------------------------------------------
---------------------------------------------------------------------------------
    function UpdateWeaponPanel( delta )
        layer = lookup( guiroot .. '/gameRootWindow/WeaponPanel')
        if layer then
            self.weaponPanel.stateTime = self.weaponPanel.stateTime + delta
            
            local x0a, x1a, x0b, x1b, x0, x1, y0, y1
            
            local animate = false
            local showStats = false
            
            local t = self.weaponPanel.stateTime
            local tf = self.weaponPanel.animDuration
            
            if self.weaponPanel.isHiding then
                x0a = self.weaponPanel.showPositionX0
                x1a = self.weaponPanel.showPositionX1
                x0b = self.weaponPanel.hidePositionX0
                x1b = self.weaponPanel.hidePositionX1
                if t > tf then
                    self.weaponPanel.isHiding = false
                    t = tf
                end
                animate = true
                
            elseif self.weaponPanel.isShowing then
                x0a = self.weaponPanel.hidePositionX0
                x1a = self.weaponPanel.hidePositionX1
                x0b = self.weaponPanel.showPositionX0
                x1b = self.weaponPanel.showPositionX1
                if t > tf then
                    self.weaponPanel.isShowing = false
                    self.weaponPanel.isWaiting = true
                    self.weaponPanel.stateTime = 0
                    t = tf
                end
                animate = true
                
            elseif self.weaponPanel.isWaiting then
                showStats = true
                if t > self.weaponPanel.waitDuration then
                    self.weaponPanel.isWaiting = false
                    self.weaponPanel.isHiding = true
                    self.weaponPanel.stateTime = 0
                end
            end
            
            if animate then
                x0, x1, y0, y1 = layer:getrect()
                
                x0 = ( x0b - x0a ) * ( t / tf ) + x0a
                x1 = ( x1b - x1a ) * ( t / tf ) + x1a

                layer:setrect( x0, x1, y0, y1 )
            end
            
            local accuracyLabel = lookup( guiroot .. '/gameRootWindow/WeaponAccuracy')
            local stabilityLabel = lookup( guiroot .. '/gameRootWindow/WeaponStability')
            local aimingLabel = lookup( guiroot .. '/gameRootWindow/WeaponAiming')
            if accuracyLabel and stabilityLabel and aimingLabel then
                if showStats then
                    accuracyLabel:show()
                    stabilityLabel:show()
                    aimingLabel:show()
                    local rnsentitymanager = nebula.sys.servers.rnsentitymanager
                    if rnsentitymanager then
                        local player = rnsentitymanager:getlocalplayer()
                        if player then
                            local weapon = player:getcurrentweapon()
                            if weapon then
                                local value
                                value = weapon:getaccuracy()
                                value = math.floor( ( value * 70 ) / 0.88 )
                                accuracyLabel:settext( 'Accuracy : ' .. value .. '%' )

                                value = weapon:getrecoildeviationy()
                                value = math.floor( ( value * 55 ) / 0.035 )
                                stabilityLabel:settext( 'Stability : ' .. value .. '%' )

                                value = weapon:getrecoiltime()
                                value = math.floor( ( value * 70 ) / 0.1 )
                                aimingLabel:settext( 'Aiming : ' .. value .. '%' )
                            end
                        end
                    end
                else
                    accuracyLabel:hide()
                    stabilityLabel:hide()
                    aimingLabel:hide()
                end
            end
        end
    end

---------------------------------------------------------------------------------
---------------------------------------------------------------------------------
    local curTime = nebula.sys.servers.time:gettime()

    if curTime - lastUpdateTime > 0.1 then    
        local rnsentitymanager = nebula.sys.servers.rnsentitymanager
        if rnsentitymanager ~= nil then
            -- update numeric displays
            local player = rnsentitymanager:getlocalplayer()
            local playercl = nil
            if player then
                playercl = player:getentityclass()
            end
            
            if player and playercl then
                healthBar:setfillstate( player:gethealth() / playercl:getmaxhealth() )
                staminaBar:setfillstate( player:getstamina() / playercl:getmaxstamina() )

                local weapon = player:getcurrentweapon()
                if ( weapon ) then
                    -- update ammo clip and extra clip
                    local ammo1 = weapon:getammo()
                    if ammo1 > 99 then
                            ammo1 = 99
                        end
                    if ( weapon:getammo() < 10 ) then
                        ammo1 = '0' .. ammo1
                    end
                    firstClip:settext( ammo1 )

                    -- check if weapon has clamp (quick reload trait)
                    if weapon:hastrait( 32 ) then
                        if not secondClipEnabled then
                            secondClipEnabled = true
                            secondClip:setcolor(1,1,1,1)
                            iconClipSmall:setcolor(1,1,1,1)
                            clampedClip:setcolor(1,1,1,1)
                            chainClip:setcolor(1,1,1,1)
                        end
                    else
                        if secondClipEnabled then
                            secondClipEnabled = false
                            secondClip:settext( '00' )
                            secondClip:setcolor(1,1,1,0.5)
                            iconClipSmall:setcolor(1,1,1,0.5)
                            clampedClip:setcolor(1,1,1,0.5)
                            chainClip:setcolor(1,1,1,0.5)
                        end
                    end
                    if secondClipEnabled then
                        local ammo2 = weapon:getammoextra()
                        if ammo2 > 99 then
                            ammo2 = 99
                        end
                        if ( weapon:getammoextra() < 10 ) then
                            ammo2 = '0' .. ammo2
                        end
                        secondClip:settext( ammo2 )
                        lastUpdateTime = curTime
                    end
                    
                    -- update inventory ammo pool
                    local inventory = player:getinventory()
                    if ( inventory ) then
                        local invBullets = inventory:getnumbullets( weapon:getammotype() )
                        if invBullets > 999 then
                            invBullets = 999
                        end
                        local invBulletsStr = invBullets
                        if ( invBullets < 10 ) then
                            invBulletsStr = '00'..invBulletsStr
                        elseif ( invBullets < 100 ) then
                            invBulletsStr = '0'..invBulletsStr
                        end
                        ammoPool:settext( invBulletsStr )
    
                        -- todo: where are the grenades?
                    end
                    
                    -- update weapon fire mode
                    local fireMode = weapon:getfiremode()
                    
                    if fireMode ~= currentFireMode then
                        if fireMode == 1 then
                            labelFullAuto:show()
                            labelSemiAuto:hide()
                        else
                            labelFullAuto:hide()
                            labelSemiAuto:show()
                        end
                        currentFireMode = fireMode
                    end
                end
            end
        end

    end

--  ZOMBIE REMOVE
--    if accessoryTimerEnabled then
--        nebula.gui.GuiSignalReceiver:refreshaccessorytimer()
--    end

--    if nebula.sys.servers.input:getbutton('MapMissionInfo') then
--        nebula.gui.GuiSignalReceiver:initobjectivesanim( not objectivesPanelShown )
--    end

--    if objectivesPanelAnimationEnabled then
--        nebula.gui.GuiSignalReceiver:objectivesanim()
--    end

    local delta
    if (lookup('/app/conjurer')) then
        delta = nebula.app.conjurer:getframetime()
    else
        delta = nebula.app.renaissance:getframetime()
    end
    
    UpdateWeaponPanel( delta )
end
