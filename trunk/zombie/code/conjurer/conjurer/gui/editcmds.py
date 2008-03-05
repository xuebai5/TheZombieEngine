##\file editcmds.py
##\brief Edit menu commands

import wx

import pynebula

import app
import layersdlg
import menu
import objdlg
import objtrackdlg
import prefsdlg
import servers
import soundlibdlg
import sysdlg
import batchdlg
import particlelistdlg

import waitdlg
import nodelibdlg

import propertyview
import scriptingmodelmultiobject
import conjurerframework as cjr


# Edit menu IDs
ID_Undo = wx.NewId()
ID_Redo = wx.NewId()
ID_Cut = wx.NewId()
ID_Copy = wx.NewId()
ID_Paste = wx.NewId()
ID_Delete = wx.NewId()
ID_Batch = wx.NewId()
ID_Unbatch = wx.NewId()
ID_Autobatch = wx.NewId()
ID_Load = wx.NewId()
ID_Unload = wx.NewId()
ID_Reload = wx.NewId()
ID_LoadSelection = wx.NewId()
ID_SaveSelection = wx.NewId()
ID_CleanUndos = wx.NewId()
ID_CleanMaterials = wx.NewId()
ID_LayerManager = wx.NewId()
ID_SoundLibrary = wx.NewId()
ID_ParticleSystems = wx.NewId()
ID_SelectedEntities = wx.NewId()
ID_TrackSelectedEntity = wx.NewId()
ID_SysInfo = wx.NewId()
ID_Preferences = wx.NewId()


# CleanMenu class
class CleanMenu(menu.Menu):
    """Err... The clean menu"""
    
    def __init__(self, frame):
        menu.Menu.__init__(self, frame)
    
    def create(self):
        self.Append( ID_CleanUndos, "Clean &undo history" )
        self.Append( ID_CleanMaterials, "Clean material library" )
        self.bind_function(ID_CleanUndos, self.on_clean_undos)
        self.bind_function(ID_CleanMaterials, self.on_clean_materials)
    
    def on_clean_undos(self, evt):
        """Clean the undo server's buffer"""
        servers.get_command_server().clean()

    def on_clean_materials(self, evt):
        """Clean the material server's library"""
        servers.get_material_server().deletelibrary()

# CamerasMenu class
class CamerasMenu(menu.Menu):
    """Err... The cameras menu"""
    
    def __init__(self, frame):
        menu.Menu.__init__(self, frame)
        self.cameras = {}
    
    def create(self):
        pass
    
    def refresh(self):
        """Rebuild the menu"""
        # Remove old menu items
        for key in self.cameras.keys():
            self.unbind_function(key)
            self.Delete(key)
        self.cameras = {}
        
        # Add current viewports
        viewport = app.get_viewports_dir().gethead()
        while viewport is not None:
            menu_id = wx.NewId()
            self.cameras[menu_id] = viewport.getfullname()
            self.Append(menu_id,  viewport.getname())
            self.bind_function(menu_id, self.on_camera)
            viewport = viewport.getsucc()
        # Add special rnsview
        menu_id = wx.NewId()
        self.cameras[menu_id] = '/usr/rnsview'
        self.Append(menu_id, 'rnsview')
        self.bind_function(menu_id, self.on_camera)
    
    def on_camera(self, event):
        """Show an object inspector for a camera"""
        win = objdlg.create_window(
                    self.get_frame(),
                    self.cameras[event.GetId()]
                    )
        win.display()


# LightsMenu class
class LightsMenu(menu.Menu):
    """Err... The lights menu"""

    def __init__(self, frame):
        menu.Menu.__init__(self, frame)
        self.lights = {}

    def create(self):
        pass

    def refresh(self):
        """Rebuild the menu"""
        # Remove old menu items
        for key in self.lights.keys():
            self.unbind_function(key)
            self.Delete(key)
        self.lights = {}

        # Add the standard light
        menu_id = wx.NewId()
        self.lights[menu_id] = "stdlight"
        self.Append(menu_id, "stdlight")
        self.bind_function(menu_id, self.on_light)

    def on_light(self, event):
        """Show an object inspector for a light"""
        win = objdlg.create_window(
                    self.get_frame(),
                    self.lights[event.GetId()] 
                    )
        win.display()


