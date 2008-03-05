--------------------------------------------------------------------------------
--  data/scripts/conjurer_materials.lua
--
--  Callback functions for nD3D9MaterialBuilder
--  All material levels are implemented.
--
--  (C) 2004 Tragnarion Studios
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
-- const definitions
--------------------------------------------------------------------------------

local LEVEL_PRO     = 0
local LEVEL_PIXEL   = 1
local LEVEL_FLAT    = 2
local LEVEL_UNLIT   = 3
local LEVEL_EDITOR  = 4

--------------------------------------------------------------------------------
--  BuildShaderLevels
--------------------------------------------------------------------------------

function BuildShaderLevels()

    -- get current builder and material
    local builder = lookup(nebula.sys.servers.material:getbuilder())
    local material = lookup(builder:getmaterial())

    local numLevels = 5

    -- material specific for shadow volumes (single level)
    if (material:hasparam('mat', 'shadowvolume')) then
        numLevels = 1
    end

    builder:setnumlevels(numLevels)

end

--------------------------------------------------------------------------------
--  BuildShaderPasses
--------------------------------------------------------------------------------

function BuildShaderPasses()

    -- get current builder, material and scene pass
    local builder = lookup(nebula.sys.servers.material:getbuilder())
    local material = lookup(builder:getmaterial())
    local hasAlpha = material:hasparam('hasalpha', 'true') and
                     (builder:getlevel() < LEVEL_EDITOR)

    if (material:hasparam('map', 'clipmap') and material:hasparam('pro', 'fadeclipmap') and
        (builder:getlevel() < LEVEL_EDITOR)) then
        hasAlpha = true
    end

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
        builder:addpass('sdep')

    elseif (material:hasparam('mat', 'singlecolorpass')) then
        builder:addpass('inst')

    elseif (isSky) then
        builder:addpass('sky0')

    else
        builder:addpass('dept')
        builder:addpass('colr')
        builder:addpass('sdep')
    end

    if (material:hasparam('map', 'illuminationmap') or
        material:hasparam('env', 'illuminationcolor')) then
        builder:addpass('emis')
    end
    
    if (builder:getlevel() < LEVEL_UNLIT) then
        builder:addpass('lase')
    end
    
    -- normal map generation (EDITOR profile only)
    if (builder:getlevel() == LEVEL_EDITOR) then
        if (material:hasparam('deform', 'instanced') and material:hasparam('env', 'diffuselighting')) then
            builder:addpass('norm')
        end
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

    -- geometry transformations
    if (material:hasparam('deform', 'billboard')) then
        builder:addattrib('billboard', 'true')
    end

    if (material:hasparam('deform', 'swinging')) then
        builder:addattrib('swinging', 'true')
    end

    if (material:hasparam('deform', 'skinned')) then
        builder:addattrib('skinned', 'true')
    end

    if (material:hasparam('deform', 'instanced')) then
        builder:addattrib('instanced', 'true')
    end
    
    if (material:hasparam('mat', 'singlecolorpass')) then
        builder:addattrib('singlecolorpass', 'true')
    end

    if (material:hasparam('deform', 'wave')) then
        builder:addattrib('wave', 'true')
    end

    if (material:hasparam('pro' , 'notculling')) then
        builder:addattrib('notculling', 'true')
    end

    if (material:hasparam('pro', 'depthbias')) then
        builder:addattrib('depthbias', 'true')
    end

    -- shadow volumes
    if (pass == 'shdv') then
        builder:addattrib('shadowvolume', 'true')
    end

    -- skydome
    if (pass == 'sky0') then
        if (material:hasparam('pro', 'sky')) then
            builder:addattrib('sky0', 'true')
        end
    end
    
    -- attributes for depth/instanced pass
    if (pass == 'dept' or pass == 'inst' or pass == 'norm') then
        builder:addattrib('alphablend', 'false')
        if (material:hasparam('map', 'clipmap')) then
            builder:addattrib('clipmap', 'true')
            if (material:hasparam('pro' , 'grassfadeout')) then
                builder:addattrib('grassfadeout', 'true')
            end
        end
    end
    
    -- atributes for receiver laser
    if (pass == 'lase') then
        builder:addattrib('alphablend', 'false')
        builder:addattrib('laserreceiver', 'true')
        if (material:hasparam('map', 'clipmap')) then
            builder:addattrib('clipmap', 'true')
            if (material:hasparam('pro' , 'grassfadeout')) then
                builder:addattrib('grassfadeout', 'true')
            end
        end
    end

    -- shadow depth
    if (pass == 'sdep') then
        builder:addattrib('writedepth', 'True')
        builder:addattrib('depthtest', 'True')
        builder:addattrib('depthfunc', 'LessEqual')
        builder:addattrib('alphablend', 'false')
        builder:addattrib('shadowdepth', 'true')
        if (material:hasparam('map', 'clipmap')) then
            builder:addattrib('clipmap', 'true')
        end
    end

    -- parallax mapping
    if ((builder:getlevel() < LEVEL_FLAT) and (pass ~= 'dept') or material:hasparam('map', 'clipmap')) then
        if (material:hasparam('map', 'parallax')) then builder:addattrib('parallax', 'true') end
    end

    -- attribute for normal buffer generation
    if (pass == 'norm') then
        builder:addattrib('alphablend', 'false')
        builder:addattrib('rendernormal', 'true')
    end

    -- attributes for solid, alpha-blended and emissive passes
    if ((pass == 'colr') or (pass == 'alph') or (pass == 'emis') or (pass == 'inst') or
        (pass == 'sky0')) then

        if (pass == 'alph') then
            builder:addattrib('alphablend', 'true')

            if (material:hasparam('map', 'clipmap')) then
                builder:addattrib('clipmap', 'true')
            end

            if (material:hasparam('alphatwoside', 'true')) then
                builder:addattrib('alphatwoside', 'true')
            end
            
            -- per pixel fresnel term
            if (material:hasparam('pro', 'fresnel')) then
                builder:addattrib('fresnel', 'true')
            end
            
            if (material:hasparam('pro', 'decalmodulation')) then
                builder:addattrib('decalmodulation', 'true')
            end
            
        end

        -- TODO: instanced shapes dont support alpha
        if (pass == 'colr' or pass == 'inst' or pass == 'sky0') then
            builder:addattrib('alphablend', 'false')

        else -- pass == 'alph'
            builder:addattrib('alphablend', 'true')
        end

        -- texture layers
        if (material:hasparam('map', 'colormap')) then builder:addattrib('colormap', 'true') end

        if (builder:getlevel() < LEVEL_EDITOR) then

            if (pass == 'colr' or pass == 'alph' or pass == 'inst' or pass == 'sky0') then
                if (material:hasparam('map', 'lightmap')) then builder:addattrib('lightmap', 'true') end
                if (material:hasparam('map', 'controlmap')) then builder:addattrib('controlmap', 'true') end
                if (material:hasparam('env', 'vcolorblend')) then builder:addattrib('vcolorblend', 'true') end
                if (material:hasparam('map', 'terrainlightmap')) then builder:addattrib('terrainlightmap', 'true') end
                if (material:hasparam('env', 'useterraincolor')) then builder:addattrib('useterraincolor', 'true') end
            end

            if (material:hasparam('map', 'illuminationmap')) then builder:addattrib('illuminationmap', 'true') end
            if (material:hasparam('env', 'illuminationcolor')) then builder:addattrib('illuminationcolor', 'true') end
        else
            -- distinguish editor shaders
            builder:addattrib('level', 'editor')
        end

        -- lighting and shading
        if (builder:getlevel() < LEVEL_UNLIT) then

            -- environment mapping
            if (material:hasparam('map', 'environmentmap2d')) then builder:addattrib('environmentmap2d', 'true') end
            if (material:hasparam('map', 'environmentmap3d')) then builder:addattrib('environmentmap3d', 'true') end

            if (material:hasparam('map', 'levelmapasenvmaskmap') and
               (material:hasparam('map', 'environmentmap2d') or material:hasparam('map', 'environmentmap3d'))) then
                builder:addattrib('levelmapasenvmaskmap', 'true')
            end

            if ((pass == 'colr') or (pass == 'alph') or (pass == 'inst') or (pass == 'sky0')) then
                if (material:hasparam('env', 'ambientlighting')) then builder:addattrib('ambientlighting', 'true') end
                if (material:hasparam('env', 'diffuselighting')) then builder:addattrib('diffuselighting', 'true') end
                if (material:hasparam('env', 'specularlighting')) then builder:addattrib('specularlighting', 'true') end
                if (material:hasparam('map', 'levelmap')) then builder:addattrib('levelmap', 'true') end
                if (material:hasparam('env', 'vcolormatambient')) then builder:addattrib('vcolormatambient', 'true') end

                -- fog and light
                local fogdisable = material:hasparam('env', 'fogdisable')
                local diffuselighting = material:hasparam('env', 'diffuselighting')
                local specularlighting = material:hasparam('env', 'specularlighting')
                local ambientlighting = material:hasparam('env', 'ambientlighting')
                local lightmaplighting = (ambientlighting or diffuselighting or specularlighting) and 
                                         not material:hasparam('map', 'lightmap')

                -- begin fog case
                if (not fogdisable) then
                    builder:begincasevar('fogmode')
                    builder:addcasevalue('lnrf')
                    --builder:addcasevalue('lyrf')
                end

                -- begin lightmap case
                if (lightmaplighting) then
                    builder:begincasevar('LightEnv')
                    builder:addcasevalue('lmap')
                end

                -- light cases
                if (diffuselighting or specularlighting or ambientlighting) then
                    builder:begincasevar('LightType')
                    builder:addcasevalue('spot')

                    -- prevent second point light used with lightmaps
                    -- The inddoors need a spot with one omnilight
                    if (not material:hasparam('env', 'useterraincolor')) then
                        builder:begincasevar('LightType')
                        builder:addcasevalue('pntl')
                            builder:begincasevar('LightType')
                            builder:addcasevalue('pntl')
                            builder:endcasevar()
                        builder:endcasevar()
                    end

                    builder:endcasevar()
    
                    builder:begincasevar('LightType')
                    builder:addcasevalue('pntl')

                    -- prevent second point light used with lightmaps
                    if (not material:hasparam('env', 'useterraincolor')) then
                        builder:begincasevar('LightType')
                        builder:addcasevalue('pntl')
                        builder:endcasevar()
                    end

--                    builder:addcasevalue('spot') --TODO: rewrite as a case of point
                    --TODO add second light
                    builder:endcasevar()
                end
                
                -- end lightmap case
                if (lightmaplighting) then
                    builder:endcasevar()
                end

                -- end fog case
                if (not fogdisable) then
                    builder:endcasevar()
                end
            end -- (pass == 'colr') or (pass == 'alph') or (pass == 'inst') or (pass == 'sky0')
        end

        -- per pixel surface maps
        if (builder:getlevel() < LEVEL_FLAT) then
            if ((pass == 'colr') or (pass == 'alph') or (pass == 'inst') or (pass == 'sky0') or 
                (pass == 'norm')) then
                if (material:hasparam('map', 'bumpmap')) then builder:addattrib('bumpmap', 'true') end
                if (material:hasparam('map', 'normalmap')) then builder:addattrib('normalmap', 'true') end
            end
        end

        -- per-pixel normal
        if (builder:getlevel() < LEVEL_PIXEL) then
            if ((pass == 'colr') or (pass == 'alph') or (pass == 'sky0')) then
                if (material:hasparam('map', 'bumpmap') or material:hasparam('map', 'normalmap') or
                    material:hasparam('map', 'environmentmap3d')) then
                    builder:addattrib('level', 'pro')
                end
            end
        end

    end -- (pass == 'colr') or (pass == 'alph') or (pass == 'emis') or (pass == 'inst') or (pass == 'norm')
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

    -- add an attribute
    local function GetSlot(builder, attrib, value, slot)

        if (builder:getattrib(attrib) == value) then
            return slot
        else
            return ''
        end
    end

    -- add a case value
    local function GetCaseSlot(builder, case, value, slot)

        if (builder:getcasevalue(case) == value) then
            return slot
        else
            return ''
        end
    end

    -- get current builder and material
    local builder = lookup(nebula.sys.servers.material:getbuilder())

    -- build shader name as 'wc:libs/shaders/' [name]_[qualifiers].fx
    local shaderName = ''

    if (builder:getattrib('shadowvolume') == 'true') then
        
        builder:setshaderfile('shaders:trag_shadowvolumes.fx')
        if (builder:getattrib('skinned') == 'true') then
            builder:settechnique('tShadowSkinned')
        else
            builder:settechnique('tShadowStatic')
        end
        
        builder:setbuildshader(false)
        return
    end


    shaderName = shaderName .. GetSlot(builder, 'alphablend',       'true', 'a')
    shaderName = shaderName .. GetSlot(builder, 'alphatwoside',     'true', 'b')
    shaderName = shaderName .. GetSlot(builder, 'billboard',        'true', 'c')
    shaderName = shaderName .. GetSlot(builder, 'bumpmap',          'true', 'd')
    shaderName = shaderName .. GetSlot(builder, 'clipmap',          'true', 'e')
    shaderName = shaderName .. GetSlot(builder, 'colormap',         'true', 'f')
    shaderName = shaderName .. GetSlot(builder, 'controlmap',       'true', 'g')

    shaderName = shaderName .. GetSlot(builder, 'depthtest',        'True', 'h')
    shaderName = shaderName .. GetSlot(builder, 'writedepth',       'True', 'i')
    shaderName = shaderName .. GetSlot(builder, 'depthfunc',        'Equal', 'j') -- 'LessEqual'

    shaderName = shaderName .. GetSlot(builder, 'ambientlighting',  'true', 'k')
    shaderName = shaderName .. GetSlot(builder, 'diffuselighting',  'true', 'l')
    shaderName = shaderName .. GetSlot(builder, 'specularlighting', 'true', 'm')

    shaderName = shaderName .. GetSlot(builder, 'environmentmap2d', 'true', 'n')
    shaderName = shaderName .. GetSlot(builder, 'fadebumpmap',      'true', 'o')
    shaderName = shaderName .. GetSlot(builder, 'fadeclipmap',      'true', 'p')
    shaderName = shaderName .. GetSlot(builder, 'illuminationcolor','true', 'q')
    shaderName = shaderName .. GetSlot(builder, 'illuminationmap',  'true', 'r')
    shaderName = shaderName .. GetSlot(builder, 'levelmap',         'true', 's')
    shaderName = shaderName .. GetSlot(builder, 'lightmap',         'true', 't')
    shaderName = shaderName .. GetSlot(builder, 'morpher',          'true', 'u')
    shaderName = shaderName .. GetSlot(builder, 'normalmap',        'true', 'v')
    shaderName = shaderName .. GetSlot(builder, 'shadowmap',        'true', 'w')
    shaderName = shaderName .. GetSlot(builder, 'skinned',          'true', 'x')
    shaderName = shaderName .. GetSlot(builder, 'swinging',         'true', 'z')

