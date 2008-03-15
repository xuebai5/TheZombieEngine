##\file aicmds.py
##\brief AI menu commands

import wx

import aitestdlg
import app
import format
import fsmattachdlg
import fsmlibs
import iehelpwindow
import menu
import objdlg
import servers
import waitdlg
import waypointdlg

import conjurerframework as cjr


# AI menu IDs
ID_BuildOutdoor = wx.NewId()
ID_BuildIndoors = wx.NewId()
ID_LinkNavGraphs = wx.NewId()
ID_NavGenParams = wx.NewId()
ID_SaveOutdoor = wx.NewId()
ID_LoadTempMesh = wx.NewId()
ID_SaveTempMesh = wx.NewId()
ID_FSMsLib = wx.NewId()
##ID_ConditionScriptsLib = wx.NewId()
##ID_FSMSelectionScriptsLib = wx.NewId()
##ID_ActionScriptsLib = wx.NewId()
ID_AttachFSM = wx.NewId()
ID_FSMHelp = wx.NewId()
##ID_TriggerScriptsLib = wx.NewId()
ID_MissionObjectives = wx.NewId()
ID_WaypointEditor = wx.NewId()
ID_AITestSpeed = wx.NewId()
ID_SaveGameMaterials = wx.NewId()
ID_UpdateGameMaterials = wx.NewId()


