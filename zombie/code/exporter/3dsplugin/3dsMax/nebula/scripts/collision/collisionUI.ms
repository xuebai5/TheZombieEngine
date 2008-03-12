/**
   @file collisionUI.ms
   @ingroup n3dsMaxScripts
   @author Luis Jose Cabellos Gomez <luis.cabellos@tragnarion.com>
   @brief Collision User Interface.
*/
--------------------------------------------------------------------------------
/**
*/
utility 
	nebula_collisions 
	"Nebula Collisions"
	rolledUp: false
(
	group "Properties"
	(
	dropdownlist dd_materials "Material:"
	dropdownlist dd_spaces "Physic Space:"
	dropdownlist dd_categories "Category:"
	button btn_physicSpace  "New Space" align:#center width:110 tooltip:"Create a new space. The parent is the actual space"
	button btn_deleteSpace  "Delete Space" align:#center width:110 tooltip:"Delete the actual space"
	button btn_renameSpace  "Rename Space" align:#center width:110 tooltip:"Rename the actual space"
	checkbox cb_wall "Wall" across: 2
	checkbox cb_ground "Ground"
	checkbox cb_ceiling "Ceiling" across: 2
	checkbox cb_walkable "Walkable"
	button btn_fixPivot "Fix Pivot" aling:#center width:110 tooltip:"Fix the pivot of object using the center"
	)
	group "Simple Creation"
	(
	button btn_newboxCollision "New Box" align:#center width:110 tooltip:"Create a new physic box in [0,0,0]"
	button btn_AAboxCollision  "Box AWorld" align:#center width:110 tooltip:"Create a physic box aligned to the world"
	button btn_boxCollision  "Fit Box" align:#center width:110 tooltip:"Create a physic box that fit the selection"
	button btn_boxCollisionXY  "Fit & Align Box" align:#center width:110 tooltip:"Create a physic box aligned to world XY plane that fit the selection"
	button btn_sphereCollision  "Sphere" align:#center width:110 offset:[0,10] tooltip:"Create a physic sphere that bound the selection"
	button btn_cylinderCollision  "Cylinder" align:#center width:110 tooltip:"Create a physic cylinder that bound the selection"
	button btn_capsuleCollision  "Capsule" align:#center width:110 tooltip:"Create a physic capsule that bound the selection"
	button btn_meshCollision  "PhyMesh" align:#center width:110 tooltip:"Create a new physic mesh using selection mesh"
	button btn_bestBounding  "Best Bounding" align:#center width:110 tooltip:"Create the sphere, box or capsule that better fit the selection"
	button btn_convertToCollision "Convert to PhyObject" align:#center width:110 offset:[0,10] tooltip:"Convert selection in Physic objects"
	) 
	group "Multiple Creation"
	(
	button btn_multiplePhyMesh "Multiple PhyMesh" align:#center width:110 tooltip:"Create a PhyMesh for every selected object"
	button btn_multipleSubPhyMesh "Multiple SubPhyMesh" align:#center width:110 tooltip:"Create a PhyMesh for every part in every selected object"
	button btn_multipleBounding "Multiple Bounding" align:#center width:110 tooltip:"Create the Best Bounding for every part in the selection"
	button btn_newcomposite "New Composite" align:#center width:110 tooltip:"create a new composite unsing a multiple bounding" offset:[0,10]
	button btn_createcomposite "Create Composite" align:#center width:110 tooltip:"create a new composite using the selection"
	button btn_breakcomposite "Break Composite" align:#center width:110 tooltip:"break the composite object"
	button btn_attachComposite "Attach Composite" align:#center width:110 tooltip:"attach selection into selecte composite"
	button btn_detachComposite "Detach Composite" align:#center width:110 tooltip:"detach selection from his composite"
	)
	group "Multiple Select"
	(
	button btn_selectWalls "Walls" align:#center width:40 across: 3 tooltip:"Select physic objects with wall flag"
	button btn_selectGrounds "Ground" align:#center width:40 tooltip:"Select physic objects with ground flag"
	button btn_selectCeilings "Ceiling" align:#center width:40 tooltip:"Select physic objects with ceiling flag"
	button btn_selectWalkable "Walkable" align:#center width:60 across: 2 tooltip:"Select physic objects with walkable flag"
	button btn_selectNoflag "No flag" align:#center width:60 tooltip:"Select physic objects with no flag"
	button btn_selectSpace "Objects in Space" align:#center width:110 tooltip:"Select physic objects inside actual physic space"
	button btn_selectDuplicates "Select Duplicate" align:#center tooltip:"Select physic objects in the same position"
	)

	local physicMaterial = "basic"
	local physicSpace = "world"
	local physicCategory = "STATIC"
	
	--------------------------------------------------------------------------------
	/**
	*/
	fn updateButtons =
	(
		local objects = getCurrentSelection()
		
		if objects.count > 0 then
		(
			btn_newboxCollision.enabled = false
			btn_AAboxCollision.enabled = true
			btn_boxCollision.enabled = true
			btn_boxCollisionXY.enabled = true
			btn_meshCollision.enabled = true
			btn_bestBounding.enabled = true
			btn_convertToCollision.enabled = true
			btn_multiplePhyMesh.enabled = true
			btn_multipleSubPhyMesh.enabled = true
			btn_multipleBounding.enabled = true
			btn_newcomposite.enabled = true
			btn_createcomposite.enabled = true
			btn_breakcomposite.enabled = true
			btn_attachComposite.enabled = true
			btn_detachComposite.enabled = true
			btn_fixPivot.enabled = true
		)
		else
		(
			btn_newboxCollision.enabled = true
			btn_AAboxCollision.enabled = false
			btn_boxCollision.enabled = false
			btn_boxCollisionXY.enabled = false
			btn_meshCollision.enabled = false
			btn_bestBounding.enabled = false
			btn_convertToCollision.enabled = false
			btn_multiplePhyMesh.enabled = false
			btn_multipleSubPhyMesh.enabled = false
			btn_multipleBounding.enabled = false
			btn_newcomposite.enabled = false
			btn_createcomposite.enabled = false
			btn_breakcomposite.enabled = false
			btn_attachComposite.enabled = false
			btn_detachComposite.enabled = false
			btn_fixPivot.enabled = false
		)

		if physicSpace == "<none>" then
		(
			btn_deleteSpace.enabled = false
			btn_renameSpace.enabled = false
		)
		else
		(
			btn_deleteSpace.enabled = true
			btn_renameSpace.enabled = true
		)
	)
	
	--------------------------------------------------------------------------------
	/**
	*/
	fn updateSpaces =
	(
		dd_spaces.items = NebCollOp.getPhysicSpaceNames()
		
		redefineCollParamsData()
		
		dd_spaces.selection = dd_spaces.items.count
		physicSpace = dd_spaces.selected

		updateButtons()		
	)

	--------------------------------------------------------------------------------
	/**
	*/
	fn updateSelected =
	(
		local object = getCurrentSelection()
		if (object.count >= 1 ) and
		   (isPhysicCollision object) do
		(
			local index = 1
			local property = object[1].CollParams.material
			local same_material = true
			local same_space = true
			local same_category = true
			local same_wall = true
			local same_ground = true
			local same_ceiling = true
			local same_walkable = true
			
		    local obj1_category = getUserProp object[1] "physicCategory"
			-- search the same properties values in all selected objects
			for obj in object do
			(
			    same_material = same_material and ( object[1].CollParams.material == obj.CollParams.material)
			    same_space    = same_space    and ( object[1].CollParams.space    == obj.CollParams.space )
			    same_category = same_category and ( obj1_category == ( getUserProp obj "physicCategory"))
			    same_wall     = same_wall     and ( object[1].CollParams.wall     == obj.CollParams.wall )
			    same_ground   = same_ground   and ( object[1].CollParams.ground   == obj.CollParams.ground )
			    same_ceiling  = same_ceiling  and ( object[1].CollParams.ceiling  == obj.CollParams.ceiling )
			    same_walkable = same_walkable and ( object[1].CollParams.walkable == obj.CollParams.walkable)
			)

			
			if  not same_material  then-- All object has not the same material
			(
			    dd_materials.selection = 0 -- not has selection
			)  else
			(	
    			for name in dd_materials.items do
    			(
    				if name == property do
    				(
    					exit
    				)
    				index = index + 1
    			)
    			if index > dd_materials.items.count then
    			(
    				messagebox ("Collision object " + object[1].name + " has an invalid material\n Change it")
    			)
    			else
    			(
    				dd_materials.selection = index
    				physicMaterial = dd_materials.selected
    			)
    		)

		
			if not same_space then-- All object has not the same material
			(
			    dd_spaces.selection = 0 -- not has selection
			)  else
			(
			    index = 1
			    property = object[1].CollParams.space
    			for name in dd_spaces.items do
    			(
    				if name == property do
    				(
    					exit
    				)
    				index = index + 1
    			)
    			if index > dd_spaces.items.count then
    			(
    				messagebox ("Collision object " + object[1].name + " has an invalid space\n Change it")
    			)
    			else
    			(
    				dd_spaces.selection = index
    				physicSpace = dd_spaces.selected
    			)
    	    )

			if not same_category then-- All object has not the same material
			(
			    dd_categories.selection = 0 -- not has selection
			) else
			(
			    index = 1
			    property = getUserProp object[1] "physicCategory"
    			if property == undefined do
    			(
    				property = "STATIC"
    				setUserProp object[1] "physicCategory" "STATIC"
    			)
    			
    			for name in dd_categories.items do
    			(
    				if name == property do
    				(
    					exit
    				)
    				index = index + 1
    			)
    			if index > dd_categories.items.count then
    			(
    				messagebox ("Collision object " + object[1].name + " has an invalid category\n Change it")
    			)
    			else
    			(
    				dd_categories.selection = index
    				physicCategory = dd_categories.selected
    			)
    		)
    		
    	    if same_wall then
    	    (
			    cb_wall.checked = object[1].CollParams.wall
			) else
			(
			    cb_wall.tristate = 2 -- indeterminate 
			)
			
			if same_ground then
			(
			    cb_ground.checked = object[1].CollParams.ground
			) else
			(
			    cb_ground.tristate = 2 -- Indeterminate
			)
			
			if same_ceiling then
			(
			    cb_ceiling.checked = object[1].CollParams.ceiling
			) else
			(
			    cb_ceiling.tristate = 2
			)
			
			if same_walkable then
			(
			    cb_walkable.checked = object[1].CollParams.walkable
			) else
			(
			    cb_walkable.tristate = 2
		    )
		)
		
		updateButtons()
	)
	
	--------------------------------------------------------------------------------
	/**
	*/
	on nebula_collisions open do
	(
		dd_materials.items = getCollisionMaterials()
		physicMaterial = dd_materials.selected
	
		dd_categories.items = #( "STATIC", "RAMP", "STAIRS" )
		physicCategory = dd_categories.selected
		
		updateSpaces()
		updateSelected()
	
		local txt1 = "nebula_collisions.updateSpaces()"
		local txt2 = "nebula_collisions.updateSelected()"
		
		callbacks.removescripts id:#nebulacollisionsrollout
		callbacks.addscript #systemPostNew txt1 id:#nebulacollisionsrollout
		callbacks.addscript #systemPostReset txt1 id:#nebulacollisionsrollout
		callbacks.addscript #filePostOpen txt1 id:#nebulacollisionsrollout
		callbacks.addscript #selectionSetChanged txt2 id:#nebulacollisionsrollout
	)
	
	--------------------------------------------------------------------------------
	/**
	*/
	on nebula_collisions close do
	(
		callbacks.removescripts id:#nebulacollisionsrollout
	)
	
	--------------------------------------------------------------------------------
	/**
	*/
	on dd_materials selected i do
	(
		physicMaterial = dd_materials.selected
		local objects = getCurrentSelection()
		if (isPhysicCollision objects) do
		(
			changePhysicCollision objects #(physicMaterial, physicSpace, physicCategory) #(true,false,false)
		)
	)
	
	--------------------------------------------------------------------------------
	/**
	*/
	on dd_spaces selected i do
	(
		physicSpace = dd_spaces.selected
		local objects = getCurrentSelection()
		if (isPhysicCollision objects) do
		(
			changePhysicCollision objects #(physicMaterial, physicSpace, physicCategory) #(false,true,false)
		)

		updateButtons()
	)
	
	--------------------------------------------------------------------------------
	/**
	*/
	on dd_categories selected i do
	(
		physicCategory = dd_categories.selected
		local objects = getCurrentSelection()
		if (isPhysicCollision objects) do
		(
			changePhysicCollision objects #(physicMaterial, physicSpace, physicCategory) #(false,false,true)
		)
	)
	
	--------------------------------------------------------------------------------
	/**
	*/
	on cb_wall changed state do
	(
		local objects = getCurrentSelection()
		for obj in objects do
		(
			if (isPhysicCollision obj) do
			(
				obj.CollParams.wall = state
				if( state ) do
				(
					obj.CollParams.ceiling = false
					cb_ceiling.checked = false
					obj.CollParams.ground = false
					cb_ground.checked = false
				)
				
				NebCollOp.setCollisionMaterial obj
			)
		)
	)
	
	--------------------------------------------------------------------------------
	/**
	*/
	on cb_ceiling changed state do
	(
		local objects = getCurrentSelection()
		for obj in objects do
		(
			if (isPhysicCollision obj) do
			(
				obj.CollParams.ceiling = state
				if( state ) do
				(
					obj.CollParams.wall = false
					cb_wall.checked = false
					obj.CollParams.ground = false
					cb_ground.checked = false
				)
				
				NebCollOp.setCollisionMaterial obj
			)
		)
	)
	
	--------------------------------------------------------------------------------
	/**
	*/
	on cb_ground changed state do
	(
		local objects = getCurrentSelection()
		for obj in objects do
		(
			if (isPhysicCollision obj) do
			(
				obj.CollParams.ground = state
				if( state ) do
				(
					obj.CollParams.ceiling = false
					cb_ceiling.checked = false
					obj.CollParams.wall = false
					cb_wall.checked = false
				)
				
				NebCollOp.setCollisionMaterial obj
			)
		)
	)
	
	--------------------------------------------------------------------------------
	/**
	*/
	on cb_walkable changed state do
	(
		local objects = getCurrentSelection()
		for obj in objects do
		(
			if (isPhysicCollision obj) do
			(
				obj.CollParams.walkable = state
			)
		)
	)
	
	--------------------------------------------------------------------------------
	/**
	*/
	on btn_fixPivot pressed do with undo "Fix Pivot" on
	(
		-- Get objects		
		local objects = getCurrentSelection()

		for object in objects do
		(
			object.pivot = object.center
		)
	)
	
	--------------------------------------------------------------------------------
	/**
	*/
	on btn_newboxCollision pressed do with undo "Create Collision" on
	(
		local userProps = #(physicMaterial, physicSpace, 
			cb_wall.checked, cb_ceiling.checked, cb_ground.checked, cb_walkable.checked )

		NebCollOp.newCollisionObj userProps NebCollOp.collisionBox
	)
	
	--------------------------------------------------------------------------------
	/**
	*/
	on btn_AAboxCollision pressed do with undo "Create Collision" on
	(
		-- Get objects		
		local objects = getCurrentSelection()

		local userProps = #(physicMaterial, physicSpace, 
			cb_wall.checked, cb_ceiling.checked, cb_ground.checked, cb_walkable.checked )

		if objects.count > 0 do
		(
			NebCollOp.collisionObj objects userProps NebCollOp.createAACollisionBox 
		)
	)
	
	--------------------------------------------------------------------------------
	/**
	*/
	on btn_boxCollision pressed do with undo "Create Collision" on
	(
		-- Get objects		
		local objects = getCurrentSelection()

		local userProps = #(physicMaterial, physicSpace, 
			cb_wall.checked, cb_ceiling.checked, cb_ground.checked, cb_walkable.checked )

		if objects.count > 0 do
		(
			NebCollOp.collisionObj objects userProps NebCollOp.createCollisionBox 
		)
	)
	
	--------------------------------------------------------------------------------
	/**
	*/
	on btn_boxCollisionXY pressed do with undo "Create Collision" on
	(
		-- Get objects		
		local objects = getCurrentSelection()

		local userProps = #(physicMaterial, physicSpace, 
			cb_wall.checked, cb_ceiling.checked, cb_ground.checked, cb_walkable.checked )

		if objects.count > 0 do
		(
			NebCollOp.collisionObj objects userProps NebCollOp.createCollisionBoxXY 
		)
	)
	
	--------------------------------------------------------------------------------
	/**
	*/
	on btn_sphereCollision pressed do with undo "Create Collision" on
	(
		-- Get objects		
		local objects = getCurrentSelection()
		
		local userProps = #(physicMaterial, physicSpace, 
			cb_wall.checked, cb_ceiling.checked, cb_ground.checked, cb_walkable.checked )

		if objects.count > 0 then
		(
			NebCollOp.collisionObj objects userProps NebCollOp.createCollisionSphere
		)
		else
		(
			NebCollOp.newCollisionObj userProps NebCollOp.collisionSphere
		)
	)
	
	--------------------------------------------------------------------------------
	/**
	*/
	on btn_cylinderCollision pressed do with undo "Create Collision" on
	(
		-- Get objects		
		local objects = getCurrentSelection()
		
		local userProps = #(physicMaterial, physicSpace, 
			cb_wall.checked, cb_ceiling.checked, cb_ground.checked, cb_walkable.checked )

		if objects.count > 0 then
		(
			NebCollOp.collisionObj objects userProps NebCollOp.createCollisionCylinder
		)
		else
		(
			NebCollOp.newCollisionObj userProps NebCollOp.collisionCylinder
		)
	)
	
	--------------------------------------------------------------------------------
	/**
	*/
	on btn_capsuleCollision pressed do with undo "Create Collision" on
	(
		-- Get objects		
		local objects = getCurrentSelection()
		
		local userProps = #(physicMaterial, physicSpace, 
			cb_wall.checked, cb_ceiling.checked, cb_ground.checked, cb_walkable.checked )

		if objects.count > 0 then
		(
			NebCollOp.collisionObj objects userProps NebCollOp.createCollisionCapsule
		)
		else
		(
			NebCollOp.newCollisionObj userProps NebCollOp.collisionCapsule
		)
	)
	
	--------------------------------------------------------------------------------
	/**
	*/
	on btn_meshCollision pressed do with undo "Create Collision" on
	(
		-- Get objects		
		local objects = getCurrentSelection()
		
		local userProps = #(physicMaterial, physicSpace, 
			cb_wall.checked, cb_ceiling.checked, cb_ground.checked, cb_walkable.checked )

		if objects.count > 0 then
		(
			NebCollOp.collisionMeshObj objects userProps
		)
	)
	
	--------------------------------------------------------------------------------
	/**
	*/
	on btn_multiplePhyMesh pressed do with undo "Create Collision" on
	(
		-- Get objects		
		local objects = getCurrentSelection()
		
		local userProps = #(physicMaterial, physicSpace, 
			cb_wall.checked, cb_ceiling.checked, cb_ground.checked, cb_walkable.checked )

		for obj in objects do
		(
			NebCollOp.collisionMeshObj #(obj) userProps
		)	
	)
	
	--------------------------------------------------------------------------------
	/**
	*/
	on btn_multipleSubPhyMesh pressed do with undo "Create Collision" on
	(
		-- Get objects
		local objects = getCurrentSelection()
		
		local userProps = #(physicMaterial, physicSpace, 
			cb_wall.checked, cb_ceiling.checked, cb_ground.checked, cb_walkable.checked )

		for obj in objects do
		(
			local myobj = copy obj
			collapseStack myobj
			convertToMesh myobj
			
			while myobj.numfaces != 0 do
			(
				local elem = meshop.getElementsUsingFace myobj #{1}
				local tmesh = meshop.detachFaces myobj elem delete:true asMesh: true
				update myobj
				
				emesh = Editable_Mesh()
				emesh.mesh = tmesh		
				emesh.pos = myobj.pos
				update emesh
				
				NebCollOp.collisionMeshObj #(emesh) userProps
				
				delete emesh
			)
			
			delete myobj
		)	
	)
	--------------------------------------------------------------------------------
	/**
	*/
	on btn_bestBounding pressed do with undo "Create Collision" on
	(
		-- Get objects		
		local objects = getCurrentSelection()
		
		local userProps = #(physicMaterial, physicSpace, 
			cb_wall.checked, cb_ceiling.checked, cb_ground.checked, cb_walkable.checked )

		if objects.count > 0 then
		(
			NebCollOp.collisionObj objects userProps NebCollOp.createBestBounding
		)
	)
	
	--------------------------------------------------------------------------------
	/**
	*/
	on btn_convertToCollision pressed do with undo "Create Collision" on
	(
		-- Get objects		
		local objects = getCurrentSelection()
		
		local userProps = #(physicMaterial, physicSpace, 
			cb_wall.checked, cb_ceiling.checked, cb_ground.checked, cb_walkable.checked )

		if objects.count > 0 then
		(
			NebCollOp.convertToCollisionObj objects userProps
		)
	)
	
	--------------------------------------------------------------------------------
	/**
	*/
	on btn_multipleBounding pressed do with undo "Create Collision" on
	(
		local objects = getCurrentSelection()
		
		local userProps = #(physicMaterial, physicSpace, 
			cb_wall.checked, cb_ceiling.checked, cb_ground.checked, cb_walkable.checked )

		if objects.count > 0 then
		(
			NebCollOp.collisionMultipleObj objects userProps NebCollOp.createBestBoundingPoints
		)
	)
	
	--------------------------------------------------------------------------------
	/**
	*/
	on btn_physicSpace pressed do with undo "Create Physic Space" on
	(
		nebula_getname_text = "Name can not begin with a number"
		createDialog nebula_getname modal: true
		if nebula_getname.end_state do
		(
			local name = nebula_getname.ed_name.text

			if not ( isDigit name[1] ) then
			(
				-- Search for repetition of name
				if undefined == (NebCollOp.searchPhysicSpace name) then
				(
					-- Add name to spaces
					NebCollOp.newPhysicSpace name
					
					updateSpaces()
				)
				else
				(
					messageBox "Physic Space already created"
				)
			)
			else
			(
				messageBox "Physic Space name can't begin with number"
			)
		)
	)

	--------------------------------------------------------------------------------
	/**
	*/
	on btn_deleteSpace pressed do with undo "Delete Physic Space" on
	(
		if "<none>" != physicSpace do
		(
			NebCollOp.deletePhysicSpace physicSpace

			updateSpaces()
		)
	)

	--------------------------------------------------------------------------------
	/**
	*/
	on btn_renameSpace pressed do with undo "Rename Physic Space" on
	(
		if "<none>" != physicSpace do
		(
			nebula_getname_text = "Name can not begin with a number"
			createDialog nebula_getname modal: true
			if nebula_getname.end_state do
			(
				local name = nebula_getname.ed_name.text
	
				if not ( isDigit name[1] ) then
				(
					-- Search for repetition of name
					if ("<none>" != name) and 
						(undefined == (NebCollOp.searchPhysicSpace name)) then
					(
						NebCollOp.renamePhysicSpace physicSpace name
						
						updateSpaces()
					)
					else
					(
						messageBox "Physic Space already exist"
					)
				)
				else
				(
					messageBox "Physic Space name can't begin with number"
				)
			)
		)
	)
	
	--------------------------------------------------------------------------------
	/**
	*/
	on btn_newcomposite pressed do
	(
		local objects = getCurrentSelection()
		
		local userProps = #(physicMaterial, physicSpace, 
			cb_wall.checked, cb_ceiling.checked, cb_ground.checked, cb_walkable.checked )

		if objects.count > 0 then
		(
			NebCollOp.collisionCompositeMultipleObj objects userProps NebCollOp.createBestBoundingPoints
			loadChangeHandlers()
		)
	)
	
	--------------------------------------------------------------------------------
	/**
	*/
	on btn_createcomposite pressed do with undo "Create Collision" on
	(
		local objects = getCurrentSelection()
		local userProps = #(physicMaterial, physicSpace, 
			cb_wall.checked, cb_ceiling.checked, cb_ground.checked, cb_walkable.checked )

		if( (objects.count > 1) and (isPhysicCollision objects) and 
			(notPhysicCollisionComposite objects) )do
		(
			NebCollOp.createCollisionCompositeObj objects userProps
			loadChangeHandlers()
		)
	)
	
	--------------------------------------------------------------------------------
	/**
	*/
	on btn_breakcomposite pressed do with undo "Break Composite" on
	(
		local objects = getCurrentSelection()
		for object in objects do
		(
			if( isPhysicCollisionComposite object ) then
			(
				NebCollOp.breakCollisionCompositeObj object
			)
			else if( true == (getUserProp object "physicComposite") ) do
			(
				local composite = (getCompositeParent object)
				if( composite != undefined ) then
				(
					NebCollOp.breakCollisionCompositeObj composite
				)
				else
				(
					setUserProp object "physicComposite" false
				)
			)
		)
	)
	
	--------------------------------------------------------------------------------
	/**
	*/
	on btn_attachComposite pressed do with undo "Attach Composite" on
	(
		local objects = getCurrentSelection()
		NebCollOp.attachCollisionComposite objects
	)
	
	--------------------------------------------------------------------------------
	/**
	*/
	on btn_detachComposite pressed do with undo "Detach Composite" on
	(
		local objects = getCurrentSelection()
		NebCollOp.detachCollisionComposite objects
	)
	
	--------------------------------------------------------------------------------
	/**
	*/
	on btn_selectWalls pressed do
	(
		clearSelection()
		local objects = $CollObject*
		for object in objects do
		(
			if object.wall do
			(
				selectMore object				
			)			
		)
	)
	
	--------------------------------------------------------------------------------
	/**
	*/
	on btn_selectGrounds pressed do
	(
		clearSelection()
		local objects = $CollObject*
		for object in objects do
		(
			if object.ground do
			(
				selectMore object				
			)			
		)
	)
	
	--------------------------------------------------------------------------------
	/**
	*/
	on btn_selectCeilings pressed do
	(
		clearSelection()
		local objects = $CollObject*
		for object in objects do
		(
			if object.ceiling do
			(
				selectMore object				
			)			
		)
	)

	--------------------------------------------------------------------------------
	/**
	*/
	on btn_selectWalkable pressed do
	(
		clearSelection()
		local objects = $CollObject*
		for object in objects do
		(
			if object.walkable do
			(
				selectMore object				
			)			
		)
	)

	--------------------------------------------------------------------------------
	/**
	*/
	on btn_selectNoflag pressed do
	(
		clearSelection()
		local objects = $CollObject*
		for object in objects do
		(
			if not (object.ground or object.ceiling or object.walkable or object.wall) do
			(
				selectMore object				
			)			
		)
	)

	--------------------------------------------------------------------------------
	/**
	*/
	on btn_selectSpace pressed do
	(
		clearSelection()
		local objects = $CollObject*
		for object in objects do
		(
			if object.space == physicSpace do
			(
				selectMore object				
			)			
		)
	)
	
	--------------------------------------------------------------------------------
	/**
	*/
	on btn_selectDuplicates pressed do
	(
		selectDuplicatePhysicObjects()
	)
)
--------------------------------------------------------------------------------
--   EOF
--------------------------------------------------------------------------------
