---------------------------------------------------------------------------------
--  data/scripts/conjurer_input.lua
--  
--  Input handling methods for Conjurer
--  
--  (c) 2005 Conjurer Services, S.A.
---------------------------------------------------------------------------------

---------------------------------------------------------------------------------
--  Set maximum detail level for materials in the current viewport
---------------------------------------------------------------------------------

function set_maxMaterialLevel(level)
    
    local curViewport = lookup('/app/conjurer/appviewportui/' .. nebula.app.conjurer:getcurrentviewport())
    local viewport = curViewport:getviewportentity()
    viewport:setintvariable('maxMaterialLevel', level)
    
end

---------------------------------------------------------------------------------
--  Defines the standard input mappings for conjurer
---------------------------------------------------------------------------------

function OnMapInput()

    pushcwd('.')
    i = lookup('/sys/servers/input')
    i:beginmap()

    -- MOUSE

    i:map('mouse0:btn0.pressed', 'buton0')
    i:map('mouse0:btn0.down', 'buton0_down')
    i:map('mouse0:btn0.up',   'buton0_up')
    
    i:map('mouse0:btn0&menu.pressed', 'buton0_alt')
    i:map('mouse0:btn0&menu.down', 'buton0_down_alt')
    i:map('mouse0:btn0&menu.up',   'buton0_up_alt')
    
    i:map('mouse0:btn0&ctrl.pressed', 'buton0_ctrl')
    i:map('mouse0:btn0&ctrl.down', 'buton0_down_ctrl')
    i:map('mouse0:btn0&ctrl.up',   'buton0_up_ctrl')
    
    i:map('mouse0:btn0&shift.pressed', 'buton0_shift')
    i:map('mouse0:btn0&shift.down', 'buton0_down_shift')
    i:map('mouse0:btn0&shift.up',   'buton0_up_shift')
    
    i:map('mouse0:btn1.pressed', 'buton1')
    i:map('mouse0:btn1.down', 'buton1_down')
    i:map('mouse0:btn1.up',   'buton1_up')

    i:map('mouse0:btn2.pressed', 'buton2')
    i:map('mouse0:btn2.down', 'buton2_down')
    i:map('mouse0:btn2.up',   'buton2_up')
    i:map('relmouse0:+zbtn&shift.down', 'zoom_in')
    i:map('relmouse0:-zbtn&shift.down', 'zoom_out')
    i:map('relmouse0:+zbtn&ctrl.down', 'wheel_up_ctrl')
    i:map('relmouse0:-zbtn&ctrl.down', 'wheel_down_ctrl')

    i:map('relmouse0:-x',      'slider_left')
    i:map('relmouse0:+x',      'slider_right')
    i:map('relmouse0:-y',      'slider_up')
    i:map('relmouse0:+y',      'slider_down')

    -- PREVIEW
    
    i:map('keyb0:space.down',       'prev_reset')
    i:map('relmouse0:btn0.pressed', 'prev_look')
    i:map('relmouse0:btn1.pressed', 'prev_zoom')
    i:map('relmouse0:btn2.pressed', 'prev_pan')
    i:map('relmouse0:-x',           'prev_left')
    i:map('relmouse0:+x',           'prev_right')
    i:map('relmouse0:-y',           'prev_up')
    i:map('relmouse0:+y',           'prev_down')

    -- KEYBOARD

    i:map('keyb0:t.down',  'debug_t')
    i:map('keyb0:o.down',  'debug_o')
    i:map('keyb0:p.down',  'debug_p')
    i:map('keyb0:m.down',  'debug_m')

    -- ALT+W        Switch single viewport mode
    i:map ('keyb0:w&menu.down',    'viewport_single_switch')

    -- CTRL+Z Undo
    i:map ('keyb0:z&ctrl.down', 'undo')
 
    -- CTRL+Y Redo
    i:map ('keyb0:y&ctrl.down', 'redo')
    
    -- CTRL+X Cut
    i:map ('keyb0:x&ctrl.down', 'cut')

    -- CTRL+C Copy
    i:map ('keyb0:c&ctrl.down', 'copy')
    
    -- CTRL+V Paste
    i:map ('keyb0:v&ctrl.down', 'paste')
    
    -- Supr Delete
    i:map ('keyb0:delete.down', 'del')
    
    -- Load selection
    i:map ('keyb0:d&ctrl.down', 'script:nebula.app.conjurer.appstates.object:loadentities()')
    -- Unload selection
    i:map ('keyb0:u&ctrl.down', 'script:nebula.app.conjurer.appstates.object:unloadentities()')
    -- Reload selection
    i:map ('keyb0:o&ctrl.down', 'script:nebula.app.conjurer.appstates.object:reloadentities()')
    
    -- OBJECT STATE input
    
    -- E      Entity selection tool 
    i:map ('keyb0:e.down', 'selection')
    -- ALT+E  Toggle edit indoor mode on/off
    i:map ('keyb0:e&menu.down', 'toggle_indoor_mode')
    
    -- ALT+I  Toggle edit all selected indoors mode on/off
    i:map ('keyb0:i&menu.down', 'toggle_all_indoor_mode')
    
    -- T Entity translation tool
    i:map ('keyb0:t.down', 'translation')
    
    -- R Entity rotation tool
    i:map ('keyb0:r.down', 'rotation')
    
    -- Y Entity scale tool
    i:map ('keyb0:y.down', 'scale')

    -- Lock transform tool    
    i:map('keyb0:space&ctrl.down', 'togglelock')
    
    -- ALT+G  toggle editor grid 
    i:map('keyb0:g&menu.down', 'grid')
   
    -- Cursor object movement
    i:map('keyb0:up.pressed', 'cursor_fwd')
    i:map('keyb0:num8.pressed', 'cursor_fwd')
    i:map('keyb0:down.pressed', 'cursor_aft')
    i:map('keyb0:num5.pressed', 'cursor_aft')
    i:map('keyb0:left.pressed', 'cursor_left')
    i:map('keyb0:num4.pressed', 'cursor_left')
    i:map('keyb0:right.pressed', 'cursor_right')
    i:map('keyb0:num6.pressed', 'cursor_right')
    i:map('keyb0:home.pressed', 'cursor_up')
    i:map('keyb0:num7.pressed', 'cursor_up')
    i:map('keyb0:end.pressed', 'cursor_down')
    i:map('keyb0:num1.pressed', 'cursor_down')

    
    -- END OBJECT STATE input
    
    i:map('keyb0:space.down', 'reset')
    i:map('keyb0:esc.down', 'cancel')
    i:map('keyb0:return.down', 'return')

    i:map('keyb0:w.pressed', 'up')
    i:map('keyb0:s.pressed', 'down')
    i:map('keyb0:a.pressed', 'left')
    i:map('keyb0:d.pressed', 'right')
    
    i:map('keyb0:up.down', 'pitch_up')
    i:map('keyb0:num8.pressed', 'pitch_up')
    i:map('keyb0:down.pressed', 'pitch_down')
    i:map('keyb0:num5.pressed', 'pitch_down')
    i:map('keyb0:left.pressed', 'turn_left')
    i:map('keyb0:num4.pressed', 'turn_left')
    i:map('keyb0:right.pressed', 'turn_right')
    i:map('keyb0:num6.pressed', 'turn_right')
    i:map('keyb0:home.pressed', 'strafe_up')
    i:map('keyb0:num7.pressed', 'strafe_up')
    i:map('keyb0:end.pressed', 'strafe_down')
    i:map('keyb0:num1.pressed', 'strafe_down')
    
    i:map('keyb0:k.down', 'movelight')
    i:map('keyb0:j.down' , 'showedit')
    i:map('keyb0:x.down' , 'showaxes')
    i:map('keyb0:c.down' , 'show3daxes')
    i:map('keyb0:m.down' , 'toggle_map')
    i:map('keyb0:i.down' , 'toggle_stats')
    --i:map('keyb0:p.down' , 'toggle_cursor')
    
    i:map('keyb0:f10.down' , 'toggle_obeylightlinks')

    i:map('keyb0:0&ctrl.down', 'script:set_maxMaterialLevel(0)')
    i:map('keyb0:1&ctrl.down', 'script:set_maxMaterialLevel(1)')
    i:map('keyb0:2&ctrl.down', 'script:set_maxMaterialLevel(2)')
    i:map('keyb0:3&ctrl.down', 'script:set_maxMaterialLevel(3)')
    i:map('keyb0:4&ctrl.down', 'script:set_maxMaterialLevel(4)')
    
    i:map('keyb0:0.down', 'fillmode')
    i:map('keyb0:1.down', 'diffmap')
    i:map('keyb0:2.down', 'bumpmap')
    i:map('keyb0:3.down', 'lightmap')
    i:map('keyb0:4.down', 'diffuse')
    i:map('keyb0:5.down', 'specular')    
    i:map('keyb0:6.down', 'terrainlod')
    i:map('keyb0:8.down', 'skeleton')
    i:map('keyb0:9.down', 'wireframe')
    
    i:map('keyb0:f5.down', 'toggle_useoctrees')
    i:map('keyb0:b.down', 'toggle_drawbboxes')
    i:map('keyb0:n.down', 'toggle_drawportals')
    i:map('keyb0:u.down', 'recalculate_campos')
    i:map('keyb0:f.down', 'toggle_physics')
    i:map('keyb0:l.down', 'set_lightpos')
    -- ALT+F toggle FPS
    i:map('keyb0:f&menu.down', 'toggle_fps')
    i:map('keyb0:h.down', 'toggle_hdr')
    i:map('keyb0:f6.down', 'toggle_isometric')
    
    -- standard views ( with control pressed )
    i:map('keyb0:t&ctrl.down', 'view_top')
    i:map('keyb0:l&ctrl.down', 'view_left')
    i:map('keyb0:f&ctrl.down', 'view_front')
    i:map('keyb0:k&ctrl.down', 'view_back')
    i:map('keyb0:r&ctrl.down', 'view_right')

    -- editor camera velocity
    -- keypad +/-
    i:map('keyb0:minus.down', 'velocity_minus')
    i:map('keyb0:plus.down', 'velocity_plus')
    -- shift + wheel
    i:map('relmouse0:-zbtn.down', 'wheel_velocity_minus')
    i:map('relmouse0:+zbtn.down', 'wheel_velocity_plus')
    
    i:map('keyb0:f1.down', 'free_cam_navigator')
    i:map('keyb0:f2.down', 'game_cam_navigator') 
    i:map('keyb0:f3.down', 'game_cam_wander') 
    i:map('keyb0:f11.down', 'console')
    i:map('keyb0:f12.down', 'screenshot')
    
    i:map('keyb0:g.down', 'outgui')  
   
    -- Direct keys
    i:map('keyb0:1.down',      '1')
    i:map('keyb0:2.down',      '2')
    i:map('keyb0:3.down',      '3')
    i:map('keyb0:4.down',      '4')
    i:map('keyb0:5.down',      '5')
    i:map('keyb0:6.down',      '6')
    i:map('keyb0:7.down',      '7')
    i:map('keyb0:8.down',      '8')
    i:map('keyb0:9.down',      '9')
    i:map('keyb0:0.down',      '0')
    
    i:map('keyb0:1&menu.down',      'alt1')
    i:map('keyb0:2&menu.down',      'alt2')
    i:map('keyb0:3&menu.down',      'alt3')
    i:map('keyb0:4&menu.down',      'alt4')
    i:map('keyb0:5&menu.down',      'alt5')
    i:map('keyb0:6&menu.down',      'alt6')
    i:map('keyb0:7&menu.down',      'alt7')
    i:map('keyb0:8&menu.down',      'alt8')
    i:map('keyb0:9&menu.down',      'alt9')
    i:map('keyb0:0&menu.down',      'alt0')

    --[[
     Input Mapping from Renaissance. Most of the input names come 
     from the design files from Renaissance Designers.
     TODO: merge better the input names with conjurer.
     ]]
    
    i:map( 'keyb0:esc.up',        'exit' )
    i:map( 'keyb0:f1.down',      'functionkey1' )
    i:map( 'keyb0:f2.down',      'functionkey2' )
    i:map( 'keyb0:f2.down',      'initServer' )
    i:map( 'keyb0:f3.down',      'functionkey3' )
    i:map( 'keyb0:f3.down',      'initClient' )
    i:map( 'keyb0:f4.down',      'joinServer' )
    i:map( 'keyb0:f4.down',      'functionkey4' )
    i:map( 'keyb0:f5.down',      'functionkey5' )
    i:map( 'keyb0:f6.down',      'functionkey6' )
    i:map( 'keyb0:f7.down',      'artistview' )
    i:map( 'keyb0:f8.down',      'functionkey8' )
    i:map( 'keyb0:f10.down',     'cleanscreen' )
    i:map( 'keyb0:f11.down',     'console' )
    i:map( 'keyb0:f12.down',     'screenshot')
    i:map( 'relmouse0:btn2.pressed', 'FreeMove' )
    i:map( 'keyb0:c&menu.down',  'change_camera' )
    i:map( 'keyb0:i.down',     'ShowWeaponStats' )

    i:map( 'keyb0:bs&shift.down',     'Suicide' )

