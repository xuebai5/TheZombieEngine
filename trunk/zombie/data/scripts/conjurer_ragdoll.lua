
-- setup physics material
function SetupPhysicsMaterial()

    mat1 = selNew( 'nphymaterial', '/usr/physics/mat1' )
    
    mat1:setnormalfriction(1)
    
    mat1:setsoftnesserp(1.0)
    mat1:setsoftnesscfm(0.0)
    
    mat1:setslip(0)
    
    mat1:setbounce(0.0)
    
    mat1:update()
end

-- create stairs
function makestep(name)

    geometry = selNew( 'nphygeombox', '/usr/physics/world/box_' .. name )
    geometry:setlengths( 100,2,3 )
    
    object = selNew( 'nphysimpleobj', '/usr/physics/world/obj_' .. name )
    object:setgeometry( geometry )
    object:setmaterial( mat1 )
    
    world:addobject( object )
    
    return object
    
end

-- setup physics world
function SetupPhysicsWorld()

    SetupPhysicsMaterial()
    
    world = selNew( 'nphysicsworld', '/usr/physics/world' )
    world:setgravity( 0,0,0 )
    
    worldspace = selNew( 'nphygeomhashspace', '/usr/physics/world/space' )
    
    world:setspace( worldspace )
    
    --FIXME for i in range(10) do
    --i = 0
        --step = makestep( 'step' .. tostring(i) )
        --step:setposition( 0, -2*i, 3*i-3 )
    --end

    -- Create Stupid Box
    bgeom = selNew( 'nphygeombox', '/usr/physics/world/bgeom' )
    bobj = selNew( 'nphysimpleobj', '/usr/physics/world/bobj' )
    bobj:setgeometry( bgeom )
    bobj:setmaterial(mat1)
    bobj:setmass(1)
    bobj:setposition( 0, 0, -20 )
    
    world:addobject( bobj )

    -- Create sea
    objsea = selNew( 'nphysea', '/usr/physics/world/osea' )
    
    objsea:setlevel(-32)
    objsea:setdensity(1.5)

    world:addobject(objsea)
    
end

