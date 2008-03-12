/**
   @file mainCallbacks.ms
   @ingroup n3dsMaxScripts
   @brief main Callbacks
*/

global  NebMergeCopyOfNebulaVersion  --Alway needs a copy of original persitent variable before merge scene
global  NebMergeNewObjectsList = #() -- List of objects append to scene 
global  NebIsInMerge = false  -- Global variable is only true if is the merge procces

--------------------------------------------------------------------------------
/**
*/
fn NebUpdateFileFrom version =
(   
	logNebulaSystem.log 1 ( "update version from " + ( NebOp.VersionToString version ) + " TO " + ( NebOp.VersionToString scriptNebulaVersion ) )
    NebOp.updateFromVersion version
    NebMatOp.updateFromVersion version
    NebVisOp.updateFromVersion version
    NebLodOp.updateFromVersion version
	NebCollOp.updateFromVersion version
	NebExportOp.updateFromVersion version
)

--------------------------------------------------------------------------------
/**
*/
fn NebSelectedNodesPreDelete =
(
    logNebulaSystem.log 2 "Callback SelectedNodesPreDelete"
    local objects  = callbacks.notificationParam()      
	physicNodesPreDelete  objects
)

--------------------------------------------------------------------------------
/**
*/
fn NebSelectedNodesPostDelete =
(
    logNebulaSystem.log 2 "Callback SelectedNodesPostDelete"
	physicNodesPostDelete()	
)

--------------------------------------------------------------------------------
/**
*/
fn NebSystemPreNew =
(
    logNebulaSystem.log 2 "Callback NebSystemPreNew"
)

--------------------------------------------------------------------------------
/**
*/
fn NebSystemPostNew =
(
    logNebulaSystem.log 2 "Callback NebSystemPostNew"
    NebExportOp.FilePostNew()
    NebLodOp.FilePostNew()
	physicCollisionPostFile()
)

--------------------------------------------------------------------------------
/**
*/
fn NebSystemPreReset =
(
    logNebulaSystem.log 2 "Callback NebSystemPreReset"
)

--------------------------------------------------------------------------------
/**
*/
fn NebSystemPostReset =
(
    logNebulaSystem.log 2 "Callback NebSystemPostReset"
    NebExportOp.FilePostNew()
    NebLodOp.FilePostNew()
	physicCollisionPostFile()
)

--------------------------------------------------------------------------------
/**
*/
fn NebFilePreMerge =
(
    logNebulaSystem.log 2 "Callback FilePreMerge"
    NebIsInMerge = true -- Set Flag
    NebMergeNewObjectsList = #() -- Create empty list
    NebMergeCopyOfNebulaVersion = nebulaVersion  -- Copy persistent variable
	
	NebOp.FilePreMerge()
	NebMatOp.FilePreMerge()
	NebVisOp.FilePreMerge()
	NebCollOp.FilePreMerge()
	NebLodOp.FilePreMerge()
	NebExportOp.FilePreMerge()
	
	
)

--------------------------------------------------------------------------------
/**
*/
fn NebSceneNodeAdded arg =
(
   logNebulaSystem.log 2 ( "Callback NebSceneNodeAdded " + ( Arg as string )  )
  -- format "callback NebSceneNodeAdded  %\n" arg
    if (NebIsInMerge) do
    (
        try
        (
            append NebMergeNewObjectsList arg
        ) catch()
    )
)

--------------------------------------------------------------------------------
/**
*/
fn NebFilePostMerge =
(
    logNebulaSystem.log 2 "Callback FilePostMerge"
    local mergedObjectVersion = nebulaVersion
    nebulaVersion = NebMergeCopyOfNebulaVersion -- restore the original version
   
    logNebulaSystem.log 2 ( "Total of merged objects is % " + ( NebMergeNewObjectsList.count as string ) )
    --for obj in NebMergeNewObjectsList do ( format "\t%\n" obj )
    
    
    NebOp.FilePostMerge mergedObjectVersion NebMergeNewObjectsList
	NebMatOp.FilePostMerge mergedObjectVersion NebMergeNewObjectsList
	NebVisOp.FilePostMerge mergedObjectVersion NebMergeNewObjectsList
	NebCollOp.FilePostMerge mergedObjectVersion NebMergeNewObjectsList
	NebLodOp.FilePostMerge mergedObjectVersion NebMergeNewObjectsList
	NebExportOp.FilePostMerge mergedObjectVersion NebMergeNewObjectsList
	
	
	NebMergeNewObjectsList = #() -- Create empty list
    NebIsInMerge = false -- Set Flag
)

