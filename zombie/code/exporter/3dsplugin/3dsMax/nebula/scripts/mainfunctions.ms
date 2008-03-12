/**
   @file mainfunctions.ms
   @ingroup n3dsMaxScripts
   @author Cristobal Castillo <cristobal.castillo@tragnarion.com>
   @brief main functions of scripts
   @remarks
   functions
	- NeblogMatrix3, friendly function for log a matrix
	- CreateVolFromPoints, create a volInfo for a best obb, sphere, capsule
	- CreatePoints , return a list of vertex from objects
	- createListPoints, return a list of list vertex , descompose one object in subojects
	- joinObjects, return a editable mesh with all objects
	- ReducePoints2D, return a list of vertex without duplicates in XY projection
	- IsAncestorOf, return true if first node is in top of second node hierarchy
	- SelectParentPoints, return the list of nodes from an array than have not parents
		in that array
	- LinkWithFirst, link nodes in a list with the first if it is possible
		
	
	Structs
	- volInfo, 
  
*/

global nebula_getname_text = ""
--------------------------------------------------------------------------------
/**
*/
rollout
	nebula_getname
	"Get Name"
	width:200
(
	-- additional text  of rollout
	label ed_label1 align:#center
	edittext ed_name
	button btn_Ok  "Ok" align:#center across:2
	button btn_Cancel  "Cancel" align:#center
	
	local end_state
	
	on nebula_getname open do
	(
		ed_label1.text = nebula_getname_text
		end_state = false
		ed_name.enabled
	)	
	
	on btn_Ok pressed do
	(
		end_state = true
		destroyDialog nebula_getname
	)
	
	on btn_Cancel pressed do
	(
		end_state = false
		destroyDialog nebula_getname
	)
)

--------------------------------------------------------------------------------
/**
*/
fn NebLogMatrix3 name level message mat =
(
	log = logserver.find name
	if ( log != 0) then
	(
		log.log level message
		mesg  = message + "\n\t" 
		mesg += ( mat.row1 as string) + "\n\t"
		mesg += ( mat.row2 as string) + "\n\t"
		mesg += ( mat.row3 as string) + "\n\t"
		mesg += ( mat.row4 as string) + "\n"
	)
)


--------------------------------------------------------------------------------
/**
	@remarks struct Volinfo
	- pos       center of volume
	- rot       orientarion of new figure
	- mat       Convert from worls to local
	- XYZ       MAX of BBOX in local, size of BBOX = 2 * XYZ , volume = 2*x * 2*y * 2*z
	- disXYZ    Radius of sphere
	- disXY     Radius of cylinder , base in XY height in Z
	- volBox    Voloume of bounding box
	- volEsf    volume of sphere
	- volCyl    volume of cylinder
	- heightCap height of capsule
	
	Note in local, the BBOX Z >= Y >= X
*/
struct volinfo 
(
	pos,   
	rot,   
	mat,   
	XYZ,   
	disXYZ,
	disXY,
	disYZ, 
	volBox,
	volEsf,
	volCyl,
	volRealCyl,
	heightCap,
	heightCyl,
	radiusCyl,
	isCylinderInX,
	
	fn changecylinder myCyl  =
	(
			mycyl.height = heightCyl
			mycyl.radius = radiusCyl
			mycyl.pivot = mycyl.center
			if isCylinderInX do
			(
				rotate mycyl ( eulerangles 0 90 0 )
			)
			rotate mycyl ( eulerangles rot.x rot.y rot.z )
			mycyl.pos = pos
	),
	
	fn changeCapsule myCap  =
	(
			mycap.heighttype = 0
			mycap.height = heightcap
			mycap.radius = disXY
			mycap.pivot = mycap.center
			rotate mycap ( eulerangles rot.x rot.y rot.z )
			mycap.pos = pos
	),
	
	fn changeSphere mysphere =
	(
		mysphere.pos = pos
		mysphere.radius = disXYZ
	),
	
	fn changeBox mybox =
	(
		local size = 2.0 * xyz
		local len = size.y
		local wid = size.x
		local heig = size.z

		mybox.length = len
		mybox.width =  wid
		mybox.height =  heig
		
		mybox.pivot =  mybox.center
		
		rotate mybox  ( eulerangles rot.x rot.y rot.z )
		mybox.pos = pos
	),
	
	fn AutomaticCreate =
	(
		local obj
		case of
		(
			(volRealCyl < volCyl and volRealCyl < volEsf and volRealCyl < volBox): 
			(
				obj = Cylinder ()
				changeCylinder obj
			)
			(volCyl < volEsf and volCyl<volBox): 
			(
				obj = Capsule ()
				changeCapsule obj
			)
			(volEsf < volBox and volEsf <= volCyl): 
			(
				obj = Sphere()
				changeSphere obj
				obj.segs = 30
			)
			default: 
			(
				obj = box()
				changeBox obj
			)
		)
		return obj
	)

)

