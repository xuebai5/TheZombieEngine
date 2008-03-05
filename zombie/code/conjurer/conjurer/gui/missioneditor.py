##\file missioneditor.py
##\brief Specific mission handler editor panel

import wx

import editorpanel
import format

import conjurerconfig as cfg
import conjurerframework as cjr

# ObjectiveDialog
class ObjectiveDialog(wx.Dialog):
    """Class to ask the parameters of an objective"""
    
    def __init__(self, parent, name="", description=""):
        wx.Dialog.__init__(
            self, 
            parent, 
            -1, 
            "Set the objective",
            style = wx.DEFAULT_DIALOG_STYLE|wx.RESIZE_BORDER|wx.TAB_TRAVERSAL
            )
        
        self.label_name = wx.StaticText(self, -1, "Name")
        self.text_name = wx.TextCtrl(self, -1, name)
        self.label_description = wx.StaticText(self, -1, "Description")
        self.text_description = wx.TextCtrl(self, -1, description)
        self.button_ok = wx.Button(self, -1, "&OK")
        self.button_cancel = wx.Button(self, wx.ID_CANCEL, "&Cancel")
        
        self.__set_properties()
        self.__do_layout()
        self.__bind_events()
    
    def __set_properties(self):
        self.button_ok.SetDefault()
        format.align_labels( [ self.label_name, self.label_description ] )
    
    def __do_layout(self):
        sizer_border = wx.BoxSizer(wx.VERTICAL)
        sizer_layout = wx.BoxSizer(wx.VERTICAL)
        sizer_name = wx.BoxSizer(wx.HORIZONTAL)
        sizer_description = wx.BoxSizer(wx.HORIZONTAL)
        sizer_buttons = wx.BoxSizer(wx.HORIZONTAL)
        sizer_name.Add(
            self.label_name, 
            0, 
            cfg.LABEL_ALIGN|wx.FIXED_MINSIZE|wx.ALIGN_CENTER_VERTICAL
            )
        sizer_name.Add(
            self.text_name, 
            0,
            wx.LEFT|wx.FIXED_MINSIZE|wx.ALIGN_CENTER_VERTICAL,
            cfg.BORDER_WIDTH
            )
        sizer_layout.Add(
            sizer_name, 
            0,
            wx.FIXED_MINSIZE
            )
        sizer_description.Add(
            self.label_description, 
            0,
            wx.FIXED_MINSIZE|wx.ALIGN_CENTER_VERTICAL
            )
        sizer_description.Add(
            self.text_description, 
            1, 
            wx.LEFT|wx.FIXED_MINSIZE|wx.ALIGN_CENTER_VERTICAL, 
            cfg.BORDER_WIDTH
            )
        sizer_layout.Add(
            sizer_description,
            0,
            wx.TOP|wx.BOTTOM|wx.EXPAND, 
            cfg.BORDER_WIDTH
            )
        sizer_layout.Add(
            (0,0),
            1,
            wx.EXPAND
            )
        horizontal_line = wx.StaticLine(self)
        sizer_layout.Add(
            horizontal_line, 
            0,
            wx.EXPAND
            )
        sizer_buttons.Add(
            self.button_ok, 
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
        self.SetSizerAndFit(sizer_border)

    def __bind_events(self):
        self.Bind(wx.EVT_BUTTON, self.on_ok, self.button_ok)
        self.Bind(wx.EVT_BUTTON, self.on_cancel, self.button_cancel)

    def on_ok(self, event):
        self.EndModal( wx.ID_OK )

    def on_cancel(self, event):
        self.EndModal( wx.ID_CANCEL )

    def get_name(self):
        return self.text_name.GetValue()

    def get_description(self):
        return self.text_description.GetValue()


# ObjectivesPanel class
class ObjectivesPanel(editorpanel.EditorPanel):
    """Specific mission handler editor panel"""
    
    # Objectives list columns indices
    ID_Name = 0
    ID_Description = 1
    
    def __init__(self, *args, **kwds):
        editorpanel.EditorPanel.__init__(self, *args, **kwds)
        self.entity = None
        
        self.label = wx.StaticText(self, -1, "Mission objectives")
        self.list = wx.ListCtrl(self, -1, style=wx.LC_REPORT|wx.LC_SINGLE_SEL)
        self.button_add = wx.Button(self, -1, "Add")
        self.button_delete = wx.Button(self, -1, "Delete")
        self.button_edit = wx.Button(self, -1, "Edit")
        
        self.__set_properties()
        self.__do_layout()
        self.__bind_events()

    def __set_properties(self):
        self.SetScrollRate(10, 10)
        
        # Objectives list columns
        self.list.InsertColumn(self.ID_Name, "Name")
        self.list.InsertColumn(self.ID_Description, "Description")

    def __do_layout(self):
        sizer_border = wx.BoxSizer(wx.VERTICAL)
        sizer_layout = wx.BoxSizer(wx.VERTICAL)
        sizer_buttons = wx.BoxSizer(wx.HORIZONTAL)
        sizer_layout.Add(
            self.label, 
            0, 
            wx.TOP|wx.FIXED_MINSIZE,
            cfg.BORDER_WIDTH
            )
        sizer_layout.Add(
            self.list, 
            1, 
            wx.TOP|wx.EXPAND, 
            cfg.BORDER_WIDTH
            )
        sizer_buttons.Add(
            self.button_add, 
            0, 
            wx.FIXED_MINSIZE
            )
        sizer_buttons.Add(
            self.button_delete,
            1,
            wx.LEFT|wx.FIXED_MINSIZE, 
            cfg.BORDER_WIDTH
            )
        sizer_buttons.Add(
            self.button_edit,
            1,
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
        self.SetSizerAndFit(sizer_border)

    def __bind_events(self):
        self.Bind(
            wx.EVT_LIST_ITEM_SELECTED, 
            self.on_select_objective, 
            self.list
            )
        self.Bind(
            wx.EVT_LIST_ITEM_ACTIVATED, 
            self.on_edit_objective, 
            self.list
            )
        self.Bind(
            wx.EVT_BUTTON, 
            self.on_add_objective,
            self.button_add
            )
        self.Bind(
            wx.EVT_BUTTON, 
            self.on_delete_objective,
            self.button_delete
            )
        self.Bind(
            wx.EVT_BUTTON, 
            self.on_edit_objective, 
            self.button_edit
            )

    def __get_selected_list_index(self):
        return self.list.GetNextItem(-1, wx.LIST_NEXT_ALL, wx.LIST_STATE_SELECTED)
    
    def __update_sel_ctrls(self):
        enable = self.__get_selected_list_index() != -1
        self.button_delete.Enable( enable )
        self.button_edit.Enable( enable )
    
    def __name_exists(self, name):
        for i in range( self.entity.getobjectivesnumber() ):
            if self.entity.getobjectivenamebyindex(i) == name:
                return True
        return False
    
    def on_select_objective(self, event):
        self.__update_sel_ctrls()
    
    def on_add_objective(self, event):
        dlg = ObjectiveDialog( self )
        if dlg.ShowModal() == wx.ID_OK:
            name = dlg.get_name()
            desc = dlg.get_description()
            if self.__name_exists(name):
                cjr.show_error_message(
                    "There's already an objective named '" \
                    + name + "'."
                    )
            else:
                self.entity.appendobjective( str(name), str(desc) )
                self.list.Append( [name, desc] )
        dlg.Destroy()
    
    def on_delete_objective(self, event):
        index = self.__get_selected_list_index()
        if index != -1:
            self.entity.deleteobjectivebyindex( index )
            self.list.DeleteItem( index )
            self.__update_sel_ctrls()
    
    def on_edit_objective(self, event):
        index = self.__get_selected_list_index()
        if index != -1:
            name = self.list.GetItem(index, self.ID_Name).GetText()
            desc = self.list.GetItem(index, self.ID_Description).GetText()
            dlg = ObjectiveDialog( self, name, desc )
            if dlg.ShowModal() == wx.ID_OK:
                new_name = dlg.get_name()
                new_desc = dlg.get_description()
                if name != new_name and self.__name_exists(new_name):
                    cjr.show_error_message(
                        "There's already an objective named '" \
                        + new_name + "'."
                        )
                else:
                    self.entity.setobjectivenamebyindex( index, str(new_name) )
                    self.entity.setobjectivedescriptionbyindex( index, str(new_desc) )
                    self.list.SetStringItem( index, self.ID_Name, new_name )
                    self.list.SetStringItem( index, self.ID_Description, new_desc )
            dlg.Destroy()

    def set_entity(self, entity):
        self.entity = entity
        
        # Fill objectives list
        self.list.DeleteAllItems()
        for i in range( self.entity.getobjectivesnumber() ):
            name = self.entity.getobjectivenamebyindex(i)
            desc = self.entity.getobjectivedescriptionbyindex(i)
            self.list.Append( [name, desc] )
        
        # Disable some controls on start up
        self.__update_sel_ctrls()
    
    def refresh(self):
        try:
            self.set_entity( self.entity )
        except:
            self.Close()


# create_all_editors function
def create_all_editors(entity, parent):
    editor = ObjectivesPanel(parent, -1)
    editor.set_entity(entity)
    return [ ('Objectives', editor) ]
