---------------------------------------------------------------------------------
--   "game" application state GUI initialization
--
--   This procedure is called when the state is entered
---------------------------------------------------------------------------------
function ( self )

    local rw = lookup('/gui/gameRootWindow')
    if (not rw) then

        local guiroot = nebula.sys.servers.gui:getrootpath()
        luaSel(guiroot)

        -- Select resolution multiplier
        local screenWidth = 1.0
        local screenHeight = 1.0
        local wTitle, wType, scrPosX, scrPosY, screenWidth, screenHeight, vertSync = nebula.sys.servers.gfx:getdisplaymode()
        -- Constant GUI size (variable relative size)
        resMulX = 1.0 / screenWidth
        resMulY = 1.0 / screenHeight
        -- If resolution lower than 1024x768 (800x600 or 640x480), do constant relative size instead
        if ( screenWidth < 1024 and screenHeight / screenWidth == 0.75) then
            resMulX = 1.0 / 1024.0
            resMulY = 1.0 / 768.0
        end

        -- Timer counters and global variables
        
        --Numeric display last update time
        lastUpdateTime = 0
        secondClipEnabled = false
        
        -- accessory timer
        accessoryTimerEnabled = false
        accessoryTimerInitTime = 0
        accessoryTimerDuration = 0
        currentAccessoryIcon = 0
        
        -- objectives panel
        objectivesPanelInitTime = 0
        objectivesPanelAnimDuration = 0.25
        objectivesPanelAnimationEnabled = false
        objectivesPanelShown = false
        objectivesPanelOutPos = 322.0 * resMulX

        -- Create game root window    
        nebula.sys.servers.gui:setrootwindow('')
        local rootWindow = nebula.sys.servers.gui:newwindownamed('nguiwindow', true, 'gameRootWindow')
        rootWindow:setrect( 0.0, 1.0, 0.0, 1.0 )

        pushcwd(rootWindow)
    
            -- Create message window
            nebula.sys.servers.gui:setrootwindow('')
            local messageWindow = nebula.sys.servers.kernelserver:new('nguitextlabel', 'MessageWindow')
            messageWindow:show()
            --messageWindow:setrect( 0.05, 0.45, 0.6, 0.85 )
            messageWindow:setrect( 0.22, 0.85, 0.78, 0.92 )
            messageWindow:setwordbreak(true)
            messageWindow:setfont('arial24')
            messageWindow:setcolor(1.0,1.0,1.0,1.0)
            messageWindow:setalignment('left')

            -- Create health display
            local healthPanel = nebula.sys.servers.kernelserver:new('nguiwindow', 'HealthDisplay')
            healthPanel:setdefaultbrush('panelhealth')
            local x0 = 1.0 - 185.0 * resMulX
            local y0 = 16.0 * resMulY
            healthPanel:setrect( x0, 1.0, y0, y0 + 106.0 * resMulY )
            healthPanel:show()
            pushcwd( healthPanel )

                -- Create health bar
                healthBar = nebula.sys.servers.kernelserver:new('nguibar', 'HealthBar')
                healthBar:setdefaultbrush('healthbar_n')
                healthBar:setdisabledbrush('healthbar_d')
                local x1 = 76.0 * resMulX
                local y1 = 34.0 * resMulY
                healthBar:setrect( x1, x1 + 6 * 16.0 * resMulX, y1, y1 + 1 * 16.0 * resMulY )
                healthBar:setfillstate(1.0)
                
                -- Create stamina bar
                staminaBar = nebula.sys.servers.kernelserver:new('nguibar', 'StaminaBar')
                staminaBar:setdefaultbrush('staminabar_n')
                staminaBar:setdisabledbrush('staminabar_d')
                x1 = 77.0 * resMulX
                y1 = 58.0 * resMulY
                staminaBar:setrect( x1, x1 + 6 * 16.0 * resMulX, y1, y1 + 1 * 16.0 * resMulY )
                staminaBar:setfillstate(1.0)

            popcwd()
            
            -- Create ammo display
            local ammoDisplay = nebula.sys.servers.kernelserver:new('nguiwindow', 'AmmoDisplay')
            ammoDisplay:setdefaultbrush('panelammunition')
            x0 = 0.0 * resMulX
            y0 = 1.0 - (160.0 + 16.0) * resMulY
            ammoDisplay:setrect( x0, x0 + 185.0 * resMulX, y0, y0 + 160.0 * resMulY )
            ammoDisplay:show()
            pushcwd( ammoDisplay )

                firstClip = nebula.sys.servers.kernelserver:new('nguibrushlabel', 'FirstClip')
                firstClip:settypefacename('rnsguifont01')
                firstClip:setbrushsize(32,32)
                firstClip:setcharseparation(-12.0/32.0)
                firstClip:addbrushdefinition('0','9',0,0,32,32,'rns_interface_objects')
                x1 = 29.0 * resMulX
                y1 = 20.0 * resMulY
                firstClip:setrect( x1, x1 + 2 * 32.0 * resMulX, y1, y1 + 32.0 * resMulY )
                firstClip:settext('99')

                secondClip = nebula.sys.servers.kernelserver:new('nguibrushlabel', 'SecondClip')
                secondClip:settypefacename('rnsguifont02')
                secondClip:setbrushsize(16,16)
                secondClip:setcharseparation(-2.0/16.0)
                secondClip:addbrushdefinition('0','9',0,32,16,16,'rns_interface_objects')
                x1 = 109.0 * resMulX
                y1 = 28.0 * resMulY
                secondClip:setrect( x1, x1 + 2 * 16.0 * resMulX, y1, y1 + 16.0 * resMulY )
                secondClip:settext('99')
                
                ammoPool = nebula.sys.servers.kernelserver:new('nguibrushlabel', 'AmmoPool')
                ammoPool:settypefacename('rnsguifont01')
                ammoPool:setbrushsize(32,32)
                ammoPool:setcharseparation(-12.0/32.0)
                ammoPool:addbrushdefinition('0','9',0,0,32,32,'rns_interface_objects')
                x1 = 18.0 * resMulX
                y1 = 108.0 * resMulY
                ammoPool:setrect( x1, x1 + 3 * 32.0 * resMulX, y1, y1 + 32.0 * resMulY )
                ammoPool:settext('999')

                grenadeAmmoPool = nebula.sys.servers.kernelserver:new('nguibrushlabel', 'GrenadeAmmoPool')
                grenadeAmmoPool:settypefacename('rnsguifont01')
                grenadeAmmoPool:setbrushsize(32,32)
                grenadeAmmoPool:setcharseparation(-12.0/32.0)
                grenadeAmmoPool:addbrushdefinition('0','9',0,0,32,32,'rns_interface_objects')
                x1 = 111.0 * resMulX
                y1 = 108.0 * resMulY
                grenadeAmmoPool:setrect( x1, x1 + 2 * 32.0 * resMulX, y1, y1 + 32.0 * resMulY )
                grenadeAmmoPool:settext('99')

                local iconClipLarge = nebula.sys.servers.kernelserver:new('nguilabel', 'IconClipLarge')
                iconClipLarge:setdefaultbrush('cliplarge')
                x1 = 41.0 * resMulX
                y1 = 42.0 * resMulY
                iconClipLarge:setrect( x1, x1 + 2 * 16.0 * resMulX, y1, y1 + 5 * 16.0 * resMulY )

                iconClipSmall = nebula.sys.servers.kernelserver:new('nguilabel', 'IconClipSmall')
                iconClipSmall:setdefaultbrush('clipsmall')
                x1 = 103.0 * resMulX
                y1 = 45.0 * resMulY
                iconClipSmall:setrect( x1, x1 + 3 * 16.0 * resMulX, y1, y1 + 4 * 16.0 * resMulY )

                clampedClip = nebula.sys.servers.kernelserver:new('nguilabel', 'IconClampClip')
                clampedClip:setdefaultbrush('clampclip')
                x1 = 70.0 * resMulX
                y1 = 42.0 * resMulY
                clampedClip:setrect( x1, x1 + 3 * 16.0 * resMulX, y1, y1 + 3 * 16.0 * resMulY )

                chainClip = nebula.sys.servers.kernelserver:new('nguilabel', 'IconChainClip')
                chainClip:setdefaultbrush('chainclip')
                x1 = 79.0 * resMulX
                y1 = 84.0 * resMulY
                chainClip:setrect( x1, x1 + 2 * 16.0 * resMulX, y1, y1 + 1 * 16.0 * resMulY )

                labelSemiAuto = nebula.sys.servers.kernelserver:new('nguilabel', 'LabelSemiAuto')
                labelSemiAuto:setdefaultbrush('labelsemiauto')
                x1 = 62.0 * resMulX
                y1 = 102.0 * resMulY
                labelSemiAuto:setrect( x1, x1 + 4 * 16.0 * resMulX, y1, y1 + 1 * 16.0 * resMulY )
                labelSemiAuto:hide()

                labelFullAuto = nebula.sys.servers.kernelserver:new('nguilabel', 'LabelFullAuto')
                labelFullAuto:setdefaultbrush('labelfullauto')
                x1 = 62.0 * resMulX
                y1 = 102.0 * resMulY
                labelFullAuto:setrect( x1, x1 + 4 * 16.0 * resMulX, y1, y1 + 1 * 16.0 * resMulY )