--------------------------------------------------------------------------------
/**
*/
fn NebFilePreSave =
(
    logNebulaSystem.log 2 "Callback FilePreSave"
	-- call all definition of persistent variables
	persistent global nebulaVersion = scriptNebulaVersion
	NebOp.declarePersistent()
	NebMatOp.declarePersistent()
	NebVisOp.declarePersistent()
	NebCollOp.declarePersistent()
	NebLodOp.declarePersistent()
	NebExportOp.declarePersistent()
)

--------------------------------------------------------------------------------
/**
*/
fn NebFilePostSave =
(
    logNebulaSystem.log 2 "Callback FilePostSave"
)

--------------------------------------------------------------------------------
/**
*/
fn NebFilePreOpen =
(
    logNebulaSystem.log 2 "Callback FilePreOpen"
	global nebulaVersion = unversionedNebulaVersion
	if MatEditor.isOpen() do MatEditor.Close() -- Prevent problem in update materials
	
)

--------------------------------------------------------------------------------
/**
*/
fn NebFilePostOpen =
(
    logNebulaSystem.log 2 "Callback FilePostOpen"


    --First update version and after call file post open callbacks
    
    NebAssert( nebulaVersion <= scriptNebulaVersion) ( "The nebula version of this file is greater, file: " + (NebOp.VersionToString nebulaVersion)  )
    
	if ( nebulaVersion < scriptNebulaVersion ) do ( NebUpdateFileFrom nebulaVersion)
	nebulaVersion = scriptNebulaVersion

	physicCollisionPostFile()
	NebVisOp.FilePostOpen()
	
	format "PostOpen before gc1\n"
	gc()
	format "PostOpen before freebitmpas\n"
    freeSceneBitmaps() 
    format "PostOpen before gc2\n"
    gc()
    format "PostOpen after gc2\n"
)

--------------------------------------------------------------------------------
/**
*/
fn NebSceneUndo =
(
	undo off(
		nebula_collisions.updateSpaces()
	)
)

--------------------------------------------------------------------------------
/**
*/
fn NebSceneRedo =
(
	undo off(
		nebula_collisions.updateSpaces()
	)
)

fn NebNodeCloned arg =
(
    format "callback NebNodeCloned  %\n" arg
    
)

fn NebNodeCreated arg =
(
    format "callback NebNodeCreated  %\n" arg
)



fn NebNodePreDelete node =
(
    logNebulaSystem.log 2 "Callback NebNodePreDelete"
    
    format "--predelete %\n" node
    
    if node != undefined do -- prevent two deletes of same object
    (
        local type1 = getUserprop node "z_type"
        if type1 == "visibility" then    NebVisOp.NodePreDelete node
    )
)

fn NebNodePostDelete arg =
(
    logNebulaSystem.log 2 "Callback NebNodePostDelete"
    NebVisOp.NodePostDelete()
)

