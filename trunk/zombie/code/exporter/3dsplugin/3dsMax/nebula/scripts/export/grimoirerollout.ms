global GrimoireViewerGamelib = true
global GrimoireViewerOnlyOf = ""
rollout GrimoireViewer "Nebula Grimoire Viewer" width:360 height:540
(
    -- activeX treeview control
	button  btn_ok "Ok" across: 2
	button  btn_cancel "Cancel"
    ActiveXControl tv "MSComctlLib.TreeCtrl.2" pos:[5,52] width:280 height:460
	ActiveXControl ilTv "MSComctlLib.ImageListCtrl" height:0 width:0	
	
	local end_state
	local foldermark
	
	local classname
	local librarypath

	fn addFolderChildren parent strings =
	(
		local index = undefined
		local child = parent.child
		while child != undefined do
		(
			if child.text == strings[ 1 ] then
			(
				index = child
				child = undefined
			)
			else
			(
				child = child.next
			)
		)

		if index == undefined do
		(		
			index = tv.nodes.add parent 4 "" strings[1]
			index.bold = foldermark
			if foldermark then
			(
				index.image = 1
			)
			else
			(
				index.image = 2
			)
		)

		strings = deleteItem strings 1
		
		if strings.count > 0 do
		(
			addFolderChildren index strings
		)
	)
	
    fn addFolder name = 
    (
		local strings = filterString name "/"

		local index = undefined
		local i = 0
		for node in tv.nodes do
		(
			i = i + 1
			if node.text == strings[1] do
			(
				index = tv.nodes[ i ]
			)
		)
		
		if index == undefined do
		(
			index = tv.nodes.add()
 			index.text = strings[ 1 ]
			index.bold = foldermark
			if foldermark then
			(
				index.image = 1
			)
			else
			(
				index.image = 2
			)
		)
		
		strings = deleteItem strings 1
		
		if strings.count > 0 do
		(
			addFolderChildren index strings
		)
    )
	
    on GrimoireViewer open  do
    (
		tv.PathSeparator = "/"
		tv.LabelEdit = #tvwManual
		tv.Indentation = 40
		tv.Style = #tvwTreelinesPlusMinusPictureText
		tv.LineStyle = #tvwRootLines
		
		ilTv.imagewidth = 16
		ilTv.imageheight = 16

		iconDir = (getDir #maxroot) + "\\nebula\\images\\"

		ilTv.listImages.add 1 #folder (loadPicture (iconDir + "folder.bmp"))
		ilTv.listImages.add 2 #class (loadPicture (iconDir + "class.bmp"))
		
		tv.imageList = ilTv
		
		end_state = false
		classname = ""
		librarypath = ""

		if GrimoireViewerOnlyOf == "" then
		(		
			classes = nGetEntityClasses GrimoireViewerGamelib
			
			foldermark = true
			if classes != undefined and classes[1] != undefined do
			(		
				for name in classes[1] do
				(
	    	    	addFolder name
				)
			)
			
			foldermark = false
			if classes != undefined and classes[2] != undefined do
			(		
				for name in classes[2] do
				(
	    	    	addFolder name
				)
			)
		)
		else
		(
			classes = nGetEntityClassesOf GrimoireViewerGamelib GrimoireViewerOnlyOf
			
			foldermark = false
			if classes != undefined do
			(		
				for name in classes do
				(
	    	    	addFolder name
				)
			)
		)
    )
	
	on tv NodeClick clicked_node do
	(
		if clicked_node != undefined do
		(
			if clicked_node.bold then
			(
				classname = ""
				librarypath = clicked_node.fullPath
			)
			else
			(
				classname = clicked_node.text
				if clicked_node.parent != undefined then
				(
					librarypath = clicked_node.parent.fullpath
				)
				else
				(
					librarypath = ""
				)
			)
		)
	)
	
	on btn_cancel pressed do
	(
		end_state = false
		destroyDialog GrimoireViewer
	)
	
	on btn_Ok pressed do
	(
		end_state = true
		destroyDialog GrimoireViewer
	)
)

fn OpenGrimoire = 
(
	local roll = newRolloutFloater "Grimoire" 360 540
	addRollout GrimoireViewer roll
)
