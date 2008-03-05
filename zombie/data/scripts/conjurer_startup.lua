--------------------------------------------------------------------------------
--  data/scripts/conjurer_startup.lua
--
--  Conjurer startup function
---------------------------------------------------------------------------------
---------------------------------------------------------------------------------
--   This procedure is called at startup
---------------------------------------------------------------------------------
function OnStartup()

    pushcwd('.')

    f = nebula.sys.servers.file2
    proj = f:manglepath('proj:')
    home = f:manglepath('home:')

    if (exists('/sys/servers/gfx')) then
        featureSet = nebula.sys.servers.gfx:getfeatureset()
        if ((featureSet == 'dx9') or (featureSet == 'dx9flt')) then
            f:setassign('shaders', home .. '/data/shaders/2.0/')
            puts('Shader directory: ' .. home .. '/data/shaders/2.0\n')
        else
            f:setassign('shaders', home .. '/data/shaders/fixed/')
            puts('Shader directory: ' .. home .. '/data/shaders/fixed\n')
        end
    else
        f:setassign('shaders', home .. '/data/shaders/2.0/')
        puts('Shader directory: ' .. home .. '/data/shaders/2.0\n')
    end

    popcwd()

    if (exists('/sys/servers/material')) then
        matserver = nebula.sys.servers.material
        matserver:setdatabase(home .. '/data/shaders/materials.xml')
    end

    nebula.sys.servers.persist:setdependencyclass('ngeometrynode', 'setsurface', 'nsurfacedependencyserver')
    nebula.sys.servers.persist:setdependencyclass('nmaterialnode', 'setmaterial', 'nmaterialdependencyserver')
    nebula.sys.servers.persist:setdependencyclass('nscenenode', 'addanimator', 'nanimatordependencyserver')
    nebula.sys.servers.persist:setdependencyclass('ncassetclass', 'setresourcefile', 'nassetdependencyserver')
    nebula.sys.servers.persist:setdependencyclass('ncloader', 'setbatchresource', 'nbatchdependencyserver')

    if (f:directoryexists('E:/renaissance/trunk/texturas')) then
        f:setassigngroup2('wctextures', 'E:/renaissance/trunk/texturas/', 'wc:export/textures/')
    else
        f:setassign('wctextures', 'wc:export/textures/')
    end

end

---------------------------------------------------------------------------------
--   This procedure is called when graphics are initialized
---------------------------------------------------------------------------------
function OnGraphicsStartup()

    -- empty

end

---------------------------------------------------------------------------------
--  This procedure is called when conjurer is fully initialized
--  All states are already loaded, all parameters are assigned, all
--  servers are up and ready, all viewports are open, levels is loaded etc.
--  Use it to perform any required additional initialization.
---------------------------------------------------------------------------------
function OnConjurerStartup()

    if (exists('/app/renaissance')) then
        democonfig()
    end

end