# IndoorsMenu class
class IndoorsMenu(menu.Menu):
    """Err... The indoors menu"""

    def __init__(self, frame):
        menu.Menu.__init__(self, frame)
        self.indoors = {}

    def create(self):
        pass

    def refresh(self):
        """Rebuild the menu"""
        # Remove old menu items
        for key in self.indoors.keys():
            self.unbind_function(key)
            self.Delete(key)
        self.indoors = {}

        # Add the indoor menu options
        menu_id = wx.NewId()
        self.indoors[menu_id] = "Connect indoor"
        self.Append(menu_id, "Connect indoor")
        self.bind_function(menu_id, self.on_connect_indoor)
        menu_id = wx.NewId()
        self.indoors[menu_id] = "Edit indoor"
        self.Append(menu_id, "Edit indoor")
        self.bind_function(menu_id, self.on_edit_indoor)

    def on_connect_indoor(self, event):
        """Connect indoors tool"""
        servers.get_spatial_server().connectindoors()

    def on_edit_indoor(self, event):
        """Edit indoor tool"""
        servers.get_spatial_server().toggleeditindoor()


# StreamsMenu class
class StreamsMenu(menu.Menu):
    """The geometry streams menu"""

    def __init__(self, frame):
        menu.Menu.__init__(self, frame)
        self.streams = {}

    def create(self):
        pass

    def refresh(self):
        """Rebuild the streams menu"""
        # Remove old menu items
        for key in self.streams.keys():
            self.unbind_function(key)
            self.Delete(key)
        self.streams = {}

        # Add the geometry stream menu options
        menu_id = wx.NewId()
        self.streams[menu_id] = "Save streams"
        self.Append(menu_id, "Save all")
        self.bind_function(menu_id, self.on_save_library)
        menu_id = wx.NewId()
        self.streams[menu_id] = "Build streams"
        self.Append(menu_id, "Build all")
        self.bind_function(menu_id, self.on_build_streams)
        self.AppendSeparator()
        menu_id = wx.NewId()
        self.streams[menu_id] = "New stream"
        self.Append(menu_id, "New...")
        self.bind_function(menu_id, self.on_create_stream)
        menu_id = wx.NewId()
        self.streams[menu_id] = "Open stream"
        self.Append(menu_id, "Open...")
        self.bind_function(menu_id, self.on_open_stream)
        menu_id = wx.NewId()
        self.streams[menu_id] = "Delete stream"
        self.Append(menu_id, "Delete...")
        self.bind_function(menu_id, self.on_delete_stream)
        menu_id = wx.NewId()
        self.streams[menu_id] = "Apply stream"
        self.Append(menu_id, "Apply...")
        self.bind_function(menu_id, self.on_apply_stream)
        menu_id = wx.NewId()
        self.streams[menu_id] = "Build stream"
        self.Append(menu_id, "Build...")
        self.bind_function(menu_id, self.on_build_stream)
        menu_id = wx.NewId()
        self.streams[menu_id] = "Remove stream"
        self.Append(menu_id, "Remove...")
        self.bind_function(menu_id, self.on_remove_stream)

    def on_save_library(self, event):
        """Save all streams"""
        # Save all streams to file
        if pynebula.exists( str("/usr/streams") ):
            app.get_state("geometrystream").savestreamlibrary()

    def on_build_streams(self, event):
        """Build all streams"""
        # Build all dirty streams
        if not pynebula.exists( str("/usr/streams") ):
            app.get_state("geometrystream").loadstreamlibrary()
        dlg2 = waitdlg.WaitDialog( self.get_frame(), "Building streams ..." )
        app.get_state("geometrystream").buildallstreams()
        dlg2.Destroy()

    def on_create_stream(self, event):
        """Create geometry stream"""
        # Ask for the new stream name
        if not pynebula.exists( str("/usr/streams") ):
            app.get_state("geometrystream").loadstreamlibrary()

        dlg = nodelibdlg.NodeLibDialog(
                    self.get_frame(), 
                    nodelibdlg.NEW,
                    'stream', 
                    'Stream', 
                    "/usr/streams"
                    )
        if dlg.ShowModal() == wx.ID_OK:
            # Check that's a new stream name
            if dlg.node_exists():
                msg = "There's already a stream named"
                msg = msg + " '" + dlg.get_guiname() + "'.\n"
                msg = msg + "You should enter a new stream name."
                cjr.show_error_message(msg)
            else:
                # create the new stream
                obj = app.get_state("geometrystream").createstream(
                            str( dlg.get_guiname() ) 
                            )
                win = objdlg.create_window(
                            wx.GetApp().GetTopWindow(), 
                            obj.getfullname() 
                            )
                win.display()
        dlg.Destroy()

    def on_open_stream(self, event):
        """Open geometry stream"""
        # Ask for the stream name
        if not pynebula.exists( str("/usr/streams") ):
            app.get_state("geometrystream").loadstreamlibrary()

        dlg = nodelibdlg.NodeLibDialog(
                    self.get_frame(), 
                    nodelibdlg.OPEN,
                    'stream', 
                    'Stream', 
                    "/usr/streams"
                    )
        if dlg.ShowModal() == wx.ID_OK:
            # Check that's an existing stream
            if not dlg.node_exists():
                msg = "There is no stream called '%s'"  % dlg.get_guiname()
                cjr.show_error_message(msg)
            else:
                # open the stream for editing
                win = objdlg.create_window(
                            wx.GetApp().GetTopWindow(), 
                            dlg.get_node().getfullname() 
                            )
                win.display()
        dlg.Destroy()

    def on_delete_stream(self, event):
        """Delete geometry stream"""
        # Ask for the stream name
        if not pynebula.exists( str("/usr/streams") ):
            app.get_state("geometrystream").loadstreamlibrary()

        dlg = nodelibdlg.NodeLibDialog(
                    self.get_frame(), 
                    nodelibdlg.DELETE,
                    'stream', 
                    'Stream', 
                    "/usr/streams"
                    )
        ok_to_continue = dlg.ShowModal() == wx.ID_OK
        if ok_to_continue:
            msg = "Delete stream '%s'?" % dlg.get_guiname()
            delete = cjr.warn_yes_no( self.get_frame(), msg )
            if delete == wx.ID_YES:
                # Delete the stream
                app.get_state("geometrystream").deletestream(
                    str( dlg.get_guiname() ) 
                    )
        dlg.Destroy()

    def on_apply_stream(self, event):
        """Apply geometry stream"""
        if not pynebula.exists( str("/usr/streams") ):
            app.get_state("geometrystream").loadstreamlibrary()

        dlg = nodelibdlg.NodeLibDialog(
                    self.get_frame(), 
                    nodelibdlg.OPEN,
                    'stream', 
                    'Stream', 
                    "/usr/streams"
                    )
        if dlg.ShowModal() == wx.ID_OK:
            # Check that's an existing stream
            if not dlg.node_exists():
                msg = "There is no stream called '%s'" % dlg.get_guiname()
                cjr.show_error_message(msg)
            else:
                # apply the stream
                dlg2 = waitdlg.WaitDialog(
                            self.get_frame(),
                            "Applying stream %s ..." % dlg.get_guiname()
                            )
                dlg.get_node().applystream()
                dlg2.Destroy()
        dlg.Destroy()

    def on_build_stream(self, event):
        """Build geometry stream"""
        if not pynebula.exists( str("/usr/streams") ):
            app.get_state("geometrystream").loadstreamlibrary()

        dlg = nodelibdlg.NodeLibDialog(
                    self.get_frame(), 
                    nodelibdlg.OPEN,
                    'stream', 
                    'Stream', 
                    "/usr/streams"
                    )
        if dlg.ShowModal() == wx.ID_OK:
            # Check that's an existing stream
            if not dlg.node_exists():
                msg = "There is no stream called '%s'" % dlg.get_guiname()
                cjr.show_error_message(msg)
            else:
                # apply the stream
                dlg2 = waitdlg.WaitDialog(
                            self.get_frame(),
                            "Building stream '%s' ..." % dlg.get_guiname()
                            )
                dlg.get_node().buildstream()
                dlg2.Destroy()
        dlg.Destroy()

    def on_remove_stream(self, event):
        """Remove geometry stream"""
        if not pynebula.exists( str("/usr/streams") ):
            app.get_state("geometrystream").loadstreamlibrary()

        dlg = nodelibdlg.NodeLibDialog(
                    self.get_frame(), 
                    nodelibdlg.OPEN,
                    'stream', 
                    'Stream', 
                    "/usr/streams"
                    )
        if dlg.ShowModal() == wx.ID_OK:
            # Check that's an existing stream
            if not dlg.node_exists():
                msg = "There is no stream called '%s'" % dlg.get_guiname()
                cjr.show_error_message(msg)
            else:
                # remove the stream
                dlg2 = waitdlg.WaitDialog(
                            self.get_frame(),
                            "Removing stream '%s' ..." % dlg.get_guiname()
                            )
                dlg.get_node().removestream()
                dlg2.Destroy()
        dlg.Destroy()

