fn NebCreateSubMenuItemProgramers  =
(
    local menu = menuMan.createMenu nebProgramersMenu
    local ItemsTemp = #()
    ItemsTemp[1] = menuMan.createActionItem "tragNebula_NebReset" "tragNebula"
    
    append ItemsTemp (menuMan.createActionItem "tragNebula_LogConfig" "tragNebula")
    
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
    ItemsTemp[1] = menuMan.createActionItem "tragNebula_mtlClearMtlEditor" "tragNebula"
    
    append ItemsTemp (menuMan.createActionItem "tragNebula_renderRollout" "tragNebula")
    
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
    ItemsTemp[1] = menuMan.createActionItem "tragNebula_visToPortal" "tragNebula"
    append ItemsTemp (menuMan.createActionItem "tragNebula_visToOccluder" "tragNebula")
    append ItemsTemp (menuMan.createActionItem "tragNebula_visToCell" "tragNebula")
    append ItemsTemp (menuMan.createActionItem "tragNebula_visCreateCell" "tragNebula")
    
    for i=1 to itemsTemp.count do 
    (
          menu.addItem ItemsTemp[i] -1
    )
)

fn NebAddItemPlugsQuadMenu menu=
(
    local ItemsTemp = #()
    ItemsTemp[1] = menuMan.createActionItem "tragNebula_plugToPlug" "tragNebula"

    for i=1 to itemsTemp.count do 
    (
          menu.addItem ItemsTemp[i] -1
    )
)

fn NebAddItemMaterialQuadMenu menu=
(
    local ItemsTemp = #()
    ItemsTemp[1] = menuMan.createActionItem "tragNebula_UpdateBackedMaterialSelected" "tragNebula"
    append ItemsTemp (menuMan.createActionItem "tragNebula_ShowLightmaps" "tragNebula")
    append ItemsTemp (menuMan.createActionItem "tragNebula_ShowMaterials" "tragNebula")

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
        
        ItemsTemp[1] = (menuMan.createActionItem "tragNebula_Export" "tragNebula")
        append ItemsTemp  (menuMan.createActionItem "tragNebula_ExportSelected" "tragNebula")
        append ItemsTemp  (menuMan.createActionItem "tragNebula_ExportDialog" "tragNebula")
        
        
		--ItemsTemp[1] = (menuMan.createActionItem "tragNebula_Export" "tragNebula")
        append ItemsTemp (menuMan.createSeparatorItem())
        append ItemsTemp (NebCreateSubMenuItemMaterial())
        append ItemsTemp ( NebCreateSubMenuItemProgramers() )
        
        append ItemsTemp (menuMan.createSeparatorItem())
        append ItemsTemp (menuMan.createActionItem "tragNebula_WikiArtistDocList" "tragNebula")		
        append ItemsTemp (menuMan.createActionItem "tragNebula_WikiMaxExporter" "tragNebula")
        append ItemsTemp (menuMan.createActionItem "tragNebula_WikiConjurer" "tragNebula")
        
        append ItemsTemp (menuMan.createSeparatorItem())
        append ItemsTemp (menuMan.createActionItem "tragNebula_AboutDialog" "tragNebula")		

        
        
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
        quadMenu = menuMan.CreateQuadMenu "TragNebula2Quad" "visibility" "plugs" "Materials" ""
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