--  shaderName = shaderName .. GetSlot(builder, 'fadeclipmap',      'true', '0')
--  shaderName = shaderName .. GetSlot(builder, 'fadebumpmap',      'true', '1')

    shaderName = shaderName .. GetSlot(builder, 'environmentmap3d', 'true', '3')
--  shaderName = shaderName .. GetSlot(builder, 'notculling',       'true', '4')
    shaderName = shaderName .. GetSlot(builder, 'parallax',         'true', '5')
    shaderName = shaderName .. GetSlot(builder, 'shadowvolume',     'true', '6')
    shaderName = shaderName .. GetSlot(builder, 'shadowdepth',      'true', '7')
    shaderName = shaderName .. GetSlot(builder, 'levelmapasenvmaskmap', 'true', '8')

    shaderName = shaderName .. GetSlot(builder, 'instanced',        'true', '9')

    shaderName = shaderName .. "-"
    shaderName = shaderName .. GetSlot(builder, 'vcolormatambient', 'true',  'a')
    shaderName = shaderName .. GetSlot(builder, 'vcolorblend',      'true',  'b')
    shaderName = shaderName .. GetSlot(builder, 'grassfadeout',     'true',  'c')
    shaderName = shaderName .. GetSlot(builder, 'terrainlightmap',  'true',  'd')
    shaderName = shaderName .. GetSlot(builder, 'useterraincolor',  'true',  'e')
    shaderName = shaderName .. GetSlot(builder, 'depthbias',        'true',  'f')
    shaderName = shaderName .. GetSlot(builder, 'sky0',             'true',  'g')
    shaderName = shaderName .. GetSlot(builder, 'wave',             'true',  'h')
    shaderName = shaderName .. GetSlot(builder, 'fresnel',          'true',  'i')
    shaderName = shaderName .. GetSlot(builder, 'singlecolorpass',  'true',  'j')
    shaderName = shaderName .. GetSlot(builder, 'laserreceiver',    'true',  'k')
    shaderName = shaderName .. GetSlot(builder, 'rendernormal',     'true',  'l')
    shaderName = shaderName .. GetSlot(builder, 'decalmodulation',  'true',  'm')

    shaderName = shaderName .. GetSlot(builder, 'level', 'editor',  '_EDITOR')
    shaderName = shaderName .. GetSlot(builder, 'level', 'pro',     '_PRO')

    -- character slots for case values
    shaderName = shaderName .. GetCaseSlot(builder, 'fogmode',      'lnrf', '_l')
    shaderName = shaderName .. GetCaseSlot(builder, 'fogmode',      'lyrf', '_y')
    shaderName = shaderName .. GetCaseSlot(builder, 'LightEnv',     'lmap', '_m')
    
    local lightindex = 0
    local lighttmp = builder:getcasevalue('LightType('..lightindex..')')
    while lighttmp ~='' and lighttmp~= nil do
        if lighttmp == 'spot' then
            shaderName = shaderName .. '_s'
        elseif lighttmp == 'dirl' then
            shaderName = shaderName .. '_d'
        elseif lighttmp == 'pntl' then
            shaderName = shaderName .. '_p'
        else
            shaderName = shaderName .. '_'..tostring(val)
        end
        lightindex = lightindex + 1
        lighttmp = builder:getcasevalue('LightType('..lightindex..')')
    end

    local shaderFile = 'wc:libs/shaders/' .. shaderName .. '.fx'

    --local shaderFile = 'wc:libs/shaders/' .. builder:getshadername() .. '.fx'

    builder:setshaderfile(shaderFile)

    -- set shader sequence
    if (builder:getattrib('clipmap') == 'true') and (builder:getattrib('shadowdepth') ~= 'true') then
        if (builder:getattrib('notculling') == 'true') and not (builder:getattrib('alphatwoside') == 'true') then
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

    elseif (builder:getcasevalue('fogmode') == 'lnrf') then
        if (builder:getattrib('depthbias') == 'true') then
            builder:setsequence('depthbiaslinearfog')
        else
            builder:setsequence('linearfog')
        end
    elseif (builder:getattrib('depthbias') == 'true') then
        builder:setsequence('depthbias')
        
    end

end

--------------------------------------------------------------------------------
--  BuildShader
--------------------------------------------------------------------------------