# AreasMenu class
class AreasMenu(menu.Menu):
    """The loading areas menu"""

    def __init__(self, frame):
        menu.Menu.__init__(self, frame)
        self.areas = {}
        
    def create(self):
        pass
        
    def refresh(self):
        """Rebuild the areas menu"""
        # Remove old menu items
        for key in self.areas.keys():
            self.unbind_function(key)
            self.Delete(key)
        self.areas = {}
        
        # Add the loading area menu options
        menu_id = wx.NewId()
        self.areas[menu_id] = "New outdoor area"
        self.Append(menu_id, "New outdoor area ...")
        self.bind_function(menu_id, self.on_new_outdoor_area)
        menu_id = wx.NewId()
        self.areas[menu_id] = "New indoor area"
        self.Append(menu_id, "New indoor area ...")
        self.bind_function(menu_id, self.on_new_indoor_area)
        menu_id = wx.NewId()
        self.areas[menu_id] = "Open area"
        self.Append(menu_id, "Open area ...")
        self.bind_function(menu_id, self.on_open_area)
        menu_id = wx.NewId()
        self.areas[menu_id] = "Delete area"
        self.Append(menu_id, "Delete area ...")
        self.bind_function(menu_id, self.on_delete_area)

        self.AppendSeparator()

        menu_id = wx.NewId()
        self.areas[menu_id] = "Add to area"
        self.Append(menu_id, "Add selection to ...")
        self.bind_function(menu_id, self.on_add_to_area)
        menu_id = wx.NewId()
        self.areas[menu_id] = "Remove from area"
        self.Append(menu_id, "Remove selection from ...")
        self.bind_function(menu_id, self.on_remove_from_area)
        menu_id = wx.NewId()
        self.areas[menu_id] = "Area stats"
        self.Append(menu_id, "Get area stats...")
        self.bind_function(menu_id, self.on_area_stats)

    def on_new_outdoor_area(self, event):
        """Create new outdoor loading area"""
        # Create a new outdoor area
        dlg = nodelibdlg.NodeLibDialog(
                    self.get_frame(), 
                    nodelibdlg.NEW,
                    'area', 
                    'Area', 
                    "/usr/areas"
                    )
        if dlg.ShowModal() == wx.ID_OK:
            # Check that's a new stream name
            if dlg.node_exists():
                msg = "There's already a stream named"
                msg = msg + " '" + dlg.get_guiname() + "'.\n"
                msg = msg + "You should enter a new area name."
                cjr.show_error_message(msg)
            else:
                # create the new loading area
                obj = servers.get_loader_server().createloadarea(
                            "noutdoorarea", 
                            str( dlg.get_guiname() )
                            )
                win = objdlg.create_window(
                            wx.GetApp().GetTopWindow(), 
                            obj.getfullname() 
                            )
                win.display()
        dlg.Destroy()

    def on_new_indoor_area(self, event):
        """Create new indoor loading area"""
        # Create a new indoor area
        dlg = nodelibdlg.NodeLibDialog(
                    self.get_frame(), 
                    nodelibdlg.NEW,
                    'area', 
                    'Area', 
                    "/usr/areas"
                    )
        if dlg.ShowModal() == wx.ID_OK:
            # Check that's a new stream name
            if dlg.node_exists():
                msg = "There's already an area named"
                msg = msg + " '" + dlg.get_guiname() + "'.\n"
                msg = msg + "You should enter a new area name."
                cjr.show_error_message(msg)
            else:
                # create the new loading area
                obj = servers.get_loader_server().createloadarea(
                            "nindoorarea", 
                            str( dlg.get_guiname() )
                            )
                win = objdlg.create_window(
                            wx.GetApp().GetTopWindow(), 
                            obj.getfullname() 
                            )
                win.display()
        dlg.Destroy()

    def on_open_area(self, event):
        """Open loading area"""
        # Ask for the area name
        if pynebula.exists( str("/usr/areas") ):
            dlg = nodelibdlg.NodeLibDialog(
                        self.get_frame(), 
                        nodelibdlg.OPEN,
                        'area', 
                        'Area', 
                        "/usr/areas"
                        )
            if dlg.ShowModal() == wx.ID_OK:
                # Check that's an existing area
                if not dlg.node_exists():
                    msg = "There is no area called '%s'" % dlg.get_guiname()
                    cjr.show_error_message(msg)
                else:
                    # open the area for edition
                    win = objdlg.create_window(
                                wx.GetApp().GetTopWindow(), 
                                dlg.get_node().getfullname() 
                                )
                    win.display()
            dlg.Destroy()

    def on_delete_area(self, event):
        """Delete loading area"""    
        # select an area and get a name
        dlg = nodelibdlg.NodeLibDialog(
                    self.get_frame(), 
                    nodelibdlg.DELETE,
                    'area', 
                    'Area', 
                    "/usr/areas"
                    )
        ok_to_continue = dlg.ShowModal() == wx.ID_OK
        if ok_to_continue:
            msg = "Delete area '%s'?" % dlg.get_guiname()
            delete = cjr.warn_yes_no( self.get_frame(), msg )
            if delete == wx.ID_YES:
                # Delete the area from the level
                servers.get_loader_server().deleteloadarea(
                    str( dlg.get_guiname() ) 
                    )
        dlg.Destroy()

    def on_add_to_area(self, event):
        """Add selection to loading area"""
        # select an area and add current selection to it
        dlg = nodelibdlg.NodeLibDialog(
                    self.get_frame(), 
                    nodelibdlg.OPEN,
                    'area', 
                    'Area', 
                    "/usr/areas"
                    )
        if dlg.ShowModal() == wx.ID_OK:
            # Check that's an existing area
            if not dlg.node_exists():
                msg = "There is no area called '%s'" % dlg.get_guiname()
                cjr.show_error_message(msg)
            else:
                # assign selected entities to the area
                app.get_object_state().addtoloadarea( str(dlg.get_guiname()) )
        dlg.Destroy()

    def on_remove_from_area(self, event):
        """Remove selection from loading area"""
        # select an area and remove current selection from it
        dlg = nodelibdlg.NodeLibDialog(
                    self.get_frame(), 
                    nodelibdlg.OPEN,
                    'area', 
                    'Area', 
                    "/usr/areas"
                    )
        if dlg.ShowModal() == wx.ID_OK:
            # Check that's an existing area
            if not dlg.node_exists():
                msg = "There is no area called '%s'" % dlg.get_guiname()
                cjr.show_error_message(msg)
            else:
                # remove selected entities from the area
                app.get_object_state().removefromloadarea(
                    str( dlg.get_guiname() )
                    )
        dlg.Destroy()
        
    def on_area_stats(self, event):
        """Get stats and budget for loading area"""
        # select an area and retrieve stats information for it
        dlg = nodelibdlg.NodeLibDialog(
                    self.get_frame(), 
                    nodelibdlg.OPEN,
                    'area', 
                    'Area', 
                    "/usr/areas"
                    )
        if dlg.ShowModal() == wx.ID_OK:
            # Check that's an existing area
            if not dlg.node_exists():
                msg = "There is no area called '%s'" % dlg.get_guiname()
                cjr.show_error_message(msg)
            else:
                # show log of resources for the area
                dlg2 = waitdlg.WaitDialog(
                            self.get_frame(), 
                            "Analyzing area ..." 
                            )
                if not dlg.get_node().logdebugstats():
                    msg = "No stats available for this area, "\
                                "you need to load it first"
                    cjr.show_error_message(msg)
                dlg2.Destroy()
        dlg.Destroy()

