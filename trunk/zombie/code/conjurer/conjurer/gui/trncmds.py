##\file trncmds.py
##\brief Terrain menu commands

import wx

import app
import format
import grassmaterials
import iehelpwindow
import levelcmds
import menu
import nebulaguisettings as guisettings
import servers
import trn
import trnlayersdlg
import trnimportmatsdlg
import trnnewdlg
import trnpropsdlg
import trntoolpanel
import trnlightmapdlg
import trngloballmapdlg
import waitdlg

import conjurerframework as cjr

# Terrain menu IDs
ID_NewClass = wx.NewId()
ID_NewTerrain = wx.NewId()
ID_Properties = wx.NewId()
ID_Import = wx.NewId()
ID_Export = wx.NewId()
ID_ImportTerrainMaterials = wx.NewId()
ID_ImportGrassMaterials = wx.NewId()
ID_EditTool = wx.NewId()
ID_TextureLayers = wx.NewId()
ID_GrassMaterials = wx.NewId()
ID_GenerateGlobalTexture = wx.NewId()
ID_GenerateWeightmaps = wx.NewId()
ID_GenerateLightmaps = wx.NewId()
ID_GenerateGlobalLightmaps = wx.NewId()
ID_SelectLightmapLight = wx.NewId()
ID_GenerateGrass = wx.NewId()
ID_GenerateGrassDirty = wx.NewId()
ID_GenerateGrassSelected = wx.NewId()
ID_UpdateGrassMesh = wx.NewId()
ID_UpdateGrassHeight = wx.NewId()
ID_PreprocessHorizon = wx.NewId()
ID_Help = wx.NewId()


# GenerateGrassMenu class
class GenerateGrassMenu(menu.Menu):
    """Err... The generate grass menu"""

    def __init__(self, frame):
        menu.Menu.__init__(self, frame)

    def create(self):
        # menu items
        self.Append( ID_GenerateGrass, "Generate &all grass" )
        self.Append( ID_GenerateGrassDirty, "Generate &grass" )
        self.Append( ID_GenerateGrassSelected, "Generate &selected" )
        self.AppendSeparator()
        self.Append( ID_UpdateGrassMesh, "Update grass &mesh" )
        self.Append( ID_UpdateGrassHeight, "Update grass &height" )

        # bindings
        self.bind_function(ID_GenerateGrass, self.on_generate_grass)
        self.bind_function(ID_GenerateGrassDirty, self.on_generate_grass_dirty)
        self.bind_function(
            ID_GenerateGrassSelected, self.on_generate_grass_selected
            )
        self.bind_function(ID_UpdateGrassMesh, self.on_update_mesh)
        self.bind_function(ID_UpdateGrassHeight, self.on_update_height)

    def on_generate_grass(self, event):
        """Generate terrain grass"""
        level_manager = servers.get_level_manager()
        level_name = level_manager.getcurrentlevelobject().getname()
        if level_name == "default":
            cjr.show_error_message(
                "Unable to generate grass for default level."
                )
        else:
            dlg = waitdlg.WaitDialog( self.get_frame(), "Generating grass..." )
            terrain = app.get_outdoor()
            editing = terrain.isingrowthedition()
            if not editing:
                terrain.begingrowthediton()
            terrain.buildgrowth()
            if not editing:
                terrain.endgrowtedition( False )
            dlg.Destroy()

    def on_generate_grass_dirty(self, event):
        """Generate terrain grass, only dirty"""
        level_manager = servers.get_level_manager()
        level_name = level_manager.getcurrentlevelobject().getname()
        if level_name == "default":
            cjr.show_error_message(
                "Unable to generate grass for default level."
                )
        else:
            dlg = waitdlg.WaitDialog( self.get_frame(), "Generating grass..." )
            terrain = app.get_outdoor()
            editing = terrain.isingrowthedition()
            if not editing:
                terrain.begingrowthediton()
            terrain.buildonlydirtygrowth()
            if not editing:
                terrain.endgrowtedition( False )
            dlg.Destroy()

    def on_generate_grass_selected(self, event):
        """Generate terrain grass, only selected cells"""
        level_manager = servers.get_level_manager()
        level_name = level_manager.getcurrentlevelobject().getname()
        if level_name == "default":
            cjr.show_error_message(
                "Unable to generate grass for default level."
                )
        else:
            dlg = waitdlg.WaitDialog( self.get_frame(), "Generating grass..." )
            terrain = app.get_outdoor()
            editing = terrain.isingrowthedition()
            if not editing:
                terrain.begingrowthediton()
            terrain.buildgrassonlyselected()
            if not editing:
                terrain.endgrowtedition( False )
            dlg.Destroy()     

    def on_update_mesh(self, event):
        """Update grass mesh"""
        level_manager = servers.get_level_manager()
        level_name = level_manager.getcurrentlevelobject().getname()
        if level_name == "default":
            cjr.show_error_message(
                "Unable to update grass mesh for default level."
                )
        else:
            dlg = waitdlg.WaitDialog(
                        self.get_frame(), 
                        "Updating grass mesh..." 
                        )
            terrain = app.get_outdoor()
            terrain.grassupdatemeshes()
            dlg.Destroy()

    def on_update_height(self, event):
        """Update grass height"""
        level_manager = servers.get_level_manager()
        level_name = level_manager.getcurrentlevelobject().getname()
        if level_name == "default":
            cjr.show_error_message(
                "Unable to update grass height for default level."
                )
        else:
            dlg = waitdlg.WaitDialog(
                        self.get_frame(), 
                        "Updating grass height..." 
                        )
            terrain = app.get_outdoor()
            terrain.grassupdateheight()
            dlg.Destroy()


