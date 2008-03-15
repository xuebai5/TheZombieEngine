/**
   @file export.ms
   @ingroup n3dsMaxScripts
   @author Cristobal Castillo Domingo
   @brief This module is for mexport and persisted the options to export

   (C) 2006 Conjurer Services, S.A.
*/

explog = NebCreateLog "export"  "export module" 2 true


-- module
global NebExportOp

-- persistend variables
global nebNewExportOptions-- the options for export
global nebOldExportOptions -- the options of the last exported

-- the global variables of module
global nebExportData

global nebExportOptionsRollut -- The rollout


struct NebStructExportOptions 
(	
    valid, -- used for scene that do not have information
    gameLibrary, -- true use a working copy else use a temporal directory
    exportMode,
    /*
        1 Graphics preview  ( only has effect in wc = false)
        2 Brush
        3 Indoor
        4 Character
    */
    animMode,
    /*
        1 Mesh
        2 Skeleton
        3 Skin Animation
        4 Morpher Animation
    */
    entityName,
    libraryFolder,
    resourceFile,    
    skeletonName,    
    collectTextures, -- only has effect if use a temporal directory 
    binaryResource,
    optimizeMesh,
    --loopAnimation,
    exportThumbNail,
    firstPerson,
    critterName,
    openConjurer,
    exportMotion,
    motionX,
    motionY,
    motionZ,
    
    
	--------------------------------------------------------------------------------
	/**
	*/
	fn Init =
	(
	    valid = false
	    
	    -- the next only for show the dialog
	    gameLibrary = false
	    exportmode = 1
	    animMode = 1
	    
	    collectTextures = false
	    binaryResource = true
	    optimizeMesh = true
	    --loopAnimation = true
	    exportThumbNail = true
	    firstPerson = false
	    critterName = 1

	    
        entityName = ""
        libraryFolder = ""
        resourceFile = ""
        skeletonName = ""
	    
	    openConjurer = true

        exportMotion = false
        motionX = false
        motionY = false
        motionZ = false
	)

)

struct NebStructExportData
(	
    nameExportModes,
    nameAnimModes,
    exportRunning,
	--------------------------------------------------------------------------------
	/**
	*/
	fn Init =
	(	
	    nameExportModes = #( "Brush", "Indoor", "Character"  )
	    nameAnimModes = #( "Mesh" , "Skeleton" , "Skin animation", "Morpher animation")
	    /*
            0 Mesh
            1 Skeleton
            2 Skin Animation
            3 Morpher Animation
        */
        exportRunning = false
	)
)




