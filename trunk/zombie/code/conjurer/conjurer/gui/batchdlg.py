##\file batchdlg.py
##\brief Batch dialog

import wx

import app
import floatctrl
import waitdlg

import conjurerconfig as cfg

# File dialog styles
SELECTION = 1
TERRAIN = 2

# NewBatchDialog class
class NewBatchDialog(wx.Dialog):
    """Dialog to edit options for creating or rebuilding batches"""
    def __init__(self, parent, style):
        wx.Dialog.__init__(self, parent, -1, "Configure Batch")

        self.style = style

        self.checkbox_rebuild = wx.CheckBox(self, -1, "(Re)build graphics")
        self.checkbox_testsubentities = wx.CheckBox(
                                                        self,
                                                        -1, 
                                                        "Test subentities"
                                                        )

        self.label_distance = wx.StaticText(self, -1, "Min. test distance")
        self.text_ctrl_distance = floatctrl.FloatCtrl(
                                            self, 
                                            -1, 
                                            size=(50,-1), 
                                            style=wx.TE_RIGHT
                                            )

        self.button_ok = wx.Button(self, -1, "&OK")
        self.button_cancel = wx.Button(self, wx.ID_CANCEL, "&Cancel")

        self.__set_properties()
        self.__do_layout()
        self.__bind_events()

    def __set_properties(self):
        self.button_ok.SetDefault()

        object_state = app.get_object_state()
        self.checkbox_rebuild.SetValue(
            object_state.getbatchrebuildresource()
            )
        self.checkbox_testsubentities.SetValue(
            object_state.getbatchtestsubentities()
            )
        self.text_ctrl_distance.set_value(
            object_state.getbatchmindistance()
            )

    def __do_layout(self):
        outer_sizer = wx.BoxSizer(wx.VERTICAL)
        outer_sizer.Add(
            self.checkbox_rebuild,
            0,
            wx.ALIGN_LEFT|wx.ALIGN_CENTER_VERTICAL|wx.FIXED_MINSIZE|wx.ALL, 
            cfg.BORDER_WIDTH
            )
        outer_sizer.Add(
            self.checkbox_testsubentities, 
            0,
            wx.ALIGN_LEFT|wx.ALIGN_CENTER_VERTICAL|wx.FIXED_MINSIZE|wx.ALL, 
            cfg.BORDER_WIDTH
            )

        sizer_distance = wx.BoxSizer(wx.HORIZONTAL)
        sizer_distance.Add(
            self.label_distance, 
            0, 
            cfg.LABEL_ALIGN|wx.ALIGN_CENTER_VERTICAL|wx.FIXED_MINSIZE
            )
        sizer_distance.Add(
            self.text_ctrl_distance, 
            0, 
            wx.LEFT|wx.ALIGN_CENTER_VERTICAL|wx.FIXED_MINSIZE,
            cfg.BORDER_WIDTH
            )
        outer_sizer.Add(
            sizer_distance,
            1, 
            wx.LEFT|wx.RIGHT|wx.EXPAND,
            cfg.BORDER_WIDTH
            )

        horizontal_line = wx.StaticLine(
                                self, 
                                -1, 
                                ( -1, -1 ), 
                                ( -1, -1 ), 
                                wx.LI_HORIZONTAL 
                                )
        outer_sizer.Add(
            horizontal_line, 
            0, 
            wx.EXPAND|wx.LEFT|wx.RIGHT, 
            cfg.BORDER_WIDTH
            )

        sizer_buttons = wx.BoxSizer(wx.HORIZONTAL)
        sizer_buttons.Add(
            self.button_ok, 
            0
            )
        sizer_buttons.Add(
            self.button_cancel, 
            1, 
            wx.LEFT,
            cfg.BORDER_WIDTH
            )
        outer_sizer.Add(
            sizer_buttons, 
            1, 
            wx.ALL|wx.ALIGN_RIGHT,
            cfg.BORDER_WIDTH
            )
        self.SetSizerAndFit(outer_sizer)

    def __bind_events(self):
        self.Bind(wx.EVT_BUTTON, self.on_ok, self.button_ok)
        self.Bind(wx.EVT_BUTTON, self.on_cancel, self.button_cancel)

    def on_ok(self, event):
        distance = self.text_ctrl_distance.get_value()

        rebuild = self.checkbox_rebuild.IsChecked()
        testsubentities = self.checkbox_testsubentities.IsChecked()

        object_state = app.get_object_state()
        object_state.setbatchrebuildresource(rebuild)
        object_state.setbatchtestsubentities(testsubentities)
        object_state.setbatchmindistance(distance)

        # Create the batch(es)
        if self.style == SELECTION:
            object_state.batchentities()

        elif self.style == TERRAIN:
            dlg = waitdlg.WaitDialog(
                        self.GetParent(), 
                        "Creating batch(es)..." 
                        )
            object_state.createterrainbatches()
            dlg.Destroy()

        # Close dialog reporting OK
        self.EndModal(wx.ID_OK)

    def on_cancel(self, event):
        self.EndModal(wx.ID_CANCEL)
