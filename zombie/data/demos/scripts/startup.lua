--------------------------------------------------------------------------------
--  data/demos/scripts/startup.lua
--
--  Demo startup function
--------------------------------------------------------------------------------

sel('/sys/servers/file2')
call('setassign', 'luascript', 'home:data/scripts/')

dofile (mangle('luascript:nebthunker.lua'))
dofile (mangle('luascript:console.lua'))
dofile (mangle('luascript:mainfunctions.lua'))

--------------------------------------------------------------------------------
--   This procedure is called at startup
--------------------------------------------------------------------------------
function OnStartup()

    pushcwd('.')

    f = nebula.sys.servers.file2
    proj = f:manglepath('proj:')
    home = f:manglepath('home:')

    f:setassign('wc', proj)
    f:setassign('shaders', home .. '/data/shaders/2.0/')

    popcwd()

end

--------------------------------------------------------------------------------
--   This procedure is called when graphics are initialized
--------------------------------------------------------------------------------
function OnGraphicsStartup()

    -- empty

end

--------------------------------------------------------------------------------
--   Defines the standard input mappings for demos
--------------------------------------------------------------------------------
function OnMapInput()

    pushcwd('.')
    input = lookup('/sys/servers/input')
    
    input:beginmap()
    input:map( 'keyb0:esc.pressed', 'menu' )
    input:map( 'keyb0:left.pressed', 'left' )
    input:map( 'keyb0:right.pressed', 'right' )
    input:map( 'keyb0:up.pressed', 'up' )
    input:map( 'keyb0:down.pressed', 'down' )
    input:map( 'keyb0:space.pressed', 'reset' )
    input:endmap()
    popcwd()

end
