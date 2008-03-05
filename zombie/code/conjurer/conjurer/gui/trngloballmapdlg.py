##\file trngloballightmap.py
##\brief Dialog to add a new global light map

import wx
import waitdlg
import trn
import servers

import conjurerframework as cjr
import conjurerconfig as cfg

# AddGlobalLightMapDialog class
class AddGlobalLightMapDialog(wx.Dialog):
    """Dialog to create a new global light map"""
    
    def __init__(self, parent):
        wx.Dialog.__init__(self, parent, -1, "Add global light map",
            style = wx.DEFAULT_DIALOG_STYLE|wx.TAB_TRAVERSAL )
        self.target_library = None

        self.label_classname = wx.StaticText(
                                            self, 
                                            -1,
                                            "Class name for new light map", 
                                            style=wx.ALIGN_RIGHT
                                            )
        self.text_classname = wx.TextCtrl(self, -1, "")
        
        self.label_resolution_global_lightmap = wx.StaticText(
                                                                self,
                                                                -1, 
                                                                "Resolution for new light map"
                                                                )
        self.choice_resolution_global_lightmap = wx.Choice(
                                                                self,
                                                                -1, 
                                                                choices = ["16", "32", "64", "128", "256", "512", "1024", "2048"],
                                                                size = (50, -1)
                                                                )               
        self.button_ok = wx.Button(self, -1, "&OK")
        self.button_cancel = wx.Button(self, wx.ID_CANCEL, "&Cancel")

        self.__set_properties()
        self.__do_layout()
        self.__bind_events()
    
    def __set_properties(self):
        # Disable button ok since there's no choice preselected on start up
        self.__update_button_ok()
    
    def __do_layout(self):
        sizer_layout = wx.BoxSizer(wx.VERTICAL)
        sizer_buttons = wx.BoxSizer(wx.HORIZONTAL)
        grid_sizer_inputs = wx.FlexGridSizer(
                                    2, 
                                    2, 
                                    cfg.BORDER_WIDTH, 
                                    cfg.BORDER_WIDTH * 2 
                                    )
        grid_sizer_inputs.AddGrowableCol(1)
        grid_sizer_inputs.Add(
            self.label_classname,
            0, 
            wx.ALIGN_CENTER_VERTICAL|cfg.LABEL_ALIGN
            )
        grid_sizer_inputs.Add(
            self.text_classname
            )
        grid_sizer_inputs.Add(
            self.label_resolution_global_lightmap,
            0, 
            wx.ALIGN_CENTER_VERTICAL|cfg.LABEL_ALIGN
            )
        grid_sizer_inputs.Add(
            self.choice_resolution_global_lightmap,
            )
        sizer_layout.Add(
            grid_sizer_inputs, 
            0,
            wx.TOP|wx.LEFT|wx.RIGHT|wx.EXPAND, 
            cfg.BORDER_WIDTH
            )
        horizontal_line = wx.StaticLine(
                                    self,
                                    -1
                                    )
        sizer_layout.Add(
            horizontal_line, 
            0, 
            wx.EXPAND|wx.ALL, 
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
            wx.ADJUST_MINSIZE|wx.LEFT, 
            cfg.BORDER_WIDTH
            )
        sizer_layout.Add(
            sizer_buttons,
            0, 
            wx.ALIGN_RIGHT|wx.LEFT|wx.RIGHT|wx.BOTTOM,
            cfg.BORDER_WIDTH
            )
        self.SetSizerAndFit(sizer_layout)

    def __bind_events(self):
        self.Bind(
            wx.EVT_CHOICE, 
            self.on_selection, 
            self.choice_resolution_global_lightmap
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

    def __get_class_name (self):
        return str( self.text_classname.GetValue() )

    def __has_resolution (self):
        return not (self.choice_resolution_global_lightmap.GetSelection() == wx.NOT_FOUND)

    def __get_resolution_string_selection(self):
        return self.choice_resolution_global_lightmap.GetStringSelection()

    def get_global_lightmap_resolution(self):
        return int( self.__get_resolution_string_selection() )

    def __update_button_ok(self):
        self.button_ok.Enable( self.__has_resolution() )

    def on_selection(self, event):
        self.__update_button_ok()

    def on_ok(self, event):
        if not self.__has_valid_class_name():
            cjr.show_error_message(
                "Invalid class name."
                )
            return False
        
        new_name = self.__get_class_name().capitalize()
        global_lightmap_size = self.get_global_lightmap_resolution()
        
        dlg = waitdlg.WaitDialog(
                    self.GetParent(),
                    "Creating global terrain lightmap..." 
                    )
        terrain = trn.get_terrain_module()
        result = terrain.createterraingloballightmap(
                        new_name, 
                        global_lightmap_size
                        )
        # Closes dialog reporting OK
        dlg.Destroy()
        
        if result: 
            cjr.show_information_message(
                "Successfully created global terrain lightmap %s" % new_name
                )
            self.EndModal(wx.ID_OK)
        else:
            cjr.show_error_message(
                "Unable to create the global terrain lightmap"
                )
            self.EndModal(wx.ID_OK)

    def __has_valid_class_name(self):
        name = self.__get_class_name().capitalize() 
        return ( len(name) == 0 ) or servers.get_entity_class_server().checkclassname( name )

    def on_cancel(self, event):
        self.EndModal(wx.ID_CANCEL)