function BuildShader()

    ----------------------------------------------------------------------------
    --  CreateShaderDescription
    ----------------------------------------------------------------------------
    local function CreateShaderDescription()

        local material = lookup(builder:getmaterial())
        local shaderDesc = ''

        builder:puts('//  Material description:\n')

        for iparam = 0, material:getnumparams() - 1, 1 do
            paramName, paramValue = material:getparamat(iparam)
            builder:puts('//      ' .. paramName .. '=' .. paramValue ..'\n')
        end

        builder:puts('//  Material case:\n')

        for icase = 0, builder:getcasepathlen() - 1, 1 do
            caseParam, caseValue = builder:getcasenodeat(icase)
            builder:puts('//      ' .. caseParam .. '=' .. caseValue ..'\n')
        end

    end

    ----------------------------------------------------------------------------
    --  CreateHeader
    ----------------------------------------------------------------------------
    local function CreateHeader()

        builder:puts('//------------------------------------------------------------------------------\n')
        builder:puts('//  2.0/materials/' .. builder:getshadername() .. '.fx\n')
        builder:puts('//  \n')

        CreateShaderDescription()

        builder:puts('//  \n')
        builder:puts('//  (C) 2005 Tragnarion Studios\n')
        builder:puts('//------------------------------------------------------------------------------\n')

    end

    ----------------------------------------------------------------------------
    --  CreateVar
    ----------------------------------------------------------------------------
    local function CreateVar()

        -- include required headers
        builder:puts('#include "shaders:../lib/lib.fx"\n')
        builder:puts('#include "shaders:../lib/libtrag.fx"\n')
        builder:puts('\n')

        if (builder:getattrib('billboard') == 'true' or  builder:getattrib('swinging') == 'true') then
            builder:puts('#include "shaders:../lib/bill.fx"\n')
        end

        if (hasFog) then
            builder:puts('#include "shaders:../lib/fog.fx"\n')
        end

        if (builder:getattrib('terrainlightmap') == 'true') then
            builder:puts('#include "shaders:../lib/gmmt.fx"\n')
        end

        if (hasShadowVolume) then
            builder:puts('float4   ModelLightPos;\n')
        end

        -- declare light parameters as arrays only for 2+ lights
        local lightarray = ''
        if (hasLight and numLights > 1) then
            builder:puts('#define NUM_LIGHTS ' ..numLights.. '\n')
            lightarray = '[NUM_LIGHTS]'
        end

        if (builder:getattrib('terrainlightmap') == 'true') then
            builder:puts('float4   LightDiffuse;\n')

        elseif (hasLight) then
            builder:puts('float4   LightDiffuse' ..lightarray.. ';\n')
        end

        if (builder:getattrib('terrainlightmap') == 'true') then
            builder:puts('float  TerrainDiffuseFactor;\n')
        end

        if (builder:getattrib('laserreceiver') == 'true') or
           (hasLight and builder:getcasevalue('LightType') == 'spot') then
            builder:puts('float4x4 ModelLightProjection;\n')
        end
        
        if (builder:getattrib('laserreceiver') == 'true') then
            builder:puts('//// contactPosition is lightPos + LightDir.xyz and lightDirw.w is a epsilon\n')
            builder:puts('shared float3 LightPos;\n')
            builder:puts('float4 LightDir;\n')
            builder:puts('shared float4x4 InvModel;\n')
            
        end
        
        if (hasLight) then
            -- dynamic lighting
            builder:puts('float4   LightAmbient' ..lightarray.. ';\n')
            builder:puts('float4   LightSpecular' ..lightarray.. ';\n')
            builder:puts('float4   LightAttenuation' ..lightarray.. ';\n')

            if (builder:getattrib('lightmap') == 'true') then
                builder:puts('float  LightMapAmbientFactor;\n')
                builder:puts('float  LightMapDiffuseFactor;\n')
            end

            if (builder:getattrib('terrainlightmap') == 'true') then
                builder:puts('float  TerrainAmbientFactor;\n')
            end

            if (builder:getattrib('instanced') == 'true') then
                builder:puts('float4   LightPos' ..lightarray.. ';\n')
            else
                builder:puts('float4   ModelLightPos' ..lightarray.. ';\n')
            end

            builder:puts('int      LightType' ..lightarray.. ';\n')

            -- diffuse lighting
            if (builder:getattrib('diffuselighting') == 'true') then
                builder:puts('float4 matDiffuse;\n')
            end

            -- specular lighting
            if (builder:getattrib('specularlighting') == 'true') then
                builder:puts('float4 matSpecular;\n')
                builder:puts('float  matShininess;\n')
                builder:puts('float  matLevel;\n')
                builder:puts('\n')
            end

        else
            -- ambient lighting
            if (builder:getattrib('ambientlighting') == 'true') then
                builder:puts('float4   LightAmbient = float4(0.0f, 0.0f, 0.0f, 0.0f);\n')
            end
        end

        builder:puts('\n')

        if (not hasShadowDepth) then  
            builder:puts('shared float4x4 ModelViewProjection;\n')
        else
            builder:puts('shared float4 ShadowProjectionDeform = float4( 1.f, 0.f, 0.f, 1.f);\n')
           -- Calcule in preshader the modelViewProjection
        end
        builder:puts('shared float3   ModelEyePos;\n')
        builder:puts('shared float3   EyePos;\n')
        builder:puts('\n')

        if (hasModel) then
            builder:puts('shared float4x4 Model;\n')
        end

        if (hasModelView) then
            builder:puts('shared float4x4 ModelView;\n')
        end
        
        if (hasProjection) then
            builder:puts('shared float4x4 Projection;\n')
        end

        if (hasInvModelView) then
            builder:puts('shared float4x4 InvModelView;\n')
        end

        if (builder:getattrib('instanced') == 'true') then
            builder:puts('float4 InstPositionPalette[72];//xyz=position, w=scale\n')
            builder:puts('float4 InstRotationPalette[72];//xyz=rotation\n')
        end

        if (builder:getattrib('skinned') == 'true') then
            builder:puts('matrix<float,4,3> JointPalette\[72\];\n')
        end

        if (builder:getattrib('illuminationcolor') == 'true') then
            builder:puts('float4 illuminationColor;\n')
        end

        if (builder:getattrib('alphablend') == 'true') then
            builder:puts('int funcDestBlend = 6; // = InvSrcAlpha\n')
        end

        if (hasMatAmbient) then
            builder:puts('float4 matAmbient;\n')
        end

        if  (builder:getattrib('grassfadeout') == 'true') then
            builder:puts('float TerrainGrassMinDist;\n')
            builder:puts('float TerrainGrassMaxDist;\n')
        end

        if (builder:getattrib('terrainlightmap') == 'true') then
            builder:puts('shared float TerrainSideSizeInv;    // 1 / block distance (weight map texture scale factor (scale UV));\n')
            builder:puts('float3 TerrainCellPosition;         // terrain cell position;\n')
        end

        if (builder:getcasevalue('LightEnv') == 'lmap') then
            builder:puts('float4 TerrainCellMaxMinPos;\n')
        end

        -- texture maps
        builder:puts('float mipMapLodBias = 0.0f;\n')

        if (builder:getattrib('colormap') == 'true') or  (builder:getattrib('clipmap') == 'true') then
            builder:puts('texture diffMap;\n')
        end

        if (builder:getattrib('controlmap') == 'true') then
            builder:puts('texture controlMap;\n')
        end

        if (builder:getattrib('controlmap') == 'true' or
            builder:getattrib('vcolorblend') == 'true') then
            builder:puts('texture diffMap2;\n')
        end

        if (builder:getattrib('bumpmap') == 'true') then
            builder:puts('texture bumpMap;\n')
        else
            if (builder:getattrib('normalmap') == 'true') then
                builder:puts('texture BumpMap3;\n')
            end
        end

        if (builder:getattrib('parallax') == 'true') then
            builder:puts('texture parallaxMap;\n')
            builder:puts('float parallaxFactor; ////Depends on the u texture coordinate\n')
            builder:puts('float parallaxFactorRatioUV =1.f; ////For rectangle textures \n')
        end

        --[[
        if (builder:getattrib('clipmap') == 'true') then
            builder:puts('texture clipMap;\n')
        end
        ]]--

        -- Use a NoiseMap in clipMap with fadeOut
        if (builder:getattrib('grassfadeout') == 'true' and builder:getattrib('clipmap') == 'true') then
            builder:puts('texture NoiseMap0;\n')
        end

        if ((builder:getattrib('lightmap') == 'true') or
            (builder:getattrib('terrainlightmap') == 'true') or
            (builder:getcasevalue('LightEnv') == 'lmap')) then
            builder:puts('texture lightMap;\n')
        end

        if (builder:getattrib('illuminationmap') == 'true') then
            builder:puts('texture illuminationMap;\n')
        end

        if (builder:getattrib('environmentmap2d') == 'true') then
            builder:puts('texture AmbientMap0;\n')
        end

        if (builder:getattrib('environmentmap3d') == 'true') then
            builder:puts('texture CubeMap0;\n')
        end

        if (builder:getattrib('environmentmap2d') == 'true' or builder:getattrib('environmentmap3d') == 'true') then
            if (builder:getattrib('levelmapasenvmaskmap') ~= 'true') then
                builder:puts('texture envMaskMap;\n')
            end
            builder:puts('float   envMaskFactor;\n')
        end

        if (builder:getattrib('levelmap') == 'true') or (builder:getattrib('levelmapasenvmaskmap') == 'true') then
            builder:puts('texture levelMap;\n')
        end

        if (builder:getcasevalue('LightType') == 'spot') or
           (builder:getattrib('laserreceiver') == 'true') then
            builder:puts('texture LightModMap;\n')
        end

        if (builder:getattrib('swinging') == 'true' or builder:getattrib('wave') == 'true') then
            builder:puts('shared float Time;\n')
        end

        if (builder:getattrib('wave') == 'true') then
            builder:puts('float Amplitude;\n')
            builder:puts('float Frequency;\n')
            builder:puts('float4 Velocity;\n')
        end

        -- TODO expose texture scale parameter as general purpose
        if (builder:getattrib('wave') == 'true' and builder:getattrib('bumpmap') == 'true') then
            builder:puts('float4 TexGenS;\n')
        end

        if (builder:getattrib('fresnel') == 'true') then
            builder:puts('float FresnelBias;\n')
            builder:puts('float FresnelPower;\n')
        end

        -- fading attributes
        if (hasFading) then
            builder:puts('float MinDist;\n')
            builder:puts('float MaxDist;\n')
        end

        -- Fog
        if (hasFog) then
            builder:puts('float4 fogDistances;\n')
            builder:puts('float4 fogColor = { 1.0f, 1.0f, 1.0f, 1.0f };\n')
        end

        builder:puts('\n')
    end

    ----------------------------------------------------------------------------
    --  CreateSamplers
    ----------------------------------------------------------------------------
    local function CreateSamplers()

        --[[
        if (builder:getattrib('clipmap') == 'true') then
            builder:puts('#include "shaders:../lib/clipsampler.fx"\n')
        end
        ]]--

        if (builder:getattrib('grassfadeout') == 'true' and builder:getattrib('clipmap') == 'true') then
            builder:puts('#include "shaders:../lib/noisesampler.fx"\n')
        end

        if (builder:getattrib('colormap') == 'true') or (builder:getattrib('clipmap') == 'true')  then
            builder:puts('#include "shaders:../lib/diffsampler.fx"\n')
        end

        if (builder:getattrib('controlmap') == 'true') then
            builder:puts('#include "shaders:../lib/controlsampler.fx"\n')
        end

        if (builder:getattrib('controlmap') == 'true' or
            builder:getattrib('vcolorblend') == 'true') then
            builder:puts('#include "shaders:../lib/diffSecSampler.fx"\n')
        end

        if (builder:getattrib('bumpmap') == 'true') then
            builder:puts('#include "shaders:../lib/bumpsampler.fx"\n')
        else
            if (builder:getattrib('normalmap') == 'true') then
                builder:puts('#include "shaders:../lib/normalsampler.fx"\n')
            end
        end

        if (builder:getattrib('parallax') == 'true') then
            builder:puts('#include "shaders:../lib/parallaxmap.fx"\n')
        end

        if (builder:getattrib('lightmap') == 'true') then
            builder:puts('#include "shaders:../lib/lightmapsampler.fx"\n')
        else
            if (builder:getattrib('terrainlightmap') == 'true' or
                builder:getcasevalue('LightEnv') == 'lmap') then
                builder:puts('#include "shaders:../lib/lightmapsampler_clamp.fx"\n')
            end
        end
        
        if (builder:getattrib('useterraincolor') == 'true') then
            -- Include the modulation map
            builder:puts('#include "shaders:../lib/gmmt_glot_sampler.fx"\n')
        end

        if (builder:getattrib('illuminationmap') == 'true') then
            builder:puts('#include "shaders:../lib/emissivesampler.fx"\n')
        end

        if (builder:getattrib('environmentmap2d') == 'true') then
            builder:puts('#include "shaders:../lib/env2dsampler.fx"\n')
        end

        if (builder:getattrib('environmentmap3d') == 'true') then
            builder:puts('#include "shaders:../lib/environmentsampler.fx"\n')
        end

        if (builder:getattrib('environmentmap2d') == 'true' or builder:getattrib('environmentmap3d') == 'true') then
            if (builder:getattrib('levelmapasenvmaskmap') ~= 'true') then
                builder:puts('#include "shaders:../lib/environmentmask.fx"\n')
            end
        end

        if (builder:getattrib('levelmap') == 'true' or builder:getattrib('levelmapasenvmaskmap') == 'true') then
            builder:puts('#include "shaders:../lib/levelsampler.fx"\n')
        end

        if (builder:getcasevalue('LightType') == 'spot') or
           (builder:getattrib('laserreceiver') == 'true') then
            builder:puts('#include "shaders:../lib/lightsampler.fx"\n')
        end

        if (hasShadowVolume) then
            builder:puts('#include "shaders:../lib/shadow.fx"\n')
        end

    end

    ----------------------------------------------------------------------------
    --  CreateVertexDeclaration
    ----------------------------------------------------------------------------
    local function CreateVertexDeclaration()

        -- texture and color indices
        -- TODO ma.garcias- handle the case of maximum texture/color index reached
        outtextureindex = 0
        outcolorindex = 0

        -- input structure
        -------------------
        builder:puts('struct VsInput\n')
        builder:puts('{\n')
        builder:puts('    float3 position : POSITION;\n')

        if (hasNormal) then
            builder:puts('    float3 normal   : NORMAL;\n')
        end

        if (hasUV0) then
            builder:puts('    float2 uv0      : TEXCOORD0;\n')
        end

        --[[
        if (builder:getattrib('lightmap') == 'true' or
            builder:getattrib('terrainlightmap') == 'true') then
            builder:puts('    float2 uv1      : TEXCOORD1;\n')
        end
        ]]--
        if (builder:getattrib('lightmap') == 'true') then
            builder:puts('    float2 uv1      : TEXCOORD1;\n')
        end

        if (builder:getattrib('controlmap') == 'true') then
            builder:puts('    float2 uv2      : TEXCOORD2;\n')
        end

        if (builder:getattrib('billboard') == 'true' or builder:getattrib('normalmap') == 'true') then
            builder:puts('    float2 uv3      : TEXCOORD3;\n')
        end

        if (builder:getattrib('swinging') == 'true' or  builder:getattrib('wave') == 'true') then
            builder:puts('    float3 color    : COLOR;\n')
        else
            if (builder:getattrib('vcolormatambient') == 'true' or
                builder:getattrib('vcolorblend') == 'true')
            then
                builder:puts('    float4 color    : COLOR;\n')
            end
        end

        if (hasTangentSpace) then
            builder:puts('    float3 tangent  : TANGENT;\n')
            builder:puts('    float3 binormal : BINORMAL;\n')
        end

        if (builder:getattrib('instanced') == 'true') then
            builder:puts('    float4 instIndex: BLENDINDICES;\n')
        end

        if (builder:getattrib('skinned') == 'true') then
            builder:puts('    float4 weights  : BLENDWEIGHT;\n')
            builder:puts('    float4 indices  : BLENDINDICES;\n')
        end

        builder:puts('};\n')
        builder:puts('\n')

        -- output structure
        -------------------
        builder:puts('struct VsOutput\n')
        builder:puts('{\n')
        builder:puts('    float4 position      : POSITION;\n')

        if (hasUV0) then
            builder:puts('    float2 uv0           : TEXCOORD' .. outtextureindex .. ';\n')
            outtextureindex = outtextureindex + 1;
        end
        
        if (builder:getattrib('wave') == 'true' and builder:getattrib('bumpmap') == 'true') then
            builder:puts('    float2 bumpCoord0    : TEXCOORD' .. outtextureindex .. ';\n')
            outtextureindex = outtextureindex + 1;
            builder:puts('    float2 bumpCoord1    : TEXCOORD' .. outtextureindex .. ';\n')
            outtextureindex = outtextureindex + 1;
            builder:puts('    float2 bumpCoord2    : TEXCOORD' .. outtextureindex .. ';\n')
            outtextureindex = outtextureindex + 1;
        end

        if (builder:getattrib('lightmap') == 'true') then
            builder:puts('    float2 uv1           : TEXCOORD' .. outtextureindex .. ';\n')
            outtextureindex = outtextureindex + 1;
        else
            if (builder:getattrib('terrainlightmap') == 'true') then
                if (builder:getattrib('useterraincolor') == 'true') then
                -- Use z  component for a  modulate factor(between lightmap and color)
                    builder:puts('    float3 uv1           : TEXCOORD' .. outtextureindex .. ';\n')
                else
                    builder:puts('    float2 uv1           : TEXCOORD' .. outtextureindex .. ';\n')
                end
                outtextureindex = outtextureindex + 1;
                
                if (builder:getattrib('useterraincolor') == 'true') then
                    builder:puts('    float2 uvTerrainGlobal : TEXCOORD' .. outtextureindex .. ';\n')
                end
                outtextureindex = outtextureindex + 1;
            end
        end

        if (builder:getattrib('environmentmap2d') == 'true') then
            builder:puts('    float2 uvenv         : TEXCOORD' .. outtextureindex .. ';\n')
            outtextureindex = outtextureindex + 1;
        end

        if (builder:getattrib('rendernormal') == 'true') then
            builder:puts('    float3 normal        : TEXCOORD' .. outtextureindex .. ';\n')
            outtextureindex = outtextureindex + 1;
        end

        -- if (level < LEVEL_UNLIT) and (level > LEVEL_PRO)
        if (builder:getattrib('environmentmap3d') == 'true' and not hasNormalByPixel) then
            builder:puts('    float3 uvenv         : TEXCOORD' .. outtextureindex .. ';\n')
            outtextureindex = outtextureindex + 1;
        end

        -- if (level < LEVEL_FLAT) and (level > LEVEL_PRO)
        if ((builder:getattrib('parallax') == 'true' or builder:getattrib('fresnel') == 'true') and not hasNormalByPixel) then
            builder:puts('    float3 eyeVec        : TEXCOORD' .. outtextureindex .. ';\n')
            outtextureindex = outtextureindex + 1;
        end

        if (builder:getattrib('controlmap') == 'true') then
            builder:puts('    float2 uv2           : TEXCOORD' .. outtextureindex .. ';\n')
            outtextureindex = outtextureindex + 1;
        end

        if (builder:getattrib('normalmap') == 'true') then
            builder:puts('    float2 uv3           : TEXCOORD' .. outtextureindex .. ';\n')
            outtextureindex = outtextureindex + 1;
        end

        if (builder:getcasevalue('LightType') == 'spot') then
            builder:puts('    float3 uvlight       : TEXCOORD' .. outtextureindex .. ';\n')
            outtextureindex = outtextureindex + 1;
        end
        
        if (builder:getattrib('laserreceiver') == 'true') then
            builder:puts('    float4 uvlight       : TEXCOORD' .. outtextureindex .. ';\n')
            outtextureindex = outtextureindex + 1;
        end

        if (builder:getcasevalue('LightEnv') == 'lmap') then
            builder:puts('    float2 uvlightmap    : TEXCOORD' .. outtextureindex .. ';\n')
            outtextureindex = outtextureindex + 1;
        end

        if (builder:getattrib('grassfadeout') == 'true') then
            builder:puts('    float grassFadeOut    : TEXCOORD' .. outtextureindex .. ';\n')
            outtextureindex = outtextureindex + 1;
        end

        if (hasShadowDepth) then
            builder:puts('    float depth     : TEXCOORD' .. outtextureindex .. ';\n')
            outtextureindex = outtextureindex + 1;
        end

        -- if (level < LEVEL_UNLIT)
        if (hasLight) then
            if (hasTangentSpace) then
                if (not hasNormalByPixel) then
                    if (builder:getattrib('diffuselighting') == 'true') then
                        builder:puts('    float3 lightVec      : TEXCOORD' .. outtextureindex .. ';\n')
                        outtextureindex = outtextureindex + 1;
                    end
                    if (builder:getattrib('specularlighting') == 'true') then
                        builder:puts('    float3 halfVec       : TEXCOORD' .. outtextureindex .. ';\n')
                        outtextureindex = outtextureindex + 1;
                    end
                end
            else
                if (builder:getattrib('diffuselighting') == 'true') then
                    builder:puts('    float4 diffuse       : COLOR' .. outcolorindex .. ';\n')
                    outcolorindex = outcolorindex + 1;
                end

                if (builder:getattrib('specularlighting') == 'true') then
                    builder:puts('    float4 specular      : COLOR' .. outcolorindex .. ';\n')
                    outcolorindex = outcolorindex + 1;
                end
            end
        end

        -- if (level < LEVEL_PIXEL)
        if (hasNormalByPixel) then
            builder:puts('    float3 modelPos      : TEXCOORD' .. outtextureindex .. ';\n')
            outtextureindex = outtextureindex + 1;
            builder:puts('    float3 normal        : TEXCOORD' .. outtextureindex .. ';\n')
            outtextureindex = outtextureindex + 1;

            if (hasTangentSpace) then
                builder:puts('    float4 tangent       : TEXCOORD' .. outtextureindex .. ';\n')
                outtextureindex = outtextureindex + 1;
            end
        end

        -- if (level < LEVEL_UNLIT)
        -- fog
        if (hasFog) then
            if (builder:getcasevalue('fogmode') == 'lnrf') then
                builder:puts('    float fog            : FOG;\n')
            else
                builder:puts('    float fog            : TEXCOORD' .. outtextureindex .. ';\n')
                outtextureindex = outtextureindex + 1;
            end
        end

        -- vertex color
        if (builder:getattrib('vcolormatambient') == 'true' or
            builder:getattrib('vcolorblend') == 'true')
        then
            local vdcolorsem = ''
            if (outcolorindex < 2) then
                vdcolorsem = ' COLOR' .. outcolorindex
                outcolorindex = outcolorindex + 1;
            else
                vdcolorsem = 'TEXCOORD' .. outtextureindex
                outtextureindex = outtextureindex + 1;
            end
            builder:puts('    float4 color         : ' .. vdcolorsem .. ';\n')
        end

        -- fading out properties
        if (hasFading) then
            builder:puts('    float smooth         : TEXCOORD' .. outtextureindex .. ';\n')
            outtextureindex = outtextureindex + 1;
        end

        builder:puts('};\n')
        builder:puts('\n')
    end

    ----------------------------------------------------------------------------
    --  CreateVertexShader
    ----------------------------------------------------------------------------
    local function CreateVertexShader(name, invertNormal)
        -- vertex shader
        builder:puts('VsOutput ' .. name .. '(const VsInput vsIn')

        if (hasLight) then
            builder:puts(', uniform int lightType, uniform int lightStatic')
        end

        lightindex = ''
        if (numLights > 1) then
            builder:puts(', uniform int lightIndex')
            lightindex = '[lightIndex]'
        end

        builder:puts(')\n')
        builder:puts('{\n')
        
        if (hasShadowDepth) then  
        -- Calcule the modelViewProjection
            builder:puts('    //Calcule the ModelViewProjection in preShader;\n')
            builder:puts('    float4x4 deformMatrix = { { ShadowProjectionDeform.x, ShadowProjectionDeform.y, 0.f, 0.f},\n')
            builder:puts('                              { ShadowProjectionDeform.z, ShadowProjectionDeform.w, 0.f, 0.f},\n')
            builder:puts('                              {                      0.f,                      0.f, 1.f, 0.f},\n ')
            builder:puts('                              {                      0.f,                      0.f, 0.f, 1.f}};\n')
            builder:puts('    float4x4 ModelViewProjection = mul( mul( ModelView, deformMatrix), Projection);\n')
        end

        builder:puts('    VsOutput vsOut;\n')
        builder:puts('    float4 position = float4(vsIn.position, 1.0f);\n')

        if (hasNormal) then
            builder:puts('    float3 normal   = vsIn.normal;\n')
        end

        if (hasTangentSpace) then
            builder:puts('    float3 tangent = vsIn.tangent;\n')
            builder:puts('    float3 binormal  = vsIn.binormal;\n')
            --builder:puts('    float3 binormal = cross(normal, tangent);\n')
        end

        -- geometry transformations
        if (builder:getattrib('skinned') == 'true') then
            builder:puts('    position = skinnedPosition(position, vsIn.weights, vsIn.indices, JointPalette);\n')

            if (hasNormal) then
                builder:puts('    normal   = skinnedNormal(normal, vsIn.weights, vsIn.indices, JointPalette);\n')
            end

            if (hasTangentSpace) then
                builder:puts('    tangent  = skinnedNormal(tangent, vsIn.weights, vsIn.indices, JointPalette);\n')
                builder:puts('    binormal = skinnedNormal(binormal, vsIn.weights, vsIn.indices, JointPalette);\n')
            end

            builder:puts('\n')
        end

        -- instanced eye position
        if (hasEyeDir or hasLight) then
            if (builder:getattrib('instanced') == 'true') then
                builder:puts('    float3 eyePos = EyePos;\n')
            else
                builder:puts('    float3 eyePos = ModelEyePos;\n')
            end
        end

        -- swinging
        if (builder:getattrib('swinging') == 'true') then
            builder:puts('    position = swing2(position, vsIn.color, Time);\n')
        end

        -- billboard
        if (builder:getattrib('billboard') == 'true') then
            builder:puts('    position = transformBillBoard(position, vsIn.uv3, InvModelView, eyePos);\n')
        end

        -- instancing
        if (builder:getattrib('instanced') == 'true') then
            builder:puts('    position = transformInstanced(position, InstPositionPalette[vsIn.instIndex.x], InstRotationPalette[vsIn.instIndex.x]);\n')

            if (hasNormal) then
                builder:puts('    normal = transformInstancedNormal(normal, InstRotationPalette[vsIn.instIndex.x]);\n')
            end

            if (hasTangentSpace) then
                builder:puts('    tangent = transformInstancedNormal(tangent, InstRotationPalette[vsIn.instIndex.x]);\n')
                builder:puts('    binormal = transformInstancedNormal(binormal, InstRotationPalette[vsIn.instIndex.x]);\n')
            end

        end

        if (builder:getattrib('wave') == 'true') then
            if (not hasNormal) then
                builder:puts('    float3 normal;\n')
            end
            if (not hasTangentSpace) then
                builder:puts('    float3 tangent;\n')
                builder:puts('    float3 binormal;\n')
            end
            builder:puts('    position = transformWave(position, vsIn.color, Frequency, Amplitude, Time, normal, tangent, binormal);\n')
        end
        
        -- shadow volumes
        if (hasShadowVolume) then
            builder:puts('    position = vsExtrudeShadowVolume(position, normal, ModelLightPos, 50.0, 0.01);\n')
        end

        if (not hasShadowDepth) then
            builder:puts('    vsOut.position = mul(position, ModelViewProjection);\n')
        else
            builder:puts('    float4 clipPos = mul(position, ModelViewProjection);\n')
            builder:puts('    vsOut.position = clipPos;\n')
        end

        if (builder:getattrib('sky0') == 'true') then
            builder:puts('    float signw = sign(vsOut.position.w);\n')
            builder:puts('    vsOut.position.xy = signw * vsOut.position.xy / vsOut.position.w;\n')
            builder:puts('    vsOut.position.w = signw;\n')
            builder:puts('    vsOut.position.z = sign(vsOut.position.z);\n')
        end

        if (hasEyeDir) then
            builder:puts('    float3 eyeVec = normalize(eyePos - position);\n');
        end

        -- texture coordinates
        builder:puts('    // texture coordinates\n')

        if (hasShadowDepth) then
            builder:puts('    // pass z / w to the pixel shader\n')
            builder:puts('    vsOut.depth = (clipPos.z + 0.0001f) / clipPos.w;\n')
        end

        if (hasUV0) then
            builder:puts('    vsOut.uv0 = vsIn.uv0;\n')
        end
        
        -- wave bump coordinates
        if (builder:getattrib('wave') == 'true' and builder:getattrib('bumpmap') == 'true') then
            builder:puts('    vsOut.uv0.xy = vsIn.uv0 * TexGenS.xy;\n')
            builder:puts('    float modTime = fmod(Time, 100.0f);\n')
            builder:puts('    vsOut.bumpCoord0 = vsIn.uv0 * TexGenS.xy + modTime * Velocity.xy;\n')
            builder:puts('    vsOut.bumpCoord1 = vsIn.uv0 * TexGenS.xy + modTime * Velocity.xy * 4.0f;\n')
            builder:puts('    vsOut.bumpCoord2 = vsIn.uv0 * TexGenS.xy + modTime * Velocity.xy * 8.0f;\n')
        end

        -- lightmap coordinates
        if (builder:getattrib('lightmap') == 'true') then
            builder:puts('    vsOut.uv1 = vsIn.uv1;\n')
        end

        if (builder:getattrib('terrainlightmap') == 'true') then
            builder:puts('    float4 TerrainPos = mul(position, Model);\n')
            builder:puts('    CalculateGMMWeightUV(TerrainPos, TerrainCellPosition, TerrainSideSizeInv, vsOut.uv1.xy);\n')
            if (builder:getattrib('useterraincolor') == 'true') then
                builder:puts('    vsOut.uv1.z = vsIn.color.y;\n')
            end
            builder:puts('    CalculateGMMGlobalUV( TerrainPos, TerrainGlobalMapScale, vsOut.uvTerrainGlobal );\n')
        end

        -- controlmap coordinates
        if (builder:getattrib('controlmap') == 'true') then
            builder:puts('    vsOut.uv2 = vsIn.uv2;\n')
        end

        -- normalmap coordinates
        if (builder:getattrib('normalmap') == 'true') then
            builder:puts('    vsOut.uv3 = vsIn.uv3;\n')
        end

        -- environmentmap2d coordinates
        if (builder:getattrib('environmentmap2d') == 'true') then
            builder:puts('    float3 refl = reflect(eyeVec, normal);\n');
            builder:puts('    vsOut.uvenv = refl.xy + normal.xy;\n');
        end

        -- normalmap generation
        if (builder:getattrib('rendernormal') == 'true') then
            builder:puts('    vsOut.normal = normal;\n');
        end

        --if (level < LEVEL_UNLIT) and (level > LEVEL_PRO)
        -- environmentmap3d coordinates
        if (builder:getattrib('environmentmap3d') == 'true' and not hasNormalByPixel) then
            builder:puts('    //the reflect invert the direction eyedir\n');
            builder:puts('    float3 refl = reflect(-eyeVec, normal);\n');
            builder:puts('    vsOut.uvenv = mul(refl, Model);\n');
        end

        --if (level < LEVEL_UNLIT) and (level > LEVEL_PRO)
        if ((builder:getattrib('parallax') == 'true' or builder:getattrib('fresnel') == 'true') and not hasNormalByPixel) then
            builder:puts('    vsOut.eyeVec = mul(float3x3(tangent, binormal, normal), eyeVec);\n')
        end

        --if (level < LEVEL_PIXEL)
        if (hasNormalByPixel) then
            builder:puts('    vsOut.modelPos = position.xyz;\n')
            builder:puts('    vsOut.normal   = normal;\n')
            if (hasTangentSpace) then
                builder:puts('    vsOut.tangent.xyz  = tangent;\n')
                --<TEMP> trick to get tangent direction
                builder:puts('    vsOut.tangent.w  = 0 < dot(normal, cross(tangent, binormal)) ? 1.0f : -1.0f;\n')
                --builder:puts('    vsOut.tangent.w  = 1.0f')
                --builder:puts('    vsOut.binormal = binormal;\n')
                --</TEMP>
            end
        end

        if (builder:getattrib('vcolormatambient') == 'true' or
            builder:getattrib('vcolorblend') == 'true')
        then
            builder:puts('    vsOut.color = vsIn.color;\n')
        end

        -- light projection coordinates
        if (builder:getcasevalue('LightType') == 'spot') then
            builder:puts('    float4 uvlight = mul(position, ModelLightProjection);\n')
            builder:puts('    vsOut.uvlight = uvlight.xyz / uvlight.w;\n')
            --builder:puts('    vsOut.uvlight = uvlight.xyz;\n')
        end
        
        if (builder:getattrib('laserreceiver') == 'true') then
            builder:puts('    vsOut.uvlight = mul(position, ModelLightProjection);\n')
        end

        -- lightmap projection coordinates
        if (builder:getcasevalue('LightEnv') == 'lmap') then
            builder:puts('    vsLightmap(position, TerrainCellMaxMinPos, Model, vsOut.uvlightmap);\n')
        end

        --if (level < LEVEL_UNLIT)
        if (hasLightVector) then

            builder:puts('    // lighting\n')
            
            if (builder:getattrib('instanced') == 'true') then
                builder:puts('    float4 lightPos = LightPos'..lightindex..';\n')
            else
                builder:puts('    float4 lightPos = ModelLightPos'..lightindex..';\n')
            end

            if (hasTangentSpace) then
                if (not hasNormalByPixel) then
                    -- per-pixel lighting
                    if (builder:getattrib('diffuselighting') == 'true') or
                        (builder:getattrib('specularlighting') == 'true') then
                        builder:puts('    float3 lightVec;\n')
                        builder:puts('    float3 halfVec;\n')

                        builder:puts('    if (lightType == LIGHT_DIR) {\n')
                        builder:puts('        tangentSpaceLightHalfVectorDir(position, lightPos, eyePos, normal, binormal, tangent,\n')
                        builder:puts('                                       lightVec, halfVec);\n')
                        builder:puts('    } else {\n')
                        builder:puts('        tangentSpaceLightHalfVector2(position, lightPos, eyePos, normal, binormal, tangent,\n')
                        builder:puts('                                     lightVec, halfVec);\n')
                        builder:puts('        lightVec *= distance(position, lightPos);\n')
                        builder:puts('    }\n')

                        if (builder:getattrib('diffuselighting') == 'true') then
                            builder:puts('    vsOut.lightVec = lightVec;\n')
                        end

                        if (builder:getattrib('specularlighting') == 'true') then
                            builder:puts('    vsOut.halfVec = halfVec;\n')
                        end
                    end
                end
            else
                -- per-vertex lighting
                builder:puts('    float3 lightVec;\n')
                builder:puts('    if (lightType == LIGHT_DIR) {\n')
                builder:puts('        lightVec = normalize(lightPos);\n')
                builder:puts('    } else {\n')
                builder:puts('        lightVec = normalize(lightPos - position);\n')
                builder:puts('        lightVec *= distance(position, lightPos);\n')
                builder:puts('    }\n')

                -- per-vertex light attenuattion
                builder:puts('    float lightAtt = 1.0f;\n')
                
                -- per-vertex light bulb
                builder:puts('    if (lightType == LIGHT_BULB) {\n')
                builder:puts('        float dist = distance(lightPos.xyz, position.xyz);\n')
                builder:puts('        float4 lightAttenuation = LightAttenuation'..lightindex..';\n')
                builder:puts('        lightAtt = 1.0f - smoothstep(lightAttenuation.x, lightAttenuation.y, dist);\n')

                -- per-vertex light static/directional w/ lightmap: no lighting
                if (hasLightMap) then
                    builder:puts('    } else if (lightStatic) {\n')
                    builder:puts('        lightAtt = 0.0f;\n')
                end
                
                builder:puts('    }\n')

                if (builder:getattrib('diffuselighting') == 'true') then
                    builder:puts('    // per-vertex diffuse lighting\n')
                    builder:puts('    float diffuse = saturate(dot(normal, lightVec));\n')
                    builder:puts('    vsOut.diffuse = diffuse * LightDiffuse'..lightindex..' * matDiffuse * lightAtt;\n')
                end

                if (builder:getattrib('specularlighting') == 'true') then
                    builder:puts('    // per-vertex specular lighting\n')
                    builder:puts('    float3 halfVec = normalize(lightVec + eyeVec);\n')
                    builder:puts('    float specular = pow(saturate(dot(normal, halfVec)), matShininess);\n')
                    builder:puts('    vsOut.specular = specular * LightSpecular'..lightindex..' * matSpecular * lightAtt;\n')
                end
            end
        end

        -- Fog
        if (hasFog) then
            builder:puts('    // Fog\n')

        if (builder:getcasevalue('fogmode') == 'lnrf') then
            if (numLights > 1) then
                builder:puts('    if (lightIndex == NUM_LIGHTS -1) {\n')
            end

            builder:puts('    vsOut.fog = linearFog(position, fogDistances, ModelView);\n')

            if (numLights > 1) then
                builder:puts('    } else {\n')
                builder:puts('        vsOut.fog = 1.0f;\n')
                builder:puts('    }\n')
            end
        end

        if (builder:getcasevalue('fogmode') == 'lyrf') then
            builder:puts('    vsOut.fog = layeredFog(position, EyePos, fogDistances, Model);\n')
        end

        end -- Fog

        -- Texture fading
        if (hasFading) then
            builder:puts('    vsOut.smooth = smooth(position, MinDist, MaxDist, ModelView);\n')
        end

        -- IS posible optimize grasspos with fog position
        if (builder:getattrib('grassfadeout') == 'true') then
            builder:puts('    float4 grassPos  = mul(position, ModelView);\n')
            builder:puts('    // Z is visivble from 0.0 to  -FarPlane;\n')
            builder:puts('    // Only clamp FadeOut in pixel shader;\n')
            builder:puts('    vsOut.grassFadeOut =  (-grassPos.z - TerrainGrassMinDist) / (TerrainGrassMaxDist - TerrainGrassMinDist);\n')
        end

        builder:puts('    return vsOut;\n')
        builder:puts('}\n')
        builder:puts('\n')

    end

    ----------------------------------------------------------------------------
    --  CreatePixelShader
    ----------------------------------------------------------------------------
    local function CreatePixelShader(name, withDiffuse, withSpecular)

        -- pixel shader
        builder:puts('float4 ' .. name .. '(const VsOutput psIn')
        
        if (hasLight) then
            builder:puts(', uniform int lightType, uniform bool lightStatic')
        end
        
        lightindex = ''
        if (numLights > 1) then
            builder:puts(', uniform int lightIndex')
            lightindex = '[lightIndex]'
        end
        
        if (hasAnyAutoIlu) then  -- the lightmap is needed in diffuse lighting
            builder:puts(', uniform bool useAutoIlumination')
        end
               
        
        builder:puts(') : COLOR\n')
        builder:puts('{\n')

        -- texture stages
        builder:puts('    float4 srcColor;\n')
        if (hasShadowVolume) then
            builder:puts('    float4 dstColor = float4(1.0f, 0.0f, 0.0f, 0.3f);\n')
        else
            builder:puts('    float4 dstColor = float4(0.0f, 0.0f, 0.0f, 1.0f);\n')
        end

        if (hasUV0) then
            builder:puts('    float2 uv0 = psIn.uv0;\n')
        end

        -- compute color to render normals
        if (builder:getattrib('rendernormal') == 'true') then
            builder:puts('    float3 normal = mul(normalize(psIn.normal), (float3x3) ModelView);\n')
            builder:puts('    dstColor = float4(normalize(psIn.normal) * 0.5f + 0.5f, 1.0f);\n')
        end

        -- compute tangent space matrix (before parallax)
        if (hasNormalByPixel) then
            builder:puts('    float3 normal = normalize(psIn.normal);\n')
            --@TODO this is only for envmapping, parallax and specular lighting:
            builder:puts('    float3 eyeVec = normalize(ModelEyePos - psIn.modelPos);\n');

            if (hasTangentSpace) then
                builder:puts('    float3 tangent = normalize(psIn.tangent.xyz);\n')
                builder:puts('    float3 binormal = psIn.tangent.w * cross(normal, tangent.xyz); // The w is -1 or 1 , is direction\n')
                builder:puts('    float3x3 tangentSpaceMatrix = float3x3(tangent, binormal, normal);\n');
            end
        end

        -- compute parallax before any sample, for it modifies uv0
        if (builder:getattrib('parallax') == 'true') then
            builder:puts('    //parallax with offset limit\n')
            builder:puts('    float2 parallaxMaxim = float2( parallaxFactor , parallaxFactor * parallaxFactorRatioUV); //// use preShader\n')
            builder:puts('    float2 parallaxHeight = parallaxMaxim * (2.0 * tex2D(parallaxMapSampler, psIn.uv0) - 1.0f);\n')
            builder:puts('    parallaxHeight.y = -parallaxHeight.y;\n')

            if (hasNormalByPixel) then
                builder:puts('    float3 tangentEyeVec = mul(tangentSpaceMatrix, eyeVec);\n')
                builder:puts('    uv0 -= (normalize(tangentEyeVec).xy * parallaxHeight);\n')
            else
                builder:puts('    uv0 -= (normalize(psIn.eyeVec).xy * parallaxHeight);\n')
            end
        end

        if (hasTangentSpace) then
            if (builder:getattrib('bumpmap') == 'true') then
                if (builder:getattrib('wave') == 'true') then
                    builder:puts('    float3 tangentSurfaceNormal = (2.0f * tex2D(BumpSampler, psIn.bumpCoord0) - 1.0f) +\n')
                    builder:puts('                                  (2.0f * tex2D(BumpSampler, psIn.bumpCoord1) - 1.0f) +\n')
                    builder:puts('                                  (2.0f * tex2D(BumpSampler, psIn.bumpCoord2) - 1.0f);\n')
                    builder:puts('    tangentSurfaceNormal = normalize(tangentSurfaceNormal);\n')
                else
                    builder:puts('    float3 tangentSurfaceNormal = textureToNormal( tex2D(BumpSampler, uv0) );\n')
                end
            else
                if (builder:getattrib('normalmap') == 'true') then
                    builder:puts('    float3 tangentSurfaceNormal = textureToNormal( tex2D(NormalSampler, psIn.uv3));\n')
                else
                    builder:puts('    float3 tangentSurfaceNormal = float3(0.0f, 0.0f, 1.0f);\n')
                end
            end
            
            if (hasFading) then
                builder:puts('    tangentSurfaceNormal = lerp(float3(0.0f, 0.0f, 1.0f), tangentSurfaceNormal, psIn.smooth);')
                builder:puts('    tangentSurfaceNormal = normalize(tangentSurfaceNormal);\n')
            end

        end
        
        if (builder:getattrib('laserreceiver') == 'true') then
            builder:puts('    float3 uvlight = psIn.uvlight.xyz / psIn.uvlight.w;\n')
            builder:puts('    float2 lightProjection = uvlight.xy;\n')
            builder:puts('    dstColor = tex2D(LightSampler, lightProjection);\n')
        end
        
        if(hasLight) then
            builder:puts('    float lightAtt = 1.0f;\n')
        end
        
        if (hasNormalByPixel) then
            if (hasTangentSpace) then
                -- compute surface normal in model space
                builder:puts('    float3 modelSurfaceNormal = mul(tangentSurfaceNormal, tangentSpaceMatrix);\n')                
                builder:puts('    float3 reflectEye = reflect(-eyeVec, modelSurfaceNormal);\n');
                if  builder:getattrib('decalmodulation') == 'true' then
                    builder:puts('    float3 modelSurfaceNormal2 = mul(float3(0.0,0.0,1.0), tangentSpaceMatrix);\n')
                end

                -- compute lighting in model space
                if (hasLightVector) then
                    builder:puts('    float3 lightVec;\n')
                    builder:puts('    if (lightType == LIGHT_DIR) {\n')
                    builder:puts('        lightVec = normalize(ModelLightPos'..lightindex..');\n')
                    builder:puts('    } else {\n')
                    builder:puts('        lightVec = normalize(ModelLightPos'..lightindex..' - psIn.modelPos);\n')
                    builder:puts('    }\n')
                
                    -- light attenuation
                    builder:puts('    if (lightType == LIGHT_BULB) {\n')
                    builder:puts('        float dist = distance(ModelLightPos'..lightindex..', psIn.modelPos);\n')
                    builder:puts('        float4 lightAttenuation = LightAttenuation'..lightindex..';\n')
                    builder:puts('        lightAtt = 1.0f - smoothstep(lightAttenuation.x, lightAttenuation.y, dist);\n')
                    builder:puts('    }\n')
                end
            else
                builder:puts('    float3 modelSurfaceNormal = normal;\n')
                builder:puts('    float3 reflectEye = reflect(-eyeVec, modelSurfaceNormal);\n');
            end
        else
            if (hasTangentSpace) then
                if (hasLightVector) then
                    -- light attenuation
                    builder:puts('    if (lightType == LIGHT_BULB) {\n')
                    builder:puts('        float dist = length(psIn.lightVec);\n')
                    builder:puts('        float4 lightAttenuation = LightAttenuation'..lightindex..';\n')
                    builder:puts('        lightAtt = 1.0f - smoothstep(lightAttenuation.x, lightAttenuation.y, dist);\n')
                    builder:puts('    }\n')

                end
            end
        end

        -- the spot light atenuation is by pixel. Indiferent if ilumination is by vertex.
        --only one spot ant it the fist, it has any spot 
        if (builder:getcasevalue('LightType') == 'spot') then
            builder:puts('    if (lightType == LIGHT_SPOT) \n'..
                         '    {\n' ..
                         '        lightAtt = saturate(1.f - psIn.uvlight.z);\n' .. --use the frustrum of spot1
                         '    }\n')
        end

        if (builder:getattrib('fresnel') == 'true') then
            if (hasNormalByPixel) then
                builder:puts('    float facing = 1.0f - max(dot(eyeVec, modelSurfaceNormal), 0.0f);\n')
            else
                builder:puts('    float facing = 1.0f - max(dot(normalize(psIn.eyeVec), tangentSurfaceNormal), 0.0f);\n')
            end
        end

        if (builder:getcasevalue('LightType') == 'spot') then
            --builder:puts('    float4 lightModColor = tex2D(LightSampler, psIn.uvlight);\n');
            builder:puts('    float4 lightModColor = texCUBE(LightSampler, psIn.uvlight);\n');
        end

        if (builder:getcasevalue('LightEnv') == 'lmap') then
            builder:puts('    float lmapIntensity = tex2D(LightmapSampler, psIn.uvlightmap).a;\n')
        end

        if (withDiffuse or withSpecular) then -- if light  is static then specular light need a lightmap
            -- diffuse lightmap
            if (builder:getattrib('lightmap') == 'true') then  -- the lightmap is needed in diffuse lighting
                builder:puts('    // diffuse lightmap\n')
                builder:puts('    float4 lightMapColor = tex2D(LightmapSampler, psIn.uv1);\n')
            end

            if (builder:getattrib('terrainlightmap') == 'true') then
                builder:puts('    // diffuse lightmap\n')
                builder:puts('    float4 lightMapValue = tex2D(LightmapSampler, psIn.uv1.xy);\n')
                builder:puts('    float4 lightMapColor = TerrainDiffuseFactor * LightDiffuse * float4(lightMapValue.a, lightMapValue.a, lightMapValue.a, 0.0f);\n')
            end
        end

        if (withDiffuse) then
            -- ambient color
            --if (level < LEVEL_UNLIT)

            -- Per-vertex color
            if (builder:getattrib('vcolormatambient') == 'true') then
                vertexMatAmbient = "psIn.color"
                vertexMatDiffuse = "psIn.color"
            else
                vertexMatAmbient = "matAmbient"
                vertexMatDiffuse = "matDiffuse"
            end

            if (hasMatAmbient) then
                builder:puts('    float4 vertexMatAmbient = ' .. vertexMatAmbient .. ';\n')
            end

            if (hasLight and (builder:getattrib('diffuselighting') == 'true')) then
                builder:puts('    float4 vertexMatDiffuse = ' .. vertexMatDiffuse ..';\n')
            end

            -- diffuse lighting
            --if (level < LEVEL_UNLIT)
            if (hasLight) then
                if (builder:getattrib('diffuselighting') == 'true') then
                    if (hasTangentSpace) then
                        if (hasNormalByPixel) then
                            builder:puts('    float diffIntensity = dot(modelSurfaceNormal, lightVec);\n')
                        else
                            -- per pixel diffuse lighting
                            builder:puts('    // per-pixel diffuse lighting\n')
                            builder:puts('    float3 tangentLightVec = normalize(psIn.lightVec);\n')
                            builder:puts('    float diffIntensity = dot(tangentSurfaceNormal, tangentLightVec);\n')
                        end

                        if (hasLightMap) then -- Calculate the correction of diffIntensity
                            
                            builder:puts('    if (lightStatic) {\n')
                            if (not hasNormalByPixel) then
                                builder:puts('        diffIntensity = LightMapDiffuseFactor*clamp(diffIntensity,-1,1);\n')
                            else
                                builder:puts('        diffIntensity = saturate(diffIntensity);\n')
                                builder:puts('        diffIntensity -= saturate(saturate(dot(normal, lightVec)));\n')
                            end
                            builder:puts('    }\n')
                            builder:puts('    else \n')
                            builder:puts('    {\n')
                            builder:puts('         diffIntensity = saturate(diffIntensity);\n')
                            builder:puts('    }\n')
                        elseif  builder:getattrib('decalmodulation') == 'true' then
                            --Compesate the surface.
                            builder:puts('    //decalmodulation, the original surface, need a negative values to darked\n')
                            if (hasNormalByPixel) then
                                builder:puts('    diffIntensity -= dot(modelSurfaceNormal2, lightVec);\n')  
                            else
                                builder:puts('    diffIntensity -= dot(float3(0.0,0.0,1.0), tangentLightVec);\n')  
                            end
                            -- Not saturate the didffintensity
                            
                        else
                            builder:puts('    diffIntensity = saturate(diffIntensity);\n')
                        end

                        --light attenuation
                        builder:puts('    diffIntensity *= lightAtt;\n')

                        builder:puts('    float4 diffuse = diffIntensity * LightDiffuse'..lightindex..';\n')

                        -- no diffuse color in place with shadow.
                        -- other solution is use a lightmap with threshold
                        if (hasAnyLightMap) then
                            builder:puts('    if (lightStatic) {\n')
                            builder:puts('        diffuse *= lightMapColor;\n');
                            builder:puts('    }\n')
                        end
                    else
                        -- per vertex diffuse lighting
                        builder:puts('    // per vertex diffuse lighting\n')
                        builder:puts('    float4 diffuse = psIn.diffuse*lightAtt;\n')
                    end

                    if (builder:getcasevalue('LightType') == 'spot') then
                        if (numLights > 1) then
                            builder:puts('    if (lightType == LIGHT_SPOT) {\n')
                        end
                        builder:puts('    diffuse *= lightModColor; //Only the first light is spot\n')
                        if (numLights > 1) then
                            builder:puts('    }\n')
                        end
                    end

                    if (builder:getcasevalue('LightEnv') == 'lmap') then
                        builder:puts('    if (lightStatic) {\n')
                        builder:puts('        diffuse *= lmapIntensity;\n')
                        builder:puts('    }\n')
                    end
                
                    -- diffuse color
                    if (hasTangentSpace) then
                        builder:puts('    // tangent space diffuse lighting\n')
                        builder:puts('    diffuse *= vertexMatDiffuse;\n')
                    end
                    
                    if  builder:getattrib('decalmodulation') == 'true' then
                        builder:puts('    // modulate decal, 0 darked 1 lighted\n')
                        builder:puts('    diffuse += float4(0.5,0.5,0.5,0.0);\n')
                        -- clamped 0 to 1
                        builder:puts('    diffuse = saturate(diffuse);\n')
                        
                    end

                    builder:puts('    dstColor = diffuse;\n')
                end
            end

            -- ambient lighting (even if not in a light case)
            if (builder:getattrib('ambientlighting') == 'true' and hasLight) then
                builder:puts('    float4 ambient = lightAtt * LightAmbient'..lightindex..';\n')
                
                if (hasAnyLightMap) then
                    builder:puts('    if (lightStatic) {\n')
                    if (hasLightMap) then
                        builder:puts('        ambient *= LightMapAmbientFactor;\n')
                    elseif (builder:getattrib('terrainlightmap') == 'true') then
                        builder:puts('        ambient *= TerrainAmbientFactor;\n')
                    end
                    builder:puts('    }\n')
                end
                
                if (builder:getcasevalue('LightType') == 'spot') then
                    if (numLights > 1) then
                        builder:puts('    if (lightType == LIGHT_SPOT) {\n    ')
                    end
                    builder:puts('    ambient *= lightModColor; \n')
                    if (numLights > 1) then
                        builder:puts('    }\n')
                    end
                end                
                
            else
                builder:puts('    float4 ambient = float4(0.0f, 0.0f, 0.0f, 0.0f);\n')
            end
            
                                  
            local identation = '    '
            if hasAnyAutoIlu then
                builder:puts('    if (useAutoIlumination)\n')
                builder:puts('    {\n')
                identation = '        '
            end

            if (hasAnyLightMap) then

                builder:puts(identation .. 'ambient += lightMapColor;\n')
            end

            -- emissive map
            if (builder:getattrib('illuminationmap') == 'true') then
                builder:puts(identation..'// illumination map\n')
                builder:puts(identation..'ambient += tex2D(EmissiveSampler, uv0);\n')
            end

            -- emissive color
            if (builder:getattrib('illuminationcolor') == 'true') then
                builder:puts(identation..'// self illumination\n')
                builder:puts(identation..'ambient += illuminationColor;\n')
            end
            
            if hasAnyAutoIlu then
                builder:puts('    }\n')
            end

            if (hasMatAmbient) then
                builder:puts('    // ambient lighting\n')
                builder:puts('    dstColor += ambient * vertexMatAmbient;\n')
            else
                -- no ambient or diffuse lighting (level == LEVEL_EDITOR)
                if (builder:getattrib('level') == 'editor') then
                    builder:puts('    dstColor = float4(1.0f, 1.0f, 1.0f, 1.0f);\n')
                end
            end

            -- USe terrain Color
            if ((builder:getattrib('terrainlightmap') == 'true') and
                (builder:getattrib('useterraincolor') == 'true')) then
                builder:puts('    float4 TerrainColor = lerp(float4(1.0,1.0,1.0,1.0), lightMapValue , psIn.uv1.z);\n')
                builder:puts('    dstColor *= TerrainColor;\n')
            end

            -- restore alpha value
            if (builder:getattrib('alphablend') == 'true') then
                builder:puts('    // restore alpha value\n')
                if (builder:getattrib('ambientlighting') == 'true') then
                    builder:puts('    dstColor.a = vertexMatAmbient.a;\n')
                else
                    builder:puts('    dstColor.a = 1.0;\n')
                end
            end

            -- end.ambient

            -- diffuse map
            if (builder:getattrib('colormap') == 'true') then

                if (builder:getattrib('controlmap') ~= 'true' and
                    builder:getattrib('vcolorblend') ~= 'true') then
                    builder:puts('    // diffuse map\n')
                    builder:puts('    float4 albedoColor = tex2D(DiffSampler, uv0);\n')

                else
                    local blendFactor = ''

                    if (builder:getattrib('vcolorblend') == 'true') then
                        -- vertex color blend
                        if (builder:getattrib('controlmap') ~= 'true') then
                            blendFactor = 'psIn.color'

                        else
                            blendFactor = 'lerp(tex2D(ControlSampler, psIn.uv2), float4(1.0f, 1.0f, 1.0f, 1.0f), psIn.color)'
                        end

                    else
                        -- control map
                        blendFactor = 'tex2D(ControlSampler, psIn.uv2)'
                    end

                    builder:puts('    // blend map1 1 with map2 by controlmap, and apply\n')
                    builder:puts('    float4 albedoColor = lerp(tex2D(DiffSecSampler, uv0), tex2D(DiffSampler, uv0), ' .. blendFactor .. ');\n')

                end
                
                if ((builder:getattrib('terrainlightmap') == 'true') and
                    (builder:getattrib('useterraincolor') == 'true')) then
                    builder:puts('    float4 albedoModulateByterrain = ModulateGMMColor( albedoColor, ModulationSampler, psIn.uvTerrainGlobal, TerrainModulationFactor );\n')
                    -- psIn.uv1.z = 1 get the terrain color, the lightmap contains the modulationfactor not need modulate by ModulationSampler
                    builder:puts('    albedoColor = lerp(albedoModulateByterrain, albedoColor , psIn.uv1.z);\n')
                end

                builder:puts('    dstColor *= albedoColor;\n')
                
                if (builder:getattrib('decalmodulation') == 'true') then
                    if hasLight and hasTangentSpace and (builder:getattrib('diffuselighting') == 'true') then
                        -- If use a normal map need 
                        builder:puts('    dstColor.xyz = 2.f*(dstColor.xyz);//other posibility = 2.*(dstColor.xyz)\n')
                    else
                        builder:puts('    dstColor = albedoColor;\n')
                    end
                end
            end

            -- environment lerp factor
            if (builder:getattrib('environmentmap2d') == 'true') or (builder:getattrib('environmentmap3d') == 'true') then
                builder:puts('    float envLerp = envMaskFactor;\n')
                
                if (hasFading) then
                    builder:puts('    envLerp *= psIn.smooth;\n')
                end
                
                if (builder:getattrib('levelmapasenvmaskmap') == 'true') then
                    builder:puts('    envLerp *= tex2D(LevelSampler, uv0);\n')
                else
                    builder:puts('    envLerp *= tex2D(EnvMask, uv0);\n')
                end
            end

            -- diffuse environment map
            if (builder:getattrib('environmentmap2d') == 'true') then
                builder:puts('    // diffuse environmentmap2d\n')
                builder:puts('    dstColor.xyz = lerp(dstColor.xyz, tex2D(Env2DSampler, psIn.uvenv), envLerp) ;\n')
            end

            -- environment cube map
            if (builder:getattrib('environmentmap3d') == 'true') then
                --if (level < LEVEL_TANGENT)
                builder:puts('    // diffuse environmentmap3d\n')
                if (hasNormalByPixel) then
                    builder:puts('    //reflect inverts the direction eyedir;\n');
                    builder:puts('    float3 refl = mul(reflectEye, Model);\n');
                    
                    if (hasFading) then
                        builder:puts('    refl *= psIn.smooth;\n')
                    end

                    builder:puts('    dstColor.xyz = lerp(dstColor.xyz , texCUBE(EnvironmentSampler, refl), envLerp);\n')
                else
                    builder:puts('    dstColor.xyz = lerp(dstColor.xyz, texCUBE(EnvironmentSampler, psIn.uvenv), envLerp) ;\n')
                end
            end

        end --if (withDiffuse)

        if (withSpecular) then
            -- specular lighting
            --if (level < LEVEL_UNLIT)
            if (builder:getattrib('specularlighting') == 'true' and hasLight) then
                builder:puts('\n')

                -- level color
                if (builder:getattrib('levelmap') == 'true') then
                    builder:puts('    float4 levelMapColor =  tex2D(LevelSampler, uv0);\n')
                    builder:puts('    float4 levelColor = matLevel * levelMapColor;\n')
                    builder:puts('    // the alpha chanel contains the glosiness, if it has not alpha the it always is 1.0f;\n')
                    builder:puts('    // and the shininess never less than 0;\n')
                    builder:puts('    float shininess = (matShininess-1)*levelMapColor.a+1;\n')
                else
                    builder:puts('    float4 levelColor = matLevel;\n')
                    builder:puts('    float shininess = matShininess;\n')
                end

                if (hasTangentSpace) then
                    if (hasNormalByPixel) then
                        builder:puts('    float  specIntensity = pow(saturate(dot(reflectEye, lightVec)), shininess);\n')
                        builder:puts('    float4 specular = specIntensity * LightSpecular'..lightindex..';\n')
                    else
                        builder:puts('    // per pixel specular lighting\n')
                        builder:puts('    float3 tangentHalfVec = normalize(psIn.halfVec);\n')
                        builder:puts('    float  specIntensity = pow(saturate(dot(tangentSurfaceNormal, tangentHalfVec)), shininess);\n')
                        builder:puts('    float4 specular = specIntensity * LightSpecular'..lightindex..';\n')
                    end
                    builder:puts('    specular *= matSpecular;\n')
                else
                    builder:puts('    // per vertex specular lighting\n')
                    builder:puts('    float4 specular = psIn.specular;\n')
                end
                
                builder:puts('   specular = lightAtt*specular;\n')

                if (builder:getcasevalue('LightType') == 'spot') then
                    if (numLights > 1) then
                        builder:puts('    if (lightType == LIGHT_SPOT) {\n')
                    end
                    builder:puts('    specular *= lightModColor; //Only the first light is spot \n')
                    if (numLights > 1) then
                        builder:puts('    }\n')
                    end
                end

                if (builder:getcasevalue('LightEnv') == 'lmap') then
                    builder:puts('    if (lightStatic) {\n')
                    builder:puts('        specular *= lmapIntensity;\n')
                    builder:puts('    }\n')
                end

                -- In static lights,  modulate specular lighting by lightmap
                if (hasAnyLightMap) then
                    builder:puts('\n    // modulate LightMap diffuse\n')
                    builder:puts('    if (lightStatic) {\n')
                    builder:puts('        specular *= 2.0 * lightMapColor;\n')
                    builder:puts('    }\n')
                end

                if (hasFading) then
                    builder:puts('    levelColor *= psIn.smooth;\n')
                end

                builder:puts('    dstColor += specular * levelColor;\n')
            end
        end --if (withSpecular)

        -- fog
        if (hasFog) then
            if (builder:getcasevalue('fogmode') ~= 'lnrf') then
                builder:puts('    // fog\n')
                --bug #1630- attenuate specular+alpha with fog factor
                if (withSpecular and builder:getattrib('alphablend') == 'true') then
                    builder:puts('    dstColor.rgb *= psIn.fog;\n')
                else
                    builder:puts('    dstColor.rgb = lerp(fogColor.rgb, dstColor.rgb, psIn.fog);\n')
                end
            end
        end

        -- clipmap
        if (builder:getattrib('clipmap') == 'true') then
            builder:puts('    // alpha test clipping\n')
            --builder:puts('    float clipValue = tex2D(ClipSampler, uv0).r;\n')
            builder:puts('    float clipValue = tex2D(DiffSampler, uv0).a;\n')

            if (builder:getattrib('fadeclipmap') == 'true') then
                builder:puts('    clipValue = lerp(clipValue, step(50.0f / 255.0f, clipValue), psIn.smooth);\n')
            end
            
            if (builder:getattrib('laserreceiver') == 'true') then
                -- Simulate the clip,  if it a solid use the previous alpha
                builder:puts('    clipValue =  clipValue <  (50.f / 255.f) ? 0.f : dstColor.a;\n')
            end
            
            builder:puts('    dstColor.a = clipValue;\n')

            if (builder:getattrib('grassfadeout') == 'true') then
                builder:puts('    float grassFactor;\n')
                builder:puts('    float perlin = tex2D(NoiseSampler, uv0).r;\n')
                builder:puts('    grassFactor = psIn.grassFadeOut < perlin ? 1.0f : 0.0f;\n')
                builder:puts('    dstColor.a*=grassFactor;\n')
            end
        end
        
        if (builder:getattrib('laserreceiver') == 'true') then
            -- The near plane is the begin of dot laser
            --builder:puts('    dstColor.a = (0.f >  uvlight.z ) ? 0.f : dstColor.a;\n')
            builder:puts('   //// vmin, vmax, zmin and zmax is in pre-shader;\n')
            builder:puts('   float4 vMin = float4(LightPos.xyz + (LightDir.xyz - LightDir.w*normalize(LightDir.xyz)) , 1.f);\n')
            builder:puts('   float4 vMax = float4(LightPos.xyz + (LightDir.xyz + LightDir.w*normalize(LightDir.xyz)) , 1.f);\n')
            builder:puts('   vMin = mul(vMin, InvModel);\n')
            builder:puts('   vMax = mul(vMax, InvModel);\n')
            builder:puts('   vMin = mul(vMin, ModelLightProjection);\n')
            builder:puts('   vMax = mul(vMax, ModelLightProjection);\n')
            builder:puts('   float  zMin = vMin.z/vMin.w;\n')
            builder:puts('   float  zMax = vMax.z/vMax.w;\n')
            builder:puts('   dstColor.a = (zMin >  uvlight.z ) ? 0.f : dstColor.a;\n')
            builder:puts('   dstColor.a = (zMax <  uvlight.z ) ? 0.f : dstColor.a;\n')
        end

        -- per pixel fresnel term
        if (builder:getattrib('fresnel') == 'true') then
            builder:puts('    float fresnel = FresnelBias + (1.0 - FresnelBias) * pow(facing, FresnelPower);\n')
            builder:puts('    dstColor *= fresnel;\n')
            builder:puts('    dstColor.a = fresnel * 0.5 + 0.5;\n')
        end
        
        if  builder:getattrib('decalmodulation') == 'true' then
            builder:puts('    // decal modulation, 0.5 not change the ilumination of destination pixel\n')
            builder:puts('    dstColor.xyz= lerp(float3(0.5,0.5,0.5) , dstColor.xyz , dstColor.a);\n')
        end

        --Debug normals
        if (hasLight and hasNormalByPixel) then
            --builder:puts('    dstColor.xyz = 0.5f*(1.0f + modelSurfaceNormal);\n')
            --builder:puts('    dstColor.xyz = 0.5f*(1.0f + reflectEye);\n')
            --builder:puts('    dstColor.xyz = 0.5f*(1.0f + eyeVec);\n')

            --builder:puts('    dstColor.xyz = 0.5f*(1.0f + tangent);\n')
            --builder:puts('    dstColor.xyz = 0.5f*(1.0f + tangentSurfaceNormal);\n')
            --builder:puts('    dstColor.xyz = tangent;\n')
        end

        if (hasNormalByPixel) then
            --builder:puts('    return normal;\n')
        end

        if (hasShadowDepth) then
            if (builder:getattrib('clipmap') == 'true') then
                builder:puts('    clip(clipValue - 0.5f);\n')
            end
            --builder:puts(' return EncodeToRGBA_DXT5(psIn.depth);\n')
            builder:puts(' return EncodeToRGBA(psIn.depth);\n')
            --builder:puts('    return psIn.depth;\n')
        else
            builder:puts('    return dstColor;\n')
        end

        builder:puts('}\n')
        builder:puts('\n')

    end

    ----------------------------------------------------------------------------
    --  CreatePass
    ----------------------------------------------------------------------------
    local function CreatePass(passName, vsMain, psMain, cullMode, alphaBlendEnable, srcBlend, destBlend, shadowEnabled, fogEnabled)

        -- Pass header
        builder:puts('    pass p' .. passName .. '\n')
        builder:puts('    {\n')

        -- shadow volumes
        if (shadowEnabled == '0') then
            builder:puts('        StencilFail      = Keep;\n')
            builder:puts('        StencilPass      = Keep;\n')
            builder:puts('        StencilZFail     = Incr;\n')

        elseif (shadowEnabled == '1') then
            builder:puts('        StencilZFail     = Decr;\n')
        end

        -- cullmode
        if (cullMode ~= '') then
            builder:puts('        CullMode         = ' .. cullMode .. ';\n')
        end

        -- alpha blending
        if (not hasShadowVolume) then
            if (alphaBlendEnable) then
                builder:puts('        AlphaBlendEnable = True;\n')
                builder:puts('        SrcBlend         = ' .. srcBlend .. ';\n')
                builder:puts('        DestBlend        = ' .. destBlend .. ';\n')
            else
                builder:puts('        AlphaBlendEnable = False;\n')
            end
        end

        -- alpha testing
        if (builder:getattrib('clipmap') == 'true') and
           (builder:getattrib('shadowdepth') ~= 'true') -- the shadowdepth need the alpha, then it use a clip function.
           then
