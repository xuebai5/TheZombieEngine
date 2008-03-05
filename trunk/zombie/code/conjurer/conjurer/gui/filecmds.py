##\file filecmds.py
##\brief File menu commands

import wx

import pynebula

import app
import cmdmgrdlg
import dialoggui
import events
import format
import fsm
import levelcmds
import luashellwindow
import menu
import nebulaguisettings as guisettings
import pyshellwindow
import scripteditordlg
import servers
import waitdlg

import conjurerframework as cjr
import conjurerconfig as cfg


# File menu IDs
ID_New = wx.NewId() # Really only used by the toolbar's new icon
ID_NewScript = wx.NewId()
ID_Open = wx.NewId()
ID_Save = wx.NewId()
ID_SaveAs = wx.NewId()
ID_SaveAll = wx.NewId()
ID_Close = wx.NewId()
ID_PythonShell = wx.NewId()
ID_LuaShell = wx.NewId()
ID_TemporaryMode = wx.NewId()
ID_Quit = wx.NewId()
ID_QuitConjurer = wx.NewId()


# NewFileMenu class
class NewFileMenu(menu.Menu):
    """Err... The new file menu"""

    def __init__(self, frame):
        menu.Menu.__init__(self, frame)

    def create(self):
        self.Append( ID_NewScript, "&Script" )
        self.bind_function(ID_NewScript, self.on_new_script)

    def on_new_script(self, event):
        """Open an empty script editor"""
        scripteditordlg.create_window(self.get_frame())


# FileMenu class
class FileMenu(menu.Menu):
    """Err... The file menu"""

    def __init__(self, frame):
        menu.Menu.__init__(self, frame)
        self.menu_new = NewFileMenu(frame)

    def create(self):
        # menu items
        self.menu_new.create()
        self.AppendMenu( -1, "&New", self.menu_new )
        self.Append( ID_Open, "&Open..." )
        self.Append( ID_Save, "&Save" )
        self.Append( ID_SaveAs, "Save &As..." )
        self.Append( ID_SaveAll, "Sa&ve All" )
        self.Append( ID_Close, "&Close" )
        self.AppendSeparator()
        self.Append( ID_PythonShell, "&Python Shell" )
        self.Append( ID_LuaShell, "&Lua Shell" )
        self.AppendSeparator()
        self.AppendCheckItem( ID_TemporaryMode, "&Working in temporary mode" )
        self.AppendSeparator()
        self.Append( ID_Quit, "&Quit Conjurer\tCtrl+Q" )
        self.Append( ID_QuitConjurer, "Q&uit Application" )

        self.Enable( ID_TemporaryMode, False )

        # bindings
        self.bind_function(ID_New, self.on_new) # Needed for toolbar's new icon
        self.bind_function(ID_Open, self.on_open)
        self.bind_function(ID_Save, self.on_save)
        self.bind_function(ID_SaveAs, self.on_save_as)
        self.bind_function(ID_SaveAll, self.on_save_all)
        self.bind_function(ID_Close, self.on_close)
        self.bind_function(ID_PythonShell, self.on_pythonshell)
        self.bind_function(ID_LuaShell, self.on_luashell)
        self.bind_function(ID_TemporaryMode, self.on_toggle_temporary_mode)
        self.bind_function(ID_Quit, self.on_quit)
        self.bind_function(ID_QuitConjurer, self.on_quit_conjurer)

    def on_new(self, event):
        """Open the new file menu as a popup menu"""
        frame = self.get_frame()
        frame.PopupMenu(self.menu_new, frame.get_mouse_position())

    def on_open(self, event):
        """Open a file dialog and load the selected file"""

        # IMPORTANT: Update these ids if wildcard changes
        ID_Script = 0
        wildcard = "Script files (*.lua,*.py)|*.lua;*.py"

        dir_name = guisettings.Repository.getsettingvalue(
                            guisettings.ID_BrowserPath_File 
                            )
        dlg = wx.FileDialog(
            self.get_frame(), 
            message="Choose a file", 
            defaultDir=dir_name,
            wildcard=wildcard, 
            style=wx.OPEN
            )

        if dlg.ShowModal() == wx.ID_OK:
            index = dlg.GetFilterIndex()
            if index == ID_Script:
                self.__open_script( dlg.GetPath() )
            # Record last directory
            last_dir = format.get_directory( dlg.GetPath() )
            guisettings.Repository.setsettingvalue(
                guisettings.ID_BrowserPath_File, 
                last_dir 
                )

        dlg.Destroy()

    def __open_script(self, filename):
        scripteditordlg.create_window(self.get_frame(), filename)

    def on_save(self, event):
        """Save the document of the dialog which currently has the focus,
           overwritting the old file"""
        self.__save(True)

    def on_save_as(self, event):
        """Save the document of the dialog which currently has the focus,
           asking for a new file name"""
        self.__save(False)

    def __save(self, overwrite):
        """Save the document of the dialog which currently has the focus"""
        dlg = self.get_frame().GetActiveChild()
        if dlg is not None:
            if isinstance(dlg, dialoggui.guiDialog):
                dlg.save(overwrite)

    def on_save_all(self, event):
        """Save all currently opened documents and level"""
        dlg = SaveAllDialog(self.get_frame())
        dlg.ShowModal()
        dlg.Destroy()

    def on_close(self, event):
        """Close the dialog which currently has the focus"""
        dlg = self.get_frame().GetActiveChild()
        if dlg is not None:
            if isinstance(dlg, dialoggui.guiDialog):
                dlg.Close()

    def on_pythonshell(self, event):
        """Open a Python shell"""
        pyshellwindow.create_window(self.get_frame())

    def on_luashell(self, event):
        """Open a Lua shell"""
        luashellwindow.create_window(self.get_frame())

    def on_toggle_temporary_mode(self, event):
        """Toggle working in temporary mode"""
        cjr.show_information_message(
            "When the 'wc' assign becomes and assign group let\n" \
            "carles.ros know it and he will enable this feature."
            )
        return
        temp_path = guisettings.Repository.getsettingvalue(
                            guisettings.ID_TemporaryWorkingPath 
                            )
        temp_path = servers.get_file_server().manglepath( str(temp_path) )
        fileserver = servers.get_file_server()
        if event.IsChecked():
            fileserver.setassigngroup2(
                'wc', 
                str(temp_path), 
                str(self.wc_path)
                )
        else:
            fileserver.setassigngroup2(
                'wc', 
                str(self.wc_path), 
                str(self.wc_path)
                )

    def on_quit(self, event):
        """Persist Conjurer and quit OutGUI, but not InGUI"""
        self.get_frame().Close()

    def on_quit_conjurer(self, event):
        """Persist Conjurer and quit both OutGUI and InGUI"""
        self.get_frame().quit_requested = True


