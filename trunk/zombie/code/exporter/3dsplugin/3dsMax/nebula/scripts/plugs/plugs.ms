/**
   @file plugs.ms
   @ingroup n3dsMaxScripts
   @author Cristobal Castillo Domingo
   @brief This module is for managament of visibility

   (C) 2006 Conjurer Services, S.A.
*/

plugslog = NebCreateLog "plugs"  "plugs module" 2 true


--------------------------------------------------------------------------------
/**
*/
struct NebPlugsOp (
    
	--------------------------------------------------------------------------------
	/**
	*/
	fn convertToPlug mesh1 =
	(	
		select mesh1
		NebOp.SetType mesh1 "plugs" 
	),
		
	-------------------------------------------------------------------------------
	/**
	*/
	fn initModule =
	(
	    --nebPlugOp.declareVar()
	)
)


--------------------------------------------------------------------------------
--   EOF
--------------------------------------------------------------------------------