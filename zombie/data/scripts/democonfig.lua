
function democonfig( )

    local view = nebula.app.renaissance.appstates.game
    local rnsview = nebula.usr.rnsview

    rnsview:getviewportentity():setpassenabled('edit',false)
    rnsview:loadstate 'home:data/appdata/renaissance/viewport/rnsview_hdrcam.n2'
    rnsview:setobeymaxdistance( true )  -- Set enable the max reder distance
    rnsview:setmaxrenderdistance( 260 )  -- put the max render distance
    rnsview:setfarplane( 5000 )
    rnsview:setnearplane( 0.15 )
   
    view:setmousexfactor( 3.2 )
    view:setmouseyfactor( 3.2 )
    
end
