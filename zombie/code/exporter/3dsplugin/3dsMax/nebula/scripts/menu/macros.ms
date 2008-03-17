

--------------------------------------------------------------
-- macro script for 'reseet all files' menu item.
--------------------------------------------------------------
macroScript zombie_NebReset
category:"zombie"
buttonText:"reload scripts"
tooltip:"Reload all zombie scripts" 
( 	
    on isEnabled return nebulaProgramersVersion
    on isVisible return nebulaProgramersVersion
    on execute do
    (
        nebReset()
    )
)

--------------------------------------------------------------
-- macro script for 'reseet all files' menu item.
--------------------------------------------------------------
macroscript zombie_LogConfig
category:"zombie"
buttonText:"log config"
tooltip:"enable and disable the log" 
icon:#("Tragnarion",11)
( 	
    on isEnabled return true
    on isVisible return true
    on execute do
    (
        nebLogServer.showRollout()
    )
)

--------------------------------------------------------------
-- macro script for 'convert to portal' menu item.
--------------------------------------------------------------
macroscript zombie_visToPortal
category:"zombie"
buttonText:"Convert to Portal"
tooltip:"Convert to Portal" 
icon:#("Tragnarion",5)
( 	
    on isEnabled return (try(selection.count == 1 and  ( NebVisOp.canConverToPortal selection[1] ) ) catch() )
    --on isVisible return (try(selection.count == 1 and  ( NebVisOp.canConverToPortal selection[1] ) ) catch(false) )
    on isVisible return true
    on execute do
    (
        NebVisOp.ConvertToPortal selection[1]
    )
)

--------------------------------------------------------------
-- macro script for 'convert to Occluder' menu item.
--------------------------------------------------------------
macroscript zombie_visToOccluder
category:"zombie"
buttonText:"Convert to Occluder"
tooltip:"Convert to Occluder" 
--icon:#("Tragnarion",5)
( 	
    on isEnabled return (try(selection.count == 1 and  ( NebVisOp.canConverToOccluder selection[1] ) ) catch() )
    --on isVisible return (try(selection.count == 1 and  ( NebVisOp.canConverToPortal selection[1] ) ) catch(false) )
    on isVisible return true
    on execute do
    (
        NebVisOp.ConvertToOccluder selection[1]
    )
)

--------------------------------------------------------------
-- macro script for 'convert to CEll' menu item.
--------------------------------------------------------------
macroscript zombie_visToCell
category:"zombie"
buttonText:"Convert to Cell"
icon:#("Tragnarion",6)
tooltip:"Convert to Cell" 
( 	
    on isEnabled return (Try(selection.count != 0 and (NebVisOp.canConverToCell selection[1]))Catch())
    --on isVisible return (Try(selection.count != 0 and (NebVisOp.canConverToCell selection[1]))Catch(false))
    on isVisible return true
    on execute do
    (
        NebVisOp.ConvertToCell selection
    )
)

--------------------------------------------------------------
-- macro script for 'Create a  CEll' menu item.
--------------------------------------------------------------
macroscript zombie_visCreateCell
category:"zombie"
buttonText:"Create Cell from this"
icon:#("Tragnarion",7)
tooltip:"Create Cell from mesh" 
( 	
    on isEnabled return (Try(selection.count != 0 and (NebVisOp.canConverToCell selection[1]))Catch())
    --on isVisible return (Try(selection.count != 0 and (NebVisOp.canConverToCell selection[1]))Catch(false))
    on isVisible return true
    on execute do
    (
        NebVisOp.CreateCellFrom selection
    )
)


--------------------------------------------------------------
-- macro script for 'Clear Material Editor' menu item.
--------------------------------------------------------------
macroscript zombie_mtlClearMtlEditor
category:"zombie"
buttonText:"Clear material editor"
tooltip:"clean the material editor with Zombie Material" 
icon:#("Tragnarion",9)
( 	
    on isEnabled return true
    on isVisible return true
    on execute do
    (
        clearMtlEditor()
    )
)