unregisterRedrawViewsCallback nebRedrawViewsCallback
fn nebRedrawViewsCallback =
(
    if selection.count != 1 do (
        return undefined
    )

    local obj = selection[1]
    local redraw = false
    local type1 = getUserprop obj "z_type" -- uder for more speed
    
    gw.setTransform(Matrix3 1)
    
    if ( classOf obj == nebVisPortalDummy ) then
    (
        nebop.ShowObjectNameInViewport obj (color 255 128 0 )
        
        
        if ( obj.cell1 != undefined ) do
        (
            nebop.ShowObjectNameInViewport obj.cell1 (color 255 255 0)            
        )
        
        if ( obj.cell2 != undefined ) do
        (
            nebop.ShowObjectNameInViewport obj.cell2 (color 255 255 0)            
        )
        
        redraw = true

    ) else if ( classOf obj == nebVisOccluderDummy ) then
    (
        nebop.ShowObjectNameInViewport obj (color 64 255 255 )
        redraw = true

    ) else if ( type1 == "visibility"  and  (getUserProp obj "neb_Vis_Cell") == true ) then
    (
        nebop.ShowObjectNameInViewport obj (color 255 0 0)
        for i in refs.dependents obj where classOf i == nebVisPortalDummy do
        (
            try 
            (
                nebop.ShowObjectNameInViewport i (color 255 128 0 )
                local other = i.cell1
           
                if (i.cell1 == obj) do other = i.cell2
            
                if ( other != undefined ) do
                (
                    nebop.ShowObjectNameInViewport other (color 255 255 0)            
                )
         
            ) catch()
        )
        redraw = true
    ) else if  ( type1 == "physics" ) then
    (
		local objcol = undefined 
		if obj.material != undefined do
		(
			objcol = obj.material.diffuse
		)
		
		if( objcol != undefined )then
		(
			-- adjust color
			if( objcol.r < 170 )do( objcol.r = 170 )
			if( objcol.g < 170 )do( objcol.g = 170 )
			if( objcol.b < 170 )do( objcol.b = 170 )
		)
		else
		(
			objcol = (color 0 255 128)
		)
		
        nebop.ShowObjectNameInViewport obj objcol
		
		local comparent = undefined
		if isPhysicCollisionComposite #(obj) then
		(
			local comparent = obj
		)
		else
		(
			comparent = getCompositeParent obj
		)
		
		if comparent != undefined do
		(
			gw.SetColor #line [255,0,0]
			try
			(
				for child in comparent.geometries do
				(
					if child != undefined and (not isDeleted child) do
					(
						gw.PolyLine #(comparent.center, child.center) false
					)
				)
			)catch()
		)
		
        redraw = true
    ) else if  ( type1 == "plugs" ) then
    (
        nebop.ShowObjectNameInViewport obj (color 255 0 255 )
        redraw = true
    )
    
    if ( redraw == true ) do
    (
        gw.enlargeUpdateRect #whole
        gw.updateScreen()
    )
    
)

--------------------------------------------------------------------------------
(
callbacks.removeScripts id:#nebulaCallbacks

callbacks.addscript #selectedNodesPreDelete  "NebSelectedNodesPreDelete()" id:#nebulaCallbacks
callbacks.addscript #selectedNodesPostDelete "NebSelectedNodesPostDelete()" id:#nebulaCallbacks

callbacks.addscript #systemPreNew   "NebSystemPreNew()"    id:#nebulaCallbacks
callbacks.addscript #systemPostNew   "NebSystemPostNew()"   id:#nebulaCallbacks
callbacks.addscript #systemPreReset "NebSystemPreReset()"  id:#nebulaCallbacks
callbacks.addscript #systemPostReset "NebSystemPostReset()" id:#nebulaCallbacks

callbacks.addscript #filePreMerge  "NebFilePreMerge()"  id:#nebulaCallbacks
callbacks.addscript #filePostMerge "NebFilePostMerge()" id:#nebulaCallbacks
callbacks.addscript #filePreSave   "NebFilePreSave()"   id:#nebulaCallbacks
callbacks.addscript #filePostSave  "NebFilePostSave()"  id:#nebulaCallbacks
callbacks.addscript #filePreOpen   "NebFilePreOpen()"   id:#nebulaCallbacks
callbacks.addscript #filePostOpen  "NebFilePostOpen()"  id:#nebulaCallbacks

callbacks.addscript #sceneUndo "NebSceneUndo()" id:#nebulaCallbacks
callbacks.addscript #sceneRedo "NebSceneRedo()" id:#nebulaCallbacks

callbacks.addscript #nodeCloned "NebNodeCloned ( callbacks.notificationParam() )" id:#nebulaCallbacks
callbacks.addscript #nodeCreated "NebNodeCreated ( callbacks.notificationParam() )" id:#nebulaCallbacks
callbacks.addscript #nodePreDelete  "NebNodePreDelete ( callbacks.notificationParam() )"  id:#nebulaCallbacks
callbacks.addscript #nodePostDelete  "NebNodePostDelete ( callbacks.notificationParam() )"  id:#nebulaCallbacks
callbacks.addscript #sceneNodeAdded  "NebSceneNodeAdded ( callbacks.notificationParam() )"  id:#nebulaCallbacks


registerRedrawViewsCallback nebRedrawViewsCallback

)

--------------------------------------------------------------------------------
--   EOF
--------------------------------------------------------------------------------
