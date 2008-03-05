##\file trnnewdlg.py
##\brief New terrain dialog

import wx

import app
import servers
import trn
import waitdlg
import conjurerconfig as cfg
import conjurerframework as cjr


# NewTerrainDialog class
class NewTerrainDialog(wx.Dialog):
    """Dialog to create a new terrain"""
    
    def __init__(self, parent):
        wx.Dialog.__init__(
            self, 
            parent, 
            -1, 
            "New terrain",
            style=wx.DEFAULT_DIALOG_STYLE|
                        wx.RESIZE_BORDER|
                        wx.TAB_TRAVERSAL|
                        wx.MAXIMIZE_BOX
            )
        
        self.label = wx.StaticText(self, -1, "Select a terrain class")
        self.list = wx.ListBox(self, -1, choices=[], style=wx.LB_SORT)
        self.button_ok = wx.Button(self, -1, "&OK")
        self.button_cancel = wx.Button(self, wx.ID_CANCEL, "&Cancel")
        
        self.__set_properties()
        self.__do_layout()
        self.__bind_events()

    def __set_properties(self):
        # Terrain classes list
        ec_server = servers.get_entity_class_server()
        outdoors = ec_server.getentityclass("neoutdoor")
        # make sure all subclasses are loaded
        ec_server.loadentitysubclasses(outdoors) 
        outdoor = outdoors.gethead()
        while outdoor is not None:
            self.list.Append( outdoor.getname() )
            outdoor = outdoor.getsucc()
        
        # Default button for return key press
        self.button_ok.SetDefault()

    def __do_layout(self):
        sizer_border = wx.BoxSizer(wx.VERTICAL)
        sizer_layout = wx.BoxSizer(wx.VERTICAL)
        sizer_buttons = wx.BoxSizer(wx.HORIZONTAL)
        sizer_layout.Add(
            self.label, 
            0, 
            wx.FIXED_MINSIZE
            )
        sizer_layout.Add(
            self.list,
            1, 
            wx.TOP|wx.EXPAND,
            cfg.BORDER_WIDTH
            )
        sizer_buttons.Add(
            self.button_ok, 
            1, 
            wx.FIXED_MINSIZE
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
        self.Bind(wx.EVT_LISTBOX_DCLICK, self.on_ok, self.list)
        self.Bind(wx.EVT_BUTTON, self.on_ok, self.button_ok)
        self.Bind(wx.EVT_BUTTON, self.on_cancel, self.button_cancel)

    def on_ok(self, event):
        if app.get_outdoor() is not None:
            cjr.show_error_message(
                "The level already has a terrain instance."
                )
            return
        
        if self.list.GetSelection() == wx.NOT_FOUND:
            cjr.show_error_message(
                "Please select a terrain class."
                )
        else:
            # Create a new terrain
            dlg = waitdlg.WaitDialog(
                        self.GetParent(),
                        "Creating terrain instance..." 
                        )
            created_ok = trn.get_terrain_module().createterraininstance(
                                    str( self.list.GetStringSelection() )
                                    )
            dlg.Destroy()
            if not created_ok:
                cjr.show_error_message(
                    "Unable to create the terrain instance."
                    )
            else:
                # Closes dialog reporting OK
                self.EndModal(wx.ID_OK)

    def on_cancel(self, event):
        self.EndModal(wx.ID_CANCEL)
