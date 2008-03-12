/**
   @file collisionObjects.ms
   @ingroup n3dsMaxScripts
   @author Luis Jose Cabellos Gomez <luis.cabellos@tragnarion.com>
   @brief Collision Layer functions.
   @remarks
   Physic Collision Object Properties
   - physicObj         = true
   - physicType        = 0, physic space
                         1, collision object
   - physicGeom        = [composite | box | sphere | cylinder | mesh]
   - physicMaterial    = <material>
   - physicSpace       = <space>
   - physicParentSpace = <parent space>, only in spaces
   - physicComposite   = true, if is children of a composite
*/
--------------------------------------------------------------------------------
/**
	Globals
*/
colllog = NebCreateLog "collision"  "collision module" 2 false

global nebCollSavedPreMerge = #()
global nebCollData

struct NebStructCollData
(
    minFloat,
    invalidFloat-- value not persistable
)

--------------------------------------------------------------------------------
/**
	Struct data
*/

--------------------------------------------------------------------------------
/**
	Nebula Collision Operations
*/
struct NebCollOp
(
	----------------------------------------------------------------------------
	/**
	*/
    fn declarePersistent =
    (
        persistent global nebCollSpaceNames
    ),
    
	----------------------------------------------------------------------------
	/**
	*/
	fn declareVar =
	(	
		nebNameSavedPreMerge = #()
		global nebCollData = NebStructCollData()
	),
	
	----------------------------------------------------------------------------
	/**
	*/
	fn resetValues = 
	(
	    nebCollData.minFloat = 0.0001
	    nebCollData.invalidFloat = 0.01 * nebCollData.minFloat
	),

	----------------------------------------------------------------------------
	/**
	*/
	fn initModule =
	(
		NebCollOp.declareVar()
		NebCollOp.resetValues()
		
		redefineCollParamsData()
	),
	
	----------------------------------------------------------------------------
	/**
	*/
	fn updateFromVersion version =
	(
		if( version < 000300 )do
		(
			colllog.log 4 ("Update from " + (version as string) + " to 000300" )
			-- Update from 0.2.0 to 0.3.0
		)
		
		if( version < 000400 )do
		(
			colllog.log 4 ("Update from " + (version as string) + " to 000400" )
			-- Update from 0.3.0 to 0.4.0
			for obj in $CollObject* do
			(
				if obj.material != undefined do
				(
					obj.wireColor = obj.material.diffuse
				)
			)
		)
		
		if( version < 000500 )do
		(
			colllog.log 4 ("Update from " + (version as string) + " to 000500" )
			nebCollSpaceNames = #("<none>")
			
			-- delete all old spaces
			local objects = $PhysicSpace* as array
			for space in objects do
			(
				-- put objects spaces in space array
				local spaceName = getUserProp space "physicSpace" 
				if (spaceName != "world") and 
					(spaceName != "interior") and 
					(spaceName != "exterior") then
				(
					append nebCollSpaceNames spaceName
				)
				delete space
			)
		)
		
		redefineCollParamsData()
		if undefined != nebula_collisions do
		(
			nebula_collisions.updateSpaces()
		)
	),
	
    --------------------------------------------------------------------------------
    /**
        Callback file preMerge
        Create copy of all persistent variables
    */    
    fn FilePreMerge =
	(
		nebCollSavedPreMerge = #()
		local item = #()
		-- save collision params
		for obj in $CollObject* do
		(
			item = #()
			append item obj
			append item obj.CollParams.wall
			append item obj.CollParams.ground
			append item obj.CollParams.ceiling
			append item obj.CollParams.walkable			
			append nebCollSavedPreMerge  item
		)
	),
	
    --------------------------------------------------------------------------------
    /**
        Callback file  postMerge
        @version the nebula version of objects imported
        @listObjects array of objects imported
        restore the persistent variables
        
    */    
    fn FilePostMerge version listObjects =
	(
		for obj in $CollObject* do
		(
			NebCollOp.updateObjectFromMerge obj
		)
		
		redefineCollParamsData()
		
		-- recover collision params
		for obj in nebCollSavedPreMerge do
		(
			obj[1].CollParams.wall = obj[2]
			obj[1].CollParams.ground = obj[3]
			obj[1].CollParams.ceiling = obj[4]
			obj[1].CollParams.walkable = obj[5]
		)
		nebCollSavedPreMerge = #()

		-- add objects spaces to the space array
		local phyObjects = #()
		for obj in $* do
		( 
			if isPhysicCollision( obj ) do
			(
				local spaceName = obj.CollParams.space
				if (spaceName == "world") or 
					(spaceName == "interior") or 
					(spaceName == "exterior") then
				(
					obj.CollParams.space = "<none>"
				)
				else if 0 == (findItem nebCollSpaceNames spaceName) do
				(
					append nebCollSpaceNames spaceName
				)
			)
		)

        -- delete all old spaces
        local objects = $PhysicSpace* as array
        for space in objects do
        (
            delete space
        )
		
		redefineCollParamsData()
		if undefined != nebula_collisions do
		(
			nebula_collisions.updateSpaces()
		)
	),
	
	----------------------------------------------------------------------------
	/**
	*/
	fn updateObjectCenter obj=
	(
		obj.pivot = obj.center
	),
	
	----------------------------------------------------------------------------
	/**
	*/
	fn updateObjectFromMerge obj=
	(
        local val = getUserprop obj "z_type" 
        if ( val == 1 ) do 
        ( 
        	nebOp.SetType obj "physics"
        )

		if nebOP.IsType obj "physics" do
		(
			NebCollOp.updateObjectCenter obj
			if obj.material != undefined do
			(
				obj.wireColor = obj.material.diffuse
			)
		)
	),
	
	----------------------------------------------------------------------------
	/**
	*/
	fn createBasicSpaces=
	(
		if nebCollSpaceNames == undefined do
		(
			nebCollSpaceNames = #()
			append nebCollSpaceNames "<none>"
		)
	),
	
	----------------------------------------------------------------------------
	/**
	*/
	fn setCollisionMaterial obj = 
	(
		if (getUserProp obj "physicComposite") == true then
		(
			setCollisionTexture obj "composite"		
		)
		else if obj.CollParams.wall then
		(
			setCollisionTexture obj "wall"		
		)
		else if obj.CollParams.ceiling then
		(
			setCollisionTexture obj "ceiling"		
		)
		else if obj.CollParams.ground then
		(
			setCollisionTexture obj "ground"		
		)
		else
		(
			setCollisionTexture obj "normal"		
		)
	),
	
	----------------------------------------------------------------------------
	/**
	*/
	fn setCollisionParams objCollision props=
	(
		if props[3] then
		(
			setCollisionTexture objCollision "wall"		
		)
		else if props[4] then
		(
			setCollisionTexture objCollision "ceiling"		
		)
		else if props[5] then
		(
			setCollisionTexture objCollision "ground"		
		)
		else
		(
			setCollisionTexture objCollision "normal"		
		)
		
		objCollision.CollParams.material = props[1]
		objCollision.CollParams.space = props[2]
		objCollision.CollParams.wall = props[3]
		objCollision.CollParams.ceiling = props[4]
		objCollision.CollParams.ground = props[5]
		objCollision.CollParams.walkable = props[6]
	),
	
	fn validateBox myBox =
    (
        local minFloat  = nebOp.NebToMaxUnits nebCollData.minFloat
		local validLen  = mybox.length   >= minFloat
		local validwid  = mybox.width  >= minFloat
		local validheig = mybox.height  >= minFloat
		
		if ( not validLen ) do mybox.length = minFloat
		if ( not validwid ) do mybox.width   = minFloat
		if ( not validheig) do mybox.height  = minFloat
			
		--detect is a box or is a plane
		if ( validLen and (validwid or validheig ) ) or  ( validwid and validheig)  then
		(
		    return myBox
		) else
		(
		    delete myBox
		    return undefined
	    )
    ),
	
	----------------------------------------------------------------------------
	/**
	*/
	fn createAACollisionBox mesh=
	(
		-- Calculate minimum and maximum vertex
		local vertex = GetVert mesh 1
		local minim = copy vertex
		local maxim = copy vertex
		for idx_vertex = 2 to mesh.numverts do
		(
			vertex = GetVert mesh idx_vertex
	
			-- update minimum an maximum values
			minim = (minimumPoint minim vertex)
			maxim = (maximumPoint maxim vertex)
		)
		
		-- create the box
		local len = abs(maxim.y - minim.y) as Float
		local wid = abs(maxim.x - minim.x) as Float
		local heig = abs(maxim.z - minim.z) as Float

		local mybox = Box prefix:"CollObject"
		mybox.length = len
		mybox.width = wid
		mybox.height = heig
		mybox.pivot = mybox.center
		mybox.pos.x = (maxim.x + minim.x)/2.0
		mybox.pos.y = (maxim.y + minim.y)/2.0
		mybox.pos.z = (maxim.z + minim.z)/2.0
		
		setUserProp mybox "physicGeom" "box"
		
		return (NebCollOp.validateBox mybox)
	),
	
	----------------------------------------------------------------------------
	/**
	*/
	fn createCollisionBox mesh =
	(
		local objects = #() + mesh
	   	local points = CreatePoints objects
	   	local minFloat  = nebOp.NebToMaxUnits nebCollData.invalidFloat
	   	local vol = CreateVolFromPoints points minValue:minFloat
	
		local mybox = Box prefix:"CollObject"
	
	   	vol.changeBox mybox
		
		setUserProp mybox "physicGeom" "box"
	
		return (NebCollOp.validateBox mybox)
	),
	
	----------------------------------------------------------------------------
	/**
	*/
	fn createCollisionBoxXY mesh =
	(
		local objects = #() + mesh
	   	local points = CreatePoints objects
	
		-- move all points to the middle z
		local minZ = points[1].z
		local maxZ = points[1].z
		for point in points do
		(
			if point.z < minZ do
			(
				minZ = copy point.z
			)
	
			if point.z > maxZ do
			(
				maxZ = copy point.z
			)
		)
		
		centerZ = (maxZ + minZ) / 2.0
	
		reducePoints2D points 
	
		for point in points do
		(
			point.z = centerZ
		)
		
		local minFloat  = nebOp.NebToMaxUnits nebCollData.invalidFloat
	   	local vol = CreateVolFromPoints points minValue:minFloat
	
		local mybox = Box prefix:"CollObject"
	
	   	vol.changeBox mybox
		mybox.width = abs( maxZ - minZ )
		
		setUserProp mybox "physicGeom" "box"
		
		return (NebCollOp.validateBox mybox)
	),
	
	--------------------------------------------------------------------------------
	/**
	*/
	fn collisionBox=
	(
		local mybox = Box prefix:"CollObject"
		mybox.pivot = mybox.center
		
		setUserProp mybox "physicGeom" "box"
		
		return mybox
	),
	
	
	----------------------------------------------------------------------------
	/**
	*/
	fn validateSphere mysphere =
	(
	    local minFloat  = nebOp.NebToMaxUnits nebCollData.minFloat
		if ( mysphere.radius >=  minFloat) then
		(
		    return mysphere
		) else
		(
		    delete mysphere
		    return undefined
		)
	),
	
	----------------------------------------------------------------------------
	/**
	*/
	fn createCollisionSphere mesh=
	(
		local point1
		local point2
	
		-- calculate 2 points far from each other
		point1 = GetVert mesh 1
		point2 = GetVert mesh 2
		local maxdistance = length ( point1 - point2 )
		for idx_vertex = 3 to mesh.numverts do
		(
			local temp = GetVert mesh idx_vertex
			local dist1 = length ( point1 - temp )	
			local dist2 = length ( point2 - temp )
			
			if (dist1 > dist2) and (dist1 > maxdistance) then
			(
				maxdistance = dist1
				point2 = temp
			)
			else if (dist2 > dist1) and (dist2 > maxdistance) do
			(
				maxdistance = dist2
				point1 = temp
			)
		)
		
		-- calculate the center
		center = (point1 + point2) / 2.0
		
		-- calculate the radius
		maxdistance = maxdistance / 2.0
		for idx_vertex = 1 to mesh.numverts do
		(
			local vertex =  GetVert mesh idx_vertex
			local distance = length (vertex - center)
			if distance > maxdistance do
			(
				maxdistance = distance
			)
		)
		
	    -- create the sphere
	    local mysphere = Sphere prefix:"CollObject"
	    mysphere.pos = center
	    mysphere.radius = maxdistance
	    mysphere.segs = 30

	    setUserProp mysphere "physicGeom" "sphere"
	    return ( NebCollOp.validateSphere mysphere)
	),
	
	----------------------------------------------------------------------------
	/**
	*/
	fn collisionSphere=
	(
		local mysphere = Sphere prefix:"CollObject"
		mysphere.segs = 30
	
		setUserProp mysphere "physicGeom" "sphere"
	
		return ( NebCollOp.validateSphere mysphere)
	),
	
	----------------------------------------------------------------------------
	/**
	*/	
	fn validateCylinder mycyl =
	(
	    local minFloat  = nebOp.NebToMaxUnits nebCollData.minFloat
	    if ( mycyl.height >=minFloat) and (mycyl.radius >=minFloat ) then
		(
		    return mycyl 
		) else
		(
		    delete mycyl
		    return undefined
		)
	),
	
	----------------------------------------------------------------------------
	/**
	*/
	fn createCollisionCylinder mesh=
	(
		local objects = #() + mesh
	   	local points = CreatePoints objects
	   	local minFloat  = nebOp.NebToMaxUnits nebCollData.invalidFloat
	   	local vol = CreateVolFromPoints points minValue:minFloat
	
		-- create the capsule
		local mycyl = Cylinder prefix:"CollObject"
		
	   	vol.changeCylinder mycyl
		setUserProp mycyl "physicGeom" "realcylinder"
		return ( NebCollOp.validateCylinder mycyl)
	),
	
	----------------------------------------------------------------------------
	/**
	*/
	fn collisionCylinder=
	(
		local mycyl = Cylinder prefix:"CollObject"
		mycyl.pivot = mycyl.center
	
		setUserProp mycyl "physicGeom" "realcylinder"
	
		return mycyl 
	),
	
    ----------------------------------------------------------------------------
	/**
	*/
	fn validateCapsule mycap =
	(
	    local minFloat  = nebOp.NebToMaxUnits nebCollData.minFloat
	    
		if ( mycap.height >= minFloat) and (mycap.radius >=minFloat ) then
		(
		    return mycap 
		) else
		(
		    delete mycap
		    return undefined
		)
	),
	
	----------------------------------------------------------------------------
	/**
	*/
	fn createCollisionCapsule mesh=
	(
		local objects = #() + mesh
	   	local points = CreatePoints objects
	   	local minFloat  = nebOp.NebToMaxUnits nebCollData.invalidFloat
	   	local vol = CreateVolFromPoints points minValue:minFloat
	
		-- create the capsule
		local mycap = Capsule prefix:"CollObject"
		
	   	vol.changeCapsule mycap
		
		setUserProp mycap "physicGeom" "cylinder"
		
	    return (NebCollOp.validateCapsule myCap)
	),
	
	----------------------------------------------------------------------------
	/**
	*/
	fn collisionCapsule=
	(
		local mycap = Capsule prefix:"CollObject"
		mycap.heightType = 0
		mycap.height = 50;
		mycap.pivot = mycap.center
	
		setUserProp mycap "physicGeom" "cylinder"
	
		return mycap 
	),
	
	----------------------------------------------------------------------------
	/**
	*/
	fn createBestBounding mesh =
	(
		local objects = #() + mesh
	   	local points = CreatePoints objects
	   	local minFloat  = nebOp.NebToMaxUnits nebCollData.invalidFloat	   	
	   	local vol = CreateVolFromPoints points minValue:minFloat
	
	   	local obj = vol.AutomaticCreate()
		obj.name = uniqueName "CollObject"
		
	
		case (classof obj) of
		(
			(Box): (
				setUserProp obj "physicGeom" "box"
				obj = NebCollOp.validateBox obj
			)
			(Sphere): (
				setUserProp obj "physicGeom" "sphere"
				obj = NebCollOp.validateSphere obj
			)
			(Capsule): (
				setUserProp obj "physicGeom" "cylinder"
				obj = NebCollOp.validateCapsule obj
			)
			(Cylinder): (
				setUserProp obj "physicGeom" "realcylinder"
				obj = NebCollOp.validateCylinder obj
			)
		)
	
		return obj
	),
	
	----------------------------------------------------------------------------
	/**
	*/
	fn createBestBoundingPoints points =
	(
	    local minFloat  = nebOp.NebToMaxUnits nebCollData.invalidFloat
	   	local vol = CreateVolFromPoints points minValue:minFloat
	
	   	local obj = vol.AutomaticCreate()
		obj.name = uniqueName "CollObject"
	
		case (classof obj) of
		(
			(Box): (
				setUserProp obj "physicGeom" "box"
				obj = NebCollOp.validateBox obj
			)
			(Sphere): (
				setUserProp obj "physicGeom" "sphere"
				obj = NebCollOp.validateSphere obj
			)
			(Capsule): (
				setUserProp obj "physicGeom" "cylinder"
				obj = NebCollOp.validateCapsule obj
			)
			(Cylinder): (
				setUserProp obj "physicGeom" "realcylinder"
				obj = NebCollOp.validateCylinder obj
			)
		)
	
		return obj
	),
	
	----------------------------------------------------------------------------
	/**
		@param props array of propertys [material, space]
	*/
	fn collisionObj objects props funcCollision=
	(
		-- Select Collision Layer
		local old_layer = LayerManager.current
		setCollisionLayer()
	
		-- Join all objects in a Editable Mesh
		local mesh = joinObjects objects
		
		-- Create Collision Object
		if mesh.numverts > 0 do
		(
			local objCollision = funcCollision mesh
			
		    if ( objCollision != undefined ) then
		    (
    			
    			NebOp.SetType objCollision "physics" 
    			
    			custAttributes.add objCollision CollParamsData
    			
    			NebCollOp.setCollisionParams objCollision props 
    			
    			setUserProp objCollision "physicObj" "true"
    			setUserProp objCollision "physicType" "1"
    			setUserProp objCollision "physicComposite" "false"
    			
    			-- Link collision with objects
    			local selec = SelectParentNodes( objects )
    			local parent = LinkWithFirst( selec )
    			objCollision.parent = parent
    			select parent
    		) else
    		(
    		    local errorName = " Not create  collison to "  + (objects[1] as string )
    		    messageBox errorName
    		)
		)
		
		delete mesh
		
		-- Select Old Layer
		old_layer.current = true
	),
	
	----------------------------------------------------------------------------
	/**
		@param props array of propertys [material, space]
	*/
	fn collisionMeshObj objects props =
	(
		-- Select Collision Layer
		local old_layer = LayerManager.current
		setCollisionLayer()
	
		-- Join all objects in a Editable Mesh
		local objCollision = joinObjects objects
		objCollision.name = uniqueName "CollObject"
		setUSerProp objCollision "physicGeom" "mesh"
		
		-- Create Collision Object
		if objCollision.numverts > 0 then
		(
			NebOp.SetType objCollision "physics" 
			custAttributes.add objCollision CollParamsData
	
			NebCollOp.setCollisionParams objCollision props 
			
			setUserProp objCollision "physicObj" "true"
			setUserProp objCollision "physicType" "1"
			setUserProp objCollision "physicComposite" "false"
			
			-- Link collision with objects
			local selec = SelectParentNodes( objects )
			local parent = LinkWithFirst( selec )
			objCollision.parent = parent
			select parent
		)
		else
		(
			delete objCollision
		)
		
		-- Select Old Layer
		old_layer.current = true
	),
	
	----------------------------------------------------------------------------
	/**
		@param props array of propertys [material, space]
	*/
	fn collisionMultipleObj objects props funcCollision=
	(
		-- Select Collision Layer
		local old_layer = LayerManager.current
		setCollisionLayer()
	
		local newobjects = #()
		for obj in objects do
		(
			append newobjects (copy obj)
		)
		
		resetxform newobjects

		local listPoints = createListPoints newobjects
		
		delete newobjects
	    
	    local idx = 1
		for points in listPoints do
		(
	       	local objCollision = funcCollision points
	       	if  objCollision != undefined then 
	       	(
			
    			NebOp.SetType objCollision "physics" 
    			custAttributes.add objCollision CollParamsData
    	
    			NebCollOp.setCollisionParams objCollision props 
    	
    			setUserProp objCollision "physicObj" "true"
    			setUserProp objCollision "physicType" "1"
    			setUserProp objCollision "physicComposite" "false"
    			
    			-- Link collision with objects
    			local selec = SelectParentNodes( objects )
    			local parent = LinkWithFirst( selec )
    			objCollision.parent = parent
    			select parent
    		) else
    		(
    		    local errorName = " Not create  collison to "  + (objects[idx] as string )
    		    messageBox errorName
    		)
    		idx = idx + 1
	   	)
		
		-- Select Old Layer
		old_layer.current = true
	),
	
	----------------------------------------------------------------------------
	/**
		@param props array of propertys [material, space]
	*/
	fn newCollisionObj props funcCollision=
	(
		-- Select Collision Layer
		local old_layer = LayerManager.current
		setCollisionLayer()
	
		local objCollision = funcCollision prefix:"CollObject"
		
		if ( objCollision != undefined ) then
		(
		
    		NebOp.SetType objCollision "physics" 
    		custAttributes.add objCollision CollParamsData
    	
    		NebCollOp.setCollisionParams objCollision props 
    	
    		setUserProp objCollision "physicObj" "true"
    	    setUserProp objCollision "physicType" "1"
    		setUserProp objCollision "physicComposite" "false"
    		select objCollision
    	) else
    	(
    	    local errorName = " Not create  collison object " 
    		messageBox errorName
    	)
		
		-- Select Old Layer
		old_layer.current = true
	),
		
	----------------------------------------------------------------------------
	/**
	    TODO: TEsT it is valid sizes
	*/
	fn convertToCollisionObj objects props =
	(
		local collision_layer = getCollisionLayer()
		
		for obj in objects do
		(
			if not (isPhysicCollision obj) do
			(
				local geomType = undefined
				
				local phyObj = copy obj
				delete obj

				case (classof phyObj) of
				(
					(Box): (
						geomType = "box"
					)
					(Sphere): (
						geomType = "sphere"
					)
					(Capsule): (
						geomType = "cylinder"
					)
					(Editable_mesh): (
						geomType = "mesh"
					)
					(Cylinder): (
						geomType = "realcylinder"
					)
				)
				
				if geomType != undefined do
				(
					collision_layer.addnode phyObj
					phyObj.name = uniqueName "CollObject"
					NebOp.SetType phyObj "physics"
					custAttributes.add phyObj CollParamsData
					NebCollOp.setCollisionParams phyObj props
					
					setUserProp phyObj "physicObj" "true"
					setUserProp phyObj "physicType" "1"
					setUserProp phyObj "physicComposite" "false"
					setUserProp phyObj "physicGeom" geomType
					
					if geomType != "mesh" do
					(
						phyObj.pivot = phyObj.center
					)
					
				)
			)
		)
	),
	
	----------------------------------------------------------------------------
	/**
		@param props array of propertys [material, space]
	*/
	fn createCollisionCompositeObj objects props =
	(
		-- Select Collision Layer
		local old_layer = LayerManager.current
		setCollisionLayer()
	
		local composite = Dummy prefix:"CollObject"
		NebOp.SetType composite "physics" 
		custAttributes.add composite CollParamsData
	
		NebCollOp.setCollisionParams composite props 
	
		setUserProp composite "physicObj" "true"
	    setUserProp composite "physicType" "1"
		setUserProp composite "physicGeom" "composite"
		setUserProp composite "physicComposite" "false"
		custAttributes.add composite CollCompositeParamsData
	
		for object in objects do
		(
			setUserProp object "physicComposite" "true"
			append composite.CollCompositeParams.geometries object
			setCollisionTexture object "composite"
		)
		
		calculateCompositeCenter composite
		
		-- Select Old Layer
		old_layer.current = true
	),
	
	----------------------------------------------------------------------------
	/**
		@param props array of propertys [material, space]
	*/
	fn collisionCompositeMultipleObj objects props funcCollision =
	(
		-- Select Collision Layer
		local old_layer = LayerManager.current
		setCollisionLayer()
	
		objects = objects as Array
		local listPoints = createListPoints objects
	    
		local children = #()
		
		local idx = 1
		for points in listPoints do
		(
	       	local objCollision = funcCollision points
	       	
	       	if (objCollision!= undefined ) then
	       	(
    			NebOp.SetType objCollision "physics" 
    			custAttributes.add objCollision CollParamsData
    		
    			NebCollOp.setCollisionParams objCollision props 
    	
    			setUserProp objCollision "physicObj" "true"
    			setUserProp objCollision "physicType" "1"
    			setUserProp objCollision "physicComposite" "false"
    			
    			-- Link collision with objects
    			local selec = SelectParentNodes( objects )
    			local parent = LinkWithFirst( selec )
    			objCollision.parent = parent
    			select parent
    			
    			append children objCollision
    		) else
    		(
    		    local errorName = " Not create  collison to "  + (objects[idx] as string )
    		    messageBox errorName
    		)
    		idx = idx + 1
	   	)
		
		-- Select Old Layer
		old_layer.current = true

		NebCollOp.createCollisionCompositeObj children props
	),
	
	----------------------------------------------------------------------------
	/**
	*/
	fn breakCollisionCompositeObj composite =
	(
		local objects = composite.CollCompositeParams.geometries
		
		for object in objects do
		(
			if object != undefined and (not isDeleted object) do
			(
				setUserProp object "physicComposite" "false"
				NebCollOp.setCollisionMaterial object
			)
		)
		
		delete composite
	),
	
	----------------------------------------------------------------------------
	/**
	*/
	fn attachCollisionComposite objects =
	(
		-- find free objects
		local free_objects = #()
		local comp_objects = #()
		
		for obj in objects do
		(
			if isPhysicCollision obj do
			(
				if notPhysicCollisionComposite obj then
				(
					append free_objects obj
				)
				else
				(
					append comp_objects obj
				)
			)
		)
		
		-- find composite
		local composite = undefined
		for obj in comp_objects do
		(
			if isPhysicCollisionComposite obj do
			(
				composite = obj
				exit
			)
			composite = getCompositeParent obj
			if composite != undefined do
			(
				exit
			)
		)
		
		-- attach free objects to composite
		if composite != undefined do
		(
			for obj in free_objects do
			(
				setUserProp obj "physicComposite" "true"
				append composite.CollCompositeParams.geometries obj
				setCollisionTexture obj "composite"
			)
			
			calculateCompositeCenter composite
		)
	),
	--------------------------------------------------------------------------------
	/**
	*/
	fn detachCollisionComposite objects =
	(
		-- find composite childrens to detach
		local child_objects = #()
		for obj in objects do
		(
			if (getUserProp obj "physicComposite") == true do
			(
				append child_objects obj
			)
		)
		
		-- detach from parent
		for obj in child_objects do
		(
			local composite = getCompositeParent obj
			if composite != undefined do
			(
				local ind = findItem composite.geometries obj
				deleteItem composite.geometries ind
				if composite.geometries.count <= 0 then
				(
					delete composite
				)
				else
				(
					calculateCompositeCenter composite
				)
				
				setUserProp obj "physicComposite" "false"
				NebCollOp.setCollisionMaterial obj
			)
		)
	),
	
	--------------------------------------------------------------------------------
	/**
	*/
	fn getPhysicSpaceNames =
	(
		if nebCollSpaceNames == undefined do
		(
			NebCollOp.createBasicSpaces()
		)
		
		return nebCollSpaceNames
	),
	
	--------------------------------------------------------------------------------
	/**
	*/
	fn searchPhysicSpace name =
	(
		if nebCollSpaceNames == undefined do
		(
			NebCollOp.createBasicSpaces()
		)
		
		local space = findItem nebCollSpaceNames name
		if space == 0 do
		(
			return undefined
		)
		
		return space
	),
	
	--------------------------------------------------------------------------------
	/**
	*/
	fn newPhysicSpace name =
	(
		if undefined == (NebCollOp.searchPhysicSpace name) do
		(
			if undefined == nebCollSpaceNames do
			(
				NebCollOp.createBasicSpaces()
			)
			append nebCollSpaceNames name
		)
	),
	
	--------------------------------------------------------------------------------
	/**
	*/
	fn deletePhysicSpace name =
	(
		local space = (NebCollOp.searchPhysicSpace name)
		if undefined != space then
		(
			-- search objects in that space an put in <none>
			for object in $* do
			(
				if isPhysicCollision object do
				(
					if object.space == name do
					(
						object.space = "<none>"
					)
				)
			)
		
			-- delete space
			deleteItem nebCollSpaceNames space
		)
		else
		(
		    messageBox "No Physic Space to Delete"
		)
	),
	
	--------------------------------------------------------------------------------
	/**
	*/
	fn renamePhysicSpace name newname =
	(
		local space = (NebCollOp.searchPhysicSpace name)
		if undefined != space then
		(
			-- rename space
			deleteItem nebCollSpaceNames space
			append nebCollSpaceNames newname
			
			-- search objects in that space an put in new name
			for object in $* do
			(
				if isPhysicCollision object do
				(
					if object.space == name do
					(
						object.space = newname
					)
				)
			)
		)
		else
		(
		    messageBox "No Physic Space to Rename"
		)
	)

)

--------------------------------------------------------------------------------
--   EOF
--------------------------------------------------------------------------------
