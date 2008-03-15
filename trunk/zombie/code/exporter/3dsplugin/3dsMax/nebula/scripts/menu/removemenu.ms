/**
   @file removemenu.ms
   @ingroup n3dsMaxScripts
   @author Cristobal Castillo Domingo
   @brief this script has functions for remove the menu in current version.

   (C) 2006 Conjurer Services, S.A.
*/

global nebMainMenu = "TragNebula2"
global nebQuadMainMenu = "TragNebula2Quad"
global nebProgramersMenu = "nProgramers"
global nebMaterialMenu = "nMaterial"


fn nebRemoveMenu =
(
    /** @Todo Think remove menu with new release
    */
    
    -- list of menu in preorder  hierarcy
    local listmenu = #(nebProgramersMenu, nebMainMenu)
    
    
    for nameMenu in listMenu do
    (
        local menu = menuMan.findMenu nameMenu;
        --format "check menu %\n" nameMenu
        if menu != undefined then
        (
            menuMan.unRegisterMenu menu;
            menuMan.updateMenuBar();
            --format "remove menu %\n" nameMenu
        )
    )  
    
    
    
    -- Set the default quad menu
    defaultMenu = menuman.findQuadMenu "Custom                    [Shift+Ctrl+RMB]"
    if defaultMenu != undefined then
    (
        menuman.setViewportRightClickMenu #shiftAndControlPressed defaultMenu
    )
    
    -- Unregister quad menu
    
    local menu = menuMan.findQuadMenu nebQuadMainMenu;
    format "check quad menu %\n" menu
    if menu != undefined then
    (
        menuMan.unRegisterQuadMenu menu;
        menuMan.updateMenuBar();
        format "remove Quad menu %\n" menu
    )
)