##\file trnlightmapdlg.py
##\brief Terrain lightmap dialog

import wx

import app
import floatctrl
import servers
import trn
import waitdlg

import conjurerframework as cjr
import conjurerconfig as cfg


# NewTerrainDialog class
class NewTerrainLightMapDialog(wx.Dialog):
    """Dialog to create a new terrain lightmap class"""

    def __init__(self, parent):
        wx.Dialog.__init__(self, parent, -1, "Generate terrain lightmap")

        self.label_classname = wx.StaticText(
                                            self, 
                                            -1, 
                                            "Class", 
                                            style=wx.ALIGN_RIGHT
                                            )
        self.text_classname = wx.TextCtrl(self, -1, "")
        self.label_resolution = wx.StaticText(
                                            self, 
                                            -1, 
                                            "Lightmap resolution"
                                            )
        self.choice_resolution = wx.Choice(
                                            self, 
                                            -1, 
                                            choices = ["16", "32", "64", "128", "256", "512"],
                                            size = (50,-1)
                                            )
        self.label_resolution_shadowmap = wx.StaticText(self, -1, "Shadowmap resolution")
        self.choice_resolution_shadowmap = wx.Choice(
                                                            self, 
                                                            -1, 
                                                            choices = ["16", "32", "64", "128", "256", "512", "1024", "2048"],
                                                            size = (50,-1)
                                                            )
        self.label_resolution_globallightmap = wx.StaticText(self, -1, "Global lightmap resolution")
        self.choice_resolution_globallightmap = wx.Choice(
                                                                self,
                                                                -1, 
                                                                choices = ["16", "32", "64", "128", "256", "512", "1024", "2048"],
                                                                size = (50,-1)
                                                                )
        self.label_distance = wx.StaticText(self, -1, "Distance")
        self.text_ctrl_distance = floatctrl.FloatCtrl(
                                            self, 
                                            -1, 
                                            size=(50,-1),
                                            style=wx.TE_RIGHT
                                            )
        self.label_offset_u = wx.StaticText(self, -1, "Offset u")
        self.text_ctrl_offset_u = floatctrl.FloatCtrl(
                                            self,
                                            -1, 
                                            size=(50,-1), 
                                            style=wx.TE_RIGHT
                                            )
        self.label_offset_v = wx.StaticText(self, -1, "Offset v")
        self.text_ctrl_offset_v = floatctrl.FloatCtrl(
                                            self,
                                            -1,
                                            size=(50,-1), 
                                            style=wx.TE_RIGHT
                                            )
        self.label_aaliasing = wx.StaticText(self, -1, "A Aliasing disk size")
        self.text_ctrl_aaliasing = floatctrl.FloatCtrl(
                                            self,
                                            -1,
                                            size=(50,-1), 
                                            style=wx.TE_RIGHT
                                            )
        self.checkbox_overwrite = wx.CheckBox(self, -1, "Overwrite")
        self.button_ok = wx.Button(self, -1, "&OK")
        self.button_cancel = wx.Button(self, wx.ID_CANCEL, "&Cancel")
        
        self.__set_properties()
        self.__do_layout()
        self.__bind_events()

    def __set_properties(self):
        self.button_ok.SetDefault()
        self.choice_resolution.SetStringSelection("512")
        self.choice_resolution_shadowmap.SetStringSelection("512")
        self.choice_resolution_globallightmap.SetStringSelection("512")
        self.text_ctrl_distance.set_value(200)
        self.text_ctrl_offset_u.set_value(0)
        self.text_ctrl_offset_v.set_value(0)
        self.text_ctrl_aaliasing.set_value(1)
        self.checkbox_overwrite.SetValue(True)

    def __do_layout(self):
        outer_sizer = wx.BoxSizer(wx.VERTICAL)

        sizer_classname = wx.BoxSizer(wx.HORIZONTAL)
        sizer_classname.Add(
            self.label_classname,
            0, 
            wx.ALIGN_CENTER_VERTICAL|cfg.LABEL_ALIGN
            )
        sizer_classname.Add(
            self.text_classname, 
            1, 
            wx.LEFT|wx.EXPAND, 
            cfg.BORDER_WIDTH
            )
        outer_sizer.Add(
            sizer_classname, 
            1, 
            wx.ALL|wx.EXPAND, 
            cfg.BORDER_WIDTH
            )

        grid_sizer_resolution = wx.FlexGridSizer(
                                        3, 
                                        2, 
                                        cfg.BORDER_WIDTH, 
                                        cfg.BORDER_WIDTH * 2
                                        )
        grid_sizer_resolution.AddGrowableCol(1)
        grid_sizer_resolution.Add(
            self.label_resolution, 
            0, 
            cfg.LABEL_ALIGN|wx.ALIGN_CENTER_VERTICAL
            )
        grid_sizer_resolution.Add(
            self.choice_resolution
            )
        grid_sizer_resolution.Add(
            self.label_resolution_shadowmap, 
            0, 
            cfg.LABEL_ALIGN|wx.ALIGN_CENTER_VERTICAL
            )
        grid_sizer_resolution.Add(
            self.choice_resolution_shadowmap
            )
        grid_sizer_resolution.Add(
            self.label_resolution_globallightmap, 
            0, 
            cfg.LABEL_ALIGN|wx.ALIGN_CENTER_VERTICAL
            )
        grid_sizer_resolution.Add(
            self.choice_resolution_globallightmap
            )
        outer_sizer.Add(
            grid_sizer_resolution, 
            0,
            wx.ALL|wx.EXPAND,
            cfg.BORDER_WIDTH
            )

        grid_sizer_2 = wx.FlexGridSizer(
                                3, 
                                2, 
                                cfg.BORDER_WIDTH, 
                                cfg.BORDER_WIDTH * 2
                                )
        grid_sizer_2.AddGrowableCol(1)
        grid_sizer_2.Add(
            self.label_distance, 
            0, 
            cfg.LABEL_ALIGN|wx.ALIGN_CENTER_VERTICAL
            )
        grid_sizer_2.Add(
            self.text_ctrl_distance
            )
        grid_sizer_2.Add(
            self.label_offset_u, 
            0, 
            cfg.LABEL_ALIGN|wx.ALIGN_CENTER_VERTICAL
            )
        grid_sizer_2.Add(
            self.text_ctrl_offset_u
            )
        grid_sizer_2.Add(
            self.label_offset_v, 
            0, 
            cfg.LABEL_ALIGN|wx.ALIGN_CENTER_VERTICAL
            )
        grid_sizer_2.Add(
            self.text_ctrl_offset_v
            )
        outer_sizer.Add(
            grid_sizer_2,
            0,
            wx.ALL|wx.EXPAND,
            cfg.BORDER_WIDTH
            )

        sizer_aaliasing = wx.BoxSizer(wx.HORIZONTAL)
        sizer_aaliasing.Add(
            self.label_aaliasing, 
            0, 
            cfg.LABEL_ALIGN|wx.ALIGN_CENTER_VERTICAL
            )
        sizer_aaliasing.Add(
            self.text_ctrl_aaliasing, 
            0, 
            wx.LEFT|wx.FIXED_MINSIZE,
            cfg.BORDER_WIDTH
            )
        outer_sizer.Add(
            sizer_aaliasing, 
            1, 
            wx.ALL|wx.EXPAND, 
            cfg.BORDER_WIDTH
            )

        outer_sizer.Add(
            self.checkbox_overwrite,
            0,
            wx.ALL|wx.ALIGN_CENTER_VERTICAL,
            cfg.BORDER_WIDTH
            )

        horizontal_line = wx.StaticLine(
                                self, 
                                -1
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
            0, 
            wx.ADJUST_MINSIZE
            )
        sizer_buttons.Add(
            self.button_cancel, 
            1, 
            wx.LEFT|wx.ADJUST_MINSIZE,
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

    def __get_lightmap_selection(self):
        return self.choice_resolution.GetStringSelection()

    def get_lightmap_resolution(self):
        return int( self.__get_lightmap_selection() )

    def __get_shadowmap_selection(self):
        return self.choice_resolution_shadowmap.GetStringSelection()

    def get_shadowmap_resolution(self):
        return int( self.__get_shadowmap_selection() )

    def __get_global_lightmap_selection(self):
        return self.choice_resolution_globallightmap.GetStringSelection()

    def get_global_lightmap_resolution(self):
        return int( self.__get_global_lightmap_selection() )

    def on_ok(self, event):
        # Verify that has been given a valid class name
        name = str( self.text_classname.GetValue().capitalize() )
        if not servers.get_entity_class_server().checkclassname( name ):
            cjr.show_error_message(
                "Invalid class name."
                )
            return
        
        # get lightmap size
        lightmap_size = self.get_lightmap_resolution()

        # get lightmap size
        shadowmap_size = self.get_shadowmap_resolution()
        
        # get the global lightmap size
        global_lightmap_size = self.get_global_lightmap_resolution()

        # get light id
        lightid = app.get_level().findentity("terrain_lightmap_light")

        distance = self.text_ctrl_distance.get_value()
        offset_u = self.text_ctrl_offset_u.get_value()
        offset_v = self.text_ctrl_offset_v.get_value()
        aaliasing = self.text_ctrl_aaliasing.get_value()

        overwrite = self.checkbox_overwrite.IsChecked()

        # if light not found
        if lightid == 0:
            cjr.show_information_message(
                "Please use the 'Select lightmap light' " \
                "option to select the light"
                )
            return

        # Create the terrain class
        dlg = waitdlg.WaitDialog(
                    self.GetParent(), 
                    "Creating terrain lightmap..." 
                    )
        terrain = trn.get_terrain_module()
        terrain.createterrainlightmaps(
            name, 
            lightmap_size, 
            shadowmap_size,
            global_lightmap_size,
            lightid, 
            overwrite, 
            distance,
            offset_u, 
            offset_v, 
            aaliasing 
            )
        
        # Closes dialog reporting OK
        dlg.Destroy()
        self.EndModal(wx.ID_OK)

    def on_cancel(self, event):
        self.EndModal(wx.ID_CANCEL)

