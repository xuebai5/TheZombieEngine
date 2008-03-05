##\file viewcmds.py
##\brief View menu commands

import wx

import app
import grimoire
import layoutdlg
import menu
import nebulaguisettings as cfg
import objbrowserwindow
import servers
import pynebula
import entitybrowser
import conjurerframework as cjr

# View menu IDs
ID_SnapToGrid = wx.NewId()
ID_AngleSnap = wx.NewId()
ID_SizeSnap = wx.NewId()
ID_LocalTransform = wx.NewId()
ID_MoveThroughTerrain = wx.NewId()
ID_MoveToTerrain = wx.NewId()
ID_MoveOnTerrain = wx.NewId()
ID_LockSelection = wx.NewId()
ID_SelectObjects = wx.NewId()
ID_SelectWaypoints = wx.NewId()
ID_SelectTerrainCells = wx.NewId()
ID_ViewportsLayout = wx.NewId()
ID_SingleViewport = wx.NewId()
ID_ObjBrowser = wx.NewId()
ID_Grimoire = wx.NewId()
ID_EntityBrowser = wx.NewId()
ID_ViewToolbar = wx.NewId()
ID_LoadWindowLayout = wx.NewId()
ID_SaveWindowLayout = wx.NewId()

# EditMenu class
class ViewMenu(menu.Menu):
    """Err... The view menu"""

    def __init__(self, frame):
        menu.Menu.__init__(self, frame)

    def create(self):
        # menu items
        self.AppendCheckItem( ID_SnapToGrid, "Snap to &grid" )
        self.AppendCheckItem( ID_AngleSnap, "&Angle snap" )
        self.AppendCheckItem( ID_SizeSnap, "Si&ze snap" )
        self.AppendSeparator()
        self.AppendCheckItem( ID_LocalTransform, "&Local transform" )
        self.AppendRadioItem(
            ID_MoveThroughTerrain, 
            "&No terrain collision",
            "Translate objects without colliding with the terrain" 
            )
        self.AppendRadioItem(
            ID_MoveToTerrain, 
            "&Terrain collision",
            "Translate objects colliding with the terrain" 
            )
        self.AppendRadioItem( 
            ID_MoveOnTerrain, 
            "&Snap to terrain",
            "Translate objects snapping them on the terrain" 
            )
        self.AppendSeparator()
        self.AppendCheckItem( ID_LockSelection, "Loc&k selection" )
        self.AppendRadioItem(
            ID_SelectObjects, 
            "Select objects mode",
            "Select and transform tools work on objects" 
            )
        self.AppendRadioItem(
            ID_SelectWaypoints, 
            "&Select subentities mode",
            "Select and transform tools work on subentities " \
                "(for example, waypoints)"
            )
        self.AppendRadioItem(
            ID_SelectTerrainCells, 
            "Select terrain cells mode",
            "Select tool work on terrain cells" 
            )
        self.AppendSeparator()
        self.AppendCheckItem( ID_ViewportsLayout, "&Viewports layout" )
        self.AppendCheckItem( ID_SingleViewport, "S&ingle viewport\tAlt+W" )
        self.AppendSeparator()
        self.Append( ID_ObjBrowser, "&Object Browser\tCtrl+O" )
        self.Append( ID_Grimoire, "&Grimoire\tCtrl+G" )
        self.AppendCheckItem( ID_EntityBrowser, "&Entity Browser\tCtrl+E" )
        self.AppendSeparator()
        self.AppendCheckItem( ID_ViewToolbar, "&Toolbar" )
        self.AppendSeparator()
        self.Append ( ID_LoadWindowLayout, "Load &window layout from file...")
        self.Append ( ID_SaveWindowLayout, "Save window layout to &file...")

        # initial state
        self.Check( ID_LocalTransform, True )
        self.Check( ID_MoveThroughTerrain, False )
        self.Check( ID_ViewToolbar, True )
        
        # bindings
        self.bind_function(ID_SnapToGrid, self.on_snap_to_grid)
        self.bind_function(ID_AngleSnap, self.on_angle_snap)
        self.bind_function(ID_SizeSnap, self.on_size_snap)
        self.bind_function(ID_LocalTransform, self.on_local_transform)
        self.bind_function(ID_MoveThroughTerrain, self.on_move_through_terrain)
        self.bind_function(ID_MoveToTerrain, self.on_move_to_terrain)
        self.bind_function(ID_MoveOnTerrain, self.on_move_on_terrain)
        self.bind_function(ID_LockSelection, self.on_lock_selection)
        self.bind_function(ID_SelectObjects, self.on_select_objects)
        self.bind_function(ID_SelectWaypoints, self.on_select_waypoints)
        togwinmgr = self.get_frame().get_togwinmgr()
        togwinmgr.add_window(
            layoutdlg.create_window,
            self.FindItemById(ID_ViewportsLayout)
            )
        self.bind_function(ID_SingleViewport, self.__on_single_viewport)
        self.bind_function(ID_ObjBrowser, self.__on_obj_browser)
        self.bind_function(ID_Grimoire, self.__on_grimoire)
        togwinmgr.add_window(
            entitybrowser.create_window,
            self.FindItemById(ID_EntityBrowser)
            )
        self.bind_function(ID_ViewToolbar, self.__on_view_toolbar)
        self.bind_function(ID_LoadWindowLayout, self.__on_load_window_layout)
        self.bind_function(ID_SaveWindowLayout, self.__on_save_window_layout)

    def refresh( self ):
        """Update some checkmarks"""
        self.Check( ID_SnapToGrid,
            cfg.Repository.getsettingvalue(cfg.ID_Grid)['snap enabled'] )
        self.Check( ID_AngleSnap,
            cfg.Repository.getsettingvalue(cfg.ID_AngleSnapping)['enabled'] )
        self.Check( ID_SizeSnap,
            cfg.Repository.getsettingvalue(cfg.ID_SizeSnapping)['enabled'] )
        self.Check( ID_SingleViewport,
            servers.get_viewport_ui().getsingleviewport() )
        pynebula.pyBindSignal(
            app.get_object_state(), 
            'lockedmodechanged',
            self, 
            'on_locked_mode_changed', 
            0
            )

    def __get_toolbar(self):
        return self.get_frame().get_toolbar()

    def on_locked_mode_changed(self):
        self.Check(
            ID_LockSelection, 
            app.get_select_object_tool().getlockedmode() 
            )
        self.__get_toolbar().ToggleTool(
            ID_LockSelection, 
            app.get_select_object_tool().getlockedmode() 
            )
    
    def on_snap_to_grid(self, event):
        """Toggle snap to grid"""
        grid = cfg.Repository.getsettingvalue(cfg.ID_Grid)
        grid['snap enabled'] = event.Checked()
        app.get_grid().setsnapenabled( grid['snap enabled'] )
    
    def on_angle_snap(self, event):
        """Toggle angle snap"""
        angle = cfg.Repository.getsettingvalue(cfg.ID_AngleSnapping)
        angle['enabled'] = event.Checked()
        if angle['enabled']:
            app.get_rotate_tool().setsnap( angle['angle'] )
        else:
            app.get_rotate_tool().setsnap(0)
    
    def on_size_snap(self, event):
        """Toggle size snap"""
        size = cfg.Repository.getsettingvalue(cfg.ID_SizeSnapping)
        size['enabled'] = event.Checked()
        if size['enabled']:
            app.get_scale_tool().setsnap( size['size'] )
        else:
            app.get_scale_tool().setsnap(0)
    
    def on_local_transform(self, event):
        """Switch between local and world transform"""
        local = event.Checked()
        self.Check( ID_LocalTransform, local )
        self.__get_toolbar().toggle_axis_transform(local)
        app.get_object_state().setworldmode( not local )
    
    def on_move_through_terrain(self, event):
        """Toggle move through terrain"""
        enabled = event.Checked()
        self.Check( ID_MoveThroughTerrain, enabled )
        self.__get_toolbar().select_move_mode( 0 )
        app.get_object_state().setcanmovethroughterrain( 0 )
    
    def on_move_to_terrain(self, event):
        """Toggle move to terrain"""
        enabled = event.Checked()
        self.Check( ID_MoveToTerrain, enabled )
        self.__get_toolbar().select_move_mode( 1 )
        app.get_object_state().setcanmovethroughterrain( 1 )
    
    def on_move_on_terrain(self, event):
        """Toggle move on terrain"""
        enabled = event.Checked()
        self.Check( ID_MoveOnTerrain, enabled )
        self.__get_toolbar().select_move_mode( 2 )
        app.get_object_state().setcanmovethroughterrain( 2 )
    
    def on_lock_selection(self, event):
        """Lock/unlock the current selection"""
        lock = event.Checked()
        self.Check( ID_LockSelection, lock )
        self.__get_toolbar().ToggleTool( ID_LockSelection, lock )
        app.get_select_object_tool().setlockedmode( lock )
    
    def on_select_objects(self, event):
        """Set the selection objects mode"""
        enabled = event.Checked()
        self.Check( ID_SelectObjects, enabled )
        msg = "Do you want to save subentity changes?"
        result = cjr.confirm_yes_no(
                        app.get_top_window(),
                        msg
                        )
        save = (result == wx.ID_YES)
        succeed = app.get_object_state().setselectionmode( 0, save )
        if succeed:
            self.__get_toolbar().select_selection_mode( 0 )

    def on_select_waypoints(self, event):
        """Set the selection waypoints mode"""
        enabled = event.Checked()
        self.Check( ID_SelectWaypoints, enabled )
        succeed = app.get_object_state().setselectionmode( 1, False )
        if succeed:
            self.__get_toolbar().select_selection_mode( 1 )

    def __on_single_viewport(self, event):
        """Switch between single and multiple viewports"""
        layoutdlg.set_single_viewport(
            event.Checked()
            )

    def __on_obj_browser(self, evt):
        """Open an object browser"""
        try:
            try:
                wx.BeginBusyCursor()
                browser = objbrowserwindow.create_window(
                                self.get_frame()
                                )
            finally:
                wx.EndBusyCursor()
        except:
            # make sure any errors are not hidden
            raise
        browser.Show()

    def __on_grimoire(self, evt):
        """Open an object browser"""
        try:
            try:
                wx.BeginBusyCursor()
                win = grimoire.create_window(
                            self.get_frame()
                            )
            finally:
                wx.EndBusyCursor()
        except:
            # make sure any errors are not hidden
            raise
        win.Show()

    def __on_view_toolbar(self, evt):
        """Toggle toolbar display"""
        # How #(/*$&?! can the toolbar be hidden?
        # HAHAHA, this works!
        self.__get_toolbar().Show( evt.Checked() )
        self.get_frame().SendSizeEvent()

    def __on_load_window_layout(self, evt):
        """Load a window layout from file"""
        self.get_frame().load_window_layout()

    def __on_save_window_layout(self, evt):
        """Save the current window layout to file."""
        self.get_frame().save_current_window_layout()