--------------------------------------------------------------------------------
/**
*/
struct NebExportOp (
    
	fn declareVar =
    (
        global nebNewExportOptions = NebStructExportOptions()
        global nebOldExportOptions = NebStructExportOptions()

        
        global nebExportData = NebStructExportData()
        
    ),
    
    --------------------------------------------------------------------------------
    /** 
        declare persistent variables of this file
	*/
    fn declarePersistent =
    (
        persistent global nebNewExportOptions
        persistent global nebOldExportOptions
    ),
	
	--------------------------------------------------------------------------------
	/**
	*/
	fn initModule =
	(
	    explog.log 4 "Initialize visibility module\n"
	    NebExportOp.declareVar()
		nebNewExportOptions.init()
		nebOldExportOptions.init()
		nebExportData.init()
	),
	
	
    --------------------------------------------------------------------------------
	/**
	*/
	fn ShowExportOptionsRolout =
	(
	    CreateDialog nebExportOptionsRollut modal:true
	),

	--------------------------------------------------------------------------------
	/**
	*/	
	fn internalExport  sel:false =
	(
	    if (nebNewExportOptions.exportThumbNail and ( sel == false ) ) then
	    (
	        NebExportOp.SaveBigThumbs()
	    )
	    local filename = (getDir #maxroot) + "nebula/tmpexport/tmp.n2" --trick for call exporte
	    exportFile filename #noPrompt selectedOnly:sel
	    nebExportData.exportRunning = false 
	),
	
	--------------------------------------------------------------------------------
	/**
	*/
	fn Export selectedOnly:false =
	(
	    if ( nebExportData.exportRunning ) do return 	    
	    nebExportData.exportRunning = true
	    
	    if  nebulaProgramersVersion  then
	    (
	        NebExportOp.internalExport sel:selectedOnly
	    ) else
	    (
    	    try 
    		(
    		    NebExportOp.internalExport sel:selectedOnly
    	    ) catch ( messagebox  "The exporter crashes\n exit of max"	    )	        
	    )
	),
	
	
    --------------------------------------------------------------------------------
	/**
	*/
	fn CanToExport =
	(
	    return nebNewExportOptions.valid and ( nebExportData.exportRunning == false)
	),
	
    --------------------------------------------------------------------------------
	/**
	*/
	fn CanShowDialog =
	(
	    return [0,0] == GetDialogPos nebExportOptionsRollut
	),
	
	
	--------------------------------------------------------------------------------
	/**
	*/
	fn SaveBigThumbs=
	(
	    local thumbs
	    local isWire = viewport.isWire() 
	    local needRedraw = false
	   
	    if isWire do 
	    ( 
             max wire smooth -- switch to solid
		 	 needRedraw = true
        ) 
   
   		-- hide layers
	    local isHidePhysics
		local physicLayer = (layermanager.getlayerfromname "collision_layer")
	   	if physicLayer != undefined do
		(
	   		isHidePhysics = physicLayer.isHidden
			physicLayer.isHidden = true
			needRedraw = true
		)
		
	    local isHideVisibility
		local visibilityLayer = (layermanager.getlayerfromname "n_visibility_layer")
	   	if visibilityLayer != undefined do
		(
	   		isHideVisibility = visibilityLayer.isHidden
			visibilityLayer.isHidden = true
			needRedraw = true
		)

		if needRedraw do
		(
	       	max views redraw
		)
		
        --Save image
        thumbs = gw.getViewportDib()
        thumbs.filename = (getDir #maxroot) + "nebula/tmpexport/thumbnail.bmp"
        save thumbs
   
        if isWire do ( max wire smooth ) -- switch to wire if is necesary
	    
   		-- restore layers
	   	if physicLayer != undefined do
		(
			physicLayer.isHidden = isHidePhysics
		)
		
	   	if visibilityLayer != undefined do
		(
			visibilityLayer.isHidden = isHideVisibility
		)
	),

	--------------------------------------------------------------------------------
	/**
	    for update object or persist data , from old version max file
	    
	*/
    fn updateFromVersion version =
    (
        explog.log 3 "export Update\n"
		
        if (version < 000500) do
        (
            -- Reset value
            global nebNewExportOptions = NebStructExportOptions()
            global nebOldExportOptions = NebStructExportOptions()
		    nebNewExportOptions.init()
		    nebOldExportOptions.init()
        )
        
        if (version < 000600) do
        (
            exportThumbNail = true
        )
        
        if (version < 000700) do
        (
            -- keep old data
            temp = nebNewExportOptions
            global nebNewExportOptions = NebStructExportOptions()            
            
            -- actualize data, add firstPerson to false
            nebNewExportOptions.valid = temp.valid
            nebNewExportOptions.gameLibrary = temp.gameLibrary
            nebNewExportOptions.exportmode = temp.exportmode
            nebNewExportOptions.animMode = temp.animMode
            
            nebNewExportOptions.entityName = temp.entityName
            nebNewExportOptions.libraryFolder = temp.libraryFolder
            nebNewExportOptions.resourceFile = temp.resourceFile
            nebNewExportOptions.skeletonName = temp.skeletonName
            nebNewExportOptions.collectTextures = temp.collectTextures
            nebNewExportOptions.binaryResource = temp.binaryResource
            nebNewExportOptions.optimizeMesh = temp.optimizeMesh
            nebNewExportOptions.exportThumbNail = temp.exportThumbNail
            nebNewExportOptions.firstPerson = false            
        )
        
        if (version < 000801) do
        (
            -- keep old data
            temp = nebNewExportOptions
            global nebNewExportOptions = NebStructExportOptions()            
            
            -- actualize data, add crittername to human (the first one)
            nebNewExportOptions.valid = temp.valid
            nebNewExportOptions.gameLibrary = temp.gameLibrary
            nebNewExportOptions.exportmode = temp.exportmode
            nebNewExportOptions.animMode = temp.animMode
            
            nebNewExportOptions.entityName = temp.entityName
            nebNewExportOptions.libraryFolder = temp.libraryFolder
            nebNewExportOptions.resourceFile = temp.resourceFile
            nebNewExportOptions.skeletonName = temp.skeletonName
            nebNewExportOptions.collectTextures = temp.collectTextures
            nebNewExportOptions.binaryResource = temp.binaryResource
            nebNewExportOptions.optimizeMesh = temp.optimizeMesh
            nebNewExportOptions.exportThumbNail = temp.exportThumbNail
            nebNewExportOptions.firstPerson = temp.firstPerson            
            nebNewExportOptions.critterName = 1
        )
        
        if (version < 000803) do
        (
            -- keep old data
            temp = nebNewExportOptions
            global nebNewExportOptions = NebStructExportOptions()            
            
            -- actualize data, add checkbox to open / do not open conjurer after exporting
            nebNewExportOptions.valid = temp.valid
            nebNewExportOptions.gameLibrary = temp.gameLibrary
            nebNewExportOptions.exportmode = temp.exportmode
            nebNewExportOptions.animMode = temp.animMode
            
            nebNewExportOptions.entityName = temp.entityName
            nebNewExportOptions.libraryFolder = temp.libraryFolder
            nebNewExportOptions.resourceFile = temp.resourceFile
            nebNewExportOptions.skeletonName = temp.skeletonName
            nebNewExportOptions.collectTextures = temp.collectTextures
            nebNewExportOptions.binaryResource = temp.binaryResource
            nebNewExportOptions.optimizeMesh = temp.optimizeMesh
            nebNewExportOptions.exportThumbNail = temp.exportThumbNail
            nebNewExportOptions.firstPerson = temp.firstPerson            
            nebNewExportOptions.critterName = temp.critterName
            nebNewExportOptions.openConjurer = true
        )    

        if (version < 000804) do
        (
            -- keep old data
            temp = nebNewExportOptions
            global nebNewExportOptions = NebStructExportOptions()            
            
            -- actualize data, add checkbox to open / do not open conjurer after exporting
            nebNewExportOptions.valid = temp.valid
            nebNewExportOptions.gameLibrary = temp.gameLibrary
            nebNewExportOptions.exportmode = temp.exportmode
            nebNewExportOptions.animMode = temp.animMode
            
            nebNewExportOptions.entityName = temp.entityName
            nebNewExportOptions.libraryFolder = temp.libraryFolder
            nebNewExportOptions.resourceFile = temp.resourceFile
            nebNewExportOptions.skeletonName = temp.skeletonName
            nebNewExportOptions.collectTextures = temp.collectTextures
            nebNewExportOptions.binaryResource = temp.binaryResource
            nebNewExportOptions.optimizeMesh = temp.optimizeMesh
            nebNewExportOptions.exportThumbNail = temp.exportThumbNail
            nebNewExportOptions.firstPerson = temp.firstPerson            
            nebNewExportOptions.critterName = temp.critterName
            nebNewExportOptions.openConjurer = true
            nebNewExportOptions.exportMotion = false
            nebNewExportOptions.motionX = false
            nebNewExportOptions.motionY = false
            nebNewExportOptions.motionZ = false
        )    
            
    ),      
    
     --------------------------------------------------------------------------------
    /**
        Callback file preMerge
        Create copy of all persistent variables
    */    
    fn FilePreMerge =
    (
        global NebMergeCopyOfNebNewExportOptions = nebNewExportOptions
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
         nebNewExportOptions = NebMergeCopyOfNebNewExportOptions -- restore original values
    ),

    
    --------------------------------------------------------------------------------
    /**
     Callback file post open 
    */
    
    fn FilePostOpen =
    (
    ),


    --------------------------------------------------------------------------------
    /**
     Callback file post open 
    */
    fn FilePostNew =
    (
        global nebNewExportOptions = NebStructExportOptions()
        global nebOldExportOptions = NebStructExportOptions()
	    nebNewExportOptions.init()
	    nebOldExportOptions.init()
    )
	
)


--------------------------------------------------------------------------------
--   EOF
--------------------------------------------------------------------------------
