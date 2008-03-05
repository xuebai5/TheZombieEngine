##\file grassbrushes.py
##\brief Grass material brushes editor

import wx

import grimoire
import propertyview
import scriptingmodelsingleobject
import conjurerconfig as cfg
import conjurerframework as cjr

# MaterialBrushesDialog class
class MaterialBrushesDialog(wx.Dialog):
    """Dialog to manage grass material brushes"""
    
    def __init__(self, parent, material):
        wx.Dialog.__init__(
            self, 
            parent, 
            -1, 
            "Brushes for '%s' grass material" % material.getname(),
            style=wx.DEFAULT_DIALOG_STYLE|wx.RESIZE_BORDER|wx.TAB_TRAVERSAL
            )
        self.material = material

        self.list_brushes = wx.ListBox(self, -1, style=wx.LB_SORT)
        self.button_edit = wx.Button(self, -1, "&Edit...")
        self.button_new = wx.Button(self, -1, "&New...")
        self.button_delete = wx.Button(self, -1, "&Delete")

        self.__set_properties()
        self.__do_layout()
        self.__bind_events()
    
    def __set_properties(self):
        # Fill the materials list
        self.__update_brushes_list()
    
    def __do_layout(self):
        sizer_layout = wx.BoxSizer(wx.HORIZONTAL)
        sizer_brushes = wx.BoxSizer(wx.VERTICAL)
        sizer_buttons = wx.BoxSizer(wx.VERTICAL)
        sizer_brushes.Add(
            self.list_brushes, 
            1, 
            wx.EXPAND
            )
        sizer_layout.Add(
            sizer_brushes, 
            1, 
            wx.LEFT|wx.TOP|wx.BOTTOM|wx.EXPAND, 
            cfg.BORDER_WIDTH
            )
        sizer_layout.Add(
            (cfg.BORDER_WIDTH, cfg.BORDER_WIDTH), 
            0, 
            wx.ADJUST_MINSIZE
            )
        sizer_buttons.Add(self.button_edit, 0, wx.EXPAND)
        sizer_buttons.Add(
            (cfg.BORDER_WIDTH, cfg.BORDER_WIDTH), 
            1, 
            wx.EXPAND
            )
        sizer_buttons.Add(self.button_new, 0, wx.EXPAND)
        sizer_buttons.Add(
            self.button_delete, 
            0, 
            wx.TOP|wx.EXPAND, 
            cfg.BORDER_WIDTH
            )
        sizer_layout.Add(
            sizer_buttons, 
            0, 
            wx.RIGHT|wx.TOP|wx.BOTTOM|wx.EXPAND, 
            cfg.BORDER_WIDTH
            )
        self.SetAutoLayout(True)
        self.SetSizerAndFit(sizer_layout)
        self.SetSize( (150, 250) )
        self.Layout()
    
    def __bind_events(self):
        self.Bind(wx.EVT_LISTBOX, self.on_select_brush, self.list_brushes)
        self.Bind(wx.EVT_LISTBOX_DCLICK, self.on_edit_brush, self.list_brushes)
        self.Bind(wx.EVT_BUTTON, self.on_new_brush, self.button_new)
        self.Bind(wx.EVT_BUTTON, self.on_delete_brush, self.button_delete)
        self.Bind(wx.EVT_BUTTON, self.on_edit_brush, self.button_edit)
    
    def __update_brushes_list(self):
        self.list_brushes.Clear()
        for i in range( self.material.getnumberofgrowthbrushes() ):
            brush = self.material.getgrowthbrush(i)
            self.__add_brush_to_list(brush.getname(), i)
        self.__update_sel_ctrls()

    def __add_brush_to_list(self, brush_name, brush_id):
        self.list_brushes.Append(brush_name, brush_id)
    
    def __update_sel_ctrls(self):
        enable = self.list_brushes.GetSelection() != wx.NOT_FOUND
        self.button_edit.Enable( enable )
        self.button_delete.Enable( enable )
    
    def on_select_brush(self, event):
        self.__update_sel_ctrls()
    
    def on_new_brush(self, event):
        # Ask for a brush class
        try:
            wx.BeginBusyCursor()
            dlg = grimoire.create_dialog( self, True )
        finally:
            wx.EndBusyCursor()
        if dlg.ShowModal() == wx.ID_OK:
            name = dlg.GetTitle()
            if name != "":
                brush_name = str(name)
                # Validate name
                if self.list_brushes.FindString(brush_name) != wx.NOT_FOUND:
                    cjr.show_error_message(
                        "There is already a grass brush for the '%s' class." \
                            % brush_name
                        )
                else:
                    # Add new brush
                    new_brush = self.material.creategrowthbrush(brush_name)
                    # We need to update the brush list 
                    # since the indexing may have changed
                    self.__update_brushes_list()
        dlg.Destroy()

    def on_delete_brush(self, event):
        index = self.list_brushes.GetSelection()
        if index == wx.NOT_FOUND:
            cjr.show_error_message(
                "No brush selected. Please choose a brush from the list."
                )
        else:
            # Ask for confirmation
            name = self.list_brushes.GetString(index)
            msg = "Are you sure you want to delete the '%s' grass brush?" \
                        % name
            result = cjr.confirm_yes_no(self, msg)
            if result == wx.ID_YES:
                # Delete brush
                brush_id = self.list_brushes.GetClientData(index)
                self.material.deletegrowthbrush(brush_id)
                # We need to update the brush list 
                # since the indexing may have changed
                self.__update_brushes_list()

    def __currently_selected_brush_id(self):
        index = self.list_brushes.GetSelection()
        if index == wx.NOT_FOUND:
            return None
        else:
            return self.list_brushes.GetClientData(index)

    def on_edit_brush(self, event):
        brush_id = self.__currently_selected_brush_id()
        if brush_id == None:
            cjr.show_error_message(
                "No brush selected. Please choose a brush from the list."
                )
        else:
            brush = self.material.getgrowthbrush( brush_id )
            model = scriptingmodelsingleobject.new_scripting_model_for_object(
                            brush 
                            )
            dlg = propertyview.PropertyViewDialog(
                        self, 
                        "Brush '%s'" % brush.getname(), 
                        model 
                        )
            dlg.ShowModal()
            dlg.Destroy()
