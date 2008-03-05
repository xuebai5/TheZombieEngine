-------------------------------------------------------------------------------
--   data/scripts/conjurer_gui.lua
--
--   custom GUI initialization
--------------------------------------------------------------------------------
---------------------------------------------------------------------------------
--   This procedure is called when the gui server is opened
---------------------------------------------------------------------------------
function OnGuiServerOpen ()
    pushcwd('.')

    nebula.sys.servers.gui:addsystemfont('arial20b', 'Arial', 20, true, false, false )
    nebula.sys.servers.gui:addsystemfont('arial24', 'Arial', 24, false, false, false )
    nebula.sys.servers.gui:addsystemfont('courier15', 'Courier new', 15, false, false, false )
    nebula.sys.servers.gui:addsystemfont('courier20', 'Courier new', 20, false, false, false )
    nebula.sys.servers.gui:addsystemfont('courier20bi', 'Courier new', 20, true, false, false )
    nebula.sys.servers.gui:addsystemfont('courier25', 'Courier new', 25, false, false, false )
    nebula.sys.servers.gui:addsystemfont('courier30bi', 'Courier new', 30, true, false, false )
    nebula.sys.servers.gui:addsystemfont('fixedsys16b', 'fixedsys', 16, false, false, false )
    
    guiroot = nebula.sys.servers.gui:getrootpath()
    luaSel(guiroot)

    -- define the system skin
    skin = nebula.sys.servers.gui:newskin('system')
            
    -- set texture path pre- and postfix (NOTE: don't change path to textures:system!!!)            
    skin:settextureprefix('wc:libs/system/textures/')
    skin:settexturepostfix('.tga')
    
    -- active and inactive window modulation color
    skin:setactivewindowcolor(1.0,1.0,1.0,1.0)
    skin:setinactivewindowcolor(1.0,1.0,1.0,1.0)
    skin:setbuttontextcolor(0.0, 0.0, 0.0, 1.0)
    skin:settitletextcolor(0.0, 0.0, 0.0, 1.0)
    skin:setlabeltextcolor(0.0, 0.0, 0.0, 1.0)
    skin:setentrytextcolor(0.0, 0.0, 0.0, 1.0)
    skin:settextcolor(0.0, 0.0, 0.0, 1.0)
    skin:setmenutextcolor(0.0, 0.0, 0.0, 1.0)
    
    -- define brushes  
    skin:beginbrushes()
	
    -- RNS Load Brushes
    skin:addbrush( 'loadscreen', 'rns_loadscreen', 0, 0, 1024, 768, 1.0, 1.0, 1.0, 1.0 )

    skin:addbrush( 'loadbar_n', 'rns_loadscreen', 0, 768, 292, 39, 1.0, 1.0, 1.0, 1.0 )
    skin:addbrush( 'loadbar_d', 'rns_loadscreen', 362, 624, 292, 39, 1.0, 1.0, 1.0, 1.0 )

    -- RNS Menu Brushes
    skin:addbrush( 'rns_menubg00', 'rns_menubg00', 0,0, 256,384, 1.0, 1.0, 1.0, 1.0 )
    skin:addbrush( 'rns_menubg01', 'rns_menubg01', 0,0, 1024,768, 1.0, 1.0, 1.0, 1.0 )
    skin:addbrush( 'rns_drops01', 'rns_menubg02', 512,0, 512,768, 1.0, 1.0, 1.0, 1.0 )
    skin:addbrush( 'rns_drops02', 'rns_menubg02', 0,0, 512,768, 1.0, 1.0, 1.0, 1.0 )
    skin:addbrush( 'rns_drops01b', 'rns_menubg03', 512,0, 512,768, 1.0, 1.0, 1.0, 1.0 )
    skin:addbrush( 'rns_drops02b', 'rns_menubg03', 0,0, 512,768, 1.0, 1.0, 1.0, 1.0 )
    skin:addbrush( 'rns_crystal', 'rns_menuscreen', 512, 0, 149,151, 1.0, 1.0, 1.0, 1.0 )
    skin:addbrush( 'rns_bottom', 'rns_menuscreen', 0, 0, 512,256, 1.0, 1.0, 1.0, 1.0 )
    skin:addbrush( 'rns_bottomb', 'rns_menuscreen', 0, 257, 512,256, 1.0, 1.0, 1.0, 1.0 )

    skin:addbrush( 'rns_iplist', 'rns_menuscreen', 512, 287, 200,219, 1.0, 1.0, 1.0, 1.0 )
    skin:addbrush( 'ipupbtn_d', 'rns_menuscreen', 661, 0, 30, 30, 1.0, 1.0, 1.0, 1.0 )
    skin:addbrush( 'ipupbtn_n', 'rns_menuscreen', 691, 0, 30, 30, 1.0, 1.0, 1.0, 1.0 )
    skin:addbrush( 'ipupbtn_h', 'rns_menuscreen', 721, 0, 30, 30, 1.0, 1.0, 1.0, 1.0 )
    skin:addbrush( 'ipupbtn_p', 'rns_menuscreen', 751, 0, 30, 30, 1.0, 1.0, 1.0, 1.0 )
    skin:addbrush( 'ipdownbtn_d', 'rns_menuscreen', 661, 30, 30, 30, 1.0, 1.0, 1.0, 1.0 )
    skin:addbrush( 'ipdownbtn_n', 'rns_menuscreen', 691, 30, 30, 30, 1.0, 1.0, 1.0, 1.0 )
    skin:addbrush( 'ipdownbtn_h', 'rns_menuscreen', 721, 30, 30, 30, 1.0, 1.0, 1.0, 1.0 )
    skin:addbrush( 'ipdownbtn_p', 'rns_menuscreen', 751, 30, 30, 30, 1.0, 1.0, 1.0, 1.0 )
    
    skin:addbrush( 'startbtn_d', 'rns_menuscreen', 0,612, 200, 50, 1.0, 1.0, 1.0, 1.0 )
    skin:addbrush( 'startbtn_n', 'rns_menuscreen', 200,612, 200, 50, 1.0, 1.0, 1.0, 1.0 )
    skin:addbrush( 'startbtn_h', 'rns_menuscreen', 400,612, 200, 50, 1.0, 1.0, 1.0, 1.0 )
    skin:addbrush( 'startbtn_p', 'rns_menuscreen', 600,612, 200, 50, 1.0, 1.0, 1.0, 1.0 )

    skin:addbrush( 'joinbtn_d', 'rns_menuscreen', 0,562, 200, 50, 1.0, 1.0, 1.0, 1.0 )
    skin:addbrush( 'joinbtn_n', 'rns_menuscreen', 200,562, 200, 50, 1.0, 1.0, 1.0, 1.0 )
    skin:addbrush( 'joinbtn_h', 'rns_menuscreen', 400,562, 200, 50, 1.0, 1.0, 1.0, 1.0 )
    skin:addbrush( 'joinbtn_p', 'rns_menuscreen', 600,562, 200, 50, 1.0, 1.0, 1.0, 1.0 )
    
    skin:addbrush( 'exitbtn_d', 'rns_menuscreen', 0,512, 200, 50, 1.0, 1.0, 1.0, 1.0 )
    skin:addbrush( 'exitbtn_n', 'rns_menuscreen', 200,512, 200, 50, 1.0, 1.0, 1.0, 1.0 )
    skin:addbrush( 'exitbtn_h', 'rns_menuscreen', 400,512, 200, 50, 1.0, 1.0, 1.0, 1.0 )
    skin:addbrush( 'exitbtn_p', 'rns_menuscreen', 600,512, 200, 50, 1.0, 1.0, 1.0, 1.0 )
    
    skin:addbrush( 'canceljoinbtn_d', 'rns_menuscreen', 0,691, 105, 29, 1.0, 1.0, 1.0, 1.0 )
    skin:addbrush( 'canceljoinbtn_n', 'rns_menuscreen', 200,691, 105, 29, 1.0, 1.0, 1.0, 1.0 )
    skin:addbrush( 'canceljoinbtn_h', 'rns_menuscreen', 400,691, 105, 29, 1.0, 1.0, 1.0, 1.0 )
    skin:addbrush( 'canceljoinbtn_p', 'rns_menuscreen', 600,691, 105, 29, 1.0, 1.0, 1.0, 1.0 )

    skin:addbrush( 'joinokbtn_d', 'rns_menuscreen', 0,662, 86, 29, 1.0, 1.0, 1.0, 1.0 )
    skin:addbrush( 'joinokbtn_n', 'rns_menuscreen', 200,662, 86, 29, 1.0, 1.0, 1.0, 1.0 )
    skin:addbrush( 'joinokbtn_h', 'rns_menuscreen', 400,662, 86, 29, 1.0, 1.0, 1.0, 1.0 )
    skin:addbrush( 'joinokbtn_p', 'rns_menuscreen', 600,662, 86, 29, 1.0, 1.0, 1.0, 1.0 )
    
    skin:addbrush( 'ipbtn_n', 'rns_menuscreen', 513,306, 198, 39, 1.0, 1.0, 1.0, 0.8 )
    skin:addbrush( 'ipbtn_h', 'rns_menuscreen', 512,248, 198, 39, 1.0, 1.0, 1.0, 0.8 )
    
    -- RNS HUD Brushes
    -- Panels
    skin:addbrush( 'panelobjectives', 'rns_interface_panels', 0, 19, 322, 442, 1.0, 1.0, 1.0, 0.8)
    skin:addbrush( 'panelammunition', 'rns_interface_panels', 327, 38, 185, 160, 1.0, 1.0, 1.0, 0.8)
    skin:addbrush( 'panelhealth', 'rns_interface_panels', 324, 360, 188, 106, 1.0, 1.0, 1.0, 0.8)
    skin:addbrush( 'panelweapon', 'rns_interface_panels', 323, 200, 187, 160, 1.0, 1.0, 1.0, 0.8)

    --Icons and labels
    skin:addbrush( 'cliplarge', 'rns_interface_objects', 26*16, 3*16, 2*16, 5*16, 1.0, 1.0, 1.0, 1.0)
    skin:addbrush( 'clipsmall', 'rns_interface_objects', 29*16, 5*16, 3*16, 4*16, 1.0, 1.0, 1.0, 1.0)
    skin:addbrush( 'clampclip', 'rns_interface_objects', 29*16, 9*16, 3*16, 3*16, 1.0, 1.0, 1.0, 1.0)
    skin:addbrush( 'chainclip', 'rns_interface_objects', 2*16, 13*16, 2*16, 1*16, 1.0, 1.0, 1.0, 1.0)
    skin:addbrush( 'labelsemiauto', 'rns_interface_objects', 6*16, 13*16, 4*16, 1*16, 1.0, 1.0, 1.0, 1.0)
    skin:addbrush( 'labelfullauto', 'rns_interface_objects', 10*16, 13*16, 4*16, 1*16, 1.0, 1.0, 1.0, 1.0)
    skin:addbrush( 'bullets', 'rns_interface_objects', 0*16, 14*16, 2*16, 2*16, 1.0, 1.0, 1.0, 1.0)
    skin:addbrush( 'grenade', 'rns_interface_objects', 30*16, 14*16, 1*16, 2*16, 1.0, 1.0, 1.0, 1.0)
    skin:addbrush( 'timer1', 'rns_interface_objects', 16*16, 14*16, 2*16, 2*16, 1.0, 1.0, 1.0, 1.0)
    skin:addbrush( 'timer2', 'rns_interface_objects', 14*16, 14*16, 2*16, 2*16, 1.0, 1.0, 1.0, 1.0)
    skin:addbrush( 'timer3', 'rns_interface_objects', 12*16, 14*16, 2*16, 2*16, 1.0, 1.0, 1.0, 1.0)
    skin:addbrush( 'timer4', 'rns_interface_objects', 10*16, 14*16, 2*16, 2*16, 1.0, 1.0, 1.0, 1.0)
    skin:addbrush( 'timer5', 'rns_interface_objects', 8*16, 14*16, 2*16, 2*16, 1.0, 1.0, 1.0, 1.0)
    skin:addbrush( 'timer6', 'rns_interface_objects', 6*16, 14*16, 2*16, 2*16, 1.0, 1.0, 1.0, 1.0)
    skin:addbrush( 'timer7', 'rns_interface_objects', 4*16, 14*16, 2*16, 2*16, 1.0, 1.0, 1.0, 1.0)
    skin:addbrush( 'timer8', 'rns_interface_objects', 2*16, 14*16, 2*16, 2*16, 1.0, 1.0, 1.0, 1.0)
    skin:addbrush( 'checked', 'rns_interface_objects', 0*16, 13*16, 1*16, 1*16, 1.0, 1.0, 1.0, 1.0)
    skin:addbrush( 'unchecked', 'rns_interface_objects', 1*16, 13*16, 1*16, 1*16, 1.0, 1.0, 1.0, 1.0)
    skin:addbrush( 'addongrenadelauncher', 'rns_interface_objects', 9*16, 8*16, 5*16, 2*16, 1.0, 1.0, 1.0, 1.0)
    skin:addbrush( 'addonflashlight', 'rns_interface_objects', 14*16, 8*16, 3*16, 3*16, 1.0, 1.0, 1.0, 1.0)
    skin:addbrush( 'addonhicap', 'rns_interface_objects', 14*16, 11*16, 3*16, 3*16, 1.0, 1.0, 1.0, 1.0)
    skin:addbrush( 'addonsilencer', 'rns_interface_objects', 17*16, 7*16, 4*16, 3*16, 1.0, 1.0, 1.0, 1.0)
    skin:addbrush( 'addonreflex', 'rns_interface_objects', 18*16, 13*16, 3*16, 3*16, 1.0, 1.0, 1.0, 1.0)
    skin:addbrush( 'addonreddotlaser', 'rns_interface_objects', 17*16, 10*16, 4*16, 3*16, 1.0, 1.0, 1.0, 1.0)
    skin:addbrush( 'addonscope', 'rns_interface_objects', 21*16, 9*16, 4*16, 4*16, 1.0, 1.0, 1.0, 1.0)
    skin:addbrush( 'addonflechette', 'rns_interface_objects', 21*16, 13*16, 3*16, 3*16, 1.0, 1.0, 1.0, 1.0)
    skin:addbrush( 'addonclamp', 'rns_interface_objects', 24*16, 13*16, 3*16, 3*16, 1.0, 1.0, 1.0, 1.0)
    skin:addbrush( 'addonbipod', 'rns_interface_objects', 25*16, 8*16, 4*16, 4*16, 1.0, 1.0, 1.0, 1.0)
    skin:addbrush( 'addonforegrip', 'rns_interface_objects', 27*16, 12*16, 3*16, 4*16, 1.0, 1.0, 1.0, 1.0)
    
    -- Bars
    skin:addbrush( 'healthbar_n', 'rns_interface_objects', 9*16, 5*16, 6*16, 1*16, 1.0, 1.0, 1.0, 1.0)
    skin:addbrush( 'healthbar_d', 'rns_interface_objects', 9*16, 5*16, 6*16, 1*16, 1.0, 1.0, 1.0, 0.1)
    skin:addbrush( 'staminabar_n', 'rns_interface_objects', 9*16, 4*16, 6*16, 1*16, 1.0, 1.0, 1.0, 1.0)
    skin:addbrush( 'staminabar_d', 'rns_interface_objects', 9*16, 4*16, 6*16, 1*16, 1.0, 1.0, 1.0, 0.1)
    skin:addbrush( 'bar_rule', 'rns_interface_objects', 4*16, 6*16, 5*16, 1*16, 1.0, 1.0, 1.0, 1.0)

    skin:endbrushes()

    nebula.sys.servers.gui:setsystemskin(skin)
    nebula.sys.servers.gui:setskin(skin)

    popcwd()
end

--------------------------------------------------------------------------------
--   This procedure is called when the gui server is closed
---------------------------------------------------------------------------------
function OnGuiServerClose ()
    -- Empty
end