---------- Movement ----------
    ---- Player will move forward while pressed.
    i:map( 'keyb0:w.pressed',     'Forward' )     
    
    ---- Player will move backward while pressed.
    i:map( 'keyb0:s.pressed',     'Backward' )    
    
    ---- Player will turn to the left while pressed.
    i:map( 'keyb0:q.pressed',      'TurnLeft' )
    
    ---- Player will turn to the right while pressed.
    i:map( 'keyb0:e.pressed',      'TurnRight' )
    
    ---- Player will step left or right while this key is pressed in combination with either the Turn Left
    ---- or Turn Right key.
    i:map( 'keyb0:space.pressed', 'Strafe' )		        
    
    ---- Player will strafe to the left while pressed. Turn Left + Strafe .
    i:map( 'keyb0:a.pressed',     'StrafeLeft' )	 
    
    ---- Player will strafe to the right while pressed. Turn Left + Strafe simultaneously.
    i:map( 'keyb0:d.pressed',     'StrafeRight' ) 
    
    ---- Player will lean left or right while this key is pressed in combination with the Turn Left or
    ---- Turn Right key.
    i:map( 'keyb0:shift.pressed',  'Lean' )
    
    ---- Left lean enable
    i:map( 'keyb0:q.pressed', 'LeftLeanEnable' )
    
    ---- Right lean enable
    i:map('keyb0:e.pressed', 'RightLeanEnable' )
    
    ---- Player will lean to the left while pressed. Lean + Turn Left simultaneously.
    -- 'LeanLeft'
    ---- Player will lean to the right while pressed. Lean + Turn Right simultaneously.
    -- 'LeanRight'

    ---- The players view will rotate upwards while this key is pressed. Probably only be used by players
    ---- who do not use Mouselook.
    i:map( 'keyb0:insert.pressed',      'LookUp' )
    
    ---- The players view will rotate downwards while this key is pressed. Probably only be used by players
    ---- who do not use Mouselook.
    i:map( 'keyb0:delete.pressed',      'LookDown' )
    
    ---- The players view is changed so that he is facing straight ahead and perfectly level (not up or down).    
    i:map( 'keyb0:end.down',         'CentreView' )

    