--            builder:puts('        AlphaTestEnable  = True;\n')
--            builder:puts('        AlphaRef         = 128;\n')
--            builder:puts('        AlphaFunc        = GREATER;\n')
        else
--            builder:puts('        AlphaTestEnable  = False;\n')
        end

        -- fog
        if (hasFog) then
            if (builder:getcasevalue('fogmode') == 'lnrf') then
                builder:puts('        FogEnable        = ' .. fogEnabled .. ';\n')
                builder:puts('        fogColor         = <fogColor>;\n')
                builder:puts('        FogVertexMode    = Linear;\n')
                builder:puts('        FogTableMode     = None;\n')
            end
        else
--            builder:puts('        FogEnable        = False;\n')
        end

        -- depth bias
        if builder:getattrib('depthbias') ~= 'true' then
--            builder:puts('        DepthBias        = 0.0f;\n')
        else
--            builder:puts('        DepthBias        = -0.00001f;\n')
        end

        -- vertex and pixel shaders
        builder:puts('        \n')
        builder:puts('        VertexShader = ' .. vsMain .. ';\n')
        builder:puts('        PixelShader  = ' .. psMain .. ';\n')
        builder:puts('    }\n')

    end

    -- get builder
    builder = lookup(nebula.sys.servers.material:getbuilder())

    -- local vars
    hasModelView    = (builder:getcasevalue('fogmode') ~= '') or
                      (builder:getattrib('grassfadeout') == 'true') or
                      (builder:getattrib('shadowdepth') == 'true') or
                      (builder:getattrib('rendernormal') == 'true')
    hasProjection   = (builder:getattrib('shadowdepth') == 'true')
                      
    hasInvModelView = (builder:getattrib('billboard') == 'true')
    hasModel        = (builder:getcasevalue('fogmode') == 'lyrf') or
                      (builder:getcasevalue('LightEnv') == 'lmap') or
                      (builder:getattrib('environmentmap3d') == 'true') or
                      (builder:getattrib('terrainlightmap') == 'true')
    hasUV0          = (builder:getattrib('colormap') ~= '') or
                      (builder:getattrib('clipmap') ~= '') or
                      (builder:getattrib('levelmap') ~= '') or
                      (builder:getattrib('illuminationmap') ~= '') or
                      (builder:getattrib('environmentmap3d') == 'true') or
                      (builder:getattrib('environmentmap2d') == 'true') or -- enviroment mask
                      (builder:getattrib('parallax') == 'true') or
                      (builder:getattrib('bumpmap') == 'true')
    hasNormal       = (builder:getattrib('environmentmap3d') == 'true') or
                      (builder:getattrib('environmentmap2d') == 'true') or
                      (builder:getattrib('parallax') == 'true') or
                      (builder:getattrib('rendernormal') == 'true')
    hasEyeDir       = (builder:getattrib('billboard') == 'true') or
                      (builder:getattrib('specularlighting') == 'true') or
                      (builder:getattrib('environmentmap3d') == 'true') or
                      (builder:getattrib('environmentmap2d') == 'true') or
                      (builder:getattrib('parallax') == 'true') or
                      (builder:getattrib('fresnel') == 'true')

    hasShadowVolume = builder:getattrib('shadowvolume') == 'true'
    hasShadowDepth  = builder:getattrib('shadowdepth') == 'true'

    --if (level < LEVEL_UNLIT)
    --hasFading       = false
    hasFading       = ((builder:getattrib('fadeclipmap') == 'true') or
                      (builder:getattrib('bumpmap') == 'true') or
                      (builder:getattrib('normalmap') == 'true') or
                      (builder:getattrib('specularlighting') == 'true') or
                      (builder:getattrib('environmentmap3d') == 'true') or
                      (builder:getattrib('environmentmap2d') == 'true')
                      ) and
                      ( not (builder:getattrib('parallax') == 'true') ) --because not has enought aritmetic instructions in ps2_0
    hasFading = false -- Demo polish and bug-fixing for Leipzig conference 2006,  to many shaders not compile in ps_20
                     
                      
                      
    hasModelView    = hasModelView or hasFading
    
    hasFog          = builder:getcasevalue('fogmode') ~= ''
    hasLight        = builder:getcasevalue('LightType') ~= ''
    
    hasLightVector  = hasLight and ( (builder:getattrib('diffuselighting') == 'true') or (builder:getattrib('specularlighting') == 'true') )
    hasNormal       = hasNormal or hasShadowVolume or hasLightVector

    hasMatAmbient   = (builder:getattrib('ambientlighting') and
                      (builder:getattrib('alphablend') == 'true' or hasLight)) or
                      (builder:getattrib('illuminationcolor') == 'true') or
                      (builder:getattrib('illuminationmap') == 'true') or
                      (builder:getattrib('lightmap') == 'true')

    --if (level < LEVEL_FLAT)
    hasTangentSpace = (builder:getattrib('bumpmap') ~= '' or builder:getattrib('normalmap') ~= '') and hasLight or
                      (builder:getattrib('parallax') == 'true') or
                      (builder:getattrib('fresnel') == 'true')

    --if (level < LEVEL_PIXEL)
    hasNormalByPixel = (hasTangentSpace or builder:getattrib('environmentmap3d') == 'true') and
                       (builder:getattrib('level') == 'pro')

    lightIsStatic   = (builder:getcasevalue('LightType') ~= 'spot') -- Todo: this is temporal hack for use dinamic and static lights
    hasLightMap     = (builder:getattrib('lightmap') == 'true')
    hasAnyLightMap  = hasLightMap or (builder:getattrib('terrainlightmap') == 'true')
    hasAnyAutoIlu   = hasAnyLightMap or
                      (builder:getattrib('illuminationmap') == 'true') or
                      (builder:getattrib('illuminationcolor') == 'true')

    -- determine number of lights
    if (hasLight) then
        numLights = 0
        while builder:getcasevalue('LightType('..numLights..')') ~= '' do
            numLights = numLights + 1
        end
        -- etc.
    else
        numLights = 0
    end

    -- file header
    CreateHeader()

    -- variable declarations
    CreateVar()

    -- texture samplers
    CreateSamplers()

    -- vertex declaration
    CreateVertexDeclaration()

    -- vertex shader body
    CreateVertexShader('vsMain', false)

    if (builder:getattrib('alphatwoside') == 'true') then
        CreateVertexShader('vsMain_ccw', true)
    end

    -- pixel shader body
    hasSpecular = hasLight and (builder:getattrib('specularlighting') == 'true')

    if (builder:getattrib('alphablend') == 'true') then
        -- first pass only diffuse
        CreatePixelShader('psMain_pass0', true, false)
        -- second pass only specular
        if (hasSpecular) then
            CreatePixelShader('psMain_pass1', false, true)
        end
    else
        CreatePixelShader('psMain', true, true)
    end

    -- create vertex shader array
    local function CreateVertexShaderArray(arrayName, shaderName, lightIndex)

        local paramIndex = ''
        if (lightIndex ~= '') then
            paramIndex = ', '..lightIndex
        end

        builder:puts('VertexShader ' .. arrayName .. '[4] = {\n')
        --TODO: iterate over all cases to allow all combinations
        builder:puts('             compile VS_CURRENT '..shaderName..'(LIGHT_POINT, true'..paramIndex..'),\n')
        builder:puts('             compile VS_CURRENT '..shaderName..'(LIGHT_SPOT,  false'..paramIndex..'),\n')
        builder:puts('             compile VS_CURRENT '..shaderName..'(LIGHT_DIR,   true'..paramIndex..'),\n')
        builder:puts('             compile VS_CURRENT '..shaderName..'(LIGHT_BULB,  true'..paramIndex..')\n')
        --...morpher cases with VertexStreams selector
        builder:puts('};\n')
        
        if (lightIndex ~= '') then
            return '('..arrayName..'[LightType['..lightIndex..']])'
        else
            return '('..arrayName..'[LightType])'
        end
    end
    
    local function SearchTheFirstStaticLight()
        local lightindex = 0
        local lighttmp = builder:getcasevalue('LightType('..lightindex..')')
        while lighttmp ~='pntl'and lighttmp ~='dirl' and lighttmp~= nil do
            lightindex = lightindex + 1
            lighttmp = builder:getcasevalue('LightType('..lightindex..')')
        end
        if lighttmp ~='pntl'and lighttmp ~='dirl' then
            return -1
        else
            return lightindex
        end
    end
    

    -- create pixel shader array
    local function CreatePixelShaderArray(arrayName, shaderName, lightIndex, useLightmap)
    
        local function CreatePixelShader(lightType, lightStatic, lightIndex, useLightmap)
        
            local spaces = '                                     '  
            
            local paramLightStatic = spaces..','..tostring(lightStatic)..'//// Light static\n'
            
            local paramIndex = ''
            if (lightIndex ~= '') then
                paramIndex = spaces..','..lightIndex..'//// the light index\n'
            end
            
            local paramLightMap = ''
            if hasAnyAutoIlu then
                paramLightMap = spaces..','..tostring(useLightmap)..' //// use lightmap and autoilumination\n' 
            end
            
            local pixelShader = ''

            pixelShader = '             compile PS_CURRENT '..shaderName..'(\n' ..
                          spaces .. lightType..'\n' ..
                          paramLightStatic ..
                          paramIndex ..
                          paramLightMap ..
                          spaces .. ')'
        
            return pixelShader
        end
        

        
        builder:puts('PixelShader  ' .. arrayName .. '[4] = {\n')
        --TODO: iterate over all cases to allow all combination
        builder:puts( CreatePixelShader('LIGHT_POINT', true,  lightIndex, useLightmap) .. ',\n' )
        builder:puts( CreatePixelShader('LIGHT_SPOT',  false, lightIndex, useLightmap) .. ',\n' )
        builder:puts( CreatePixelShader('LIGHT_DIR',   true,  lightIndex, useLightmap) .. ',\n' )
        builder:puts( CreatePixelShader('LIGHT_BULB',  true, lightIndex, useLightmap) .. '\n'  )
        builder:puts('};\n')
        
        if (lightIndex ~= '') then
            return '('..arrayName..'[LightType['..lightIndex..']])'
        else
            return '('..arrayName..'[LightType])'
        end
    end

    -- create shader arrays for light types
    if (hasLight) then
        if (numLights > 1) then
            vsArray = {}
            psArray = {}
            vsArray_ccw = {}
            psArray_pass0 = {}
            psArray_pass1 = {}
            
            -- Use lightmap only in the first point light
            local useLightmapIdx = SearchTheFirstStaticLight()

            for index = 0, numLights - 1 do
                vsArray[index] = CreateVertexShaderArray('vsArray_light'..index, 'vsMain', index)
                local useLightmap = useLightmapIdx == index
                
                if (builder:getattrib('alphablend') == 'false') then
                    psArray[index] = CreatePixelShaderArray('psArray_light'..index, 'psMain', index, useLightmap)
                else
                    if (builder:getattrib('alphatwoside') == 'true') then
                        vsArray_ccw[index] = CreateVertexShaderArray('vsArray_ccw_light'..index, 'vsMain_ccw', index)
                    end
                    psArray_pass0[index] = CreatePixelShaderArray('psArray_pass0_light'..index, 'psMain_pass0', index,useLightmap)
                    if (hasSpecular) then
                        psArray_pass1[index] = CreatePixelShaderArray('psArray_pass1_light'..index, 'psMain_pass1', index,false)
                    end
                end
            end
        else
            vsMain = CreateVertexShaderArray('vsArray', 'vsMain', '')
            
            if (builder:getattrib('alphablend') == 'false') then
                psMain = CreatePixelShaderArray('psArray', 'psMain', '',true)
            else
                if (builder:getattrib('alphatwoside') == 'true') then
                    vsMain_ccw = CreateVertexShaderArray('vsArray_ccw', 'vsMain_ccw', '')
                end
                psMain_pass0 = CreatePixelShaderArray('psArray_pass0', 'psMain_pass0', '',true)
                if (hasSpecular) then
                    psMain_pass1 = CreatePixelShaderArray('psArray_pass1', 'psMain_pass1', '',false)
                end
            end
        end
        builder:puts('\n')
    else
    
        local paramUseLightMap = ''
        if hasAnyAutoIlu then
            paramUseLightMap = 'true'
        end
    
    
        --all other cases
        vsMain = 'compile VS_CURRENT vsMain()'
        psMain = 'compile PS_CURRENT psMain('..paramUseLightMap..')'
        
        vsMain_ccw = 'compile VS_CURRENT vsMain_ccw()'
        

        
       psMain_pass0 = 'compile PS_CURRENT psMain_pass0('..paramUseLightMap..')'
       psMain_pass1 = 'compile PS_CURRENT psMain_pass1('..paramUseLightMap..')'
        
    end

    -- technique and passes
    builder:puts('technique t0\n')
    builder:puts('{\n')

    -- create pass
    --if (builder:getattrib('notculling') == 'true') and not (builder:getattrib('alphatwoside') == 'true') then
    --    cullMode = 'None'
    --else
    --    cullMode = 'CW'
    --end
    
    -- CreatePass(name, vertexShader, pixelShader, cullMode, alphaBlendEnable, SrcBlend, DestBlend, shadowEnabled, fogEnabled)
    if (hasShadowVolume) then
        CreatePass('_shadow0', vsMain, psMain, 'CCW', false, '', '', '0', 'True')
        CreatePass('_shadow1', vsMain, psMain, 'CW', false, '', '', '1', 'True')
    else
        if (builder:getattrib('laserreceiver') == 'true' ) then
            CreatePass('_CW_DIFF', vsMain, psMain, '', true, 'SrcAlpha', 'One', '', 'True')

        else
            local destBlend = '<funcDestBlend>'
            local srcBlend = 'SrcAlpha'
            local destBlend2Light = 'One'
            local srcBlend2Light = 'SrcAlpha'             
            if (builder:getattrib('decalmodulation') == 'true' ) then
                destBlend = 'SrcColor'
                srcBlend = 'DestColor'
                destBlend2Light = destBlend
                srcBlend2Light = srcBlend
            end
            
            if (numLights > 1) then
                if (builder:getattrib('alphablend') == 'false') then
                    for index = 0, numLights - 1 do
                        alphaBlendEnable = (index > 0)
                        CreatePass('Light'..index, vsArray[index], psArray[index], '', alphaBlendEnable, 'One', 'One', '', 'True')
                    end
                else
                    cullMode = ''

                    -- for two-sided alpha, draw first all inner surfaces for all lights
                    if (builder:getattrib('alphatwoside') == 'true') then
                        cullMode = 'CCW'
                        CreatePass('_CCW_DIFF_light'..0, vsArray_ccw[0], psArray_pass0[0], cullMode, true, srcBlend, destBlend, '', 'True')

                        for index = 1, numLights - 1 do
                            CreatePass('_CCW_DIFF_light'..index, vsArray_ccw[index], psArray_pass0[index], cullMode, true, srcBlend2Light, destBlend2Light, '', 'True')
                        end
                        
                        if (hasSpecular) then
                            for index = 0, numLights - 1 do
                                CreatePass('_CCW_SPEC_light'..index, vsArray_ccw[index], psArray_pass1[index], cullMode, true, 'One', 'One', '', 'True')
                            end
                        end
                        
                        cullMode = 'CW'
                    end
                    
                    CreatePass('_CW_DIFF_light'.. 0, vsArray[0], psArray_pass0[0], cullMode, true, srcBlend, destBlend, '', 'True')
                    for index = 1, numLights - 1 do
                        CreatePass('_CW_DIFF_light'..index, vsArray[index], psArray_pass0[index], cullMode, true, srcBlend2Light, destBlend2Light, '', 'True')
                    end
                    if (hasSpecular) then
                        for index = 0, numLights - 1 do
                            CreatePass('_CW_SPEC_light'..index, vsArray[index], psArray_pass1[index], cullMode, true, 'One', 'One', '', 'False')
                        end
                    end
                end

            else
                if (builder:getattrib('alphablend') == 'false') then
                    CreatePass('0', vsMain, psMain, '', false, '', '', '', 'True')
        
                else
                    -- passes for alpha blended surfaces
                    if (builder:getattrib('alphatwoside') == 'true') then
                        CreatePass('_CCW_DIFF', vsMain_ccw, psMain_pass0, 'CCW', true, srcBlend, destBlend, '', 'True')
                        if (hasSpecular) then
                            CreatePass('_CCW_SPEC', vsMain_ccw, psMain_pass1, 'CCW', true, 'One', 'One', '', 'True')
                        end
                    end
        
                    -- diffuse pass only, default blend
                    CreatePass('_CW_DIFF', vsMain, psMain_pass0, 'CW', true, srcBlend, destBlend, '', 'True')
                    if (hasSpecular) then
                        CreatePass('_CW_SPEC', vsMain, psMain_pass1, 'CW', true, 'One', 'One', '', 'False')
                    end
                end
            end
        end
    end

    builder:puts('}\n')

end
