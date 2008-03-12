macroScript Zombie_render_settings
	ButtonText:"Zombie render" 
	Category:"Zombie" 
	internalCategory:"Zombie render" 
	Tooltip:"zombie material, set the render settin in 3dsmax" 
(	

	On isEnabled return ( true )
	On isVisible return ( true ) 
	On Execute Do	
	(
	    CreateDialog zombieRenderRollout
	)
)

macroScript Zombie_logging_settings
	ButtonText:"Config log" 
	Category:"Zombie" 
	internalCategory:"Zombie log" 
	Tooltip:"config log system" 
(	

	On isEnabled return ( true )
	On isVisible return ( true ) 
	On Execute Do	
	(
	    showConfigLogRollout()
	)
)