----- AI MODE

    ---- Shows/hides the geometry
    i:map( 'keyb0:o.down',         'show_geometry' )    
    ---- Shows/hides the physics boxes
    i:map( 'keyb0:p.down',         'show_physics' )
    ---- Shows/hides links between navigation nodes
    i:map( 'keyb0:l.down',         'show_links' )
    ---- Shows/hides path finder debug info
    i:map( 'keyb0:z.down',         'show_path_info')
    ---- Shows/hides navigation mesh
    i:map( 'keyb0:m.down',         'show_nav_mesh')    
    ---- Execute the waitsomtime basicaction
    i:map( 'keyb0:q.down',         'wait')

----- AI MODE END


    ---- Player will jump into the air. Keeping this button pressed will not increase the duration of the jump
    ---- and will not cause another jump when the player lands. Multiple jumps require the player to press
    ---- the Jump key for each jump.
    i:map( 'keyb0:space.down', 'Jump' )
    ---- Player will crouch down on one knee. Tapping this key will switch the player to Crouch mode.
    ---- Holding the key down will maintain Crouch mode until it is released, at which point the player
    ---- will return to his previous mode (Prone or Standing).
    i:map( 'keyb0:c.down',       'crouch_start')
    i:map( 'keyb0:space.down',   'crouch_end')
    ---- Player will lie down flat on the ground. Tapping this key will switch the player to Prone mode.
    ---- Holding the key down will switch to Prone mode and maintain it until the key is released, at which
    ---- point the player will return to his previous mode (Standing or Crouching).
    -- 'Prone'
    i:map( 'keyb0:x.down',       'prone_start')
    i:map( 'keyb0:space.down',   'prone_end')
    
    ---- If player is currently running, tapping this key will switch to Walk mode. If currently walking,
    ---- tapping this key will switch to Run mode. If the key is pressed and maintained, the mode will
    ---- switch to the opposite (Run to Walk, or Walk to Run) until the key is released.
    i:map( 'keyb0:tab.pressed',      'RunWalkTogglePressed' )
    i:map( 'keyb0:tab.up',			 'RunWalkToggleReleased' )

    --i:map( 'relmouse0:-x',             'leftMouse' )
    --i:map( 'relmouse0:+x',             'rightMouse' )
    --i:map( 'relmouse0:-y',             'upMouse' )
    --i:map( 'relmouse0:+y',             'downMouse' )
    
