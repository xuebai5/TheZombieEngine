/**
   @file startupnebula.ms
   @ingroup n3dsMaxScripts
   @author Cristobal Castillo Domingo
   @brief this script initialize al modules of plugins.

   (C) 2006 Conjurer Services, S.A.
*/

NebOp.initModule() -- The main module 
NebMatOp.initModule()
NebMatOp.resetValues()
NebVisOp.initModule()
NebPlugsOp.initModule()
NebLodOp.initModule()
NebExportOp.initModule()
NebCollOp.initModule()


/**
	This function is only for developer
	reload all scripts
*/
fn NebReset =
(
    fileIn ( (getDir #maxroot) + "plugins/nebula.ms" )
    fileIn ( (getDir #maxroot) + "scripts/startup/startupnebula.ms" )
	return ok
    
)

fileIn "nebula/scripts/menu/removemenu.ms"
fileIn "nebula/scripts/menu/macros.ms"
fileIn "nebula/scripts/menu/menu.ms"

--------------------------------------------------------------------------------
--   EOF
--------------------------------------------------------------------------------