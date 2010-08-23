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
    
    input:map('mouse0:btn0.pressed', 'left_pressed')
    input:map('mouse0:btn1.pressed', 'right_pressed')
    
    input:map('relmouse0:-x', 'slider_left')
    input:map('relmouse0:+x', 'slider_right')
    input:map('relmouse0:-y', 'slider_up')
    input:map('relmouse0:+y', 'slider_down')

    input:map('keyb0:w.pressed', 'forward')
    input:map('keyb0:s.pressed', 'backward')
    input:map('keyb0:a.pressed', 'strafe_left')
    input:map('keyb0:d.pressed', 'strafe_right')
    
    input:map( 'keyb0:esc.pressed', 'menu' )
    input:map( 'keyb0:left.pressed', 'left' )
    input:map( 'keyb0:right.pressed', 'right' )
    input:map( 'keyb0:up.pressed', 'up' )
    input:map( 'keyb0:down.pressed', 'down' )
    
    input:map( 'keyb0:0.down', 'wireframe' )
    
    input:map( 'keyb0:space.down', 'reset' )
    input:map( 'keyb0:t.down', 'toggle' )
    input:map( 'keyb0:l.down', 'light' )

    input:endmap()
    popcwd()

end