--                local iconBullets = nebula.sys.servers.kernelserver:new('nguilabel', 'IconBullets')
--                iconBullets:setdefaultbrush('bullets')
--                x1 = 11.0 * resMulX
--                y1 = 108.0 * resMulY
--                iconBullets:setrect( x1, x1 + 2 * 16.0 * resMulX, y1, y1 +  2 * 16.0 * resMulY )
--                
--                local iconGrenade = nebula.sys.servers.kernelserver:new('nguilabel', 'IconGrenade')
--                iconGrenade:setdefaultbrush('grenade')
--                x1 = 111.0 * resMulX
--                y1 = 108.0 * resMulY
--                iconGrenade:setrect( x1, x1 + 1 * 16.0 * resMulX, y1, y1 + 2 * 16.0 * resMulY )
                
            popcwd()

            -- Create accessory timer display
            local accessoryTimerDisplay = nebula.sys.servers.kernelserver:new('nguiwindow', 'accessoryTimerDisplay')
            x0 = 0.5 - 45.0 * resMulX
            y0 = ( 699.0 / 764.0 ) - 45.0 * resMulY
            accessoryTimerDisplay:setrect( x0, x0 + 91.0 * resMulX, y0, y0 + 91.0 * resMulY )
            accessoryTimerDisplay:show()
            pushcwd( accessoryTimerDisplay )
            
                -- Create accessory timer icons
                accessoryTimerIcon = { }
                for i = 1, 8 do
                    local accIcon = nebula.sys.servers.kernelserver:new('nguilabel', 'accessoryTimerIcon' .. i)
                    accIcon:setdefaultbrush('timer'..i)
                    accIcon:sethighlightbrush('timer'..i)
                    accessoryTimerIcon[i] = accIcon
                end
                
                x1 = 46.0 * resMulX
                y1 = -6.0 * resMulY
                accessoryTimerIcon[1]:setrect( x1, x1 + 2 * 16.0 * resMulX, y1, y1 +  2 * 16.0 * resMulY )
                
                x1 = 66.0 * resMulX
                y1 = 16.0 * resMulY
                accessoryTimerIcon[2]:setrect( x1, x1 + 2 * 16.0 * resMulX, y1, y1 +  2 * 16.0 * resMulY )
                
                x1 = 65.0 * resMulX
                y1 = 45.0 * resMulY
                accessoryTimerIcon[3]:setrect( x1, x1 + 2 * 16.0 * resMulX, y1, y1 +  2 * 16.0 * resMulY )
                
                x1 = 45.0 * resMulX
                y1 = 65.0 * resMulY
                accessoryTimerIcon[4]:setrect( x1, x1 + 2 * 16.0 * resMulX, y1, y1 +  2 * 16.0 * resMulY )
                
                x1 = 15.0 * resMulX
                y1 = 65.0 * resMulY
                accessoryTimerIcon[5]:setrect( x1, x1 + 2 * 16.0 * resMulX, y1, y1 +  2 * 16.0 * resMulY )
                
                x1 = -7.0 * resMulX
                y1 = 45.0 * resMulY
                accessoryTimerIcon[6]:setrect( x1, x1 + 2 * 16.0 * resMulX, y1, y1 +  2 * 16.0 * resMulY )
                
                x1 = -7.0 * resMulX
                y1 = 14.0 * resMulY
                accessoryTimerIcon[7]:setrect( x1, x1 + 2 * 16.0 * resMulX, y1, y1 +  2 * 16.0 * resMulY )
                
                x1 = 14.0 * resMulX
                y1 = -5.0 * resMulY
                accessoryTimerIcon[8]:setrect( x1, x1 + 2 * 16.0 * resMulX, y1, y1 +  2 * 16.0 * resMulY )

                -- Create accessory icons
                accessoryIcon = { }

                accessoryIcon[1] = nebula.sys.servers.kernelserver:new('nguilabel', 'IconGrenadeLauncher')
                accessoryIcon[1]:setdefaultbrush('addongrenadelauncher')
                x1 = 5 * 16.0 * resMulX / 2
                y1 = 2 * 16.0 * resMulY / 2
                accessoryIcon[1]:setrect( -x1 + 91.0 / 2 * resMulX, x1 + 91.0 / 2 * resMulX, -y1 + 91.0 / 2 * resMulY, y1 + 91.0 / 2 * resMulY )

                accessoryIcon[2] = nebula.sys.servers.kernelserver:new('nguilabel', 'IconFlashLight')
                accessoryIcon[2]:setdefaultbrush('addonflashlight')
                x1 = 3 * 16.0 * resMulX / 2
                y1 = 3 * 16.0 * resMulY / 2
                accessoryIcon[2]:setrect( -x1 + 91.0 / 2 * resMulX, x1 + 91.0 / 2 * resMulX, -y1 + 91.0 / 2 * resMulY, y1 + 91.0 / 2 * resMulY )

                accessoryIcon[3] = nebula.sys.servers.kernelserver:new('nguilabel', 'IconHiCap')
                accessoryIcon[3]:setdefaultbrush('addonhicap')
                x1 = 3 * 16.0 * resMulX / 2
                y1 = 3 * 16.0 * resMulY / 2
                accessoryIcon[3]:setrect( -x1 + 91.0 / 2 * resMulX, x1 + 91.0 / 2 * resMulX, -y1 + 91.0 / 2 * resMulY, y1 + 91.0 / 2 * resMulY )
                
                accessoryIcon[4] = nebula.sys.servers.kernelserver:new('nguilabel', 'IconSilencer')
                accessoryIcon[4]:setdefaultbrush('addonsilencer')
                x1 = 4 * 16.0 * resMulX / 2
                y1 = 3 * 16.0 * resMulY / 2
                accessoryIcon[4]:setrect( -x1 + 91.0 / 2 * resMulX, x1 + 91.0 / 2 * resMulX, -y1 + 91.0 / 2 * resMulY, y1 + 91.0 / 2 * resMulY )
                
                accessoryIcon[5] = nebula.sys.servers.kernelserver:new('nguilabel', 'IconReflex')
                accessoryIcon[5]:setdefaultbrush('addonreflex')
                x1 = 4 * 16.0 * resMulX / 2
                y1 = 3 * 16.0 * resMulY / 2
                accessoryIcon[5]:setrect( -x1 + 91.0 / 2 * resMulX, x1 + 91.0 / 2 * resMulX, -y1 + 91.0 / 2 * resMulY, y1 + 91.0 / 2 * resMulY )
                
                accessoryIcon[6] = nebula.sys.servers.kernelserver:new('nguilabel', 'IconRedDot')
                accessoryIcon[6]:setdefaultbrush('addonreddotlaser')
                x1 = 3 * 16.0 * resMulX / 2
                y1 = 3 * 16.0 * resMulY / 2
                accessoryIcon[6]:setrect( -x1 + 91.0 / 2 * resMulX, x1 + 91.0 / 2 * resMulX, -y1 + 91.0 / 2 * resMulY, y1 + 91.0 / 2 * resMulY )
                
                accessoryIcon[7] = nebula.sys.servers.kernelserver:new('nguilabel', 'IconScope')
                accessoryIcon[7]:setdefaultbrush('addonscope')
                x1 = 4 * 16.0 * resMulX / 2
                y1 = 4 * 16.0 * resMulY / 2
                accessoryIcon[7]:setrect( -x1 + 91.0 / 2 * resMulX, x1 + 91.0 / 2 * resMulX, -y1 + 91.0 / 2 * resMulY, y1 + 91.0 / 2 * resMulY )
                
                accessoryIcon[8] = nebula.sys.servers.kernelserver:new('nguilabel', 'IconFlechette')
                accessoryIcon[8]:setdefaultbrush('addonflechette')
                x1 = 3 * 16.0 * resMulX / 2
                y1 = 3 * 16.0 * resMulY / 2
                accessoryIcon[8]:setrect( -x1 + 91.0 / 2 * resMulX, x1 + 91.0 / 2 * resMulX, -y1 + 91.0 / 2 * resMulY, y1 + 91.0 / 2 * resMulY )
                
                accessoryIcon[9] = nebula.sys.servers.kernelserver:new('nguilabel', 'IconClamp')
                accessoryIcon[9]:setdefaultbrush('addonclamp')
                x1 = 3 * 16.0 * resMulX / 2
                y1 = 3 * 16.0 * resMulY / 2
                accessoryIcon[9]:setrect( -x1 + 91.0 / 2 * resMulX, x1 + 91.0 / 2 * resMulX, -y1 + 91.0 / 2 * resMulY, y1 + 91.0 / 2 * resMulY )
                
                accessoryIcon[10] = nebula.sys.servers.kernelserver:new('nguilabel', 'IconBipod')
                accessoryIcon[10]:setdefaultbrush('addonbipod')
                x1 = 4 * 16.0 * resMulX / 2
                y1 = 4 * 16.0 * resMulY / 2
                accessoryIcon[10]:setrect( -x1 + 91.0 / 2 * resMulX, x1 + 91.0 / 2 * resMulX, -y1 + 91.0 / 2 * resMulY, y1 + 91.0 / 2 * resMulY )
                
                accessoryIcon[11] = nebula.sys.servers.kernelserver:new('nguilabel', 'IconForeGrip')
                accessoryIcon[11]:setdefaultbrush('addonforegrip')
                x1 = 3 * 16.0 * resMulX / 2
                y1 = 4 * 16.0 * resMulY / 2
                accessoryIcon[11]:setrect( -x1 + 91.0 / 2 * resMulX, x1 + 91.0 / 2 * resMulX, -y1 + 91.0 / 2 * resMulY, y1 + 91.0 / 2 * resMulY )
         
            popcwd()

            -- Objectives panel
            local missionObjectivesPanel = nebula.sys.servers.kernelserver:new('nguiwindow', 'MissionObjectivesPanel')
            missionObjectivesPanel:setdefaultbrush('panelobjectives')
            missionObjectivesPanel:setrect( -objectivesPanelOutPos, 0.0, 16.0 * resMulY, (442.0 + 16.0) * resMulY )

            pushcwd( missionObjectivesPanel )
            
                local missionObjectivesLayout = nebula.sys.servers.kernelserver:new('nguilinelayout', 'MissionObjectivesLayout')
                missionObjectivesLayout:setorientation(1)
                missionObjectivesLayout:setrect( 1.0 * 16.0 * resMulX, 19.0 * 16.0 * resMulX, 5.0 * 16.0 * resMulY, 28.0 * 16.0 * resMulY )

            popcwd()
            
            -- Weapon Stats panel
            local weaponPanel = nebula.sys.servers.kernelserver:new('nguiwindow', 'WeaponPanel')
            weaponPanel:setdefaultbrush('panelweapon')
            weaponPanel:setrect( 0.7, 1.0, 0.7, 0.9 )
            
            x0 = 1.0 - 187 * resMulX
            x1 = 1.0
            y0 = 1.0 - (160.0 + 16.0) * resMulY
            weaponPanel:setrect( x0, x1, y0, y0 + 160.0 * resMulY )
            weaponPanel:show()

            local weaponLabel
            weaponLabel = newthunk('nguitextlabel', 'WeaponAccuracy')
            weaponLabel:hide()
            y0 = y0 + 0.04
            weaponLabel:setrect( x0, x1, y0, y0 + 0.04 )
            weaponLabel:setwordbreak(false)
            weaponLabel:setfont('courier15')
            weaponLabel:setcolor(1.0,1.0,1.0,1.0)
            weaponLabel:setalignment('center')
            weaponLabel:settext('Accuracy : ')
            
            weaponLabel = newthunk('nguitextlabel', 'WeaponStability')
            weaponLabel:hide()
            y0 = y0 + 0.04
            weaponLabel:setrect( x0, x1, y0, y0 + 0.04 )
            weaponLabel:setwordbreak(false)
            weaponLabel:setfont('courier15')
            weaponLabel:setcolor(1.0,1.0,1.0,1.0)
            weaponLabel:setalignment('center')
            weaponLabel:settext('Stability : ')
            
            weaponLabel = newthunk('nguitextlabel', 'WeaponAiming')
            weaponLabel:hide()
            y0 = y0 + 0.04
            weaponLabel:setrect( x0, x1, y0, y0 + 0.04 )
            weaponLabel:setwordbreak(false)
            weaponLabel:setfont('courier15')
            weaponLabel:setcolor(1.0,1.0,1.0,1.0)
            weaponLabel:setalignment('center')
            weaponLabel:settext('Aiming : ')
        popcwd()
