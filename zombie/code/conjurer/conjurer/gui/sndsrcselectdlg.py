##\file sndsrcselectdlg.py
##\brief Dialog to select a sound source from a list

import wx

import servers
import conjurerconfig as cfg
import conjurerframework as cjr


# Select sound source class
class SoundSourceSelectionDialog(wx.Dialog):
    """Dialog to select a sound source class"""
    def __init__(self, parent):
        wx.Dialog.__init__(
            self, 
            parent, 
            -1, 
            "Sound Source",
            style=cfg.DEFAULT_DIALOG_STYLE
            )
        self.sound_source_class = None
        self.label = wx.StaticText(self, -1, "Select a sound source")
        self.list = wx.ListBox(
                        self, 
                        -1, 
                        choices=[], 
                        style=wx.LB_SORT
                        )
        self.button_ok = wx.Button(self, -1, "&OK")
        self.button_cancel = wx.Button(self, wx.ID_CANCEL, "&Cancel")
        self.__set_properties()
        self.__do_layout()
        self.__bind_events()
    
    def __set_properties(self):
        # Sound source class list
        ec_server = servers.get_entity_class_server()
        sound_source = ec_server.getentityclass("nesoundsource")
        #we need to make sure all subclasses are loaded
        ec_server.loadentitysubclasses(sound_source) 
        next_sound_source = sound_source.gethead()
        while next_sound_source is not None:
            self.list.Append( next_sound_source.getname() )
            next_sound_source = next_sound_source.getsucc()
        self.__update_ok_button()
        # Default button for return key press
        self.button_ok.SetDefault()
    
    def __do_layout(self):
        sizer_layout = wx.BoxSizer(wx.VERTICAL)
        sizer_buttons = wx.BoxSizer(wx.HORIZONTAL)
        sizer_layout.Add(
            self.label, 
            0, 
            wx.FIXED_MINSIZE | wx.ALL, 
            cfg.BORDER_WIDTH
            )
        sizer_layout.Add(
            self.list, 
            1, 
            wx.LEFT | wx.RIGHT |wx.EXPAND, 
            cfg.BORDER_WIDTH
            )
        horizontal_line = wx.StaticLine(
            self, 
            -1, 
            ( -1, -1 ), 
            ( -1, -1 ), 
            wx.LI_HORIZONTAL
            )
        sizer_layout.Add(
            horizontal_line, 
            0, 
            wx.EXPAND | wx.LEFT | wx.RIGHT | wx.TOP, 
            cfg.BORDER_WIDTH
            )
        sizer_buttons.Add(
            self.button_ok, 
            1, 
            wx.FIXED_MINSIZE, 
            0
            )
        sizer_buttons.Add(
            self.button_cancel, 
            1, 
            wx.LEFT|wx.FIXED_MINSIZE, 
            cfg.BORDER_WIDTH
            )
        sizer_layout.Add(
            sizer_buttons, 
            0, 
            wx.ALL|wx.ALIGN_RIGHT, 
            cfg.BORDER_WIDTH
            )
        self.SetAutoLayout(True)
        self.SetSizer(sizer_layout)
        sizer_layout.Fit(self)
        sizer_layout.SetSizeHints(self)
        self.Layout()
    
    def __bind_events(self):
        self.Bind(wx.EVT_LISTBOX, self.__on_select_class, self.list)
        self.Bind(wx.EVT_LISTBOX_DCLICK, self.__on_ok, self.list)
        self.Bind(wx.EVT_BUTTON, self.__on_ok, self.button_ok)
        self.Bind(wx.EVT_BUTTON, self.__on_cancel, self.button_cancel)
    
    def __has_class_selected(self):
        return self.sound_source_class != None

    def __update_ok_button(self):
        self.button_ok.Enable(self.__has_class_selected() )
        
    def __on_select_class(self, event):
        if event.GetSelection == wx.NOT_FOUND:
            self.sound_source_class = None
        else:
            self.sound_source_class = self.list.GetStringSelection()
        self.__update_ok_button()
        event.Skip()
    
    def __on_ok(self, event):
        if self.sound_source_class == None:
            cjr.show_error_message(
                "Please select a sound source class from the list.")
        else:
            # Close dialog reporting OK
            self.EndModal(wx.ID_OK)
    
    def __on_cancel(self, event):
        self.EndModal(wx.ID_CANCEL)

