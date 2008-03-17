fn NebCreateSubMenuItemProgramers  =
(
    local menu = menuMan.createMenu nebProgramersMenu
    local ItemsTemp = #()
    ItemsTemp[1] = menuMan.createActionItem "zombie_NebReset" "zombie"
    
    append ItemsTemp (menuMan.createActionItem "zombie_LogConfig" "zombie")
    
    for i=1 to itemsTemp.count do 
    (
          menu.addItem ItemsTemp[i] -1
    )
    
    
    local subMenuItem = menuMan.createSubMenuItem "Programmers"  menu
    
    return subMenuItem
)

fn NebCreateSubMenuItemMaterial  =
(
    local menu = menuMan.createMenu nebMaterialMenu
    local ItemsTemp = #()
    ItemsTemp[1] = menuMan.createActionItem "zombie_mtlClearMtlEditor" "zombie"
    
    append ItemsTemp (menuMan.createActionItem "zombie_renderRollout" "zombie")
    
    for i=1 to itemsTemp.count do 
    (
          menu.addItem ItemsTemp[i] -1
    )
    
    
    local subMenuItem = menuMan.createSubMenuItem "Material"  menu
    
    return subMenuItem
)

fn NebAddItemVisbilityQuadMenu menu=
(
    local ItemsTemp = #()
    ItemsTemp[1] = menuMan.createActionItem "zombie_visToPortal" "zombie"
    append ItemsTemp (menuMan.createActionItem "zombie_visToOccluder" "zombie")
    append ItemsTemp (menuMan.createActionItem "zombie_visToCell" "zombie")
    append ItemsTemp (menuMan.createActionItem "zombie_visCreateCell" "zombie")
    
    for i=1 to itemsTemp.count do 
    (
          menu.addItem ItemsTemp[i] -1
    )
)

fn NebAddItemPlugsQuadMenu menu=
(
    local ItemsTemp = #()
    ItemsTemp[1] = menuMan.createActionItem "zombie_plugToPlug" "zombie"

    for i=1 to itemsTemp.count do 
    (
          menu.addItem ItemsTemp[i] -1
    )
)

fn NebAddItemMaterialQuadMenu menu=
(
    local ItemsTemp = #()
    ItemsTemp[1] = menuMan.createActionItem "zombie_UpdateBackedMaterialSelected" "zombie"
    append ItemsTemp (menuMan.createActionItem "zombie_ShowLightmaps" "zombie")
    append ItemsTemp (menuMan.createActionItem "zombie_ShowMaterials" "zombie")

    for i=1 to itemsTemp.count do 
    (
          menu.addItem ItemsTemp[i] -1
    )
)

fn NebAddMenu =
(

    if (menuMan.findMenu nebMainMenu) == undefined  then
    ( 
        local mainMenu = menuMan.getMainMenuBar()
        local nebula2Menu = menuMan.createMenu nebMainMenu 
	
        local ItemsTemp = #()
        
        ItemsTemp[1] = (menuMan.createActionItem "zombie_Export" "zombie")
        append ItemsTemp  (menuMan.createActionItem "zombie_ExportSelected" "zombie")
        append ItemsTemp  (menuMan.createActionItem "zombie_ExportDialog" "zombie")
        
        
		--ItemsTemp[1] = (menuMan.createActionItem "zombie_Export" "zombie")
        append ItemsTemp (menuMan.createSeparatorItem())
        append ItemsTemp (NebCreateSubMenuItemMaterial())
        append ItemsTemp ( NebCreateSubMenuItemProgramers() )
        
        append ItemsTemp (menuMan.createSeparatorItem())
        append ItemsTemp (menuMan.createActionItem "zombie_WikiArtistDocList" "zombie")		
        append ItemsTemp (menuMan.createActionItem "zombie_WikiMaxExporter" "zombie")
        append ItemsTemp (menuMan.createActionItem "zombie_WikiConjurer" "zombie")
        
        append ItemsTemp (menuMan.createSeparatorItem())
        append ItemsTemp (menuMan.createActionItem "zombie_AboutDialog" "zombie")		

        
        
        /*
        append ItemsTemp (menuMan.createActionItem "NebulaToolBox_BatchExport" "nmaxtoolbox")		
        append ItemsTemp (menuMan.createSeparatorItem())	
		
        append ItemsTemp (menuMan.createActionItem "NebulaToolBox_MaterialEditor" "nmaxtoolbox")
        append ItemsTemp (menuMan.createActionItem "NebulaToolBox_Animation" "nmaxtoolbox")		
        append ItemsTemp (menuMan.createSeparatorItem())

        append ItemsTemp (menuMan.createActionItem "NebulaToolBox_MeshOptions" "nmaxtoolbox")
        append ItemsTemp (menuMan.createActionItem "NebulaToolBox_LOD" "nmaxtoolbox")
        append ItemsTemp (menuMan.createSeparatorItem())	

        append ItemsTemp (menuMan.createActionItem "NebulaToolBox_SetDirectory" "nmaxtoolbox")
        append ItemsTemp (menuMan.createSeparatorItem())
        
        append ItemsTemp (menuMan.createActionItem "NebulaToolBox_Help" "nmaxtoolbox")
        append ItemsTemp (menuMan.createActionItem "NebulaToolBox_About" "nmaxtoolbox")
        
        */
					
        for i=1 to itemsTemp.count do 
            nebula2Menu.addItem ItemsTemp[i] -1
            
        local subMenuItem = menuMan.createSubMenuItem nebMainMenu  nebula2Menu
        local subMenuIndex = mainMenu.numItems()
        mainMenu.addItem subMenuItem subMenuIndex
        
        -- Create a quad menu
        quadMenu = menuMan.CreateQuadMenu "ZombieQuad" "visibility" "plugs" "Materials" ""
        menuman.setViewportRightClickMenu #shiftAndControlPressed quadMenu
        
        
        visMenu = quadmenu.getmenu 1
        NebAddItemVisbilityQuadMenu visMenu
        
        plugsMenu = quadmenu.getmenu 2
        NebAddItemPlugsQuadMenu plugsMenu
        
        plugsMenu = quadmenu.getmenu 3
        NebAddItemMaterialQuadMenu plugsMenu
         
        menuMan.setShowAllQuads quadMenu true        
        menuMan.updateMenuBar()
    ) 
)

fn NebLoadMenu =
(
    NebRemoveMenu()
    NebAddMenu()
)

NebLoadMenu()

