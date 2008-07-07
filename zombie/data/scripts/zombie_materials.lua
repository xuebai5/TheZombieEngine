--------------------------------------------------------------------------------
--  data/scripts/conjurer_materials.lua
--
--  Callback functions for nD3D9MaterialBuilder
--  All material levels are implemented.
--
--  (C) 2004 Conjurer Services, S.A.
--------------------------------------------------------------------------------

function OnMaterialStartup()

    pushcwd('.')

    new('nd3d9materialbuilder','/usr/material/builder')
        sel('/usr/material/builder')
        call('setonbuildshaderlevels', 'BuildShaderLevels')
        call('setonbuildshaderpasses', 'BuildShaderPasses')
        call('setonbuildshadertree', 'BuildShaderTree')
        call('setonbuildshadername', 'BuildShaderName')
        call('setonbuildshader', 'BuildShader')

    popcwd()

    nebula.sys.servers.material:setbuilder('/usr/material/builder')

    -- set render priority for light types
    nebula.sys.servers.scene:setprioritybyselectortype( 'lnrf', 0 )
    nebula.sys.servers.scene:setprioritybyselectortype( 'lyrf', 0 )
    nebula.sys.servers.scene:setprioritybyselectortype( 'lmap', 1 )
    nebula.sys.servers.scene:setprioritybyselectortype( 'spot', 2 )
    nebula.sys.servers.scene:setprioritybyselectortype( 'pntl', 3 )

end


--------------------------------------------------------------------------------
--  BuildShaderLevels
--------------------------------------------------------------------------------

function BuildShaderLevels()

    -- get current builder and material
    local builder = lookup(nebula.sys.servers.material:getbuilder())
    local material = lookup(builder:getmaterial())

    local numLevels = 1

    builder:setnumlevels(numLevels)

end

--------------------------------------------------------------------------------
--  BuildShaderPasses
--------------------------------------------------------------------------------

function BuildShaderPasses()

    -- get current builder, material and scene pass
    local builder = lookup(nebula.sys.servers.material:getbuilder())
    local material = lookup(builder:getmaterial())

    local hasAlpha = material:hasparam('hasalpha', 'true')

    local isSky = material:hasparam('pro', 'sky')

    -- material specific for shadow volumes (single pass)
    if (material:hasparam('mat', 'shadowvolume')) then
        builder:addpass('shdv')
        return
    end

    if (hasAlpha) then
        builder:addpass('alph')

    elseif (material:hasparam('deform', 'instanced')) then
        builder:addpass('inst')

    elseif (material:hasparam('mat', 'singlecolorpass')) then
        builder:addpass('inst')

    elseif (material:hasparam('map', 'clipmap')) then
        builder:addpass('inst')

    elseif (isSky) then
        builder:addpass('sky0')

    else
        builder:addpass('dept')
        builder:addpass('colr')
    end

    if (material:hasparam('map', 'illuminationmap') or
        material:hasparam('env', 'illuminationcolor')) then
        builder:addpass('emis')
    end

end

--------------------------------------------------------------------------------
--  BuildShaderTree
--------------------------------------------------------------------------------

function BuildShaderTree()

    -- get current builder, material and scene pass
    local builder = lookup(nebula.sys.servers.material:getbuilder())
    local material = lookup(builder:getmaterial())
    local pass = builder:getpass()

    builder:addattrib('dummy', 'true') --material builder complaints with 0 attribs

end

--------------------------------------------------------------------------------
--  @func BuildShaderName
--
--  FIXME Until we have a true namespace for shaders, we'll just build a
--  unique character sequence, where each attribute and case in the builder
--  is mapped into a slot of the character sequence if the attribute is on,
--  or a 0 if the attribute is off. The following list shows all attributes
--  so far, with their #slot, and the slot value for their attribute value(s).
--------------------------------------------------------------------------------