---------- Combat ----------
    ---- Player will fire the weapon that he is currently holding, using the currently selected firing mode.
    i:map( 'mouse0:btn0.down',  'PrimaryAttack' )
    i:map( 'mouse0:btn0.pressed', 'PrimaryAttackPressed' )
    
    ---- If the currently held weapon has a secondary attack (ie. a grenade launcher), it will be fired.
   i:map( 'mouse0:btn2.down', 'SecondaryAttack' )
    
    ---- The players view will change to either zoomed (if the weapon has a scope) or iron-sight (if there is no scope).
    i:map( 'mouse0:btn1.down', 'Aim' )
    ---- Refills the clip of the current weapon, depending on available ammunition.
    i:map( 'keyb0:r.down', 'ReloadDown' )
    i:map( 'keyb0:r.pressed', 'ReloadPressed' )
    i:map( 'keyb0:r.up', 'ReloadUp' )
    ---- Execute the health basicaction
    i:map( 'keyb0:h.down', 'health')
    i:map( 'keyb0:h&shift.down', 'movetowaypoint')

    ---- Execute the melee attack basicaction
    i:map( 'keyb0:p.down', 'melee')
    i:map( 'keyb0:o.down', 'block')

    ---- Turn on/off the flashlight
    i:map( 'keyb0:l.down', 'flashlight' )    

    ---- Depending on the weapon, available firing rates are FULL AUTO, BURST or SINGLE SHOT.
    i:map( 'keyb0:b.down', 'RateOfFire' )
    
    ---- Switches to the next weapon.
    -- 'NextWeapon'
    ---- Switches to the previous weapon.
    -- 'PreviousWeapon'
    ---- Equips the last weapon or item used.
    -- 'LastItemUsed'

    --- TEMPORAL ADDONS
    i:map( 'keyb0:r&menu.down', 'AddonR' )
    i:map( 'keyb0:f5&menu.down', 'AddonF5' )
    i:map( 'keyb0:f6&menu.down', 'AddonF6' )
    i:map( 'keyb0:f7&menu.down', 'AddonF7' )
    i:map( 'keyb0:f8&menu.down', 'AddonF8' )
    i:map( 'keyb0:f9&menu.down', 'AddonF9' )
    i:map( 'keyb0:f10&menu.down', 'AddonF10' )
    i:map( 'keyb0:f11&menu.down', 'AddonF11' )
    i:map( 'keyb0:f12&menu.down', 'AddonF12' )
    
