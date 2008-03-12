/**
   @file visibilityUtil.ms
   @ingroup n3dsMaxScripts
   @author Cristobal Castillo Domingo <cristobal.castillo@tragnarion.com>
   @brief This utility is for managament of visibility
*/

utility NebVisUtil "Nebula Visibility utilities"
(
	-- Define variables that are visible to all functions in the utility

	-- Define the GUI interface

	button btnToCell "Convert to Cell" width:100 toolTip:"Convert selected to visibility cell"
	
	button btnCreateCell "Create a Cell" width:100
	
	button btnToPortal "Convert To Portal" width:100
	
	button btnToOccluder "Convert To Occluder" width:100
	
	on btnToPortal pressed  do with undo label:"ConvetToPortal" on
	(
		NebVisOp.ConvertToPortal selection[1]
	)
	
	on btnToOccluder pressed  do with undo label:"ConvetToOccluder" on
	(
		NebVisOp.ConvertToOccluder selection[1]
	)
	

	
	on btnToCell pressed  do with undo label:"Convert to Visibility Cell" on
	(
		NebVisOp.ConvertToCell selection
	)

	
    on btnCreateCell pressed do with undo label:"Create Visibility Cell" on
	(
		NebVisOp.CreateCellFrom selection
	)
	
	fn chekmenu =
	(	
		btnToCell.enabled = (Try(selection.count != 0 and NebVisOp.canConverToCell selection[1])Catch(false))
		btnCreateCell.enabled = btnToCell.enabled
		btnToPortal.enabled = selection.count != 0 and NebVisOp.canConverToPortal selection[1]
		btnToOccluder.enabled = selection.count != 0 and NebVisOp.canConverToOccluder selection[1]
	)
	
	on NebVisUtil open do
	(
		chekmenu()
		callbacks.addscript #selectionSetChanged "NebVisUtil.chekmenu()" id:#NebVisPanel
	)
	
	on NebVisUtil close do
	(
		callbacks.removescripts id:#NebVisPanel
	)
	

) -- End ZombieVisibility