-- create a ragdoll instance
function makeragdoll(name, x, y, z)
   
    -- Create body geometry for a rag doll ---
    bodygeometry = selNew( 'nphygeombox', '/usr/physics/world/bodygeometry' .. name )
        
    bodygeometry:setposition( 0, 0, 0 )
    
    bodygeometry:setlengths( 2, 2, 0.4 )
    
    bodygeometry2 = selNew( 'nphygeombox', '/usr/physics/world/bodygeometry2' .. name )
    
    bodygeometry2:setposition( 0, 0, 0.1 )
    
    bodygeometry2:setlengths( 1, 1, 0.4 )
    
    --- Create Head for Rag Doll ---
    
    headgeometry = selNew( 'nphygeomsphere', '/usr/physics/world/headgeometry' .. name )
    
    headgeometry:setradius(0.5)
    
    head = selNew( 'nphyragdolllimb', '/usr/physics/world/head' .. name )
    
    head:addgeometry( headgeometry )
    
    head:setmass(2)
    head:setdensity(0.5)
    
    head:setmaterial( mat1 );
    
    head:setposition(0,1+0.2+0.5,0)
    
    --- Create Left Arm for Rag Doll ---
    
    leftarmgeometry = selNew( 'nphygeomcylinder', '/usr/physics/world/leftarmgeometry' .. name )
    
    leftarmgeometry:setlength( 1.5 )
    leftarmgeometry:setradius(0.25)
    leftarmgeometry:setorientation( 3.14/2, 0, 0 )
    
    leftarm = selNew( 'nphyragdolllimb', '/usr/physics/world/leftarm' .. name )
    
    leftarm:addgeometry( leftarmgeometry )
    
    leftarm:setmass(4)
    
    leftarm:setmaterial( mat1 )
    
    leftarm:setposition(1.5,0.25,0)
    
    --- Create Left Fore Arm for Rag Doll ---
    
    leftforearmgeometry = selNew( 'nphygeomcylinder', '/usr/physics/world/leftforearmgeometry' .. name )
    
    leftforearmgeometry:setlength( 1.5 )
    leftforearmgeometry:setradius(0.25)
    leftforearmgeometry:setorientation( 3.14/2,0,0 )
    
    leftforearm = selNew( 'nphyragdolllimb', '/usr/physics/world/leftforearm' .. name )
    
    leftforearm:addgeometry( leftforearmgeometry )
    
    leftforearm:setmass(4)
    leftforearm:setdensity(1.5)
    
    leftforearm:setmaterial( mat1 );
    
    leftforearm:setposition(1.5,-1.4,0)
    
    --- Create Right Arm for Rag Doll ---
    
    rightarmgeometry = selNew( 'nphygeomcylinder', '/usr/physics/world/rightarmgeometry' .. name )
    
    rightarmgeometry:setlength( 1.5 )
    rightarmgeometry:setradius(0.25)
    rightarmgeometry:setorientation( 3.14/2,0,0 )
    
    rightarm = selNew( 'nphyragdolllimb', '/usr/physics/world/righttarm' .. name )
    
    rightarm:addgeometry( rightarmgeometry )
    
    rightarm:setmass(4)
    
    rightarm:setmaterial( mat1 )
    
    rightarm:setposition(-1.5,0.25,0)
    
    --- Create Right Fore Arm for Rag Doll ---
    
    rightforearmgeometry = selNew( 'nphygeomcylinder', '/usr/physics/world/rightforearmgeometry' .. name )
    
    rightforearmgeometry:setlength( 1.5 )
    rightforearmgeometry:setradius(0.25)
    rightforearmgeometry:setorientation( 3.14/2,0,0 )
    
    rightforearm = selNew( 'nphyragdolllimb', '/usr/physics/world/rightforearm' .. name )
    
    rightforearm:addgeometry( rightforearmgeometry )
    
    rightforearm:setmass(4)
    rightforearm:setdensity(1.5)
    
    rightforearm:setmaterial( mat1 );
    
    rightforearm:setposition(-1.5,-1.4,0)
    
    --- Create Left Leg for Rag Doll ---
    
    leftleggeometry = selNew( 'nphygeomcylinder', '/usr/physics/world/leftleggeometry' .. name )
    
    leftleggeometry:setlength( 1.5 )
    leftleggeometry:setradius(0.25)
    leftleggeometry:setorientation( 3.14/2,0,0 )
    
    leftleg = selNew( 'nphyragdolllimb', '/usr/physics/world/leftleg' .. name )
    
    leftleg:addgeometry( leftleggeometry )
    
    leftleg:setmass(4)
    
    leftleg:setmaterial( mat1 )
    
    leftleg:setposition(-0.75,-1-0.2-0.75,0)
    
    --- BEGIN: Create Left Fore Leg for Rag Doll ---
    
    leftforeleggeometry = selNew( 'nphygeomcylinder', '/usr/physics/world/leftforeleggeometry' .. name )
    
    leftforeleggeometry:setlength( 1.5 )
    leftforeleggeometry:setradius(0.25)
    leftforeleggeometry:setorientation( 3.14/2,0,0 )
    
    leftforeleg = selNew( 'nphyragdolllimb', '/usr/physics/world/leftforeleg' .. name )
    
    leftforeleg:addgeometry( leftforeleggeometry )
    
    leftforeleg:setmass(4)
    leftforeleg:setdensity(1.5)

    leftforeleg:setmaterial( mat1 )
    
    leftforeleg:setposition(-0.75,-1-1.75-0.2-0.2-0.75,0)
    
    --- Create Righ Leg for Rag Doll ---
    
    rightleggeometry = selNew( 'nphygeomcylinder', '/usr/physics/world/rightleggeometry' .. name )
    
    rightleggeometry:setlength( 1.5 )
    rightleggeometry:setradius(0.25)
    rightleggeometry:setorientation( 3.14/2,0,0 )
    
    rightleg = selNew( 'nphyragdolllimb', '/usr/physics/world/rightleg' .. name )
    
    rightleg:addgeometry( rightleggeometry )
    
    rightleg:setmass(4)
    
    rightleg:setmaterial( mat1 )
    
    rightleg:setposition(0.75,-1-0.2-0.75,0)
    
    --- Create Left Fore Leg for Rag Doll ---
    
    rightforeleggeometry = selNew( 'nphygeomcylinder', '/usr/physics/world/rightforeleggeometry' .. name )
    
    rightforeleggeometry:setlength( 1.5 )
    rightforeleggeometry:setradius(0.25)
    rightforeleggeometry:setorientation( 3.14/2,0,0 )
    
    rightforeleg = selNew( 'nphyragdolllimb', '/usr/physics/world/rightforeleg' .. name )
    
    rightforeleg:addgeometry( rightforeleggeometry )
    
    rightforeleg:setmass(4)
    rightforeleg:setdensity(1.5)
    
    rightforeleg:setmaterial( mat1 )
    
    rightforeleg:setposition(0.75,-1-1.75-0.2-0.2-0.75,0)
    
    --- Create Rag Doll ---
    
    ragdoll = selNew( 'nphyhumragdoll', '/usr/physics/world/ragdoll' .. name )
    
    -- Setting body geometry
    ragdoll:addgeometry( bodygeometry )
    ragdoll:addgeometry( bodygeometry2 )
    
    -- Setting mass body
    ragdoll:setmass(40)
    
    -- Setting the rag-doll head
    ragdoll:sethead( head )
    ragdoll:setneckpoint( 0, 1+ 0.1,0 )
    
    -- Setting the rag-doll left arm
    ragdoll:setarm( 0, leftarm )
    ragdoll:setshoulderpoint( 0, 1.125, 0.75 ,0 )
    
    -- Setting the rag-doll left fore arm
    ragdoll:setforearm( 0, leftforearm )
    ragdoll:setelbowpoint( 0, 1.125, 0.25-0.2-0.5 ,0 )
    
    -- Setting the rag-doll right arm
    ragdoll:setarm( 1, rightarm )
    ragdoll:setshoulderpoint( 1, -1.125, 0.75 ,0 )
    
    -- Setting the rag-doll right fore arm
    ragdoll:setforearm( 1, rightforearm )
    ragdoll:setelbowpoint( 1, -1.125, 0.25-0.2-0.5 ,0 )
    
    -- Setting the rag-doll left leg
    ragdoll:setleg( 0, leftleg )
    ragdoll:setwristpoint( 0, -0.75,-1-0.1,0 )
    
    -- Setting the rag-doll left fore leg
    ragdoll:setforeleg( 0, leftforeleg )
    ragdoll:setkneepoint( 0, -0.75, -1-1.5-0.2-0.1,0 )
    
    -- Setting the rag-doll right leg
    ragdoll:setleg( 1, rightleg )
    ragdoll:setwristpoint( 1, 0.75,-1-0.1,0 )
    
    -- Setting the rag-doll right fore leg
    ragdoll:setforeleg( 1, rightforeleg )
    ragdoll:setkneepoint( 1, 0.75, -1-1.5-0.2-0.1,0 )
    
    -- Setting material
    ragdoll:setmaterial( mat1 )
    
    -- Adding the ragdoll to the world
    world:addobject( ragdoll )
    
    ragdoll:setposition( x, y, 0 )
    
