--------------------------------------------------------------------------------
--   data/scripts/rnsstartup.lua
--
--   This is the central Renaissance runtime startup script.
--   The script runs any other required scripts
---------------------------------------------------------------------------------
sel('/sys/servers/file2')
call('setassign', 'luascript', 'home:data/scripts/')

dofile (mangle('luascript:nebthunker.lua'))
dofile (mangle('luascript:console.lua'))
dofile (mangle('luascript:mainfunctions.lua'))

dofile (mangle('luascript:conjurer_startup.lua'))
dofile (mangle('luascript:conjurer_input.lua'))
dofile (mangle('luascript:conjurer_materials.lua'))
dofile (mangle('luascript:conjurer_gui.lua'))

dofile (mangle('luascript:rnsconsole.lua'))
dofile (mangle('luascript:democonfig.lua'))
