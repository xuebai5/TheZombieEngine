/**
   @file collisionCallbacks.ms
   @ingroup n3dsMaxScripts
   @author Luis Jose Cabellos Gomez
   @brief Collision Layer functions.
   
   (C) 2006 Conjurer Services, S.A.
*/

--------------------------------------------------------------------------------
/**
*/
global nebCollisionsDeletedNodes = #()

--------------------------------------------------------------------------------
/**
*/
fn selectCollisionComposite=
(
	local objects = GetCurrentSelection()
	
	local compositeList = #()
	local normalList = #()

	for object in objects do
	(
		if( (getUserProp object "physicComposite") == true )then
		(
			if( object.parent != undefined ) do
			(
				append compositeList object.parent
			)
		)
		else
		(
			append normalList object
		)
	)
	
	if(compositeList.count >= 1) do
	(
		select (compositeList + normalList)
	)
)

--------------------------------------------------------------------------------
/**
*/
fn physicNodesPreDelete objects =
(

	for object in objects do
	(
		local composite_parent = getCompositeParent object
		
		if( isPhysicCollisionComposite object ) then
		(
			for geom in object.CollCompositeParams.geometries do
			(
				if geom != undefined and (not isDeleted geom) do
				(
                    if( (findItem nebCollisionsDeletedNodes geom) == 0 ) do
                    (
                        append nebCollisionsDeletedNodes geom
                    )
				)
			)
			object.CollCompositeParams.geometries = #()
		)
		else if composite_parent != undefined do
		(
			local ind = findItem composite_parent.geometries object
			deleteItem composite_parent.geometries ind
		)
		
	)
)
--------------------------------------------------------------------------------
/**
*/
fn physicNodesPostDelete =
(
    for object in nebCollisionsDeletedNodes do
    (
		try
		(
			if object != undefined and (not isDeleted object) do
			(
		        delete object
			)
		)catch()
    )
    nebCollisionsDeletedNodes = #()
)

--------------------------------------------------------------------------------
/**
*/
fn physicsNodesPreDelete = 
(
    
)

--------------------------------------------------------------------------------
/**
*/
fn loadChangeHandlers =
(
	deleteAllChangeHandlers id:#physicCollision
	
	when transform $CollObject* changes handleAt:#redrawViews id:#physicCollision object do
	(
		if object != undefined do
		(
			if( (getUserProp object "physicComposite") == true )then
			(
				local composite = (getCompositeParent object)
				
				if( composite != undefined ) do
				(
					calculateCompositeCenter composite
				)
			)
		)
	)
)

--------------------------------------------------------------------------------
/**
*/
fn physicCollisionPostFile =
(
	loadChangeHandlers()
	addCollAttributes()

	local spaceitems = NebCollOp.getPhysicSpaceNames()
	
	if 0 == spaceitems .count do
	(
	    NebCollOp.createBasicSpaces()
	)		
)

--------------------------------------------------------------------------------
loadChangeHandlers()

--------------------------------------------------------------------------------
--   EOF
--------------------------------------------------------------------------------
