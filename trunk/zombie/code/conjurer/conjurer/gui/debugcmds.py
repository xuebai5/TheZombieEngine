##\file debugcmds.py
##\brief Debug menu commands

import wx

import pynebula

import app
import debugopsdlg
import logdlg
import menu
import monitordlg
import reloaddlg
import servers
import watcherdlg


# Debug menu IDs
ID_ToggleAI = wx.NewId()
ID_DebugOptions = wx.NewId()
ID_LogInspector = wx.NewId()
ID_OutGUIReloader = wx.NewId()
ID_ToggleSound = wx.NewId()
ID_VarsMonitor = wx.NewId()
ID_Watcher = wx.NewId()


# OptionsMenu class
class OptionsMenu(menu.Menu):
    """Err... The debug options menu"""
    
    def __init__(self, frame):
        menu.Menu.__init__(self, frame)
        self.debug_modules = {}

    def create(self):
        pass

    def refresh(self):
        """
        Rebuild the debug options menu.
        
        The menu is filled with the names of all those modules registered in
        the debug server.
        """
        # Remove old menu items
        for key in self.debug_modules.keys():
            self.unbind_function(key)
            self.Delete(key)
        self.debug_modules = {}

        # Add current debug modules
        modules = servers.get_debug_server_modules_dir()
        module = modules.gethead()
        while module is not None:
            menu_id = wx.NewId()
            module_name = module.getname()
            self.debug_modules[menu_id] = module_name
            self.Append(menu_id,  module_name)
            self.bind_function(menu_id, self.on_debug_options)
            module = module.getsucc()

    def on_debug_options(self, event):
        """Show a debug options dialog for the chosen module"""
        module_name = self.debug_modules[event.GetId()]
        win = debugopsdlg.create_window(
                    self.get_frame(),
                    module_name
                    )
        win.display()


# DebugMenu class
class DebugMenu(menu.Menu):
    """Err... The debug menu"""
    
    def __init__(self, frame):
        menu.Menu.__init__(self, frame)
        self.first_refresh = True
        self.menu_options = OptionsMenu(frame)
    
    def create(self):
        # menu items
        self.menu_options.create()
        self.AppendMenu( -1, "&Options", self.menu_options )
        self.AppendCheckItem( ID_ToggleAI, "Toggle &AI" )
        self.AppendCheckItem( ID_ToggleSound, "Toggle &sound" )
        self.AppendSeparator()
        self.AppendCheckItem( ID_LogInspector, "&Log inspector" )
        self.AppendCheckItem( ID_Watcher, "Variable and command &watcher" )
        self.AppendCheckItem( ID_VarsMonitor, "Variable &monitor" )
        self.AppendSeparator()
        self.AppendCheckItem( ID_OutGUIReloader, "OutGUI &reloader" )
        
        # bindings
        togwinmgr = self.get_frame().get_togwinmgr()
        self.bind_function(ID_ToggleAI, self.on_toggle_ai)
        self.bind_function(ID_ToggleSound, self.on_toggle_sound)
        togwinmgr.add_window(
            logdlg.create_window,
            self.FindItemById(ID_LogInspector)
            )
        togwinmgr.add_window(
            watcherdlg.create_window,
            self.FindItemById(ID_Watcher)
            )
        togwinmgr.add_window(
            monitordlg.create_window,
            self.FindItemById(ID_VarsMonitor)
            )
        togwinmgr.add_window(
            reloaddlg.create_window,
            self.FindItemById(ID_OutGUIReloader)
            )

    def on_toggle_ai(self, event):
        enable = event.Checked() or app.is_in_gameplay_mode()
        self.Check( ID_ToggleAI, enable )
        self.get_frame().get_toolbar().ToggleTool( ID_ToggleAI, enable )
        if enable:
            try:
                try:
                    wx.BeginBusyCursor()
                    servers.get_command_server().clean()
                    servers.get_conjurer().saveeditorstate()
                    app.get_ai_tester().enablegameplayupdating()
                finally:
                    wx.EndBusyCursor()
            except:
                # make sure any errors are not hidden
                raise
        else:
            app.get_ai_tester().disablegameplayupdating()
            servers.get_conjurer().restoreeditorstate()

    def on_toggle_sound(self, event):
        enable = event.Checked()
        self.Check( ID_ToggleSound, enable )
        self.get_frame().get_toolbar().ToggleTool( ID_ToggleSound, enable )
        if enable:
            app.get_ai_tester().enablesoundsources()
        else:
            app.get_ai_tester().disablesoundsources()
            e_server = servers.get_entity_object_server()
            entity = e_server.getfirstentityobject()
            while entity is not None:
                if entity.hascomponent('ncAreaTrigger'):
                    entity.resets()
                if entity.hascomponent('ncSound'):
                    entity.disablesoundtrigger()
                entity = e_server.getnextentityobject()

    def refresh(self):
        self.menu_options.refresh()
        if self.first_refresh:
            # Do the binding here because the object state doesn't exist yet
            # when this menu is created
            pynebula.pyBindSignal(
                app.get_ai_tester(), 
                'gameplayupdatingenabled',
                self, 
                'ongameplayupdatingenabled', 
                0,
                )
            pynebula.pyBindSignal(
                app.get_ai_tester(), 
                'gameplayupdatingdisabled',
                self, 
                'ongameplayupdatingdisabled', 
                0
                )
            pynebula.pyBindSignal(
                app.get_ai_tester(), 
                'soundsourcesenabled',
                self, 
                'onsoundsourcesenabled', 
                0
                )
            pynebula.pyBindSignal(
                app.get_ai_tester(), 
                'soundsourcesdisabled',
                self, 
                'onsoundsourcesdisabled', 
                0
                )
            self.first_refresh = False
    
    def ongameplayupdatingenabled(self):
        """Check the menu option and tool button for the AI toggle"""
        self.Check( ID_ToggleAI, True )
        self.get_frame().get_toolbar().ToggleTool( ID_ToggleAI, True )
    
    def ongameplayupdatingdisabled(self):
        """Uncheck the menu option and tool button for the AI toggle"""
        self.Check( ID_ToggleAI, False )
        self.get_frame().get_toolbar().ToggleTool( ID_ToggleAI, False )
    
    def onsoundsourcesenabled(self):
        """Check the menu option and tool button for the sound toggle"""
        self.Check( ID_ToggleSound, True )
        self.get_frame().get_toolbar().ToggleTool( ID_ToggleSound, True )
    
    def onsoundsourcesdisabled(self):
        """Uncheck the menu option and tool button for the sound toggle"""
        self.Check( ID_ToggleSound, False )
        self.get_frame().get_toolbar().ToggleTool( ID_ToggleSound, False )
