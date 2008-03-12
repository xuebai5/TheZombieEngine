fn NebCanUseTreeNormals obj =
(
    return ( classOf obj == Editable_Poly or classOf obj == PolyMeshObject)    
)

fn NebTreeNormals obj=
(
	in coordsys world (
	
		local vertSelection = (polyOp.getVertSelection obj) as array
	
		local midle = point3 0 0 0
		local count = 0
		for i in vertSelection do
		(
			midle+= polyOp.getVert obj i
			count+=1
		)
		midle = midle / count
		
		local hasEditNormal = false
		for i in obj.modifiers where classof i == Edit_Normals do
		(
			hasEditNormal = true
		)
		
		if  ( not hasEditNormal ) do
		(
			addmodifier obj ( Edit_Normals() )
		)
		
		--obj.edit_normals
		max modify mode
		modPanel.setCurrentObject obj.modifiers[#Edit_Normals]
		
		for i in vertSelection do
		(
			local my_verts = #{i}
			local my_normals = #{}
			obj.Edit_Normals.ConvertVertexSelection &my_verts &my_normals
			local listNormal = my_normals as array
			local dir = normalize ( ( polyOp.getVert obj i ) - midle )
			
			--format " vert %  % normals %\n"  i  my_verts listNormal
			
			for k in listNormal do
			(
				--format "actualizando normal %\n" k
				obj.Edit_Normals.SetNormal k dir
			)
			
			obj.Edit_Normals.MakeExplicit selection:my_normals
		)
		

		
	)
	
	
--	addModifier obj ( Edit_Normals () )
)


utility NebToolsUtil "Nebula general tools"
(
	-- Define variables that are visible to all functions in the utility

	-- Define the GUI interface

	button btnTreeNormals "Tree normals" width:100 toolTip:"Calculate tree normals"
	
	
	on btnTreeNormals pressed  do with undo label:"TreeNormals" on
	(
		try
		(
		    NebTreeNormals selection[1]
		) catch()
	)
	
	fn chekmenu =
	(	
		btnTreeNormals.enabled = (Try(selection.count == 1 and NebCanUseTreeNormals selection[1] )Catch(false))
		
	)
	
	on NebToolsUtil open do
	(
		chekmenu()
		callbacks.addscript #selectionSetChanged "NebToolsUtil.chekmenu()" id:#NebToolsPanel
	)
	
	on NebToolsUtil close do
	(
		callbacks.removescripts id:#NebVisPanel
	)
	

) -- End ZombieVisibility
