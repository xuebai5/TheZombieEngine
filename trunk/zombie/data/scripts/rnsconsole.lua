--------------------------------------------------------------------------------
--  data/scripts/rnsconsole.lua
--
--  Renaissance console helper functions
--
--  (C) 2006 Conjurer Services, S.A.
---------------------------------------------------------------------------------

nebula.sys.servers.log:setlogenable( 'persistence', false )
nebula.sys.servers.log:setlogenable( 'resource', false )
nebula.sys.servers.log:setlogenable( 'entityObjectServer', false )
nebula.sys.servers.log:setlogenable( 'musicLog', false )

function fps( value )

    nebula.app.renaissance:showfps( value )

end

function shadows( value )

    nebula.usr.rnsview:getviewportentity():setpassenabled( 'shdv', value )

end

function hdr( value )

    if value  then
        nebula.usr.rnsview:loadstate( 'home:data/appdata/renaissance/viewport/rnsview_hdrcam.n2' )
    else
        nebula.usr.rnsview:loadstate( 'home:data/appdata/renaissance/viewport/rnsview_stdcam.n2' )
    end

end

function watch( watch )

    nebula.sys.servers.console:watch( watch )

end