end

-- TEMP
function setgravity(enabled)

    if (enabled) then
        world:setgravity(0, -1, 0)
    else
        world:setgravity(0, 0, 0)
    end
end

function placejoints()

    animator = nebula.usr.animators.anims.skinragdollanimator
    
    q0, q1, q2, q3 = animator:getcharjointrotation( 0 )
    r0, r1, r2, r3 = animator:getcharjointrotation( 1 )
    s0, s1, s2, s3 = animator:getcharjointrotation( 2 )
      
--    -- apply local coordinates
    animator:multiplyby( 0, -1*q0, -1*q1, -1*q2, q3)
    
    animator:multiplyby( 1, -1*r0, -1*r1, -1*r2, r3)
    animator:multiplyby( 2, -1*s0, -1*s1, -1*s2, s3)
--    animator:multiplyby( 1, -1*q0, -1*q1, -1*q2, q3)
--    animator:multiplyby( 2, -1*q0, -1*q1, -1*q2, q3)
--
--    -- initial state transformations
    animator:multiplyby( 1, 0, 0, -0.707, 0.707)
    animator:multiplyby( 2, 0, 0, -0.707, 0.707)
   
--    
    animator:multiplyby( 0, 0.707, 0, 0, 0.707)
    animator:multiplyby( 0, 0, 0, 0.707, 0.707)
--
--    -- apply local coordinates
    animator:multiplyby( 0, q0, q1, q2, q3)
--    animator:multiplyby( 1, q0, q1, q2, q3)
--    animator:multiplyby( 2, q0, q1, q2, q3)
    animator:multiplyby( 1, r0, r1, r2, r3)
    animator:multiplyby( 2, s0, s1, s2, s3)

end

--    -- ragdoll center good movement
--    animator:multiplyby( 0, -1*q0, -1*q1, -1*q2, q3)
--    animator:multiplyby( 0, 0.707, 0, 0, 0.707)
--    animator:multiplyby( 0, 0, 0, 0.707, 0.707)
--    animator:multiplyby( 0, q0, q1, q2, q3)