# NavMeshMenu class
class NavMeshMenu(menu.Menu):
    """Err... The navigation mesh menu"""
    
    def __init__(self, frame):
        menu.Menu.__init__(self, frame)
    
    def create(self):
        # menu items
        self.Append( ID_BuildOutdoor, "Build &outdoor mesh" )
        self.Append( ID_BuildIndoors, "Build selected &indoors meshes")
        self.Append( ID_LinkNavGraphs, "&Connect all navigation graphs")
        self.AppendSeparator()
        self.Append( ID_NavGenParams, "Mesh generation &params")
        self.AppendSeparator()
        self.Append( ID_SaveOutdoor, "&Save outdoor mesh as..." )
        self.Append( ID_LoadTempMesh, "Load temporary mesh..." )
        self.Append( ID_SaveTempMesh, "Save temporary mesh..." )
        
        # bindings
        self.bind_function(ID_BuildOutdoor, self.on_build_outdoor)
        self.bind_function(ID_BuildIndoors, self.on_build_indoors)
        self.bind_function(ID_LinkNavGraphs, self.on_link_nav_graphs)
        self.bind_function(ID_NavGenParams, self.on_nav_gen_params)
        self.bind_function(ID_SaveOutdoor, self.on_save_outdoor)
        self.bind_function(ID_LoadTempMesh, self.on_load_temp_mesh)
        self.bind_function(ID_SaveTempMesh, self.on_save_temp_mesh)
    
    def on_build_outdoor(self, event):
        """Build a navigation mesh for the outdoor"""
        dlg = waitdlg.WaitDialog( self.get_frame(),
            "Building navigation mesh..." )
        navbuilder = app.get_navbuilder()
        success = navbuilder.buildnavmesh()
        dlg.Destroy()
        if not success:
            cjr.show_error_message(
                "Unable to create the navigation mesh"
                )
    
    def on_build_indoors(self, event):
        """Build navigation meshes for all selected indoors"""
        state = app.get_object_state()
        navbuilder = app.get_navbuilder()
        dlg = waitdlg.WaitDialog( self.get_frame(),
            "Building navigation mesh for indoor 00/00..." )
        num_entities = state.getselectioncount()
        for i in range( num_entities ):
            dlg.set_message( "Building navigation mesh for indoor " \
                + str(i+1) + "/" + str(num_entities) + "..." )
            entity = state.getselectedentity(i)
            success = True
            if entity.isa('neindoor'):
                success = navbuilder.buildindoornavmesh( entity )
            elif entity.isa('newalkablebrush'):
                success = navbuilder.buildbrushnavmesh( entity )
            if not success:
                msg = "Unable to create navigation mesh for entity %s" % str( entity.getid() )
                cjr.show_error_message(msg)
        dlg.Destroy()
    
    def on_link_nav_graphs(self, event):
        """Generate external links to connect superimposed navigation graphs"""
        navbuilder = app.get_navbuilder()
        navbuilder.generateexternallinks()
    
    def on_nav_gen_params(self, event):
        """Open an object inspector for the navigation mesh builder state"""
        navbuilder = app.get_navbuilder()
        win = objdlg.create_window(
                    self.get_frame(), 
                    navbuilder.getfullname() 
                    )
        win.display()

    def on_save_outdoor(self, event):
        """Save the outdoor's navigation mesh on a user specified file"""
        outdoor = app.get_outdoor_obj()
        if outdoor == None:
            cjr.show_error_message(
                "The current level doesn't have an outdoor"
                )
            return
        dlg = wx.FileDialog(
            self.get_frame(), message="Enter a file name",
            wildcard="Navigation mesh ASCII file (*.txt)|*.txt|" \
                     "Navigation mesh binary file (*.nav)|*.nav|" \
                     "Navigation mesh compressed file (*.rle)|*.rle",
            defaultDir=format.mangle_path( "level:ai/" + \
                       hex(outdoor.getid())[2:] ),
            defaultFile="navmesh",
            style=wx.SAVE | wx.OVERWRITE_PROMPT
            )
        
        if dlg.ShowModal() == wx.ID_OK:
            path = dlg.GetPath()
            # Append extension if needed
            if dlg.GetFilterIndex() == 0 and not path.endswith(".txt"):
                path = path + ".txt"
            elif dlg.GetFilterIndex() == 1 and not path.endswith(".nav"):
                path = path + ".nav"
            elif dlg.GetFilterIndex() == 2 and not path.endswith(".rle"):
                path = path + ".rle"
            # Save the navigation mesh
            navbuilder = app.get_navbuilder()
            if not navbuilder.saveoutdoornavmesh( str(dlg.GetPath()) ):
                msg = "Unable to save the navigation mesh file '%s'" % dlg.GetPath()
                cjr.show_error_message(msg)
        dlg.Destroy()
    
    def on_load_temp_mesh(self, event):
        """Show a file browser and load the selected navigation mesh file
        as the temporary mesh"""
        dlg = wx.FileDialog(
            self.get_frame(), message="Choose a file",
            wildcard="Navigation mesh files (*.txt,*.nav,*.rle)|*.txt;*.nav;*.rle",
            style=wx.OPEN | wx.CHANGE_DIR
            )
        
        if dlg.ShowModal() == wx.ID_OK:
            navbuilder = app.get_navbuilder()
            if not navbuilder.loadnavmesh( str(dlg.GetPath()) ):
                msg = "Unable to load navigation mesh file '%s'"  % dlg.GetPath()
                cjr.show_error_message(msg)

        dlg.Destroy()
    
    def on_save_temp_mesh(self, event):
        """Show a file browser and save the temporary mesh to the selected
        file"""
        dlg = wx.FileDialog(
            self.get_frame(), message="Enter a file name",
            wildcard="Navigation mesh ASCII file (*.txt)|*.txt|" \
                     "Navigation mesh binary file (*.nav)|*.nav|" \
                     "Navigation mesh compressed file (*.rle)|*.rle",
            style=wx.SAVE | wx.OVERWRITE_PROMPT | wx.CHANGE_DIR
            )
        
        if dlg.ShowModal() == wx.ID_OK:
            path = dlg.GetPath()
            # Append extension if needed
            if dlg.GetFilterIndex() == 0 and not path.endswith(".txt"):
                path = path + ".txt"
            elif dlg.GetFilterIndex() == 1 and not path.endswith(".nav"):
                path = path + ".nav"
            elif dlg.GetFilterIndex() == 2 and not path.endswith(".rle"):
                path = path + ".rle"
            # Save the navigation mesh
            navbuilder = app.get_navbuilder()
            if not navbuilder.savenavmesh( str(dlg.GetPath()) ):
                msg = "Unable to save the navigation mesh file '%s'" % dlg.GetPath()
                cjr.show_error_message(msg)
        
        dlg.Destroy()


# FSMMenu class
class FSMMenu(menu.Menu):
    """Err... The finite state machine menu"""
    
    def __init__(self, frame):
        menu.Menu.__init__(self, frame)
    
    def create(self):
        # menu items
        self.AppendCheckItem( ID_FSMsLib, "&FSMs library" )
        self.AppendCheckItem( ID_AttachFSM, "FSM attacher" )
