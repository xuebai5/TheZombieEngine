/**
   @file materialUtil.ms
   @ingroup n3dsMaxScripts
   @author Cristobal Castillo Domingo
   @brief This utility is for managament of zombieMaterials.

   (C) 2006 Conjurer Services, S.A.
*/
utility NebMatUtil "Nebula Material utilities"
(
	-- Define variables that are visible to all functions in the utility
	local ostream, tabs = ""
	global vRTGExportInit = false
	global resServer
	global fileManager
	global Saver


	-- Define the GUI interface

	group "Render Properties"
	(
	button btn_render "Render properties" width:100
	button btn_bumpTrue "Enable BumpMaps" width:100
--	button btn_bumpFalse "Disable BumpMaps" width:100
	button btn_showTrue  "Show in viewport" width:100
	button btn_showFalse  "Hide in viewport" width:100
	button btn_showLightmap  "Show lightmaps" width:100
	)
	group "Invalid Materials"
	(
	button btn_SelectInvalids  "Select invalids" width:100
	button btn_DeselectInvalids  "Deselect invalids" width:100
	)
	group "Clean"
	(
	button btn_shadowOff  "Shadow off" width:100
	button btn_fix "Mr Proper" width:100
	button btn_freeMemory "Clear free bitmaps" width:100
	button btn_mtlEditorZ "Mtl editor Z" width:100
	button btn_mtlEditorS "Mtl editor S" width:100
	button btn_mtlNormalExt "Change normal ext" width:100 tooltip:"change the normal and bumpmap extension to dds"
	)
	button btn_info  "Info programmers" width:100
	
	group "Shader"
	(
    dropdownlist ddl_shadertype "Shader type" width:130 height:40 items:nebGfxMaterialTypeList
	button btn_setShaderType  "Set" width:100
	button btn_getShaderType  "Get" width:100
	)
	
	on btn_fix pressed do
	(
		iterateMaterial fix true
		freeSceneBitmaps()
	)
	
	on btn_shadowOff pressed do
	(
		iterateMaterial nebShadowFalse true
	)
	
    on btn_setShaderType pressed do
	(
	    nebMatData.toolSetMaterialName = ddl_shadertype.selected
	    iterateMaterial (nebMatOp.setShaderType) true selectedObjects:true
	    -- Update the material editor because the shadertype has not a automatic ui
	)
	
	on btn_getShaderType pressed do
	(
	    nebMatData.toolGetMaterialName = #()
		iterateMaterial (nebMatOp.getShaderType) true selectedObjects:true
		if ( nebMatData.toolGetMaterialName.count == 0 ) then
		(
		    ddl_shadertype.selection = 0
	    ) else 
	    (
	        local shader = nebMatData.toolGetMaterialName[1]
	        local index = 2
	        while ( index <= nebMatData.toolGetMaterialName.count) and ( nebMatData.toolGetMaterialName[index] == shader ) do
	        (
	            index += 1
	        )
	        
	        if ( index <= nebMatData.toolGetMaterialName.count ) then
	        (
	            -- different shader types
	            ddl_shadertype.selection = 0
	        ) else
	        (
	            ddl_shadertype.selection =  findItem nebGfxMaterialTypeList shader
	        )
	    )  
	)
	
	on btn_render pressed do
	(
		CreateDialog zombieRenderRollout
	)
	
	on btn_freeMemory pressed do
	(
	    freeSceneBitmaps() 
		gc light:true
	)
	
	on btn_mtlNormalExt pressed do
	(
	    iterateMaterial NebMatOp.changeNormalExtensionToDDS true
	)
	
	on btn_bumpTrue pressed do
	(
	    iterateMaterial bumpTrue true
	)
	
	
	on btn_bumpFalse pressed do
	(
		iterateMaterial bumpFalse true
	)
	
	on btn_showTrue  pressed do 
	(
		iterateMaterial showTrue true
	)

	on btn_showFalse  pressed do 
	(
		iterateMaterial showFalse true
	)
	
	on btn_showLightmap pressed do
	(
	    iterateMaterial showLightmap true
	)
	
	on btn_info pressed do
	(
		iterateMaterial Info true
		format"\nEnd\n"
	)
	on btn_SelectInvalids pressed do
	(
		selectInvalids()
	)
	
	on btn_DeselectInvalids pressed  do
	(
		deselectInvalids()
	)

    on btn_mtlEditorZ pressed do
    (
         clearMtlEditor()
    )
    
    on btn_mtlEditorS pressed do
    (
         clearMtlEditor mtlType:Standardmaterial
    )
    
    on NebMatUtil open do
    ( 
        format "abierto mat util\n"
        ddl_shadertype.selection = 0
    )

) 

--------------------------------------------------------------------------------
--   EOF
--------------------------------------------------------------------------------