# CurrentLevelSaver class
class CurrentLevelSaver:
    """Class that knows how to save the current level"""
    def is_save_allowed(self):
        level_manager = servers.get_level_manager()
        level_name = level_manager.getcurrentlevelobject().getname()
        return level_name != "default"

    def get_brief(self):
        level_manager = servers.get_level_manager()
        level_name = level_manager.getcurrentlevelobject().getname()
        return "(level) %s" % level_name

    def save(self):
        if levelcmds.prelevel_process(False):
            level_manager = servers.get_level_manager()
            level_manager.savelevel()
            levelcmds.postlevel_process(False)

# EntityClassesSaver class
class EntityClassesSaver:
    """Class that knows how to save the entity classes"""
    def __is_dirty(self, obj):
        if servers.get_entity_class_server().getentityclassdirty(obj):
            return True
        child = obj.gethead()
        while child is not None:
            if self.__is_dirty(child):
                return True
            child = child.getsucc()
        return False

    def is_save_allowed(self):
        if cmdmgrdlg.get_nclass_dirty_flag():
            return True
        if servers.get_entity_class_server().getdirty():
            return True
        return self.__is_dirty( app.get_main_entityobject() )

    def get_brief(self):
        return "(classes) all changes"

    def save(self):
        servers.get_entity_class_server().saveentityclasses()
        cmdmgrdlg.set_nclass_dirty_flag(False)