--------------------------------------------------------------
-- macro script for 'Clear Material Editor' menu item.
--------------------------------------------------------------
macroscript zombie_mtlClearStdMtlEditor
category:"zombie"
buttonText:"Clear material editor"
tooltip:"clean the material editor with Standard Material" 
icon:#("Tragnarion",8)
( 	
    on isEnabled return true
    on isVisible return true
    on execute do
    (
        clearMtlEditor mtlType:Standardmaterial
    )
)


--------------------------------------------------------------
-- macro script for 'Clear Material Editor' menu item.
--------------------------------------------------------------
macroscript zombie_renderRollout
category:"zombie"
buttonText:"Show render dialog"
tooltip:"The properties simulate in 3dsMax"
icon:#("Tragnarion",1) 
( 	
    on isEnabled return ( [0,0] == GetDialogPos zombieRenderRollout )
    on isVisible return true
    on execute do
    (
        CreateDialog zombieRenderRollout()
    )
)

/*
--------------------------------------------------------------
-- macro script for 'convert to CEll' menu item.
--------------------------------------------------------------
macroscript zombie_collCreateComposite
category:"zombie"
buttonText:"Create Composite"
tooltip:"create a new composite using the selection" 
( 	
    on isEnabled return (Try(selection.count >1 and (isPhysicCollision selection ) and (notPhysicCollisionComposite selection) )Catch())
    --on isVisible return (Try(selection.count != 0 and (NebVisOp.canConverToCell selection[1]))Catch(false))
    on isVisible return true
    on execute do
    (
        NebCollOp.createCollisionCompositeObj selection #(physicMaterial, physicSpace, cb_wall.checked, cb_ceiling.checked, cb_ground.checked, cb_walkable.checked )
    )
)
*/




--------------------------------------------------------------
-- macro script for 'internals docs of artist' menu item.
--------------------------------------------------------------
macroscript zombie_WikiArtistDocList
category:"zombie"
buttonText:"Wiki Artist Doc List"
icon:#("Tragnarion",4)
tooltip:"Wiki Artist Doc" 
( 	
    on isEnabled return true
    on isVisible return true
    on execute do
    (
        shellLaunch "http://wiki.tragnarion.com/tiki-index.php?page=ART+DOC+LST" "" 
    )
)

--------------------------------------------------------------
-- macro script for 'internals docs of artist' menu item.
--------------------------------------------------------------
macroscript zombie_WikiMaxExporter
category:"zombie"
buttonText:"Wiki Max Exporter"
icon:#("Tragnarion",2)
tooltip:"Wiki Max Exporter" 
( 	
    on isEnabled return true
    on isVisible return true
    on execute do
    (
        shellLaunch "http://wiki.tragnarion.com/tiki-index.php?page=SWD+Nebula+MAX+Exporter" ""
    )
)

--------------------------------------------------------------
-- macro script for 'internals docs of artist' menu item.
--------------------------------------------------------------
macroscript zombie_WikiConjurer
category:"zombie"
buttonText:"Wiki Conjurer"
icon:#("Tragnarion",3)
tooltip:"Wiki Conjurer" 
( 	
    on isEnabled return true
    on isVisible return true
    on execute do
    (
        shellLaunch "http://wiki.tragnarion.com/tiki-index.php?page=SWD+Nebula+Conjurer" ""
    )
)


--------------------------------------------------------------
-- macro script for 'internals docs of artist' menu item.
--------------------------------------------------------------
macroscript zombie_AboutDialog
category:"zombie"
buttonText:"About"
tooltip:"About" 
icon:#("Tragnarion",10)
( 	
    on isEnabled return ( [0,0] == GetDialogPos neb3dsAboutRollout )
    on isVisible return true
    on execute do
    (
        CreateDialog neb3dsAboutRollout
    )
)



--------------------------------------------------------------
-- macro script for 'internals docs of artist' menu item.
--------------------------------------------------------------
macroscript zombie_ExportDialog
category:"zombie"
buttonText:"ExportOptions"
tooltip:""
( 	
    on isEnabled return ( NebExportOp.CanShowDialog() )
    on isVisible return true
    on execute do
    (
        NebExportOp.ShowExportOptionsRolout()
    )
)

