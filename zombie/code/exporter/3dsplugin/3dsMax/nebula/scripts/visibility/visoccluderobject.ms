global NebVisOp
global nebVisData
plugin helper nebVisOccluderDummy
--plugin Geometry nebVisOccluderDummy
name:"Nebula Occluder helper"
classID:#(0x240d76ff, 0x39a6ba3)
extends:Dummy
--extends:Editable_Poly
replaceUI:false
invisible:true
version:1
(
	parameters main rollout:params
	(
		doubleSided type:#boolean subAnim:false ui:chk1 default:true
    )

    rollout params "Occluder Parameters" width:162 height:222
    (
		checkbox chk1 "double sided" pos:[30,10] width:152 height:20 
    )
    
	on update do 
    (
        
    )
)
