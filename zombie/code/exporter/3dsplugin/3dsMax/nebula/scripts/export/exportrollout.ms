global GrimoireViewerGamelib
global GrimoireViewerOnlyOf
rollout nebExportOptionsRollut "Zombie Export Options" width:330 height:560
(
		
	groupBox     grp_ExportMode "Export Mode" pos:[10,10] width:150 height:130
	radiobuttons rd_mode "" pos:[20,30] width:105 height:64 labels:#( "Brush", "Indoor", "Character" ) columns:1
	radiobuttons rd_anim "" pos:[40,75] width:110 height:64 labels:#("Mesh", "Skeleton", "Skin animation", "Morpher animation") columns:1
	
	
	groupBox     grp_ExportTo    "Export To" pos:[170,10] width:150 height:130
	radiobuttons rd_gameLibrary  "" pos:[180,30] width:110 height:32 labels:#("Game library", "Temporal directory") columns:1


	
	groupBox     grpName                  "Names"          pos:[9,150]   width:310 height:140
	edittext     edt_EntityClass          "Entity class"   pos:[20,170]  width:210 height:17
	edittext     edt_LibraryFolder        "Library folder" pos:[19,200]  width:210 height:17
	edittext     edt_resource             "Resource"       pos:[20,230]  width:210 height:17
    edittext     edt_SkeletonClass        "Skeleton class" pos:[20,260]  width:210 height:17
	button       btn_exploreEntityClass   "..."            pos:[240,170] width:70  height:18
	button       btn_exploreSkeletonClass "..."            pos:[240,260] width:70  height:18
	
	groupBox     grp_optionsTmp     "Options for temporal directory" pos:[9,300] width:310 height:70
	checkbox     chk_binaryresource     "Binary resource"      pos:[20,320]  width:91 height:17
	checkbox     chk_collectextures "Collect textures" pos:[180,320] width:91 height:17
	checkbox     chk_optimizemesh   "Optimize mesh"    pos:[20,345]  width:91 height:17
	
	groupBox     grp_CharOptions      "Options for skeletons and animations" pos:[9,380] width:310 height:90
	checkbox     chk_firstperson      "First person"   pos:[220,440]  width:80 height:17
	radiobuttons rd_critter "" pos:[50,400] width:105 height:64 labels:#( "Human" ) columns:1
	
--	checkbox     chk_exportmotion    "Motion"   pos:[140,440]  width:50 height:17
--	groupBox     grp_optionsmotion   "MotionOptions" pos:[130,420] width:180 height:50
--	checkbox     chk_motionX         "X"   pos:[210,440]  width:30 height:17
--	checkbox     chk_motionY         "Y"   pos:[240,440]  width:30 height:17
--	checkbox     chk_motionZ         "Z"   pos:[270,440]  width:30 height:17
	checkbox     chk_exportmotion    "Motion"   pos:[20,440]  width:50 height:17
	groupBox     grp_optionsmotion   "MotionOptions" pos:[9,420] width:180 height:50
	checkbox     chk_motionX         "X"   pos:[90,440]  width:30 height:17
	checkbox     chk_motionY         "Y"   pos:[120,440]  width:30 height:17
	checkbox     chk_motionZ         "Z"   pos:[150,440]  width:30 height:17

	checkbox     chk_openConjurer     "Open Conjurer"   pos:[180,475]  width:100 height:17
	checkbox     chk_exportThumbNail  "Export thumbnail"    pos:[20,475]  width:100 height:17
	
	button       btn_Ok     "Ok"     pos:[ 10,500] width:90 height:22		
	button       btn_Exp    "Export" pos:[110,500] width:90 height:22	
	button       btn_Cancel "Cancel" pos:[210,500] width:90 height:22
	
	
	fn check =
	(
		if rd_mode.state == 3 then --if is character hability the anim mode
		(
			rd_anim.enabled = true
		) else
		(
			rd_anim.enabled = false
		)
				
		-- resources needed but entity class name not needed
		if ( rd_mode.state == 3 and ( rd_anim.state == 3 or rd_anim.state == 4 )) then
		(
			edt_resource.enabled = true
            edt_EntityClass.enabled = false;
		) else
		(
			edt_resource.enabled = false
            edt_EntityClass.enabled = true;
		)
		
		-- skeletonclass name needed
		if ( rd_mode.state == 3 and ( rd_anim.state == 1 or rd_anim.state == 3 or rd_anim.state == 4 )) then
		(
			edt_SkeletonClass.enabled = true
			btn_exploreSkeletonClass.enabled = true	
		) else
		(
			edt_SkeletonClass.enabled = false
			btn_exploreSkeletonClass.enabled = false
		)		
		
		-- critter name needed
		if ( rd_mode.state == 3 and ( rd_anim.state == 2 or rd_anim.state == 3)) then
		(
		    rd_critter.enabled = true
		) else
		(
		    rd_critter.enabled = false
		)		
		
		-- do not add to grimoire when export skeleton or animation
		if ( rd_mode.state == 3 and ( rd_anim.state == 2 or rd_anim.state == 3 or rd_anim.state == 4 )) then
		(
		    edt_LibraryFolder.enabled = false
		) else
		(
		    edt_LibraryFolder.enabled = true
		)		
		
		if ( rd_gameLibrary.state == 2 ) then -- if is a temporal directory enable the next options
		(
			chk_binaryresource.enabled   = true
			chk_collectextures.enabled = true
			chk_optimizemesh.enabled = true
		
		) else
		(
   			chk_binaryresource.enabled = false
			chk_collectextures.enabled = false
			chk_optimizemesh.enabled = false
		)
		
		-- first person activate only when exporting skeleton or character mesh
		if ( rd_mode.state == 3 ) then
		(
		    chk_firstperson.enabled = true
		) else
		(
		    chk_firstperson.enabled = false
		)		

		-- if not skeleton, enable checkbox to open conjurer after exp
		if ( rd_mode.state == 3 and rd_anim.state == 2) then
		(
		    chk_openConjurer.enabled = false
		) else
		(
		    chk_openConjurer.enabled = true
		)
		
		-- motion only when exporting skin animation
		if ( rd_mode.state == 3 and rd_anim.state == 3 ) then
		(
			chk_exportmotion.enabled = true
		    chk_motionX.enabled = true
		    chk_motionY.enabled = true
		    chk_motionZ.enabled = true
		) else
		(
			chk_exportmotion.enabled = false
		    chk_motionX.enabled = false
		    chk_motionY.enabled = false
		    chk_motionZ.enabled = false			
		)
	)
	
	fn copyValues =
	(
	    nebNewExportOptions.gameLibrary = ( rd_gameLibrary.state == 1 ) 
		
		nebNewExportOptions.exportMode = rd_mode.state 
		
		nebNewExportOptions.animMode = rd_anim.state
		
        -- if export animation, open skeleton class, UGLY HACK
		if ( rd_mode.state == 3 and ( rd_anim.state == 3 or rd_anim.state == 4 )) then
		(
		    nebNewExportOptions.entityName = edt_SkeletonClass.text
		) else
		(
		    nebNewExportOptions.entityName = edt_EntityClass.text
		)				
		
		nebNewExportOptions.libraryFolder = edt_LibraryFolder.text
		nebNewExportOptions.resourceFile = edt_resource.text
		nebNewExportOptions.skeletonName = edt_SkeletonClass.text
		
		nebNewExportOptions.collectTextures = chk_collectextures.checked
		nebNewExportOptions.binaryResource = chk_binaryresource.checked 
		nebNewExportOptions.optimizeMesh = chk_optimizemesh.checked
		nebNewExportOptions.firstPerson = chk_firstperson.checked
		nebNewExportOptions.valid = true 
		nebNewExportOptions.critterName = rd_critter.state
		nebNewExportOptions.openConjurer = chk_openConjurer.checked
		nebNewExportOptions.exportMotion = chk_exportmotion.checked
		nebNewExportOptions.motionX = chk_motionX.checked
		nebNewExportOptions.motionY = chk_motionY.checked
		nebNewExportOptions.motionZ = chk_motionZ.checked
		
		nebNewExportOptions.exportThumbNail = chk_exportThumbNail.checked;
	)
	
	on nebExportOptionsRollut open do
	(
	    rd_gameLibrary.state = 2
	    if nebNewExportOptions.gameLibrary then
		(
			rd_gameLibrary.state = 1 
		)
		
		rd_mode.state = nebNewExportOptions.exportMode
		
		rd_anim.state = nebNewExportOptions.animMode
		
		edt_EntityClass.text   = nebNewExportOptions.entityName
		edt_LibraryFolder.text = nebNewExportOptions.libraryFolder
		edt_resource.text      = nebNewExportOptions.resourceFile
		edt_SkeletonClass.text = nebNewExportOptions.skeletonName
		
		chk_collectextures.checked = nebNewExportOptions.collectTextures 
		chk_binaryresource.checked = nebNewExportOptions.binaryResource
		chk_optimizemesh.checked = nebNewExportOptions.optimizeMesh
        chk_firstperson.checked = nebNewExportOptions.firstPerson;
        chk_exportThumbNail.checked   = nebNewExportOptions.exportThumbNail;
        rd_critter.state = nebNewExportOptions.critterName
		chk_openConjurer.checked = nebNewExportOptions.openConjurer;
		
		chk_exportmotion.checked = nebNewExportOptions.exportMotion;
		chk_motionX.checked = nebNewExportOptions.motionX;
		chk_motionY.checked = nebNewExportOptions.motionY;
		chk_motionZ.checked = nebNewExportOptions.motionZ;
		
		
		check()
	)
	
	on rd_mode changed val do
	(
		check()
	)
	
	on rd_gameLibrary changed val do
	(
	 	check()
	)
	
	on  rd_anim changed val do
	(
		check()
	)
	
	on rd_critter changed val do
	(
		check()
	)
		
	on btn_exploreEntityClass pressed do
	(
	    GrimoireViewerGamelib = (rd_gameLibrary.state == 1 )
		GrimoireViewerOnlyOf = ""
		CreateDialog GrimoireViewer modal:true	
		if GrimoireViewer.end_state do
		(
			if GrimoireViewer.classname != "" do
			(
				edt_EntityClass.text = GrimoireViewer.classname
			)
			
			if GrimoireViewer.librarypath != "" do
			(
				edt_LibraryFolder.text = GrimoireViewer.librarypath
			)
		)
	)

	on btn_cancel pressed do
	(
		destroyDialog nebExportOptionsRollut
	)
	
	on btn_Ok pressed do
	(
	    copyValues()
		destroyDialog nebExportOptionsRollut
	)
	
	on btn_Exp pressed do
	(
	    copyValues()
	    NebExportOp.Export()
		destroyDialog nebExportOptionsRollut
	)
	
	
    on btn_exploreSkeletonClass pressed do
	(
	    GrimoireViewerGamelib = (rd_gameLibrary.state == 1 )
		GrimoireViewerOnlyOf = "neskeleton"
		CreateDialog GrimoireViewer modal:true	
		if GrimoireViewer.end_state do
		(
			if GrimoireViewer.classname != "" do
			(
				edt_SkeletonClass.text = GrimoireViewer.classname
			)
			
		)
	)
)

--