--------------------------------------------------------------------------------
/**
*/
fn heightcapsule points vol =
(
	local h = vol.disXY
	local r = vol.disXY 
	local r2 = r * r
	for i in points do
	(
		local p = i * vol.mat
		local h2 = ( abs p.z ) + r -  ( sqrt ( r2 - p.x * p.x - p.y * p.y ) ) 
		if h < h2 then h = h2
	)
	
	return 2.0*h
)

--------------------------------------------------------------------------------
/**
	Create a volinfo for the best 
		- OBB
		- Sphere
		- Capsule
	return volinfo
*/

fn CreateVolFromPoints points minValue:0.5= (
	local minbox = copy points[1]
	local maxbox = copy points[1]
	local vol = volinfo()
	local mat
	local matchange = matrix3 1 -- matrix for change x < y < z
	local eigenVec
	local iter = 10 --number  or iterations minimun 2

	
	
	vol.rot = [0,0,0]
	
	
	vol.mat = matrix3 1
	
	-- original center is the average of points
	vol.pos = [0,0,0]
	for i in points do
	(
		vol.pos += i; 
	)
	vol.pos /= points.count
	
	
	
	for i= 1 to iter do
	(
		local bbox
		
		if ( i != 1 ) then -- the center is the center of bounding box
		(
			bbox = CalculateBBox points  (vol.mat)
			vol.pos = ((bbox[1] + bbox[2] ) / 2.0 ) * ( inverse vol.mat )
		)
				
		if ( i != iter) then -- if not the last iteration calculate the new orientation
		(
			mat  = covariaceMatrix points vol.pos 
			eigenVec = eigenVectorsSymMatrix mat
		)  else
		(
			--calculate matrix for boundign box maxX <= maxY <= maxZ	
			matchange = sortMatrix (bbox[2] - bbox[1]) 			
		)
		
		mat = eigenVec 
		mat = mat * matchange 
	
		-- calculate matrix world to local
		vol.mat = matrix3 [1,0,0] [0,1,0] [0,0,1] [0,0,0]
		translate vol.mat ( - vol.pos )
		vol.mat = vol.mat* mat
		
		-- calculate the rotation local to world
		vol.rot = ( (inverse mat) as eulerangles)
	
	)
	

	-- Calculate BBOX = 
	-- Sphere
	-- Cylinder ( height = bbox.z * 2) 
	
	local XYZ = [ minValue , minValue , minValue]
	local disXYZ = minValue
	local disXY = minValue
	local disYZ = minValue
	for i in points do
	(
	    local p = i * vol.mat
		local tmp = [ abs p.x , abs p.y , abs p.z ]
		if XYZ.x < tmp.x then XYZ.x = tmp.x
		if XYZ.y < tmp.y then XYZ.y = tmp.y
		if XYZ.z < tmp.z then XYZ.z = tmp.z
		
		tmp = length p
		if disXYZ < tmp then disXYZ = tmp
		
		tmp =  length [ p.x , p.y , 0.0 ]
		if disXY < tmp then disXY = tmp

		tmp =  length [ 0.0 , p.y , p.z ]
		if disYZ < tmp then disYZ = tmp
	)

	vol.XYZ = XYZ
	vol.disXYZ = disXYZ
	vol.disXY = disXY
	vol.disYZ = disYZ
	
	vol.heightcap = (heightcapsule points vol)

	vol.volBox =  vol.xyz.x*vol.xyz.y*vol.xyz.z * 8
	vol.volEsf =  4.0 * pi * (vol.disXYZ ^3 ) / 3.0
	vol.volCyl =  ( pi*(vol.disXY ^2) * ( vol.heightCap - (vol.disXY * 2.0) ) ) + ( 4.0 * pi * (vol.disXY ^3) / 3.0 )
	
	local heightCyl_1 = vol.xyz.z*2;
	local volCyl_1 = pi*(vol.disXY ^2) * heightCyl_1
	
	local heightCyl_2 = vol.xyz.x*2;
	local volCyl_2 = pi*(vol.disYZ ^2) * heightCyl_2

	if volCyl_1 < volCyl_2 then
	(
		vol.volRealCyl = volCyl_1
		vol.heightCyl = heightCyl_1
		vol.radiusCyl = vol.disXY
		vol.isCylinderInX = false
	)
	else
	(
		vol.volRealCyl = volCyl_2
		vol.heightCyl = heightCyl_2
		vol.radiusCyl = vol.disYZ
		vol.isCylinderInX = true
	)
	
	return vol
)

