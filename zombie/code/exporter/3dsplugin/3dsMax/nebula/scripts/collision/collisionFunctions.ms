/**
    @file collisionFunctions.ms
    @ingroup n3dsMaxScripts
    @author Luis Jose Cabellos Gomez
    @brief Collision Layer functions.
    @remarks
    Physic Collision Object Properties
    - physicObj         = true
    - physicType        = 0, physic space
                         1, collision object
    - physicGeom        = [composite | box | sphere | cylinder]
    - physicMaterial    = <material>
    - physicSpace       = <space>
    - physicParentSpace = <parent space>, only in spaces
    - physicComposite   = true, if is children of a composite
    
    (C) 2006 Conjurer Services, S.A.
*/
--------------------------------------------------------------------------------
global nebNewExportOptions

--------------------------------------------------------------------------------
/**
*/
fn setCollisionTexture obj name=
(
	if obj != undefined do
	(
	
		local materialName = "Collision Material " + name;
		local material = undefined
		
		-- Search collision material
		for mat in sceneMaterials do
		(
			if mat.name == materialName do
			(
				material = mat
				exit
			)
		)
	
		for mat in meditMaterials do
		(
			if mat.name == materialName do
			(
				material = mat
				exit
			)
		)
	
	    if material == undefined do
	    (
		    -- the material no need to undo because is internal to collision
		    undo off
		    (
			    -- Create collision material
			    material = standardMaterial()
			    material.name = materialName
			    material.opacity = 30.0
			    case name of (
				    "wall":      material.diffuse = ( color 255.0 255.0   0.0 )
				    "ground":    material.diffuse = ( color   0.0 255.0 255.0 )
				    "ceiling":   material.diffuse = ( color 255.0   0.0 255.0 )
				    "composite": material.diffuse = ( color 255.0   0.0   0.0 )
				    default:     material.diffuse = ( color   0.0 255.0   0.0 )
			    )
			    material.selfIllumAmount = 20.0
		    )
		)
		
		obj.material = material
		obj.wireColor = material.diffuse
	)
)

--------------------------------------------------------------------------------
/**
*/
fn getCollisionMaterials =
(
	-- always get the material list from game library
	return ( nGetMaterialList true )
)

--------------------------------------------------------------------------------
/**
*/
fn setCollisionLayer=
(
	local name = "collision_layer"

	col_layer = LayerManager.getLayerFromName name
    if( undefined == col_layer )then
    (
        col_layer = LayerManager.newLayerFromName name
    )
	
	col_layer.current = true
)

--------------------------------------------------------------------------------
/**
*/
fn getCollisionLayer=
(
	local name = "collision_layer"

	col_layer = LayerManager.getLayerFromName name
    if( undefined == col_layer )then
    (
        col_layer = LayerManager.newLayerFromName name
    )
	
	return col_layer
)

--------------------------------------------------------------------------------
/**
*/
fn isPhysicCollision objects=
(
	for object in objects do
	(
		if ( true != (getUserProp object "physicObj")) or 
			( 1 != (getUserProp object "physicType")) do
		(
			return false
		)
	)
	return true
)

--------------------------------------------------------------------------------
/**
*/
fn isPhysicCollisionComposite objects=
(
	for object in objects do
	(
		if ( true != (getUserProp object "physicObj")) or 
			( 1 != (getUserProp object "physicType")) or
			( "composite" != (getUserProp object "physicGeom")) do
		(
			return false
		)
	)
	return true
)

--------------------------------------------------------------------------------
/**
*/
fn notPhysicCollisionComposite objects=
(
	for object in objects do
	(
		if ( true == (getUserProp object "physicComposite")) or 
			( "composite" == (getUserProp object "physicGeom")) do
		(
			return false
		)
	)
	return true
)

--------------------------------------------------------------------------------
/**
*/
fn getCompositeParent child =
(
	local objects = $CollObject*
	
	for object in objects do
	(
		if( "composite" == (getUserProp object "physicGeom"))do
		(
			for geom in object.CollCompositeParams.geometries do
			(
				if( geom == child )do
				(
					return object
				)
			)
		)
	)
	return undefined
)

--------------------------------------------------------------------------------
/**
*/
fn calculateCompositeCenter composite =
(
	local center = [0,0,0]
	local objects = composite.CollCompositeParams.geometries
	for object in objects do
	(
	    if not isDeleted object do
	    (
		    center += object.pos
		)
	)
	
	center = center / (objects.count as Float)
	composite.pos = center
)

--------------------------------------------------------------------------------
/**
*/
fn selectDuplicatePhysicObjects =
(
	progressStart "Find Duplicate Physic Objects"
	local n = 0.0
	local k = 0.0
	
	local phyObjects = #()
	local objects = ($* as array)
	
	n = objects.count + objects.count * objects.count
	k = 1.0
	for obj in objects do
	( 
		if isPhysicCollision( obj ) do
		( 
			append phyObjects obj
		)

		progressUpdate ( 100.0 * k / n )
		k += 1
	)
	
	clearSelection()
	
	n = objects.count + phyObjects.count * phyObjects.count
	k = objects.count + 1.0
	for obj in phyObjects do
	(
		for temp in phyObjects do
		(
			local value = length ( obj.center - temp.center )
			/* check with 1 cm or error*/
			if ( obj != temp ) and ( value < 1.0 ) do
			(
				selectMore obj
			)
			progressUpdate ( 100.0 * k / n )
			k += 1
		)
		if getProgressCancel() do
		(
			progressEnd()
			return false
		)
	)
	progressEnd()
)

--------------------------------------------------------------------------------
/**
*/
fn setRecursiveUserProp object property value=
(
	if isPhysicCollisionComposite #(object) then
	(
		for child in object.CollCompositeParams.geometries do
		(
			setRecursiveUserProp child property value
		)
	)
	setUserProp object property value
)

--------------------------------------------------------------------------------
/**
*/
fn changePhysicCollision objects props apply=
(
	for object in objects do
	(
	    if apply[1] do object.CollParams.material =  props[1]
		if apply[2] do object.CollParams.space = props[2]

		-- search for the first parent		
		local base = object
		parent = getCompositeParent base
		while parent != undefined do
		(
			base = parent
			parent = getCompositeParent base
		)
		
		-- set the property in all
		if apply[3] do setRecursiveUserProp base "physicCategory" props[3]
	)
)

--------------------------------------------------------------------------------
--   EOF
--------------------------------------------------------------------------------
