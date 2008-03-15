/**
   @file visibility.ms
   @ingroup n3dsMaxScripts
   @author Cristobal Castillo Domingo
   @brief This module is for managament of visibility

   (C) 2006 Conjurer Services, S.A.
*/

vislog = NebCreateLog "visibility"  "visibility module" 2 true

global nebVisData


struct NebStructVisData 
(
    materials,
    portalMaterials,
    OccluderMaterials,
    updateCellList,
	--------------------------------------------------------------------------------
	/**
	*/
	fn SearchMaterial name =
	(
		for mat in sceneMaterials do
		(
			if mat.name == name do
			(
				return mat;
			)
		)

		for mat in meditMaterials do
		(
			if mat.name == name do
			(	
				return mat;
			)
		)
		return false
	),
	
    --------------------------------------------------------------------------------
	/**
	*/
	fn CreateMaterial matName filename =
	(
	    local mat = searchMaterial matName
		if ( mat == false) then
		(
		    vislog.log 5 ("visibility material: " + fileName + "not found in the scene, create it")
		    
			local tex = Bitmaptexture()
			mat = standard()
			mat.name = matName 
			tex.filename =  (getDir #maxroot) + fileName
			tex.monoOutput=1
			tex.alphasource = 0
			mat.diffusemap = tex
			mat.opacityMap = tex
			mat.showInViewport = true
			mat.selfIllumAmount = 50.0
		)
		return mat
	    
	),

	--------------------------------------------------------------------------------
	/**
	*/
	fn CreateMaterialCell num =
	(
	    vislog.log 5 ("Create visibility material: " + ( num as string))
	    local matName = "z_vis_mat_cell_"+(num as string)
	    local fileName = "nebula/images/cell_" + (num as string) + ".tga"
	    local mat =  CreateMaterial  matName fileName 
	),
	
	--------------------------------------------------------------------------------
	/**
	*/
	fn CreateMaterialPortal num =
	(
	    vislog.log 5 ("Create visibility portal material: " + ( num as string))
	    local matName = "z_vis_mat_portal_"+(num as string)
	    local fileName ="nebula/images/portal_" + (num as string) + ".tga"
	    local mat =  CreateMaterial  matName fileName 
		return mat
	),
	
		--------------------------------------------------------------------------------
	/**
	*/
	fn CreateMaterialOccluder num =
	(
	    vislog.log 5 ("Create visibility portal material: " + ( num as string))
	    local matName = "z_vis_mat_portal_"+(num as string)
	    local fileName ="nebula/images/occluder_" + (num as string) + ".tga"
	    local mat =  CreateMaterial  matName fileName 
		return mat
	),
	
	--------------------------------------------------------------------------------
	/**
	*/
	fn GetMaterial num =
	(
	    local numMat = ((mod (num-1) materials.count)+1) as integer
	    local mat = materials[numMat]
		return mat
	),
	
	
	--------------------------------------------------------------------------------
	/**
	*/
	fn Init =
	(
		materials=#()
		portalMaterials=#()
		OccluderMaterials=#()
		updateCellList=#()
		for i = 1 to 7 do
		(
		    local mat
			mat = CreateMaterialCell i
			append materials mat
		)
		
		for i = 1 to 3 do
		(
		    local mat
			mat = CreateMaterialPortal i
			append portalMaterials mat
		)
		
		for i = 1 to 1 do
		(
		    local mat
			mat = CreateMaterialOccluder i
			append OccluderMaterials mat
		)
		
	),
	
	--------------------------------------------------------------------------------
	/**
	*/
	fn CheckMaterialsValues =
	(
	    local numMat =1 
		for mat in materials do
		(
		    if ( classOf mat != Standardmaterial) then
    	    (
	            local mat = CreateMaterialCell numMat
	            materials[numMat] = mat
	        ) else -- resolve proble when persist in diffderent machines
	        (
	            mat.diffusemap.filename =  (getDir #maxroot) + "nebula/images/cell_" + (numMat as string) + ".tga"
	        )
	        
	        numMat += 1
		)
		
		numMat =1 
		for mat in portalMaterials do
		(
		    if ( classOf mat != Standardmaterial) then
    	    (
	            local mat = CreateMaterialPortal numMat
	            portalMaterials[numMat] = mat
	        ) else -- resolve proble when persist in diffderent machines
	        (
	            mat.diffusemap.filename =  (getDir #maxroot) + "nebula/images/portal_" + (numMat as string) + ".tga"
	        )
	        numMat += 1
		)
		
		numMat =1 
		for mat in OccluderMaterials do
		(
		    if ( classOf mat != Standardmaterial) then
    	    (
	            local mat = CreateMaterialOccluder numMat
	            OccluderMaterials[numMat] = mat
	        ) else -- resolve proble when persist in diffderent machines
	        (
	            mat.diffusemap.filename =  (getDir #maxroot) + "nebula/images/Occluder_" + (numMat as string) + ".tga"
	        )
	        numMat += 1
		)
	)

)




--------------------------------------------------------------------------------
/**
*/
struct NebVisOp (
    
	--------------------------------------------------------------------------------
	/**
	*/	
	fn canConverToCell obj =
	(
		return ( (CanConvertTo obj Mesh) and (superClassof obj != Shape ) and ( NebOp.IsType obj "graphics" ) )
	),
	
	--------------------------------------------------------------------------------
	/**
	*/	
	fn canConverToPortal obj =
	(
	    local tmp  = NebOp.IsType obj "graphics"
		return ( ( NebOp.IsType obj "graphics" )  and ( (classOf obj == rectangle ) or (classOf obj == box) ) )
	),
	
    --------------------------------------------------------------------------------
	/**
	*/	
	fn canConverToOccluder obj =
	(
	    local tmp  = NebOp.IsType obj "graphics"
		return ( ( NebOp.IsType obj "graphics" )  and ( (classOf obj == rectangle ) or (classOf obj == box) ) )
	),
	
	--------------------------------------------------------------------------------
	/**
	*/	
	fn setLayer =
	(
		local old_layer = LayerManager.current
		local name = "n_visibility_layer"
		local col_layer = LayerManager.getLayerFromName name
		
    	if( undefined == col_layer )then
    	(
       		col_layer = LayerManager.newLayerFromName name
    	)
		
		col_layer.current = true
		return old_layer
		
	),
	
	--------------------------------------------------------------------------------
	/**
	*/
	fn OnlyOneNormalByVertex obj =
	(
		-- Only one normal by vertex
		addModifier obj ( smooth () )
		obj.modifiers[#Smooth].autosmooth = on
		obj.modifiers[#Smooth].preventIndirect = on
		obj.modifiers[#Smooth].threshold = 180
		collapseStack obj
	),

	--------------------------------------------------------------------------------
	/**
	*/
	fn capHoles obj = 
	(		
		addModifier obj (Cap_Holes ())
		obj.modifiers[#Cap_Holes].Make_All_New_Edges_Visible = 1
		obj.modifiers[#Cap_Holes].Smooth_New_Faces = 1
		obj.modifiers[#Cap_Holes].Smooth_With_Old_Faces = 1
		collapseStack obj
	),

	--------------------------------------------------------------------------------
	/**
	*/	
	fn setCubeMap mesh1 repeat =
	(
		addModifier mesh1 (Uvwmap ())
		mesh1.modifiers[#UVW_Mapping].maptype = 4 -- Cube 
		mesh1.modifiers[#UVW_Mapping].length = repeat
		mesh1.modifiers[#UVW_Mapping].width = repeat
		mesh1.modifiers[#UVW_Mapping].height = repeat
		mesh1.modifiers[#UVW_Mapping].mapChannel = 1
		collapseStack mesh1
	),
	
	--------------------------------------------------------------------------------
	/**
	*/
	fn setValuesCell mesh1 =
	(
		select mesh1
		mesh1.name =  uniquename "z_vis_cell_"
		convertToPoly(mesh1)   -- the artists prefer to work with "Editable Poly"
		mesh1.pivot = mesh1.center
		
		-- Phisics properties
		NebOp.SetType mesh1 "visibility" 
		
		setUserProp mesh1 "neb_Vis" true
		setUserProp mesh1 "neb_Vis_Cell" true
		setUserProp mesh1 "neb_Vis_Portal" false
		setUserProp mesh1 "neb_Vis_Occluder" false
		
		
		-- Set random material
		mesh1.material = nebVisData.getMaterial ( random 1 7 )
		
		-- Add uv Map to chanel 1, repeat texture for each 0.5 meters
		NebVisOp.setCubeMap mesh1 50
		
		
		mesh1.backfacecull = off -- because is alpha
		mesh1.renderable = off   -- render off example for generate lightmaps
		mesh1.showFrozenInGray = on --For frozen cell apears with colors
	),
	
	--------------------------------------------------------------------------------
	/**
	*/
	fn setValuesToDummyPortal mesh1 =
	(	
		-- Phisics properties
		NebOp.SetType mesh1 "visibility" 
		
		setUserProp mesh1 "neb_Vis_Cell" false
		setUserProp mesh1 "neb_Vis_Portal" true
		setUserProp mesh1 "neb_Vis_Occluder" true

	),
	
    --------------------------------------------------------------------------------
	/**
	*/
	fn setValuesToDummyOccluder mesh1 =
	(	
		-- Phisics properties
		NebOp.SetType mesh1 "visibility" 
		
		setUserProp mesh1 "neb_Vis_Cell" false
		setUserProp mesh1 "neb_Vis_Portal" false
		setUserProp mesh1 "neb_Vis_Occluder" true

	),
	
	--------------------------------------------------------------------------------
	/**
	*/
	fn setValuesToRectanglePortal mesh1 isOccluder:false =
	(	
		-- Phisics properties
		select mesh1
		
		if (not isOccluder) then
		(
		    mesh1.name =  uniquename "z_vis_portal_rectangle"
		) else
		(
		    mesh1.name =  uniquename "z_vis_Occluder_rectangle"
		)
		
		convertToPoly(mesh1)   -- the artists prefer to work with "Editable Poly"
		mesh1.pivot = mesh1.center
		
		NebOp.SetType mesh1 "visibility" 
		
		setUserProp mesh1 "neb_Vis_Cell" false
		setUserProp mesh1 "neb_Vis_Portal" false
		setUserProp mesh1 "neb_Vis_Occluder" false
		
		-- Add uv Map to chanel 1, repeat texture for each 0.5 meters
		NebVisOp.setCubeMap mesh1 100
		mesh1.backfacecull = off -- because is alpha
		mesh1.renderable = off   -- render off example for generate lightmaps
		mesh1.showFrozenInGray = on --For frozen cell apears with colors
		
		if (not isOccluder) then
		(
		    mesh1.material = nebVisData.portalMaterials[3]
		) else
		(
		   mesh1.material = nebVisData.OccluderMaterials[1]
		)
	),
	

	--------------------------------------------------------------------------------
	/**
	*/	
	fn CreateCellFrom objects =
	(

	  local old_layer = NebVisOp.setLayer()
	  local mesh1 = Editable_Mesh()

	  --try
	  (
		
		for obj in objects do
		(
		    if ( NebVisOp.canConverToCell obj) do
			(
				local obj2 = copy obj
				obj2.material = undefined
				meshop.attach mesh1 obj2				
			)
		)
		if ( (getnumfaces mesh1) == 0) then delete mesh1
		else
		(
			-- fusion near vertex, 5 cm
			addModifier mesh1 ( Vertex_Weld () )
			mesh1.modifiers[#Vertex_Weld].threshold = 5
			collapseStack mesh1
			
			-- Close all holes
			NebVisOp.capHoles mesh1
		
			-- Flip normals ( cull face )
			addModifier mesh1 (Normalmodifier ())
			mesh1.modifiers[#Normal].flip = on
			collapseStack mesh1

			-- simplified the mesh export
			NebVisOp.OnlyOneNormalByVertex mesh1
		
			NebVisOp.setValuesCell mesh1
		)
		
		
		
	  )
	 --catch ( delete mesh ; )
	 -- Select Old Layer
	 old_layer.current = true
	),
	
	--------------------------------------------------------------------------------
	/**
	*/
	fn ConvertToCell objects =
	(
	  local old_layer = NebVisOp.setLayer()
	  local mesh1 = Editable_Mesh()
	  --try
	  (	
		local objects2 = objects as array  -- when delete object from scene then the selection change
		for obj in objects2 do
		(
		    if ( NebVisOp.canConverToCell obj) do 
			(
				--visop.capHoles obj
				mesh1 + obj
				delete obj
			)
		)
		
		if ( (getnumfaces mesh1) == 0) then delete mesh1
		else
		(				
			NebVisOp.onlyOneNormalByVertex mesh1	
			NebVisOp.setValuesCell mesh1
		)
	 )
	 --catch ( delete mesh)
	 old_layer.current = true
	),
	
	--------------------------------------------------------------------------------
	/**
	*/
	fn ConvertRectangleToPortal object isOccluder:false depth: =
	(
	    NebAssert ( rectangle == classOf object )  "Invalid object, can't convert to portal"
	    local old_layer = NebVisOp.setLayer()
	    
	    -- Create the same onbject in current layer
	    obj = copy object 
	    delete object
	    
	    local l = obj.length
	    local w = obj.width

        local f 	    
	    if ( depth == unsupplied ) then
	    (
      	    if ( l < w ) then  (f = l)   else ( f = w)
            f = 3.0 * f /4.0
	    ) else
	    (
	        f = depth
	    )
	    

	    
	    -- only 4 vertex
	    obj.cornerRadius = 0.0f
	    NebVisOp.setValuesToRectanglePortal obj isOccluder:isOccluder
	    
	    --catch ( delete mesh)
	     
	    local dummyobj
	    in obj ( -- change the context in scene hierarhy
	        if ( not isOccluder ) then
	        (
	            dummyobj = nebVisPortalDummy()
	            NebVisOp.SetValuesToDummyPortal dummyobj
				dummyobj.size = [ w, l, f  ]
				dummyobj.clipRect =  obj

	         ) else
	         (
	            dummyobj = nebVisOccluderDummy()
	            NebVisOp.SetValuesToDummyOccluder dummyobj
	            dummyobj.delegate.boxsize = [ w, l, f  ] 
	         )
	        dummyobj.transform = obj.transform
	        -- 
	    )
	    
	   
	    old_layer.current = true
	),
	
	
		--------------------------------------------------------------------------------
	/**
	*/
	fn ConvertBoxToPortal object isOccluder:false =
	(
	    NebAssert ( box == classOf object )  "Invalid object, not its a box , can't convert to portal"
	    local old_layer = NebVisOp.setLayer()
	    
	    -- Create the same onbject in current layer
	    object.pivot = object.center
	    
	    local rect = rectangle()

	
	    rect.length = object.height

	    rotate rect (eulerAngles 90 0 0)

	    local depth	    
	    if object.width > object.length then 
	    (
	        depth = object.length
	        rect.width = object.width
	    ) else
	    (
	        depth = object.width
	        rect.width = object.length
	        rotate rect (eulerAngles 0 0 90)
	    )
	    
	    rect.transform *= object.transform
	    delete object
	    NebVisOp.ConvertRectangleToPortal rect isOccluder:isOccluder depth:depth
	    
	    old_layer.current = true
	),
	
	
		--------------------------------------------------------------------------------
	/**
	*/
	fn ConvertToPortal object =
	(
	    if classOf object == rectangle then
	    (
	        NebVisOp.ConvertRectangleToPortal object
	    ) else if classOf object == box then
	    (
	        NebVisOp.ConvertBoxToPortal  object
	    )
	),
	
    --------------------------------------------------------------------------------
	/**
	*/
	fn ConvertToOccluder object =
	(
	    if classOf object == rectangle then
	    (
	        NebVisOp.ConvertRectangleTo object isOccluder:true
	    ) else if classOf object == box then
	    (
	        NebVisOp.ConvertBoxToPortal  object isOccluder:true
	    )
	),
	

	
	--------------------------------------------------------------------------------
    /** 
        return true if is a cell
	*/
	fn IsCell obj =
	(
	    if ( obj == undefined ) do return false
	    
	    local v = NebOp.IsType obj "visibility"
	    if (v == true) then
	    (
	        return  true == getUserProp obj "neb_Vis_Cell"  
	    ) else
	    ( 
	        return false
	    )
	),
	
	/** 
        return true if is a clip rectangle of portal
	*/
	fn IsClipRectangle obj =
	(
	    if ( obj == undefined ) do return false
	    
	    local v = NebOp.IsType obj "visibility"
	    if (v == true) then
	    (
	        return  (false  == getUserProp obj "neb_Vis_Cell") and 
	                (false  == getUserProp obj "neb_Vis_Portal" ) and
	                (false  == getUserProp obj "neb_Vis_Occluder" )
	    ) else
	    ( 
	        return false
	    )
	),
	
	
	fn declareVar =
    (
        global nebVisData = NebStructVisData()
    ),
    
    --------------------------------------------------------------------------------
    /** 
        declare persistent variables of this file
	*/
    fn declarePersistent =
    (
        persistent global nebVisData
    ),
	
	--------------------------------------------------------------------------------
	/**
	*/
	fn initModule =
	(
	    vislog.log 4 "Initialize visibility module\n"
	    nebVisOp.declareVar()
		nebVisData.init()
	),
	
    --------------------------------------------------------------------------------
	/**
	*/
	fn UpdateObjectFromVersion obj version  =
	(
	     if (NebOp.IsType obj "visibility") then
	     (
	        --add property
	        if (version < 000802) do 
            (
                setUserProp obj "neb_Vis_Occluder" false
            )
	     )
	),

	--------------------------------------------------------------------------------
	/**
	    for update object or persist data , from old version max file
	    
	*/
    fn updateFromVersion version =
    (
        vislog.log 3 "visibility Update\n"
		
        if (version < 000300) do
        (
            -- Update from 0.2.0 to 0.3.0
        )
        
        if (version < 000400) do
        (
            -- Update from 0.3.0 to 0.4.0
        )
        if ( version < 000802) do 
        (
            -- lost old data and create new
            nebVisData = NebStructVisData()
            nebVisData.init()
            
        )
        
        NebOp.IterateScene preFunc:NebVisOp.UpdateObjectFromVersion preArg:version
    ),
    
        --------------------------------------------------------------------------------
    /**
        Callback file preMerge
        Create copy of all persistent variables
    */    
    fn FilePreMerge =
    (
        global NebMergeCopyOfNebVisData = NebVisData
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
         NebVisData = NebMergeCopyOfNebVisData -- restore original values
         for obj in listObjects do
         (
            if NebVisOp.IsCell obj then
            (
                obj.material  = nebVisData.getMaterial ( random 1 7 ) -- for not mege cell materials
            ) else if (classOf obj) == nebVisPortalDummy then
            (
                obj.checkMaterial()
            )
        )
         
    ),
    
    --------------------------------------------------------------------------------
    /**
     Callback file post open 
    */
    
    fn FilePostOpen =
    (
        vislog.log 4 "visibility posOpen\n"
        nebVisData.CheckMaterialsValues()
        nebVisData.updateCellList = #()
    ),
    
    --------------------------------------------------------------------------------
    /**
     Callback NodePreDelete 
    */
    
    fn NodePreDelete node =
    ( 
        format "PreDElete %\n" node
        nebVisData.updateCellList =#()
        
        if ( (getUserProp node "neb_Vis_Cell") == true  )  do -- if is a cell 
        (
            
            for i in refs.dependents node where classOf i == nebVisPortalDummy do
            (
                append nebVisData.updateCellList i
            )
         )

    ),
    --------------------------------------------------------------------------------
    /**
     Callback file post open 
    */
    
    fn NodePostDelete =
    (
        format "PostDElete \n"
        for i in nebVisData.updateCellList do
        (   
           format "    :: % : to % --- %\n" i i.cell1 i.cell2
           i.checkMaterial()
        )
        
        nebVisData.updateCellList =#()

    )
	
)


--------------------------------------------------------------------------------
--   EOF
--------------------------------------------------------------------------------