# EntityClassesSaver class
class GrimoireLibrarySaver:
    """Class that knows how to save the entity classes"""
    def __is_dirty(self, obj):
        return app.get_libraries().isobjectdirty()

    def is_save_allowed(self):
        return app.get_libraries().isobjectdirty()

    def get_brief(self):
        return "(grimoire library) all changes"

    def save(self):
        library = app.get_libraries()
        root_path = servers.get_file_server().manglepath("wc:libs/grimoire/")
        library = library.gethead() # first library
        # save all grimoire libraries
        while library is not None:
            if library.getname() != 'natives':
                save_path = root_path + '/' + library.getname() + '.n2'
                library.saveas(save_path)
            library = library.getsucc() # next library

# CurrentPresetSaver class
class CurrentPresetSaver:
    """Class that knows how to save the preset of the current level"""
    def is_save_allowed(self):
        return guisettings.Repository.getsettingvalue(
            guisettings.ID_SavePreset 
            )

    def get_brief(self):
        return "(preset) %s" % levelcmds.get_level_preset_name()

    def save(self):
        levelcmds.save_preset( levelcmds.get_level_preset_path() )

# FSMSaver class
class FSMSaver:
    """Class that knows how to save a FSM"""
    def __init__(self, state_machine):
        self.state_machine = state_machine

    def get_brief(self):
        return "(fsm) %s" % self.state_machine.getname()

    def save(self):
        servers.get_fsm_server().savefsm( self.state_machine )
        self.state_machine.setdirty( False )
        app.get_top_window().emit_app_event(
            events.FSMSaved( self.state_machine.getfullname() )
            )

# SoundLibSaver class
class SoundLibSaver:
    """Class that knows how to save the sound library"""
    def is_save_allowed(self):
        return servers.get_sound_library().getdirty()

    def get_brief(self):
        return "(sound library) all sounds"

    def save(self):
        servers.get_sound_library().saveas("wc:export/sounds/library.n2")
        servers.get_sound_library().setdirty(False)
        app.get_top_window().emit_app_event( events.SoundLibSaved() )

# MaterialLibSaver class
class MaterialLibSaver:
    """Class that knows how to save the material library"""
    def is_save_allowed(self):
        return servers.get_material_server().islibrarydirty()

    def get_brief(self):
        return "(materials) all shaders"

    def save(self):
        servers.get_material_server().savelibrary()

# GlobalVarSaver class
class GlobalVarSaver:
    """Class that knows how to save the global variables"""
    def is_save_allowed(self):
        return servers.get_global_vars().isdirty()

    def get_brief(self):
        return "(settings) all global variables"

    def save(self):
        servers.get_global_vars().saveglobalvariables()

