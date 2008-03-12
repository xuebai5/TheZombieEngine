global NebVisOp
global nebVisData
plugin helper nebVisPortalDummy
name:"Nebula portal helper"
classID:#(0x52b9041a, 0x25af6af3)
extends:Dummy
replaceUI:true
invisible:true
version:3
(
	parameters main rollout:params
	(
		cell1 type:#node subAnim:false --ui:btn_cell1
		cell2 type:#node subAnim:false --ui:btn_cell2
		clipRect type:#node -- dirty hack to parent in scene
		size type:#Point3 subAnim:false 
		c1ToC2Active type:#boolean subAnim:false ui:chk_c1ToC2 default:true
		c2ToC1Active type:#boolean subAnim:false ui:chk_c2ToC1 default:true
		deactDist1 type:#float subAnim:false ui:deact_dist1 default:1000000000
        deactDist2 type:#float subAnim:false ui:deact_dist2 default:1000000000

	
		on cell1 set val do
		(
			if  not  ( NebVisOp.IsCell val) do cell1 = undefined
			this.checkMaterial()
		)
		
		on cell2 set val do
		(
			if  not  ( NebVisOp.IsCell val) do cell2 = undefined
			this.checkMaterial()
		)
		
		on size set val do
		(
		    delegate.boxsize = [size.x, size.y , size.z]
		)
    )

    rollout params "Cell Parameters" width:162 height:222
    (
        fn picked_filter obj = nebVisop.isCell obj 
        group "Cell "(
        label lbl1 "C1" pos:[10,38] width:20 height:28
	    pickButton btn_cell1 "Cell 1" pos:[30,30] width:110 height:28 toolTip:"Select the first cell" filter:picked_filter
	    
	    label lbl2 "C2" pos:[10,68] width:20 height:28 
		pickButton btn_cell2 "Cell 2" pos:[30,60] width:110 height:28 toolTip:"Select the second cell" filter:picked_filter
		
		checkbox chk_c1ToC2 "From C1 view C2" pos:[30,94] width:102 height:20 
		spinner deact_dist1 "DeactDist 1 "  width:120 height:16 range:[0,1e+032,1] type:#worldunits align:#center
		checkbox chk_c2ToC1 "From C2 view C1" pos:[30,144] width:102 height:20
        spinner deact_dist2 "DeactDist 2 "  width:120 height:16 range:[0,1e+032,1] type:#worldunits align:#center	    )
		
		button btn_unlink1 "C1 to OutDoor" pos:[30,204] width:110 height:30
		button btn_unlink2 "C2 to OutDoor" pos:[30,234] width:110 height:30		
				
		group "Box "(
		    
    	spinner sx "X "  width:80 height:16 range:[0,1e+032,0] type:#worldunits align:#center
    	spinner sy "Y "  width:80 height:16 range:[0,1e+032,0] type:#worldunits align:#center
    	spinner sz "Z "  width:80 height:16 range:[0,1e+032,0] type:#worldunits	align:#center
        )
		
		on deact_dist1 changed val do deactDist1 = val
        on deact_dist2 changed val do deactDist2 = val
		on sx changed val do size.x = val
		on sy changed val do size.y = val
		on sz changed val do size.z = val
		
		on btn_cell1 picked obj do 
        (
            this.cell1 = obj
            local name1 = "\"OutDoor\""
            if this.cell1 != undefined do name1 = this.cell1.name
            btn_cell1.caption = name1
  			btn_unlink1.enabled  = this.cell1 != undefined

        )
        
        on btn_cell2 picked obj do 
        (
            this.cell2 = obj
            local name2 = "\"OutDoor\""
            if this.cell2 != undefined do name2 = this.cell2.name
            btn_cell2.caption = name2
			btn_unlink2.enabled  = this.cell2 != undefined
        )
		
		on btn_unlink1 pressed do
		(
			cell1 = undefined
			btn_unlink1.enabled = false
			btn_cell1.caption = "\"OutDoor\""
		)

		on btn_unlink2 pressed do
		(
			cell2 = undefined
			btn_unlink2.enabled = false
			btn_cell2.caption = "\"OutDoor\""
		)
		
		on params open do
		(
		    deact_dist1.value = deactDist1
		    deact_dist2.value = deactDist2
			sx.value = size.x
			sy.value = size.y
			sz.value = size.z
			
			local name1 = "\"OutDoor\""
			local name2 = "\"OutDoor\""
			
			if this.cell1 != undefined  do
			(
				name1 = this.cell1.name
			)

    		if this.cell2 != undefined  do
			(
				name2 = this.cell2.name
			)

			btn_cell1.caption = name1
			btn_cell2.caption = name2
			
			btn_unlink1.enabled  = this.cell1 != undefined
			btn_unlink2.enabled  = this.cell2 != undefined

			
		)
    )
	
	fn checkMaterial =
	(
	    format "check material\n"
		local count = 3
		if ( cell1 != undefined) do
		(
			format "% : % \n"  count cell1
			count -= 1
		) 
		
		if ( cell2 != undefined ) do
		(
			format "% : % \n"  count cell2
			count -= 1
		) 
		
		if ( cell1 == cell2 ) do count = 3
		
		if ( clipRect != undefined ) do -- The parent is the clip rectangle
		(
			clipRect.material =  nebVisData.portalMaterials[count]
		)
	)
	
	on clone original do
	(
	    format "clone, original parent %\n" original.clipRect
	    format "clone, my parent %\n" clipRect
	    if ( clipRect == original.clipRect ) do
	    (
	        NebMessage "bad copy, selecting also the rectangle\n"
	        --local sceneObj = nebOp.GetSceneObjectFromObject this
	        --delete sceneObj
	    )
		
		if ( cell1 == original.cell1 ) do -- if not clone cell1
		(
	    	this.cell1 = undefined
		)
		
		if ( cell2 ==  original.cell2 ) do -- if not clone cell2
		(
	    	this.cell2 = undefined
		)
	)
	
	on update do 
    (
        if version < 2 then
        (
            this.c1ToC2Active = true
            this.c2ToC1Active = true
        )
        
        if version < 3 then
        (
        	-- In the artist repository has a two version of "version 2" because 
        	-- Invalid progamer code
        	
        	-- The best solotion is having nothing
            --this.deactDist1 = 1000000000
            --this.deactDist2 = 1000000000
        )
     )
)
