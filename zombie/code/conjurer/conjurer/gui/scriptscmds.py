##\file scriptscmds.py
##\brief Scripts menu commands

import os
import wx

import cmdmgrdlg
import format
import menu
import nebulaguisettings as cfg
import scriptmgrdlg
import servers

import conjurerframework as cjr


# Scripts menu IDs
ID_CommandManager = wx.NewId()
ID_RefreshCommands = wx.NewId()
ID_CleanCommands = wx.NewId()
ID_PythonScript = wx.NewId()
ID_ScriptManager = wx.NewId()
ID_CustomScripts = wx.NewId()


# CustomScriptsMenu class
class CustomScriptsMenu(menu.Menu):
    """Err... The custom scripts menu"""
    
    def __init__(self, frame):
        menu.Menu.__init__(self, frame)
        self.custom_scripts = {}
    
    def create(self):
        pass
    
    def refresh(self):
        """Rebuild the menu"""
        # Remove old menu items
        for key in self.custom_scripts.keys():
            self.unbind_function(key)
            self.Delete(key)
        self.custom_scripts = {}
        
        # Add all custom scripts
        fileserver = servers.get_file_server()
        scripts = cfg.Repository.getsettingvalue(cfg.ID_ScriptList)
        for i in range(len(scripts)):
            script = scripts[i]
            menu_id = wx.NewId()
            self.custom_scripts[menu_id] = i
            menu_text = script['description']
            if script['shortcut'] != "":
                menu_text = menu_text + "\t" + script['shortcut']
            self.Append(menu_id, menu_text)
            self.bind_function(menu_id, self.on_custom_script)
    
    def on_custom_script(self, event):
        # Try needed due to wxWidgets not removing/updating shortcuts properly
        try:
            scriptmgrdlg.run_script( self.custom_scripts[event.GetId()] )
        except:
            cjr.show_error_message(
            "Please, restart Conjurer to run this script."
            )


# ScriptsMenu class
class ScriptsMenu(menu.Menu):
    """Err... The scripts menu"""
    
    def __init__(self, frame):
        menu.Menu.__init__(self, frame)
        self.menu_custom_scripts = CustomScriptsMenu(frame)
    
    def create(self):
        # menu items
        self.Append( ID_CommandManager, "Command manager" )
        self.Append( ID_RefreshCommands, "Refresh all commands" )
        self.Append( ID_CleanCommands, "Clean trash commands",
            "Delete all the commands without class" )
        self.AppendSeparator()
        self.Append( ID_PythonScript, "Run Python Script..." )
        self.AppendCheckItem( ID_ScriptManager, "Script Manager" )
        self.AppendMenu( ID_CustomScripts, "&Custom scripts",
            self.menu_custom_scripts )
        
        # bindings
        self.bind_function(ID_CommandManager, self.on_command_manager)
        self.bind_function(ID_RefreshCommands, self.on_refresh_commands)
        self.bind_function(ID_CleanCommands, self.on_clean_commands)
        togwinmgr = self.get_frame().get_togwinmgr()
        self.bind_function(ID_PythonScript, self.on_python_script)
        togwinmgr.add_window(
            scriptmgrdlg.create_window,
            self.FindItemById(ID_ScriptManager)
            )
    
    def refresh(self):
        self.menu_custom_scripts.refresh()
        
        # Reattach the custom scripts menu or otherwise shortcuts are ignored
        self.Remove(ID_CustomScripts)
        self.AppendMenu( ID_CustomScripts, "&Custom scripts",
            self.menu_custom_scripts )
    
    def on_command_manager(self, event):
        """Show a new instance of the command manager"""
        cmdmgrdlg.create_window( self.get_frame() )
    
    def on_refresh_commands(self, event):
        """Reload all the command so any change is refreshed in memory"""
        servers.get_script_server().refreshclasses()
    
    def on_clean_commands(self, event):
        """Delete the whole directories of the non existent classes"""
        # Remove the classes directories
        ks = servers.get_kernel_server()
        removed_cmds = []
        classes_path = format.mangle_path( cmdmgrdlg.ClassesPath )
        filenames = os.listdir( classes_path )
        for filename in filenames:
            if filename.startswith("."):
                # skip special directories/files
                continue
            file_path = format.append_to_path( classes_path, filename )
            if os.path.isdir( file_path ) and ks.findclass(str(filename)) == "":
                cmdmgrdlg.remove_class_dir( file_path, removed_cmds, filename )
        
        # Show which have been the removed classes
        if len( removed_cmds ) == 0:
            cjr.show_information_message(
                "No trash command has been found."
                )
        else:
            dlg = CleanReportDialog( self.get_frame(), removed_cmds )
            dlg.ShowModal()
            dlg.Destroy()
    
    def on_python_script(self, event):
        """Show a file browser and run the selected Python script"""
        
        dlg = wx.FileDialog(
            self.get_frame(), message="Choose a python file",
            wildcard="Python files (*.py)|*.py",
            style=wx.OPEN | wx.CHANGE_DIR
            )
        
        if dlg.ShowModal() == wx.ID_OK:
            servers.get_python_server().runscript( str(dlg.GetFilename()) )
        
        dlg.Destroy()


# CleanReportDialog class
class CleanReportDialog(wx.Dialog):
    """Show a report of a clean trash commands operation's result"""
    
    def __init__(self, parent, cmd_list):
        wx.Dialog.__init__(self, parent, title="Clean commands result",
            style=wx.DEFAULT_DIALOG_STYLE|wx.RESIZE_BORDER)
        
        self.cmd_list = cmd_list
        
        self.label = wx.StaticText(self, -1, "Trash commands removed:")
        self.list = wx.ListCtrl(self, style=wx.LC_REPORT)
        self.button_ok = wx.Button(self, -1, "OK")
        
        self.__set_properties()
        self.__do_layout()
        self.__bind_events()
    
    def __set_properties(self):
        # List columns
        self.ClassCol = 0
        self.CmdCol = 1
        self.list.InsertColumn( self.ClassCol, 'Class', width=100 )
        self.list.InsertColumn( self.CmdCol, 'Command', width=150 )
        
        # Fill list
        for cmd in self.cmd_list:
            self.list.Append( [cmd[0],cmd[1]] )
        
        # Default button
        self.button_ok.SetDefault()
    
    def __do_layout(self):
        sizer_border = wx.BoxSizer(wx.HORIZONTAL)
        sizer_layout = wx.BoxSizer(wx.VERTICAL)
        sizer_layout.Add(self.label, 0, wx.FIXED_MINSIZE, 0)
        sizer_layout.Add(self.list, 1, wx.TOP|wx.EXPAND, 5)
        sizer_layout.Add(self.button_ok, 0, wx.TOP|wx.ALIGN_CENTER_HORIZONTAL|wx.FIXED_MINSIZE, 10)
        sizer_border.Add(sizer_layout, 1, wx.ALL|wx.EXPAND, 10)
        self.SetAutoLayout(True)
        self.SetSizer(sizer_border)
        sizer_border.Fit(self)
        sizer_border.SetSizeHints(self)
        self.SetSize( (300,-1) )
        self.Layout()
    
    def __bind_events(self):
        self.Bind(wx.EVT_BUTTON, self.on_ok, self.button_ok)
    
    def on_ok(self, event):
        self.EndModal( wx.ID_OK )
