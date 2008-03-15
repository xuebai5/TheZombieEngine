/**
   @file lodUtil.ms
   @ingroup n3dsMaxScripts
   @author Cristobal Castillo Domingo
   @brief This utility is for managament of lod
   
   (C) 2006 Conjurer Services, S.A.
*/
utility NebLodUtil "Nebula LOD utilities" width:162 height:500
(
	-- Define variables that are visible to all functions in the utility

	-- Define the GUI interface

	
	spinner spn_number_of_LOD "number of LOD" pos:[28,10] width:119 height:16 range:[0,6,0] type:#integer scale:1
	
	

	spinner spn_dis1 "Lod 0 to: " pos:[10,  40] width:120 height:16 range:[0,1e+006,0] type:#worldunits scale:1
	spinner spn_dis2 "Lod 1 to: " pos:[10,  60] width:120 height:16 range:[0,1e+006,0] type:#worldunits scale:1
	spinner spn_dis3 "Lod 2 to: " pos:[10,  80] width:120 height:16 range:[0,1e+006,0] type:#worldunits scale:1
	spinner spn_dis4 "Lod 3 to: " pos:[10, 100] width:120 height:16 range:[0,1e+006,0] type:#worldunits scale:1
	spinner spn_dis5 "Lod 4 to: " pos:[10, 120] width:120 height:16 range:[0,1e+006,0] type:#worldunits scale:1
	spinner spn_dis6 "Lod 5 to: " pos:[10, 140] width:120 height:16 range:[0,1e+006,0] type:#worldunits scale:1
	
	button btnCalculate "Calculate" pos:[20,180] width:120 height:21 toolTip:"Calculate"
	button btnSimuate   "Simulate in viewPort" pos:[20,200] width:120 height:21 toolTip:"Calculate"
	
	fn checkmenu =
	(
		btnSimuate.enabled = false
		btnCalculate.enabled = false
		
		
		spn_dis1.enabled = nebLodData.num > 0
		spn_dis2.enabled = nebLodData.num > 1
		spn_dis3.enabled = nebLodData.num > 2
		spn_dis4.enabled = nebLodData.num > 3
		spn_dis5.enabled = nebLodData.num > 4
		spn_dis6.enabled = nebLodData.num > 5
		
	    spn_dis2.range = [nebLodData.list[1],1e+006,0]
	    spn_dis3.range = [nebLodData.list[2],1e+006,0]
	    spn_dis4.range = [nebLodData.list[3],1e+006,0]
	    spn_dis5.range = [nebLodData.list[4],1e+006,0]
	    spn_dis6.range = [nebLodData.list[5],1e+006,0]
	    
	    spn_number_of_LOD.value = nebLodData.num 
		
		spn_dis1.value = nebLodData.list[1]
		spn_dis2.value = nebLodData.list[2]
		spn_dis3.value = nebLodData.list[3]
		spn_dis4.value = nebLodData.list[4]
		spn_dis5.value = nebLodData.list[5]
		spn_dis6.value = nebLodData.list[6]
		
	)
	
	on spn_number_of_LOD changed val do 
	(
	    nebLodData.num = spn_number_of_LOD.value
	    nebLodData.check()
	    checkmenu()
    )
    


    on spn_dis1 changed val do 
	(
	        nebLodData.list[1] = spn_dis1.value
	        nebLodData.check()
	        checkmenu()
    )
    

    on spn_dis2 changed val do 
	(
	        nebLodData.list[2] = spn_dis2.value
	        nebLodData.check()
	        checkmenu()
    )
    
    
     on spn_dis3 changed val do 
	(
	        nebLodData.list[3] = spn_dis3.value
	        nebLodData.check()
	        checkmenu()
    )
    
    on spn_dis4 changed val do 
	(
	        nebLodData.list[4] = spn_dis4.value
	        nebLodData.check()
	        checkmenu()

    )
    
    on spn_dis5 changed val do 
	(
	        nebLodData.list[5] = spn_dis5.value
	        nebLodData.check()
	        checkmenu()

    )
    
    
    on spn_dis6 changed val do 
	(
	        nebLodData.list[6] = spn_dis6.value
	        nebLodData.check()
	        checkmenu()

    )
    
	on NebLodUtil open do
	(
		checkmenu()
		callbacks.addscript #filePostOpen "NebLodUtil.checkmenu()" id:#NebLodPanel
	)
	
	on NebLodUtil close do
	(
		callbacks.removescripts id:#NebLodPanel
	)
	
)