/**
   @file math.ms
   @ingroup n3dsMaxScripts
   @author Cristobal Castillo
   @brief maths functions
   @remarks
   functions
   	- detmat, return a determinat matrix 3x3
	- eigenVectorsSymMatrix, eigen vectors of symetric matrix 
	- CalculateBBox
	- sortMatrix

   (C) 2006 Conjurer Services, S.A.
*/

--------------------------------------------------------------------------------
/**
	calculate determinat of matrix 3x3
*/

fn detmat  mat=
(
	kk = mat[1][1]*mat[2][2]* mat[3][3] + mat[1][2] *mat[2][3] * mat[3][1] + mat[2][1]* mat[3][2] * mat[1][3]
	kk = kk - ( mat[1][3] * mat[2][2] * mat[3][1] + mat[1][2] * mat [2][1] * mat[3][3] + mat[3][2] * mat[2][3] * mat[1][1])
	return kk;
)

--------------------------------------------------------------------------------
/**
	calculate determinat of matrix 3x3
	Eigen vectors of symetric matrix
	method: Jacobi
*/

fn eigenVectorsSymMatrix mat =
(
	local A = copy mat
	local rZ, rx , ry
	local Q1
	local Q2
	local eigenVec
	eigenVec = matrix3 1
		
	
	do
	(
		--Anulate the A[1][2] and a[2][1]
		rz = 0.5 * atan2 (2* A[1][2]) ( A[1][1] - A[2][2] ) -- Rotate angle in Z axis
		Q1 = matrix3 1
		Q2 = matrix3 1
		rotateZ Q1 (rz)
		rotateZ Q2 ( - rz )
		A =  Q1 * A * Q2
		eigenVec = eigenVec * Q2
	
	
		--Anulate the A[1][3] and a[3][1]
		ry = 0.5 * atan2 (2* A[1][3]) ( A[1][1] - A[3][3] )  -- rotate angle in Y axis
		Q1 = matrix3 1
		Q2 = matrix3 1
		rotateY Q1 ( - ry )
		rotateY Q2 ry
		A =  Q1 * A * Q2
		eigenVec = eigenVec * Q2

		--Anulate the A[2][3] and a[3][2]
		rx = 0.5 *atan2 (2* A[2][3]) ( A[2][2] - A[3][3] ) -- rotate angle in X axis
		Q1 = matrix3 1
		Q2 = matrix3 1
		rotateX Q1 rx
		rotateX Q2 ( - rx )
		A =  Q1 * A * Q2
		eigenVec = eigenVec * Q2
		
		) while (     ( 0.00001 < (abs  rx) )  or ( 0.00001 < (abs  ry) ) or ( 0.00001 < (abs  rz) ) )
	

	
	return eigenVec
	
)

--------------------------------------------------------------------------------
/**
	Return a covariance matrix respect to center
*/
fn covariaceMatrix points center =
(
	local mat = matrix3 0
	for j = 1 to 3 do
	(
		local row = [0,0,0]
		
		for k = 1 to 3 do
		(
			local val = 0
						
			for i in points do 
			(
				local p = i - center
				val = val + p[j]*p[k]		
			)
			val = val / points.count
			row[k] = val
		)
		
		if (j == 1) then mat.row1 = copy row
		if (j == 2) then mat.row2 = copy row
		if (j == 3) then mat.row3 = copy row
	)
	return mat
)


--------------------------------------------------------------------------------
/**
   points is array of points in world space
   mat transform world space to local space
   return minbox and maxbox in local space
   
   note: if mat is identity then minnbox and maxbox are in world space
*/
fn CalculateBBox points mat = 
(
	local minbox = points[1]*mat
	local maxbox = points[1]*mat
	
	for i in points do
	(
		p = i * mat
		if (maxbox.x < p.x) then maxbox.x = p.x
		if maxbox.y < p.y then 	maxbox.y = p.y
		if maxbox.z < p.z then maxbox.z = p.z

		if minbox.x > p.x then 	minbox.x = p.x
		if minbox.y > p.y then 	minbox.y = p.y
		if minbox.z > p.z then 	minbox.z = p.z
	)
	return #(minbox,maxbox)
)

--------------------------------------------------------------------------------
/**
	p is length of bbox
	return matrix
	preconditions
	l = p * mat
	|l | = |p|
    |l.x| <= |l.y| <= |l.z|
*/
fn sortMatrix p  = 
(
	local mat = matrix3 1
	
	if (p.x > p.y ) do ( swap p.x p.y ; swap mat.row1 mat.row2 )
	
	if (p.x > p.z ) do ( swap p.x p.z ; swap mat.row1 mat.row3 )
	
	if (p.y > p.z ) do ( swap p.y p.z ; swap mat.row2 mat.row3 )
	
	mat.row3 = mat.determinantsign * mat.row3 -- Positive axis change
	mat = inverse mat
	return mat
)

fn  triangleArea p0 p1 p2 =
(
	local e1 = p1 - p0
	local e2 = p2 - p0
	local c = length (cross e1 e2)
	return (c / 2.0)
)

fn triangleAreaMesh theMesh index =
(
	local Face= getFace theMesh index
	local vert2 = getVert theMesh Face.z 
	local vert1 = getVert theMesh Face.y 
	local vert0 = getVert theMesh Face.x
	return triangleArea vert0 vert1 vert2

)

--------------------------------------------------------------------------------
--   EOF
--------------------------------------------------------------------------------