##\file newcmddlg.py
##\brief Dialog to create a new scripted command

import wx

import conjurerconfig as cfg

# NewCommandDialog class
class NewCommandDialog(wx.Dialog):
    """Dialog to create ask for a new command prototype"""
    
    def __init__(self, parent):
        wx.Dialog.__init__(
            self,
            parent, 
            -1, 
            "New command",
            style = wx.DEFAULT_DIALOG_STYLE|wx.RESIZE_BORDER|wx.TAB_TRAVERSAL
            )
        
        self.type_protos = {
            'bool': 'b',
            'float': 'f',
            'int': 'i',
            'object': 'o',
            'string': 's'
            }
        
        self.sizer_output_staticbox = wx.StaticBox(
                                                    self,
                                                    -1, 
                                                    "Output parameters"
                                                    )
        self.sizer_input_staticbox = wx.StaticBox(
                                                self,
                                                -1, 
                                                "Input parameters"
                                                )
        self.label_name = wx.StaticText(self, -1, "Command name ")
        self.text_name = wx.TextCtrl(self, -1, "")
        self.list_input = wx.ListBox(self, -1)
        self.label_input_type = wx.StaticText(self, -1, "Parameter type")
        types = self.type_protos.keys()
        types.sort()
        self.choice_input_type = wx.Choice(self, -1, choices=types)
        self.button_add_input = wx.Button(self, -1, "Add")
        self.button_remove_input = wx.Button(self, -1, "Remove")
        self.list_output = wx.ListBox(self, -1)
        self.label_output_type = wx.StaticText(self, -1, "Parameter type")
        self.choice_output_type = wx.Choice(self, -1, choices=types)
        self.button_add_output = wx.Button(self, -1, "Add")
        self.button_remove_output = wx.Button(self, -1, "Remove")
        self.button_ok = wx.Button(self, -1, "Create command")
        self.button_cancel = wx.Button(self, wx.ID_CANCEL, "&Cancel")
        
        self.__set_properties()
        self.__do_layout()
        self.__bind_events()
    
    def __set_properties(self):
        self.choice_input_type.SetSelection(0)
        self.choice_output_type.SetSelection(0)
        self.button_ok.SetDefault()
        self._update_buttons()
    
    def __do_layout(self):
        sizer_border = wx.BoxSizer(wx.VERTICAL)
        sizer_layout = wx.BoxSizer(wx.VERTICAL)
        sizer_buttons = wx.BoxSizer(wx.HORIZONTAL)
        sizer_output = wx.StaticBoxSizer(
                                self.sizer_output_staticbox, 
                                wx.VERTICAL
                                )
        sizer_output_buttons = wx.BoxSizer(
                                            wx.HORIZONTAL
                                            )
        sizer_output_type = wx.BoxSizer(
                                        wx.HORIZONTAL
                                        )
        sizer_input = wx.StaticBoxSizer(
                                self.sizer_input_staticbox, 
                                wx.VERTICAL
                                )
        sizer_input_buttons = wx.BoxSizer(wx.HORIZONTAL)
        sizer_input_type = wx.BoxSizer(wx.HORIZONTAL)
        sizer_name = wx.BoxSizer(wx.HORIZONTAL)
        sizer_name.Add(
            self.label_name, 
            0, 
            cfg.LABEL_ALIGN|wx.ALIGN_CENTER_VERTICAL|wx.ADJUST_MINSIZE,
            )
        sizer_name.Add(
            self.text_name, 
            1,
            wx.LEFT|wx.ALIGN_CENTER_VERTICAL|wx.ADJUST_MINSIZE,
            cfg.BORDER_WIDTH
            )
        sizer_layout.Add(
            sizer_name, 
            0, 
            wx.EXPAND
            )
        sizer_input.Add(
            self.list_input, 
            1, 
            wx.ALL|wx.EXPAND|wx.ADJUST_MINSIZE, 
            cfg.BORDER_WIDTH
            )
        sizer_input_type.Add(
            self.label_input_type,
            0, 
            cfg.LABEL_ALIGN|wx.ALIGN_CENTER_VERTICAL|wx.ADJUST_MINSIZE
            )
        sizer_input_type.Add(
            self.choice_input_type,
            1, 
            wx.LEFT|wx.ADJUST_MINSIZE,
            cfg.BORDER_WIDTH
            )
        sizer_input.Add(
            sizer_input_type, 
            0, 
            wx.LEFT|wx.RIGHT|wx.BOTTOM|wx.EXPAND,
            cfg.BORDER_WIDTH
            )
        sizer_input_buttons.Add(
            self.button_add_input,
            0, 
            wx.ADJUST_MINSIZE
            )
        sizer_input_buttons.Add(
            self.button_remove_input, 
            0,
            wx.LEFT|wx.ADJUST_MINSIZE,
            cfg.BORDER_WIDTH
            )
        sizer_input.Add(
            sizer_input_buttons,
            0, 
            wx.LEFT|wx.RIGHT|wx.BOTTOM|wx.ALIGN_RIGHT,
            cfg.BORDER_WIDTH
            )
        sizer_layout.Add(
            sizer_input, 
            1, 
            wx.TOP|wx.EXPAND, 
            cfg.BORDER_WIDTH
            )
        sizer_output.Add(
            self.list_output, 
            1, 
            wx.ALL|wx.EXPAND|wx.ADJUST_MINSIZE, 
            cfg.BORDER_WIDTH
            )
        sizer_output_type.Add(
            self.label_output_type,
            0, 
            wx.ALIGN_CENTER_VERTICAL|wx.ADJUST_MINSIZE
            )
        sizer_output_type.Add(
            self.choice_output_type, 
            1, 
            wx.LEFT|wx.ALIGN_CENTER_VERTICAL|wx.ADJUST_MINSIZE,
            cfg.BORDER_WIDTH
            )
        sizer_output.Add(
            sizer_output_type,
            0, 
            wx.LEFT|wx.RIGHT|wx.BOTTOM|wx.EXPAND, 
            cfg.BORDER_WIDTH
            )
        sizer_output_buttons.Add(
            self.button_add_output,
            0,
            wx.ADJUST_MINSIZE
            )
        sizer_output_buttons.Add(
            self.button_remove_output, 
            0, 
            wx.LEFT|wx.ADJUST_MINSIZE, 
            cfg.BORDER_WIDTH
            )
        sizer_output.Add(
            sizer_output_buttons, 
            0, 
            wx.LEFT|wx.RIGHT|wx.BOTTOM|wx.ALIGN_RIGHT, 
            cfg.BORDER_WIDTH
            )
        sizer_layout.Add(
            sizer_output, 
            1,
            wx.TOP|wx.EXPAND, 
            cfg.BORDER_WIDTH
            )
        sizer_buttons.Add(
            self.button_ok, 
            0,
            wx.ADJUST_MINSIZE
            )
        sizer_buttons.Add(
            self.button_cancel,
            0, 
            wx.LEFT|wx.ADJUST_MINSIZE,
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
            wx.EVT_TEXT, 
            self.on_change_name, 
            self.text_name
            )
        self.Bind(
            wx.EVT_LISTBOX, 
            self.on_select_input, 
            self.list_input
            )
        self.Bind(
            wx.EVT_BUTTON, 
            self.on_add_input,
            self.button_add_input
            )
        self.Bind(
            wx.EVT_BUTTON, 
            self.on_remove_input,
            self.button_remove_input
            )
        self.Bind(
            wx.EVT_LISTBOX, 
            self.on_select_output,
            self.list_output
            )
        self.Bind(
            wx.EVT_BUTTON, 
            self.on_add_output, 
            self.button_add_output
            )
        self.Bind(
            wx.EVT_BUTTON, 
            self.on_remove_output,
            self.button_remove_output
            )
        self.Bind(
            wx.EVT_BUTTON, 
            self.on_ok, 
            self.button_ok
            )
        self.Bind(
            wx.EVT_BUTTON, 
            self.on_cancel, 
            self.button_cancel
            )

    def _update_buttons(self):
        self.button_ok.Enable( self.text_name.GetValue() != "" )
        self.button_remove_input.Enable(
            self.list_input.GetSelection() != wx.NOT_FOUND 
            )
        self.button_remove_output.Enable(
            self.list_output.GetSelection() != wx.NOT_FOUND
            )

    def on_change_name(self, event):
        self._update_buttons()

    def on_select_input(self, event):
        self._update_buttons()

    def on_add_input(self, event):
        self.list_input.Append( self.choice_input_type.GetStringSelection() )

    def on_remove_input(self, event):
        self.list_input.Delete( self.list_input.GetSelection() )
        self._update_buttons()

    def on_select_output(self, event):
        self._update_buttons()

    def on_add_output(self, event):
        self.list_output.Append( self.choice_output_type.GetStringSelection() )

    def on_remove_output(self, event):
        self.list_output.Delete( self.list_output.GetSelection() )
        self._update_buttons()

    def on_ok(self, event):
        self.EndModal( wx.ID_OK )

    def on_cancel(self, event):
        self.EndModal( wx.ID_CANCEL )

    def get_command_name(self):
        return self.text_name.GetValue()

    def get_command_prototype(self):
        proto = ""
        
        # Input
        if self.list_output.GetCount() == 0:
            proto = proto + 'v'
        else:
            for i in range( self.list_output.GetCount() ):
                proto = proto + self.type_protos[self.list_output.GetString(i)]
        
        # Name
        proto = proto + '_' + self.text_name.GetValue() + '_'
        
        # Output
        if self.list_input.GetCount() == 0:
            proto = proto + 'v'
        else:
            for i in range( self.list_input.GetCount() ):
                proto = proto + self.type_protos[ self.list_input.GetString(i) ]
        
        return proto


# NewInitActionCmdDlg class
class NewInitActionCmdDlg(NewCommandDialog):
    """Dialog to ask for a new 'init' action command prototype"""
    
    def __init__(self, parent):
        NewCommandDialog.__init__(self, parent)
        
        # Dialog title
        self.SetTitle("Define the init command")
        
        # Command name
        self.text_name.SetValue("init")
        self.text_name.Disable()
        
        # Input parameters
        self.list_input.Append('object') # parameter 'entity'
        
        # Output parameters
        self.list_output.Append('bool')
        self.list_output.Disable()
        self.choice_output_type.Disable()
        self.button_add_output.Disable()

    def _update_buttons(self):
        NewCommandDialog._update_buttons(self)
        self.button_remove_input.Enable( self.list_input.GetSelection() > 0 )