--------------------------------------------------------------------------------
/**
	Create a list of points  for ojects 
*/
fn CreatePoints objects  =
(
	local cl = classof objects
	if (cl != Array  and cl != ArrayParameter ) do objects= #(objects)

	local points = #()
	
	for i in objects do
	(
		local obj = snapshotAsMesh i
		--obj = copy i
		--converttomesh obj
		for v = 1 to obj.numVerts  do 
		(
			local tmp = getVert obj v
			append points (getVert obj v)
		)
		--delete obj
	)
	return points
)

--------------------------------------------------------------------------------
/**
	Create a list of list points  for ojects 
	// Descompose for obects in differents list
*/

fn createListPoints objects  =
(
	local listPoints = #()
	
	local cl = classof objects
	if (cl != Array  and cl!= ArrayParameter ) do objects= #(objects)
	
	for object in objects  where (canconvertto object Mesh ) do
	(
		local obj = copy object
		collapseStack obj
		convertToMesh obj
		
		while obj.numfaces != 0 do
		(
			local elem = meshop.getelementsusingface obj #{1}
			local tMesh = meshop.detachFaces obj elem delete:true asMesh:true
			update obj
			local points = #()
			
			for v = 1 to tMesh.numverts do
			(
				append points ((getVert tMesh v)*obj.transform)
			)
			if (points.count >= 3) do append listPoints points
		)	
		delete obj
	)
	return listPoints
)

--------------------------------------------------------------------------------
/**
*/
fn joinObjects objects=
(
	local all_objects = Editable_Mesh()
	for object in objects do
	(
		local temp = copy object
		temp.material = undefined
		if( undefined != (convertToMesh temp) )then
		(
			meshop.attach all_objects temp
		)
		else
		(
			delete temp
		)
	)
	return all_objects
)

--------------------------------------------------------------------------------
/**
*/
fn reducePoints2D points =
(
	fn compare2DXY v1 v2 =
	(
		if v1[1] < v2[1] do
		(
			return -1
		)
		if v1[1] > v2[1] do
		(
			return 1
		)
		return 0
	)
	
	-- reduce duplicates in XY
	qsort points compare2DXY
	
	local i = 1
	while i <= points.count do
	(
		local j = i+1
		while j <= points.count and not ( points[i][1] < points[j][1]) do
		(
			if points[i][2] == points[j][2] then
			(
				deleteItem points j
			)
			else
			(
				j += 1
			)
		)
		i += 1
	)

	-- Delaunay
)

--------------------------------------------------------------------------------
/**
*/
fn IsAncestorOf parent child =
(
	if parent == child do
	(
		return true
	)
	
	for node in parent.children do
	(
		if node == child do
		(
			return true
		)
		
		if IsAncestorOf node child  do
		(
			return true
		)
	)
	return false
)

--------------------------------------------------------------------------------
/**
*/
fn SelectParentNodes objs =
(
	local parents = #()
	if objs == undefined do
	(
		return parents
	)
	
	local objects = objs as Array
	
	for object in objects do
	(
		if( object.parent == undefined ) then
		(
			append parents object
		)
		else
		(
			local link_obj = object.parent
			local index = finditem objects link_obj
			
			if( index == 0 ) do
			(
				append parents object
			)
		)
	)
	
	return parents
)

--------------------------------------------------------------------------------
/**
*/
fn LinkWithFirst objs =
(
	if objs == undefined do
	(
		return undefined
	)
	
	local objects = objs as array
	if objects.count < 1 do
	(
		return undefined
	)

	local parent = objects[1]
	for i = 2 to objects.count do
	(
		if( objects[i].parent == undefined and 
			not (IsAncestorOf objects[i] parent) ) do
		(
			objects[i].parent = parent
		)
	)
	return parent
)

--------------------------------------------------------------------------------
/**
*/
fn isDigit car =
(
	local numbers = "0123456789"
	for i = 1 to numbers.count do
	(
		if car == numbers[i] do
		(
			return true
		)
	)
	
	return false
)
--------------------------------------------------------------------------------
--   EOF
--------------------------------------------------------------------------------