---------- Miscellaneous ----------
    -- 'Activate'	 	    Activates whatever object (if useable) is behind the crosshair and within useable range of the player.
    i:map( 'keyb0:f.down', 'Activate' )
    -- 'Drop'        		Drops whatever object or weapon the player character is currently holding in his hands.
    -- 'Use'         		Use the current active inventory item.
    -- 'NextItem'        	Cycle forward to the next inventory item.
    -- 'PreviousItem'    	Cycle backward to the previous inventory item.
    -- 'QuickSave'   		Saves the current state of the game, using the default Quicksave slot.
    -- 'QuickLoad'   		Loads the most recently saved Quicksave.
    -- 'MapMissionInfo'		Shows the Map and Mission Information screen (or overlay). Pressing this key again will remove this screen and return to the normal view.
    i:map( 'keyb0:m.down', 'MapMissionInfo' )
    -- 'Binoculars'  		Activates the binoculars, if the player has them. Pressing this key again will deactivate them.
    -- 'Flashlight'  		Activates the flashlight, if the player has one. Pressing this key again will deactivate it.

---------- Multiplayer----------
    -- 'SelectTeam'  	 	Displays a small menu which allows the player to select a team to join.
    -- 'TeamChat'    		Allows the player to type a text message which, after pressing ENTER, will be sent to the screens of each team member.
    -- 'GlobalChat'  		Allows the player to type a text message which, after pressing ENTER, will be sent to the screens of each player in the game.

---------- Vehicles----------
    i:map( 'keyb0:j.pressed', 'steerleft' )
    i:map( 'keyb0:l.pressed', 'steerright' )
    i:map( 'keyb0:i.pressed', 'accelerate' )
    i:map( 'keyb0:k.pressed', 'brake' )
    i:map( 'keyb0:space.pressed', 'reset' )

    i:endmap()
    popcwd()

end