# TerrainMenu class
class TerrainMenu(menu.Menu):
    """Err... The terrain menu"""

    def __init__(self, frame):
        menu.Menu.__init__(self, frame)
        self.menu_grass = GenerateGrassMenu(frame)

    def create(self):
        # menu items
        self.Append( ID_NewClass, "New &class..." )
        self.Append( ID_NewTerrain, "New &terrain..." )
        self.Append( ID_Properties, "View &properties..." )
        self.AppendSeparator()
        self.Append( ID_Import, "&Import heightmap..." )
        self.Append( ID_Export, "&Export heightmap..." )
        self.Append( ID_ImportTerrainMaterials, "Imp&ort terrain materials..." )
        self.Append( ID_ImportGrassMaterials, "Import &grass materials..." )
        self.AppendSeparator()
        self.AppendCheckItem( ID_EditTool, "Terrain &edit tool" )
        self.AppendCheckItem( ID_TextureLayers, "Terrain &materials" )
        self.AppendCheckItem( ID_GrassMaterials, "Gr&ass materials" )
        self.AppendSeparator()
        self.Append( ID_GenerateGlobalTexture, "Generate &global texture" )
        self.Append( ID_GenerateWeightmaps, "Generate &weightmaps" )
        self.Append( ID_GenerateLightmaps, "Generate &lightmaps" )
        self.Append( ID_GenerateGlobalLightmaps, "Generate glo&bal lightmaps" )
        self.Append( ID_SelectLightmapLight, "Select lightmap light" )
        self.menu_grass.create()
        self.AppendMenu( -1, "Generate g&rass", self.menu_grass )
        self.Append( ID_PreprocessHorizon, "Preprocess hori&zon" )
        self.AppendSeparator()
        self.Append( ID_Help, "&Help..." )

        # bindings
        togwinmgr = self.get_frame().get_togwinmgr()
        self.bind_function(ID_NewClass, self.on_new_class)
        self.bind_function(ID_NewTerrain, self.on_new_terrain)
        self.bind_function(ID_Properties, self.on_properties)
        self.bind_function(ID_Import, self.on_import_heightmap)
        self.bind_function(ID_Export, self.on_export_heightmap)
        self.bind_function(
            ID_ImportTerrainMaterials, 
            self.on_import_terrain_materials
            )
        self.bind_function(
            ID_ImportGrassMaterials, 
            self.on_import_grass_materials
            )
        togwinmgr.add_window(
            trntoolpanel.create_window,
            self.FindItemById(ID_EditTool)
            )
        togwinmgr.add_window(
            trnlayersdlg.create_window,
            self.FindItemById(ID_TextureLayers)
            )
        togwinmgr.add_window(
            grassmaterials.create_window,
            self.FindItemById(ID_GrassMaterials)
            )
        self.bind_function(
            ID_GenerateGlobalTexture, 
            self.on_generate_global_texture
            )
        self.bind_function(ID_GenerateWeightmaps, self.on_generate_weightmaps)
        self.bind_function(ID_GenerateLightmaps, self.on_generate_lightmaps)
        self.bind_function(
            ID_GenerateGlobalLightmaps,
            self.on_generate_global_lightmaps
            )
        self.bind_function(
            ID_SelectLightmapLight, 
            self.on_select_lightmap_light
            )
        self.bind_function(ID_PreprocessHorizon, self.on_preprocess_horizon)
        self.bind_function(ID_Help, self.on_help)

    def refresh(self):
        outdoor = app.get_outdoor()
        self.Enable( ID_NewTerrain, outdoor is None )
        self.Enable( ID_Properties, outdoor is not None )
        self.Enable( ID_Import, outdoor is not None )
        self.Enable( ID_Export, outdoor is not None )
        self.Enable( ID_ImportTerrainMaterials, outdoor is not None )
        self.Enable( ID_ImportGrassMaterials, outdoor is not None )

    def __postterrain_process(self):
        # Set the new outdoor in the terrain state
        if app.get_current_state() == 'terrain':
            trn.get_terrain_module().setoutdoor( app.get_outdoor_obj() )
        # Close and reopen the terrain editors dialogs
        children_data = []
        for child in self.get_frame().GetChildren():
            if isinstance(
                child, 
                (trntoolpanel.ToolDialog, trnlayersdlg.LayerManagerDialog)
                ):
                data = levelcmds.persist_window(child)
                if data is not None:
                    children_data.append(data)
                child.Close()
        for data in children_data:
            levelcmds.restore_window(data)

        # Refresh terrain menu
        self.get_frame().get_menubar().get_menu('&Terrain').refresh()

    def __post_grass_import_process(self):
        # Close and reopen the grass material list dialog
        child_data = []
        for child in self.get_frame().GetChildren():
            if isinstance(
                child, 
                (grassmaterials.GrassMaterialsDialog)
                ):
                data = levelcmds.persist_window(child)
                if data is not None:
                    child_data.append(data)
                child.Close()
        for data in child_data:
            levelcmds.restore_window(data)

    def on_new_class(self, evt):
        """ Pop up a new terrain class dialog and 
        create it if the user accepts """
        dlg = trnpropsdlg.NewTerrainClassDialog( self.get_frame() )
        if dlg.ShowModal() == wx.ID_OK:
            self.__postterrain_process()
        dlg.Destroy()

    def on_new_terrain(self, evt):
        """Pop up a new terrain dialog and create it if the user accepts"""
        dlg = trnnewdlg.NewTerrainDialog(self.get_frame())
        if dlg.ShowModal() == wx.ID_OK:
            self.__postterrain_process()
        dlg.Destroy()

    def on_properties(self, event):
        """Pop up the terrain properties dialog"""
        dlg = trnpropsdlg.PropertiesDialog(self.get_frame())
        dlg.ShowModal()
        dlg.Destroy()

    def on_import_heightmap(self, evt):
        """Show a file browser and import the selected heightmap"""
        source_directory = guisettings.Repository.getsettingvalue(
                                    guisettings.ID_BrowserPath_Heightmap
                                    )
        dlg = wx.FileDialog(
            self.get_frame(), 
            message="Choose a heightmap file to import",
            defaultDir=source_directory, 
            wildcard="Image files (*.bmp,*.tga,*.jpg,*.jpeg,*.png," \
                            "*.dds,*.ppm,*.dib,*.pfm,*.*)|*.bmp;*.tga;" \
                            "*.jpg;*.jpeg;*.png;*.dds;*.ppm;*.dib;*.pfm;*.*",
            style=wx.OPEN
            )

        if dlg.ShowModal() == wx.ID_OK:
            trn.get_terrain_module().importheightmap(
                str( dlg.GetPath() )
                )
            # Record last directory
            last_directory = format.get_directory(
                        dlg.GetPath()
                        )
            guisettings.Repository.setsettingvalue(
                guisettings.ID_BrowserPath_Heightmap, 
                last_directory 
                )
        dlg.Destroy()

    def on_export_heightmap(self, evt):
        """ Show a file browser and export the 
        heightmap with the entered file name """
        target_directory = guisettings.Repository.getsettingvalue(
                    guisettings.ID_BrowserPath_Heightmap
                    )
        dlg = wx.FileDialog(
            self.get_frame(), 
            message="Save heightmap as", 
            defaultDir=target_directory,
            wildcard="BMP image (*.bmp)|*.bmp|" \
                            "TGA image (*.tga)|*.tga|" \
                            "jpeg image (*.jpg)|*.jpg|" \
                            "PNG image (*.png)|*.png|" \
                            "dds image (*.dds)|*.dds|" \
                            "PPM image(*.ppm)|*.ppm|" \
                            "DIB image (*.dib)|*.dib|" \
                            "PFM image (*.pfm)|*.pfm|" |
                            "Any file (*.*)|*.*",
            style=wx.SAVE|wx.OVERWRITE_PROMPT
            )
        dlg.SetFilterIndex(1)

        if dlg.ShowModal() == wx.ID_OK:
            path = dlg.GetPath()
            path = servers.get_file_server().manglepath( str(path) )
            trn.get_terrain_module().exportheightmap( str(path) )
            # Record last directory
            last_directory = format.get_directory( path )
            guisettings.Repository.setsettingvalue(
                guisettings.ID_BrowserPath_Heightmap,
                last_directory 
                )
        dlg.Destroy()

    def on_import_terrain_materials(self, evt):
        """Import the terrain materials from a terrain class"""
        dlg = trnimportmatsdlg.ImportTerrainMaterialsDialog( self.get_frame() )
        if dlg.ShowModal() == wx.ID_OK:
            self.__postterrain_process()
        dlg.Destroy()

    def on_import_grass_materials(self, evt):
        """Import the grass materials from a terrain class"""
        dlg = trnimportmatsdlg.ImportGrassMaterialsDialog( self.get_frame() )
        if dlg.ShowModal() == wx.ID_OK:
            self.__post_grass_import_process()
        dlg.Destroy()

    def on_generate_global_texture(self, event):
        """Generate the terrain global texture"""
        try:
            try:
                dlg = waitdlg.WaitDialog(
                            self.get_frame(), 
                            "Generating global texture..." 
                            )
                trn.get_terrain_module().createterrainglobaltexture()
            finally:
                dlg.Destroy()
        except:
            # make sure any errors are not hidden
            raise

    def on_generate_weightmaps(self, event):
        """Generate terrain weightmaps"""
        try:
            try:
                dlg = waitdlg.WaitDialog(
                            self.get_frame(), 
                            "Generating weightmaps..." 
                            )
                trn.get_terrain_module().createterrainweightmaps()
            finally:
                dlg.Destroy()
        except:
            # make sure any errors are not hidden
            raise

    def on_preprocess_horizon(self, event):
        """Preprocess terrain horizon culling information"""
        try:
            try:
                dlg = waitdlg.WaitDialog(
                            self.get_frame(), 
                            "Preprocessing horizon..." 
                            )
                trn.get_terrain_module().preprocesshorizon()
            finally:
                dlg.Destroy()
        except:
            # make sure any errors are not hidden
            raise

    def on_generate_lightmaps(self, event):
        """Generate terrain lightmaps"""
        dlg = trnlightmapdlg.NewTerrainLightMapDialog( self.get_frame() )
        dlg.ShowModal()
        dlg.Destroy()

    def on_generate_global_lightmaps(self, event):
        """Generate global terrain lightmaps"""
        dlg = trngloballmapdlg.AddGlobalLightMapDialog( self.get_frame() )
        dlg.ShowModal()
        dlg.Destroy()

    def on_select_lightmap_light(self, event):
        # get light        
        num_entities = app.get_object_state().getselectioncount()
        lightid = -1
        for i in xrange(num_entities):
            entity = app.get_object_state().getselectedentity(i)
            if entity.isa("nelight") :
                lightid = entity.getid()
                break

        # if light not found
        if lightid == -1 :
            cjr.show_error_message(
                "No nelight instance found as lightmap light source"
                )
            return

        # if found create in the level
        app.get_level().setentityname(
            lightid,
            "terrain_lightmap_light"
            )
        cjr.show_information_message(
            "Terrain lightmap light selected"
            )

    def on_help(self, event):
        iehelpwindow.CreateWindow(
            self.get_frame(),
            "http://wiki.tragnarion.com/tiki-index.php?page=" \
                "SWD+Nebula+Conjurer+Terrain" 
            )