# VariablesMenu class
class VariablesMenu(menu.Menu):
    """The variables menu"""

    def __init__(self, frame):
        menu.Menu.__init__(self, frame)
        self.variables = {}

    def create(self):
        pass

    def refresh(self):
        """Rebuild the menu"""
        # Remove old menu items
        for key in self.variables.keys():
            self.unbind_function(key)
            self.Delete(key)
        self.variables = {}

        # Add the option for global variables
        menu_id = wx.NewId()
        self.variables[menu_id] = "global variables"
        self.Append(menu_id, "&Global variables")
        self.bind_function(menu_id, self.on_globals)

    def on_globals(self, event):
        """Show an object inspector for the global variables"""
        path = servers.get_global_vars().getfullname()
        win = objdlg.create_window(
                    self.get_frame(), 
                    path 
                    )
        win.display()

# EditMenu class
class EditMenu(menu.Menu):
    """Err... The edit menu"""
    
    def __init__(self, frame):
        menu.Menu.__init__(self, frame)
        self.menu_clean = CleanMenu(frame)
        self.menu_cameras = CamerasMenu(frame)
        self.menu_lights = LightsMenu(frame)
        self.menu_indoors = IndoorsMenu(frame)
        self.menu_streams = StreamsMenu(frame)
        self.menu_areas = AreasMenu(frame)
        self.menu_variables = VariablesMenu(frame)
    
    def create(self):
        # menu items
        self.Append( ID_Undo, "&Undo\tCtrl+Z" )
        self.Append( ID_Redo, "&Redo\tCtrl+Y" )
        self.AppendSeparator()
        self.Append( ID_Cut, "Cut\tCtrl+X" )
        self.Append( ID_Copy, "Copy\tCtrl+C" )
        self.Append( ID_Paste, "Paste\tCtrl+V" )
        self.Append( ID_Delete, "&Delete" )
        self.AppendSeparator()
        self.Append( ID_Batch, "&Batch selection" )
        self.Append( ID_Unbatch, "Re&move from batches" )
        self.Append( ID_Autobatch, "Autobatch free entities" )
        self.AppendSeparator()
        self.Append( ID_Load, "&Load selection\tCtrl+D" )
        self.Append( ID_Unload, "&Unload selection\tCtrl+U" )
        self.Append( ID_Reload, "Reload selection" )
        self.AppendSeparator()
        self.Append( ID_SaveSelection, "Save selection..." )
        self.Append( ID_LoadSelection, "Load saved selection..." )
        self.AppendSeparator()
        self.menu_clean.create()
        self.AppendMenu( -1, "Clea&n", self.menu_clean )
        self.AppendSeparator()
        self.AppendCheckItem(
            ID_LayerManager, 
            "&Layer manager" 
            )
        self.AppendCheckItem(
            ID_SoundLibrary, 
            "&Sound library" 
            )
        self.AppendCheckItem(
            ID_ParticleSystems, 
            "&Particle systems" 
            )
        self.AppendSeparator()
        self.Append(
            ID_SelectedEntities, 
            "Selected &entities\tCtrl+P" 
            )
        self.AppendCheckItem(
            ID_TrackSelectedEntity, 
            "Track selected entity\tCtrl+T",
            "Open an object inspector that always inspects the last " \
            "selected entity" )
        self.menu_cameras.create()
        self.AppendMenu( -1, "&Cameras", self.menu_cameras )
        self.menu_lights.create()
        self.AppendMenu( -1, "&Lights", self.menu_lights )
        self.AppendMenu( -1, "&Indoors", self.menu_indoors )
        self.AppendMenu( -1, "&Geometry Streams", self.menu_streams )
        self.AppendMenu( -1, "Loading &Areas", self.menu_areas )
        self.AppendMenu( -1, "&Variables", self.menu_variables )
        self.AppendSeparator()
        self.AppendCheckItem( ID_SysInfo, "System &info" )
        self.AppendCheckItem( ID_Preferences, "&Preferences" )
        
        # bindings
        togwinmgr = self.get_frame().get_togwinmgr()
        self.bind_function(ID_Undo, self.on_undo)
        self.bind_function(ID_Redo, self.on_redo)
        self.bind_function(ID_Cut, self.on_cut)
        self.bind_function(ID_Copy, self.on_copy)
        self.bind_function(ID_Paste, self.on_paste)
        self.bind_function(ID_Delete, self.on_delete)
        self.bind_function(ID_Batch, self.on_batch)
        self.bind_function(ID_Unbatch, self.on_unbatch)
        self.bind_function(ID_Autobatch, self.on_autobatch)
        self.bind_function(ID_Load, self.on_load)
        self.bind_function(ID_Unload, self.on_unload)
        self.bind_function(ID_Reload, self.on_reload)
        self.bind_function(ID_SaveSelection, self.on_saveselection)
        self.bind_function(ID_LoadSelection, self.on_loadselection)
        togwinmgr.add_window(
            layersdlg.create_window,
            self.FindItemById(ID_LayerManager)
            )
        togwinmgr.add_window(
            soundlibdlg.create_window,
            self.FindItemById(ID_SoundLibrary)
            )
        togwinmgr.add_window(
            particlelistdlg.create_window,
            self.FindItemById(ID_ParticleSystems)
            )
        togwinmgr.add_window(
            objtrackdlg.create_window,
            self.FindItemById(ID_TrackSelectedEntity)
            )
        self.bind_function(
            ID_SelectedEntities, 
            self.on_selected_entities
            )
        togwinmgr.add_window(
            sysdlg.create_window,
            self.FindItemById(ID_SysInfo)
            )
        togwinmgr.add_window(
            prefsdlg.create_window,
            self.FindItemById(ID_Preferences)
            )
    
    def refresh(self):
        self.menu_cameras.refresh()
        self.menu_lights.refresh()
        self.menu_indoors.refresh()
        self.menu_streams.refresh()
        self.menu_areas.refresh()
        self.menu_variables.refresh()
    
    def on_undo(self, event):
        """Undo the last done command"""
        try:
            # Apply undo on current editor if it implements it...
            active_editor = self.get_frame().GetActiveChild()
            if active_editor.handle_undo():
                return
        except:
            pass
        # ...otherwise delegate the undo to the command server
        servers.get_command_server().undo()
        # and update any inspector whose values may have changed due to undo
        self.get_frame().refresh_scene_values()
    
    def on_redo(self, event):
        """Redo the last undone command"""
        try:
            # Apply redo on current editor if it implements it...
            active_editor = self.get_frame().GetActiveChild()
            if active_editor.handle_redo():
                return
        except:
            pass
        # ...otherwise delegate the redo to the command server
        servers.get_command_server().redo()
        # and update any inspector whose values may have changed due to undo
        self.get_frame().refresh_scene_values()
    
    def on_cut(self, event):
        """Cut selected entities"""
        try:
            # Apply cut on current editor if it implements it...
            active_editor = self.get_frame().GetActiveChild()
            if active_editor.handle_cut():
                return
        except:
            pass
        # ...otherwise delegate the cut to the object state
        app.get_object_state().cutentities()
    
    def on_copy(self, event):
        """Copy selected entities"""
        try:
            # Apply copy on current editor if it implements it...
            active_editor = self.get_frame().GetActiveChild()
            if active_editor.handle_copy():
                return
        except:
            pass
        # ...otherwise delegate the copy to the object state
        app.get_object_state().copyentities()
    
    def on_paste(self, event):
        """Paste entities in clipboard"""
        try:
            # Apply paste on current editor if it implements it...
            active_editor = self.get_frame().GetActiveChild()
            if active_editor.handle_paste():
                return
        except:
            pass
        # ...otherwise delegate the paste to the object state
        app.get_object_state().pasteentities()
    
    def on_delete(self, event):
        """Delete selected entities"""
        # undo batches before removing them
        for entity in self.get_selected_entities():
            if entity.hascomponent('ncSpatialBatch'):
                entity.clearsubentities()
        # delete the selected entities
        app.get_object_state().delentities()
    
    def on_batch(self, event):
        """Batch the selected entities"""
        dlg = batchdlg.NewBatchDialog(self.get_frame(), batchdlg.SELECTION)
        dlg.ShowModal()
        dlg.Destroy()

    def on_autobatch(self, event):
        """Batch automatically the non yet batched entities"""
        #trn.get_terrain_module().createterrainbatches()
        dlg = batchdlg.NewBatchDialog(self.get_frame(), batchdlg.TERRAIN)
        dlg.ShowModal()
        dlg.Destroy()
    
    def on_unbatch(self, event):
        """Unbatch the selected entities"""
        app.get_object_state().removeentitiesfrombatch()

    def on_load(self, event):
        """Load resources of selected entities"""
        app.get_object_state().loadentities()
    
    def on_unload(self, event):
        """Unload resources of selected entities"""
        app.get_object_state().unloadentities()
    
    def on_reload(self, event):
        """Reload resources of selected entities"""
        app.get_object_state().reloadentities()

    def on_saveselection(self, event):
        """Save a selection of entities for later use"""
        dlg = wx.FileDialog(
                    self.get_frame(), 
                    message="Enter a file name",
                    wildcard="Nebula2 file (*.n2)|*.n2",
                    style=wx.SAVE|wx.OVERWRITE_PROMPT|wx.CHANGE_DIR
                    )

        if dlg.ShowModal() == wx.ID_OK:
            path = dlg.GetPath()
            if dlg.GetFilterIndex() == 0 and not path.endswith(".n2"):
                path = path + ".n2" 
            if not app.get_object_state().saveselectiontofile( str(path) ):
                msg = "Unable to save the selection to file '%s'" % path
                cjr.show_error_message(msg)

        dlg.Destroy()

    def on_loadselection(self, event):
        """Load a saved selection of entities """
        dlg = wx.FileDialog(
                    self.get_frame(), 
                    message="Choose a file",
                    wildcard="Nebula2 file (*.n2)|*.n2",
                    style=wx.OPEN|wx.CHANGE_DIR
                    )

        if dlg.ShowModal() == wx.ID_OK:
            path = dlg.GetPath()
            if not app.get_object_state().loadselectionfromfile( str(path) ):
                msg = "Unable to load saved selection from file '%s'" % path
                cjr.show_error_message(msg)

        dlg.Destroy()

    def on_selected_entities(self, event):
        """Open a property editor on the selected entities"""
        entity_list = self.get_selected_entities()
        select_count = len(entity_list)
        if select_count > 1:
            model = scriptingmodelmultiobject.ScriptingModelMultiObject(
                            entity_list
                            )
            propertyview.create_window(
                self.get_frame(), 
                model 
                )
        elif select_count == 1:
            entity = entity_list[0]
            entity_id = entity.getid()
            win = objdlg.create_window(
                        self.get_frame(), 
                        entity_id
                        )
            win.display()
        else:
            cjr.show_error_message(
                "There are no entities currently selected"
                )

    def get_selected_entities(self):
        state = app.get_object_state()
        number_of_items = state.getselectioncount()
        entity_list = []
        for index in range (number_of_items):
            entity_list.append(
                state.getselectedentity(index) 
                )
        return entity_list

    def on_selected_objects (self, event):
        """ Open an object inspector for each one 
            of the current selected entities. """
        for each_entity in self.get_selected_entities():
            entity_id = each_entity.getid()
            name = app.get_level().getentityname( entity_id )
            if name == ':null:':
                win = objdlg.create_window(
                            self.get_frame(), 
                            entity_id 
                            )
                win.display()
            else:
                win = objdlg.create_window(
                            self.get_frame(), 
                            name 
                            )
                win.display()