##        self.AppendCheckItem( ID_ConditionScriptsLib,
##            "&Condition scripts library" )
##        self.AppendCheckItem( ID_FSMSelectionScriptsLib,
##            "FSM &selection scripts library" )
##        self.AppendCheckItem( ID_ActionScriptsLib, "&Action scripts library" )
        self.AppendSeparator()
        self.AppendCheckItem( ID_FSMHelp, "&Help..." )
        
        # bindings
        togwinmgr = self.get_frame().get_togwinmgr()
        togwinmgr.add_window( fsmlibs.create_fsm_lib_window,
            self.FindItemById(ID_FSMsLib) )
##        togwinmgr.add_window( fsmlibs.create_condition_scripts_lib_window,
##            self.FindItemById(ID_ConditionScriptsLib) )
##        togwinmgr.add_window( fsmlibs.create_fsm_selector_scripts_lib_window,
##            self.FindItemById(ID_FSMSelectionScriptsLib) )
##        togwinmgr.add_window( fsmlibs.create_action_scripts_lib_window,
##            self.FindItemById(ID_ActionScriptsLib) )
        togwinmgr.add_window( fsmattachdlg.create_window,
            self.FindItemById(ID_AttachFSM) )
        self.bind_function(ID_FSMHelp, self.on_help)
    
    def on_help(self, event):
        iehelpwindow.CreateWindow( self.get_frame(),
            "http://thezombieengine.sourceforge.net/SWD+Nebula+Conjurer+FSM+editor" )


# TriggersMenu class
##class TriggersMenu(menu.Menu):
##    """Err... The triggers menu"""
##    
##    def __init__(self, frame):
##        menu.Menu.__init__(self, frame)
##    
##    def create(self):
##        # menu items
##        self.AppendCheckItem( ID_TriggerScriptsLib,
##            "Trigger scripts library" )
##        
##        # bindings
##        togwinmgr = self.get_frame().get_togwinmgr()
##        togwinmgr.add_window( triggerlib.create_trigger_scripts_lib_window,
##            self.FindItemById(ID_TriggerScriptsLib) )


# AIMenu class
class AIMenu(menu.Menu):
    """Err... The artificial intelligence menu"""
    
    def __init__(self, frame):
        menu.Menu.__init__(self, frame)
        self.menu_navmesh = NavMeshMenu(frame)
        self.menu_fsm = FSMMenu(frame)
##        self.menu_triggers = TriggersMenu(frame)
    
    def create(self):
        # menu items
        self.menu_navmesh.create()
        self.AppendMenu( -1, "&Navigation mesh", self.menu_navmesh )
        self.AppendCheckItem( ID_WaypointEditor, "&Waypoint editor" )
        self.AppendSeparator()
        self.menu_fsm.create()
        self.AppendMenu( -1, "&Finite state machines", self.menu_fsm )
##        self.menu_triggers.create()
##        self.AppendMenu( -1, "&Triggers", self.menu_triggers )
        self.AppendCheckItem( ID_AITestSpeed, "AI test settings" )
        self.AppendSeparator()
        self.Append( ID_MissionObjectives, "Mission &objectives" )
        self.AppendSeparator()
        self.Append( ID_SaveGameMaterials, "Save Game Materials" )
        self.Append( ID_UpdateGameMaterials, "Update Game Materials\tCtrl+U" )
        
        # bindings
        self.bind_function(ID_MissionObjectives, self.on_mission_objectives)
        togwinmgr = self.get_frame().get_togwinmgr()
        togwinmgr.add_window( waypointdlg.create_window,
            self.FindItemById(ID_WaypointEditor) )
        togwinmgr.add_window( aitestdlg.create_window,
            self.FindItemById(ID_AITestSpeed) )
        self.bind_function(ID_SaveGameMaterials, self.on_save_gamematerials)
        self.bind_function(ID_UpdateGameMaterials, self.on_update_gamematerials)
        
    def on_save_gamematerials(self, event):
        servers.get_game_materials_server().save()
    
    def on_update_gamematerials(self, event):
        servers.get_game_materials_server().update()

    def on_mission_objectives(self, event):
        path = servers.get_mission_handler().getfullname()
        objdlg.create_window( self.get_frame(), path )