-- ZOMBIE REMOVE
        -- Create object that receives signals and refreshes GUI elements
--        guiSignalReceiver = nebula.sys.servers.kernelserver:new('Nguisignalreceiver', '/gui/GuiSignalReceiver')
--        nebula.sys.servers.missionhandler:bindsignal('objectivestatechanged',guiSignalReceiver,'refreshobjectives', 0)

--        guiSignalReceiver:recreateobjectives()

        popcwd()
        
    end

    -- Initialization done every time the state is entered

    -- Set current root window
    nebula.sys.servers.gui:setrootwindow('gameRootWindow')

    -- Bind signals
--  ZOMBIE REMOVE
--    nebula.sys.servers.rnsentitymanager:bindsignal('mountingaccessory',guiSignalReceiver,'initaccessorytimer', 0)
--    nebula.sys.servers.rnsentitymanager:bindsignal('cancelaccessory',guiSignalReceiver,'cancelaccessorytimer', 0)
    nebula.sys.servers.rnsentitymanager:bindsignal('weaponchanged',self,'weaponchanged', 0)
--    nebula.sys.servers.gamemessagewindowproxy:bindsignal('ongamemessage',guiSignalReceiver,'showgamemessage', 0)

    -- Timers, animations disabled
    lastUpdateTime = 0
    accessoryTimerEnabled = false

    -- Secondary clip    
    secondClipEnabled = false
    secondClip:settext( '00' )
    secondClip:setcolor(1,1,1,0.5)
    iconClipSmall:setcolor(1,1,1,0.5)
    clampedClip:setcolor(1,1,1,0.5)
    chainClip:setcolor(1,1,1,0.5)

    -- Fire mode
    currentFireMode = 0
    labelFullAuto:show()
    labelSemiAuto:hide()

    -- Set objectives panel position
    objectivesPanelAnimationEnabled = false
    objectivesPanelShown = false
    local x0, x1, y0, y1 = nebula.gui.gameRootWindow.MissionObjectivesPanel:getrect()
    x0 = - objectivesPanelOutPos
    x1 = x0 + objectivesPanelOutPos
    nebula.gui.gameRootWindow.MissionObjectivesPanel:setrect(x0, x1, y0, y1)
    
    -- Hide accessory icons
    for i = 1, 8 do
        accessoryTimerIcon[i]:hide()
    end
    for i=1, 11 do
        accessoryIcon[i]:hide()
    end
    
    -- weapon stats variables
    self.weaponPanel = {
        stateTime = 0,
        animDuration = 0.25,
        waitDuration = 5.0,
        isHiding = true,
        isShowing = false,
        showPositionX0 = 1.0 - 187 * resMulX,
        showPositionX1 = 1.0,
        hidePositionX0 = 1.0,
        hidePositionX1 = 1.0 + 187 * resMulX,
    }
    
    pin( self )
end
