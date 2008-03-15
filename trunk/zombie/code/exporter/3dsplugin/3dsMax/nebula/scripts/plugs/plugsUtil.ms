/**
   @file materialUtil.ms
   @ingroup n3dsMaxScripts
   @author Cristobal Castillo Domingo
   @brief This utility is for managament of zombieMaterials.

   (C) 2006 Conjurer Services, S.A.
*/
utility NebPlugsUtil "Nebula Plugs utilities"
(
	-- Define the GUI interface
	button btn_convert "Convert to Plug" width:100
	
	on btn_convert pressed do
	(
	    NebPlugsOp.convertToPlug selection
	)
	
	fn canConvertToPlug obj =
	(
		return ( classOf obj == dummy ) and not ( NebOp.IsType obj "plugs" ) 
	)
	
	fn chekmenu =
	(	
		btn_convert.enabled = (Try(selection.count != 0 and NebPlugsUtil.canConvertToPlug selection[1])Catch(false))
	)
	
	on NebPlugsUtil open do
	(
		chekmenu()
		callbacks.addscript #selectionSetChanged "NebPlugsUtil.chekmenu()" id:#NebPlugsPanel
	)
	
	on NebPlugsUtil close do ( callbacks.removescripts id:#NebPlugsPanel )
	
) -- End

--------------------------------------------------------------------------------
--   EOF
--------------------------------------------------------------------------------