# SaveAllDialog class
class SaveAllDialog(wx.Dialog):
    """Allows the user to select which files to save"""
    def __init__(self, parent, exiting=False):
        wx.Dialog.__init__(
            self, 
            parent, 
            title="Save all", 
            style=wx.DEFAULT_DIALOG_STYLE|wx.RESIZE_BORDER
            )
        self.exiting = exiting
        self.savers = []

        if exiting:
            self.label_exit = wx.StaticText(
                                    self, 
                                    -1, 
                                    "Save changes before exiting?"
                                    )
        self.label = wx.StaticText(self, -1, "Check the documents to save")
        self.checklist = wx.CheckListBox(self, -1)
        if exiting:
            self.button_yes = wx.Button(self, -1, "&Yes")
            self.button_no = wx.Button(self, -1, "&No")
        else:
            self.button_save = wx.Button(self, -1, "&Save")
        self.button_cancel = wx.Button(self, wx.ID_CANCEL, "&Cancel")

        self.__set_properties()
        self.__do_layout()
        self.__bind_events()

        if self.exiting:
            wx.Bell()

    def __set_properties(self):

        # Grimoire
        saver = GrimoireLibrarySaver()
        if saver.is_save_allowed():
            self.savers.append( saver )

        # Current level
        saver = CurrentLevelSaver()
        if saver.is_save_allowed():
            self.savers.append( saver )
        
        # Entity classes
        saver = EntityClassesSaver()
        if saver.is_save_allowed():
            self.savers.append( saver )
        
        # Current preset
        saver = CurrentPresetSaver()
        if saver.is_save_allowed():
            self.savers.append( saver )

        # FSMs
        state_machine = pynebula.lookup( fsm.get_fsms_lib() ).gethead()
        while state_machine is not None:
            if state_machine.isdirty():
                self.savers.append( FSMSaver(state_machine) )
            state_machine = state_machine.getsucc()
        
        # Sound library
        saver = SoundLibSaver()
        if saver.is_save_allowed():
            self.savers.append( saver )

        # Material library
        saver = MaterialLibSaver()
        if saver.is_save_allowed():
            self.savers.append( saver )
            
        # Global variables
        saver = GlobalVarSaver()
        if saver.is_save_allowed():
            self.savers.append( saver )
        
        # Dialogs
        for child in app.get_top_window().GetChildren():
            if isinstance(child, dialoggui.guiDialog):
                if child.is_dirty():
                    saver = child.get_saver()
                    if saver is not None:
                        self.savers.append( saver )
        
        # Fill check list
        for saver in self.savers:
            i = self.checklist.Append( saver.get_brief() )
            self.checklist.Check( i, True )
        
        # Default button
        if self.exiting:
            self.button_yes.SetDefault()
        else:
            self.button_save.SetDefault()

    def __do_layout(self):
        sizer_border = wx.BoxSizer(wx.HORIZONTAL)
        sizer_layout = wx.BoxSizer(wx.VERTICAL)
        sizer_buttons = wx.BoxSizer(wx.HORIZONTAL)
        if self.exiting:
            sizer_layout.Add(
                self.label_exit, 
                0, 
                wx.BOTTOM|wx.FIXED_MINSIZE,
                cfg.BORDER_WIDTH
                )
        sizer_layout.Add(
            self.label, 
            0, 
            wx.FIXED_MINSIZE
            )
        sizer_layout.Add(
            self.checklist, 
            1, 
            wx.TOP|wx.EXPAND, 
            cfg.BORDER_WIDTH
            )
        if self.exiting:
            sizer_buttons.Add(
                self.button_yes, 
                0, 
                wx.FIXED_MINSIZE
                )
            sizer_buttons.Add(
                self.button_no, 
                0, 
                wx.LEFT|wx.FIXED_MINSIZE,
                cfg.BORDER_WIDTH
                )
        else:
            sizer_buttons.Add(
                self.button_save, 
                0, 
                wx.FIXED_MINSIZE
                )
        sizer_buttons.Add(
            self.button_cancel,
            0, 
            wx.LEFT|wx.FIXED_MINSIZE,
            cfg.BORDER_WIDTH
            )
        sizer_layout.Add(
            sizer_buttons, 
            0, 
            wx.TOP|wx.ALIGN_RIGHT,
            cfg.BORDER_WIDTH
            )
        sizer_border.Add(
            sizer_layout, 
            1, 
            wx.ALL|wx.EXPAND, 
            cfg.BORDER_WIDTH
            )
        self.SetAutoLayout(True)
        self.SetSizerAndFit(sizer_border)
        self.Layout()

    def __bind_events(self):
        if self.exiting:
            self.Bind(wx.EVT_BUTTON, self.on_yes, self.button_yes)
            self.Bind(wx.EVT_BUTTON, self.on_no, self.button_no)
        else:
            self.Bind(wx.EVT_BUTTON, self.on_save, self.button_save)
        self.Bind(wx.EVT_BUTTON, self.on_cancel, self.button_cancel)
    
    def __save_docs(self):
        dlg = waitdlg.WaitDialog( app.get_top_window() )
        for i in range( len(self.savers) ):
            if self.checklist.IsChecked(i):
                dlg.set_message(
                    "Saving %s ..." % self.savers[i].get_brief()
                    )
                self.savers[i].save()
        dlg.Destroy()
    
    def on_yes(self, event):
        self.__save_docs()
        self.EndModal( wx.ID_YES )
    
    def on_no(self, event):
        self.EndModal( wx.ID_NO )
    
    def on_save(self, event):
        self.__save_docs()
        self.EndModal( wx.ID_OK )
    
    def on_cancel(self, event):
        self.EndModal( wx.ID_CANCEL )
    
    def ShowModal(self):
        # End now if there's nothing to save
        if len(self.savers) == 0:
            self.Hide()
            if self.exiting:
                return wx.ID_YES
            else:
                return wx.ID_OK
        else:
            return wx.Dialog.ShowModal(self)
