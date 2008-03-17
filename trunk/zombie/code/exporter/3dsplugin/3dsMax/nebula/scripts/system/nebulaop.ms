/**
   @file nebulaop.ms
   @ingroup n3dsMaxScripts
   @author Cristobal Castillo Domingo
   @brief The common function for nebula module

   (C) 2006 Conjurer Services, S.A.
*/
global nebOpData
global nebOp
struct NebStructData
(
	typeList  = #("graphics","none","physics","visibility","plugs")
)

struct NebOp  (
    --------------------------------------------------------------------------------
    /**
    */
    fn VersionToString ver =
    (
    
        local str = ""
        local num 
        local warn = NebWarning (classOf ver  == Integer) "NebOp.VersionToString: ver is not integer"
        if ( warn) do ver = 0
        num = ver 
        	
        str=""
        	
        str += ((num/10000) as string) + "." 
        num = (mod ver 10000) as integer
        	
        str += ((num/100) as string) + "." 
        num = (mod ver 100) as integer
        	
        str += (num as string) 
    
        return str
    ),
    
    --------------------------------------------------------------------------------
    /** 
        @brief Check is valid type for nebula object
	*/
	fn CheckTypeName type =
	(
		NebAssert ( String == classOf type )  " set type  invalid class of name" 
		local index = 1
		local list = NebOpData.typeList
		for i in list while (stricmp i type !=0) do ( index = index+1 )
		NebAssert  ( index <= list.count) ("Set Object type invalid name: " + (type as string))
		return list[index]
	),

    --------------------------------------------------------------------------------
    /** 
        @brief set nebula object type to object
	*/		
	fn SetType obj type =
	(
		NebOp.CheckTypeName type
		setUserProp obj "z_type" type
	),
	
    --------------------------------------------------------------------------------
    /** 
        @breif return true if object's type is a "type"
	*/
	fn IsType obj type =
	(
		NebOp.CheckTypeName type
		local type1 = getUserprop obj "z_type" 
		
		if type1 == udefined then 
		(
		    if ( canConvertTo obj mesh ) then
		    (
		        type1 = NebOpData.typeList[1] --Default is graphic
		    ) else
		    (
		        type1 = NebOpData.typeList[2] -- none
		    )
		) else -- Check the read string , incompatible format
		(
		    NebOp.CheckTypeName type1 
		)
		
		return (  0 == stricmp type type1 )
	),
	
	--------------------------------------------------------------------------------
    /** 
        @brief RecursiveHierarchy scene and call function
        The format function is "namme node arg depth"
        Dont call directly , use Iterate scene
	*/
	fn RecursiveHierarchy node preFunc preArg postFunc postArg =
	(
	    if ( preFunc != 0 ) do preFunc node preArg
	    for child in node.children do ( NebOp.RecursiveHierarchy child preFunc preArg  postFunc postArg )
	    if ( postFunc != 0 ) do postFunc node postArg
	),
	--------------------------------------------------------------------------------
    /** 
        @brief iterate scene and call function
        The format function is "namme node arg depth"
	*/
	fn IterateScene preFunc:0 prearg:0  postFunc:0 postArg:0 showbar:false barName:"" =
	(
	    NebAssert ( (preFunc == 0 ) or ( MAXScriptFunction == classOf prefunc) )   "NebOp.iterateScene invalid preFunction"
	    NebAssert ( (postFunc == 0 ) or ( MAXScriptFunction == classOf postfunc) ) "NebOp.iterateScene invalid postFunction"   
	    
	    if (showbar != true) then
	    (
	        for node in rootnode.children do NebOp.RecursiveHierarchy node preFunc preArg  postFunc postArg
	    ) else
	    (
	        local inc = 100.0 / rootNode.children.count
	        local progress = 0.0
	        progressStart ( barName as string)
	        for node in rootnode.children do
    	    (
    	         NebOp.RecursiveHierarchy node preFunc preArg  postFunc postArg
    	         progress += inc
    	         progressUpdate progress
    	    )
    	    progressEnd()
	    )	   
	),
	
	--------------------------------------------------------------------------------
    /** 
        @brief Return the scene node from simple script
        note: in clone plugin event the scene node is create after of create base object
	*/	
	fn GetSceneObjectFromObject arg =
	(
	    local obj = undefined
	    format "count %\n" (refs.dependents arg).count
	    for i in refs.dependents arg do
	    (
	        format " i: % arg %\n" i arg
	        if ( classOf i == classOf arg) and (i.baseobject == arg) do obj = i
	    )
	    return obj
	),
	
	--------------------------------------------------------------------------------
    /** 
        @brief show the name of object
        note: call only in RedrawViewsCallback,
        set before gw.setTransform(Matrix3 1)
        call after gw.enlargeUpdateRect #whole ; gw.updateScreen()
        
	*/	
	fn ShowObjectNameInViewport obj color =
	(
	    local pos
	    in coordsys world (
	        pos = obj.center
	        pos.z += 0.5*( obj.max.z - obj.min.z)
	    )
	    /* 
	      the name can not begin with lower case 'z' because with directx driver
	      gw.wtext crash 3dStudio. Magic, more magic.
	      refs #543.
	    */
	    local nameStr = obj.name
	    nameStr = " " + nameStr
	    gw.wtext ( gw.wtransPoint pos) nameStr color: color
	),
	
	--------------------------------------------------------------------------------
    /** 
        @transform form neb units to max
	*/
	fn NebToMaxUnits val =
	(
	    -- Nebula  is in meters
	    -- the scale depends on file and ui parameters
	    local scale1 = case units.SystemType of
	    (
	        #Centimeters:0.01
	        #meters:1.0
	        #Mililimeters:0.001
	        #kilomenters:1000.0
	        #Inches:0.0254
	        #Feet:0.3048
	        #Miles:1609.344
	        default:1.0
	    )
	    scale1 = scale1 * units.SystemScale
	    return val / scale1
	),
	--------------------------------------------------------------------------------
    /** 
        @brief Declare all global variables
	*/	
	fn DeclareVar =
    (
        global nebOpData = NebStructData()
    ),
	
	--------------------------------------------------------------------------------
    /** 
        @brief declare Persistent varaibles
	*/	
	fn DeclarePersistent =
    (
		-- empty
    ),
	
	 --------------------------------------------------------------------------------
    /** 
        @brief Initialiaze module
	*/
	fn InitModule =
    (
        NebOp.declareVar()
    ),
	
	fn UpdateObjectFromVersion obj version =
	(   
	    
        if (version < 000400) do
        (
            logNebulaSystem.log 3 ("update Object:  " + obj.name )
            -- Update from 0.3.0 to 0.4.0
            -- update Z_type from "1" to "physics"
            local val = getUserprop obj "z_type" 
            if ( val == 1 ) do 
            ( 
                nebOp.SetType obj "physics"
            )
        )
	    
	),
	
	--------------------------------------------------------------------------------
	/**
	    @brief for update object or persist data , from old version max file
	*/
    fn UpdateFromVersion version =
    (
        if (version < 000300) do
        (
            -- Update from 0.2.0 to 0.3.0
            -- Nothing
        )
        
        if (version < 000400) do
        (
            -- Update from 0.3.0 to 0.4.0
        )
        NebOp.IterateScene preFunc:NebOp.UpdateObjectFromVersion preArg:version showbar:true barName:"NebOp update objects"
    ),
    
    --------------------------------------------------------------------------------
    /**
        Callback file preMerge
        Create copy of all persistent variables
    */    
    fn FilePreMerge =
    (
        -- Empty , Nebop not has a persistent variables
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
        for obj in listObjects do
        (
            NebOp.UpdateObjectFromVersion obj version
        )

    )
)

---------------------------------------------------------------------------------
-- rollout for 'About' dialogbox.
---------------------------------------------------------------------------------
rollout neb3dsAboutRollout "About n3DSMaxExporter" width:439 height:230
(
	GroupBox grp1 "" pos:[7,21] width:423 height:163
	label lblDesc "Zombie 3DS Max Toolkit." pos:[156,57] width:217 height:20
	label lbl2 "Script Version " pos:[156,72] width:210 height:20
	label lbl3 "Copyright (c) 2004 Conjurer Services, S.A." pos:[156,91] width:210 height:20
	bitmap Logo "" pos:[15,33] width:124 height:144 fileName:"nebula/images/logo_zombie.bmp"
	button btnOk "Ok" pos:[345,194] width:84 height:24

	
	on btnOk pressed do
	(
	    DestroyDialog neb3dsAboutRollout
	)
	
	on neb3dsAboutRollout open do
	(
	    lbl2.caption = ("Script Version "+ (NebOp.VersionToString scriptNebulaVersion  ) )
    )
)

global nebOp
--------------------------------------------------------------------------------
--   EOF
--------------------------------------------------------------------------------