--------------------------------------------------------------
-- macro script for 'internals docs of artist' menu item.
--------------------------------------------------------------
macroscript zombie_Export
category:"zombie"
buttonText:"Export"
tooltip:""
( 	
    on isEnabled return ( try (NebExportOp.CanToExport() ) catch() )
    on isVisible return true
    on execute do
    (
        NebExportOp.Export()
    )
)

--------------------------------------------------------------
-- macro script for 'internals docs of artist' menu item.
--------------------------------------------------------------
macroscript zombie_ExportSelected
category:"zombie"
buttonText:"Export selected"
tooltip:""
( 	
    on isEnabled return ( try( selection.count>0 and NebExportOp.CanToExport() ) catch() ) 
    on isVisible return true
    on execute do
    (
        NebExportOp.Export selectedOnly:true
    )
)


--------------------------------------------------------------
-- macro script for 'internals docs of artist' menu item.
--------------------------------------------------------------
macroscript zombie_treeNormals
category:"zombie"
buttonText:"Tree Normals"
tooltip:"Tree Normals"
( 	
    on isEnabled return ( try( selection.count == 1 and NebCanUseTreeNormals( selection[1])  ) catch() ) 
    on isVisible return true
    on execute do
    (
        try (
        NebTreeNormals selection[1]
        ) catch()
    )
)

--------------------------------------------------------------
-- macro script for 'convert to plug' menu item.
--------------------------------------------------------------
macroscript zombie_plugToPlug
category:"zombie"
buttonText:"Convert to Plug"
tooltip:"Convert to Plug" 
( 	
    on isEnabled return (Try(selection.count == 1 and (NebPlugsUtil.canConvertToPlug selection[1]))Catch())
    on execute do
    (
        NebPlugsOp.convertToPlug selection[1]
    )
)

--------------------------------------------------------------
-- macro script for 'convert to plug' menu item.
--------------------------------------------------------------
macroscript zombie_UpdateBackedMaterialSelected
category:"zombie"
buttonText:"Update backed"
tooltip:"Update backed material , object selected" 
( 	
    on isEnabled return (Try(selection.count != 0 and isproperty selection[1] "material" ) Catch())
    on execute do
    (
        try
        (
            local tmp = selection as array
            NebMatOp.UpdateBackedArray  tmp
        ) catch( format "*** % ***\n" (getCurrentException()) )
    )
)

--------------------------------------------------------------
-- macro script for 'Update baked material' menu item.
--------------------------------------------------------------
macroscript zombie_UpdateBackedMaterialEditor
category:"zombie"
buttonText:"Update backed"
tooltip:"Update backed material, mateditor" 
( 	
    on isEnabled return (Try(  MatEditor.isOpen() and  classOf (medit.GetCurMtl()) == zStdMaterial) Catch())
    on execute do
    (
        try
        (
            local mat = medit.GetCurMtl()            
            NebMatOp.UpdateBacked mat
        ) catch( format "*** % ***\n" (getCurrentException()) )
    )
)

--------------------------------------------------------------
-- macro script for 'Show all ligtmaps in viewport' menu item.
--------------------------------------------------------------
macroscript zombie_ShowLightmaps
category:"zombie"
buttonText:"show lightmaps"
tooltip:"Show all lightmaps in viewport" 
( 	
    on isEnabled return true
    on execute do
    (
        try
        (
            iterateMaterial showLightmap true
        ) catch( format "*** % ***\n" (getCurrentException()) )
    )
)

--------------------------------------------------------------
-- macro script for 'Show all materials in viewport' menu item.
--------------------------------------------------------------
macroscript zombie_ShowMaterials
category:"zombie"
buttonText:"show zmaterials"
tooltip:"Show all zombie materials in viewport" 
( 	
    on isEnabled return true
    on execute do
    (
        try
        (
            iterateMaterial showTrue true
        ) catch( format "*** % ***\n" (getCurrentException()) )
    )
)

--shellLaunch