function BuildShaderName()

    -- get current builder and material
    local builder = lookup(nebula.sys.servers.material:getbuilder())
    local material = lookup(builder:getmaterial())
    local pass = builder:getpass()

    -- shadow volumes
    if (builder:getpass() == 'shdv') then

        builder:setshaderfile('shaders:zombie/shadowvolumes.fx')
        if (material:hasparam( 'deform', 'skinned' ) ) then
            builder:settechnique('tShadowSkinned')
        else
            builder:settechnique('tShadowStatic')
        end

        builder:setbuildshader(false)
        return
    end

    -- depth pass
    if ( builder:getpass() == 'dept' ) then

        builder:setshaderfile( 'shaders:zombie/depth.fx' )
        if (material:hasparam( 'deform', 'skinned' )) then
            builder:settechnique('tDepthSkinned')
        else
            builder:settechnique('tDepthStatic')
        end

    elseif ( material:hasparam( 'map', 'clipmap' ) ) then

        builder:setshaderfile( 'shaders:zombie/alphatest.fx' )
        if ( material:hasparam( 'deform', 'instanced' ) ) then
            builder:settechnique( 'tInstancedAlphaTest' )
        elseif ( material:hasparam( 'deform', 'swinging' ) ) then
            builder:settechnique( 'tSwingAlphaTest' )
        else
            builder:settechnique( 'tDefaultAlphaTest' )
        end

    elseif ( material:hasparam( 'deform', 'instanced' ) ) then

        builder:setshaderfile( 'shaders:zombie/instanced.fx' )
        builder:settechnique( 'tDefaultInstanced' )

    elseif ( material:hasparam( 'map', 'colormap' ) ) then

        if ( material:hasparam( 'env', 'diffuselighting' ) ) then

            if ( material:hasparam( 'map', 'bumpmap' ) ) then
                builder:setshaderfile( 'shaders:zombie/bump.fx' )
                if ( material:hasparam( 'deform', 'skinned' ) ) then
                    builder:settechnique('tBumpSkinned')
                else
                    builder:settechnique('tBumpStatic')
                end
            else
                builder:setshaderfile( 'shaders:zombie/phong.fx' )
                if (material:hasparam( 'deform', 'skinned' )) then
                    builder:settechnique('tPhongSkinned')
                else
                    builder:settechnique('tPhongStatic')
                end
            end
        else
            builder:setshaderfile( 'shaders:zombie/color.fx' )
            if (material:hasparam( 'deform', 'skinned' )) then
                builder:settechnique('tColorSkinned')
            else
                builder:settechnique('tColorStatic')
            end
        end

    -- default shader for everything else
    else
        builder:setshaderfile( 'shaders:zombie/default.fx' )
        if ( material:hasparam( 'deform', 'skinned' ) ) then
            builder:settechnique( 'tDefaultSkinned' )
        else
            builder:settechnique( 'tDefaultStatic' )
        end
    end

    builder:setbuildshader(false)

    -- set shader sequence
    if ( material:hasparam( 'map', 'clipmap' ) ) then
        if ( material:hasparam('pro' , 'notculling') and not ( material:hasparam( 'alphatwoside', 'true' ) ) ) then
            if (builder:getcasevalue('fogmode') == 'lnrf') then
                builder:setsequence('alphatestnoculllinearfog')
            else
                builder:setsequence('alphatestnocull')
            end
        else
            if (builder:getcasevalue('fogmode') == 'lnrf') then
                builder:setsequence('alphatestlinearfog')
            else
                builder:setsequence('alphatest')
            end
        end

    elseif ( builder:getcasevalue('fogmode') == 'lnrf') then
        if ( material:hasparam( 'pro', 'depthbias' ) ) then
            builder:setsequence('depthbiaslinearfog')
        else
            builder:setsequence('linearfog')
        end

    elseif ( material:hasparam( 'pro', 'depthbias' ) ) then
        builder:setsequence('depthbias')
    end

end

--------------------------------------------------------------------------------
--  BuildShader
--------------------------------------------------------------------------------

function BuildShader()

end
