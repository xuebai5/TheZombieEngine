##\file scriptmgrdlg.py
##\brief Script manager dialog

import wx

import nebulaguisettings as guisettings
import script
import scripteditordlg
import scriptcfgdlg
import servers
import togwin

import conjurerconfig as cfg

# ScriptManagerDialog class
class ScriptManagerDialog(togwin.ChildToggableDialog):
    """Dialog to manage script files"""
    
    def __init__(self, parent):
        togwin.ChildToggableDialog.__init__(
            self, "Script manager", parent
            )
        
        # controls
        self.list_box_scripts = wx.CheckListBox(self, -1, choices=[])
        self.button_add = wx.Button(self, -1, "&Add")
        self.button_remove = wx.Button(self, -1, "Remo&ve")
        self.button_modify = wx.Button(self, -1, "&Modify")
        self.button_up = wx.Button(self, -1, "&Up")
        self.button_down = wx.Button(self, -1, "&Down")
        self.button_edit = wx.Button(self, -1, "&Open in Editor")
        self.button_run = wx.Button(self, -1, "&Run")
        
        self.__set_properties()
        self.__do_layout()
        self.__bind_events()

    def __set_properties(self):
        # load script list from repository
        scripts = guisettings.Repository.getsettingvalue(
                        guisettings.ID_ScriptList
                        )
        for each_script in scripts:
            index = self.list_box_scripts.Append(
                        each_script['description']
                        )
            self.list_box_scripts.Check(
                index, 
                each_script['show button']
                )

    def __do_layout(self):
        sizer_border = wx.BoxSizer(wx.HORIZONTAL)
        sizer_main_layout = wx.BoxSizer(wx.HORIZONTAL)
        sizer_buttons = wx.BoxSizer(wx.VERTICAL)
        sizer_main_layout.Add(
            self.list_box_scripts,
            1, 
            wx.EXPAND
            )
        sizer_buttons.Add(
            self.button_add, 
            0,
            wx.BOTTOM|wx.EXPAND,
            cfg.BORDER_WIDTH
            )
        sizer_buttons.Add(
            self.button_remove,
            0,
            wx.BOTTOM|wx.EXPAND,
            cfg.BORDER_WIDTH
            )
        sizer_buttons.Add(
            self.button_modify,
            0, 
            wx.BOTTOM|wx.EXPAND,
            cfg.BORDER_WIDTH
            )
        sizer_buttons.Add(
            self.button_up,
            0, 
            wx.BOTTOM|wx.EXPAND,
            cfg.BORDER_WIDTH
            )
        sizer_buttons.Add(
            self.button_down,
            0, 
            wx.BOTTOM|wx.EXPAND,
            cfg.BORDER_WIDTH
            )
        sizer_buttons.Add(
            self.button_edit,
            0,
            wx.BOTTOM|wx.EXPAND,
            cfg.BORDER_WIDTH
            )
        sizer_buttons.Add(
            self.button_run, 
            0,
            wx.BOTTOM|wx.EXPAND
            )
        sizer_main_layout.Add(
            sizer_buttons,
            0, 
            wx.LEFT|wx.EXPAND,
            cfg.BORDER_WIDTH
            )
        sizer_border.Add(
            sizer_main_layout, 
            1,
            wx.ALL|wx.EXPAND, 
            cfg.BORDER_WIDTH
            )
        self.SetSizerAndFit(sizer_border)

    def __bind_events(self):
        self.Bind(
            wx.EVT_BUTTON, 
            self.on_add, 
            self.button_add
            )
        self.Bind(
            wx.EVT_BUTTON, 
            self.on_remove, 
            self.button_remove
            )
        self.Bind(
            wx.EVT_BUTTON, 
            self.on_modify, 
            self.button_modify
            )
        self.Bind(
            wx.EVT_BUTTON, 
            self.on_up, 
            self.button_up
            )
        self.Bind(
            wx.EVT_BUTTON, 
            self.on_down, 
            self.button_down
            )
        self.Bind(
            wx.EVT_BUTTON, 
            self.on_edit, 
            self.button_edit
            )
        self.Bind(
            wx.EVT_BUTTON, 
            self.on_run, 
            self.button_run
            )
        self.Bind(
            wx.EVT_LISTBOX_DCLICK, 
            self.on_modify, 
            self.list_box_scripts
            )
        self.Bind(
            wx.EVT_CHECKLISTBOX, 
            self.on_toggle_script_button, 
            self.list_box_scripts
            )

    def __refresh_toolbar(self):
        self.GetParent().get_menubar().refresh()
        self.GetParent().get_toolbar().refresh()
    
    def on_add(self, event):
        # Ask for a file name
        dlg = wx.FileDialog(
                    self,
                    message="Choose a script file",
                    wildcard = "Lua script files (*.lua)|*.lua|" \
                                    "Python script files (*.py)|*.py|" \
                                    "All script files|*.lua;*.py",
                    style=wx.OPEN|wx.CHANGE_DIR
                    )
        dlg.SetFilterIndex(2)
        
        if ( dlg.ShowModal() == wx.ID_OK ):
            # Add the script to the list box
            script_to_add = {
                'description': dlg.GetFilename(),
                'path': dlg.GetPath(),
                'custom data': None,
                'show button': False,
                'shortcut': "",
                'button type': 'image',
                'button image': "outgui:images/tools/default.bmp",
                'button text': dlg.GetFilename(),
                'tooltip text': dlg.GetFilename(),
                'statusbar text': ""
                }
            self.list_box_scripts.Append( script_to_add['description'] )
            
            # Update the repository
            scripts = guisettings.Repository.getsettingvalue(
                            guisettings.ID_ScriptList
                            )
            scripts.append(script_to_add)
            guisettings.Repository.setsettingvalue(
                guisettings.ID_ScriptList, 
                scripts
                )
            self.__refresh_toolbar()
        
        dlg.Destroy()

    def on_remove(self, event):
        sel = self.list_box_scripts.GetSelection()
        if sel != wx.NOT_FOUND:
            # Remove the script from the list box
            self.list_box_scripts.Delete(sel)
            
            # Update the repository
            scripts = guisettings.Repository.getsettingvalue(
                            guisettings.ID_ScriptList
                            )
            del scripts[sel]
            guisettings.Repository.setsettingvalue(
                guisettings.ID_ScriptList, 
                scripts
                )
            
            self.__refresh_toolbar()

    def on_modify(self, event):
        sel = self.list_box_scripts.GetSelection()
        if sel != wx.NOT_FOUND:
            scripts = guisettings.Repository.getsettingvalue(
                            guisettings.ID_ScriptList
                            )
            dlg = scriptcfgdlg.ScriptSettingsDialog(self, scripts[sel])
            if dlg.ShowModal() == wx.ID_OK:
                scripts[sel] = dlg.get_script()
                guisettings.Repository.setsettingvalue(
                    guisettings.ID_ScriptList, 
                    scripts
                    )
                self.list_box_scripts.SetString(
                    sel,
                    scripts[sel]['description']
                    )
                self.list_box_scripts.Check(
                    sel,
                    scripts[sel]['show button']
                    )
            dlg.Destroy()
            self.__refresh_toolbar()

    def on_up(self, event):
        sel = self.list_box_scripts.GetSelection()
        if sel != wx.NOT_FOUND and sel > 0:
            # Move up selected script
            script_to_move = self.list_box_scripts.GetString(sel)
            self.list_box_scripts.Delete(sel)
            self.list_box_scripts.Insert( script_to_move, sel - 1 )
            self.list_box_scripts.SetSelection(sel - 1)
            
            # Update the repository
            scripts = guisettings.Repository.getsettingvalue(
                            guisettings.ID_ScriptList
                            )
            script_moved = scripts[sel]
            del scripts[sel]
            scripts.insert(sel-1, script_moved)
            self.list_box_scripts.Check(sel-1, script_moved['show button'])
            scripts = guisettings.Repository.setsettingvalue(
                            guisettings.ID_ScriptList, 
                            scripts
                            )
            self.__refresh_toolbar()

    def on_down(self, event):
        sel = self.list_box_scripts.GetSelection()
        if sel != wx.NOT_FOUND and sel < self.list_box_scripts.GetCount()-1:
            # Move up script below selection
            script_to_move = self.list_box_scripts.GetString(sel+1)
            self.list_box_scripts.Delete(sel+1)
            self.list_box_scripts.Insert( script_to_move, sel )
            
            # Update the repository
            scripts = guisettings.Repository.getsettingvalue(
                            guisettings.ID_ScriptList
                            )
            script_moved = scripts[sel+1]
            del scripts[sel+1]
            scripts.insert(sel, script_moved)
            self.list_box_scripts.Check(sel, script_moved['show button'])
            scripts = guisettings.Repository.setsettingvalue(
                            guisettings.ID_ScriptList, 
                            scripts
                            )
            self.__refresh_toolbar()

    def on_edit(self, event):
        sel = self.list_box_scripts.GetSelection()
        if sel != wx.NOT_FOUND:
            script_to_edit = guisettings.Repository.getsettingvalue(
                            guisettings.ID_ScriptList
                            )[sel]
            win = scripteditordlg.create_window(
                        self.GetParent(), 
                        script_to_edit['path']
                        )
            win.display()

    def on_run(self, event):
        sel = self.list_box_scripts.GetSelection()
        if sel != wx.NOT_FOUND:
            run_script(sel)

    def on_toggle_script_button(self, event):
        scripts = guisettings.Repository.getsettingvalue(
                        guisettings.ID_ScriptList
                        )
        scripts[event.GetSelection()]['show button'] = \
            self.list_box_scripts.IsChecked( event.GetSelection() )
        guisettings.Repository.setsettingvalue(
            guisettings.ID_ScriptList, 
            scripts
            )
        self.__refresh_toolbar()

    def persist(self):
        return [
            create_window,
            ()  #no parameters for create function
            ]


# run_script function
def run_script(index):
    scripts = guisettings.Repository.getsettingvalue(
                    guisettings.ID_ScriptList
                    )
    script_data = scripts[index]
    if index in range(len(scripts)):
        path = script_data['path']
        if path.endswith('.lua'):
            servers.get_lua_server().runscript(str(path))
        elif path.endswith('.py'):
            # Run the script, setting first its custom data
            script.custom_data = script_data['custom data']
            servers.get_python_server().runscript(str(path))
            
            # Store the new custom data
            script_data['custom data'] = script.custom_data
            scripts[index] = script_data
            guisettings.Repository.setsettingvalue(
                guisettings.ID_ScriptList, 
                scripts
                )


# create_window function
def create_window(parent):
    return ScriptManagerDialog(parent)
