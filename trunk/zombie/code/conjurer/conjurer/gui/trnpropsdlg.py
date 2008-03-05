##\file trnpropsdlg.py
##\brief Terrain properties dialog

import wx

import app
import floatctrl
import servers
import trn
import waitdlg
import conjurerconfig as cfg
import conjurerframework as cjr


# PropertiesGroup class
class PropertiesGroup:
    """Group of controls to set the terrain properties"""
    
    def __init__(self, win):
        text_box_size_for_metre_values = (65, -1)
        self.staticbox_terrain_details = wx.StaticBox(
                                                    win, 
                                                    -1, 
                                                    "Terrain details"
                                                    )
        self.label_separation = wx.StaticText(
                                            win,
                                            -1, 
                                            "Point separation"
                                            )
        self.text_ctrl_separation = floatctrl.FloatCtrl(
                                                win, 
                                                -1, 
                                                size=text_box_size_for_metre_values, 
                                                style=wx.TE_RIGHT
                                                )
        self.label_separation_units = wx.StaticText(
                                                    win,
                                                    -1, 
                                                    "metres"
                                                    )
        self.label_resolution = wx.StaticText(
                                        win, 
                                        -1, 
                                        "Terrain size"
                                        )
        self.choice_resolution = wx.Choice(
                                            win, 
                                            -1, 
                                            choices = ["17", "33", "65", "129", "257", "513", "1025", "2049"],
                                            size = (-1, -1),
                                            )
        self.label_resolution_units = wx.StaticText(
                                                win, 
                                                -1, 
                                                "pixels"
                                                )
        self.label_block = wx.StaticText(
                                    win, 
                                    -1, 
                                    "Block size"
                                    )
        self.block_size_choices = ["9", "17", "33", "65", "129"]
        self.choice_block = wx.Choice(
                                        win, 
                                        -1, 
                                        choices = self.block_size_choices,
                                        size = (-1, -1),
                                        )
        self.label_block_units = wx.StaticText(
                                            win,
                                            -1, 
                                            "pixels"
                                            )
        self.label_level_size = wx.StaticText(
                                        win,
                                        -1, 
                                        "Level size"
                                        )
        self.choice_level_size = wx.Choice(
                                            win, 
                                            -1, 
                                            choices = [ str( self.get_default_terrain_size() ) ],
                                            size = (-1, -1),
                                            style = wx.TE_RIGHT
                                            )
        self.label_level_size_units = wx.StaticText(
                                                win,
                                                -1, 
                                                "metres"
                                                )
        #set up the terrain height group of controls
        self.staticbox_height = wx.StaticBox(
                                            win, 
                                            -1, 
                                            "Height"
                                            )
        self.label_min = wx.StaticText(
                                win, 
                                -1,
                                "Minimum height"
                                )
        self.text_ctrl_min = floatctrl.FloatCtrl(
                                        win, 
                                        -1, 
                                        size=text_box_size_for_metre_values, 
                                        style=wx.TE_RIGHT
                                        )
        self.label_min_units = wx.StaticText(
                                        win, 
                                        -1, 
                                        "metres"
                                        )
        self.label_max = wx.StaticText(
                                win, 
                                -1, 
                                "Maximum height"
                                )
        self.text_ctrl_max = floatctrl.FloatCtrl(
                                        win, 
                                        -1, 
                                        size=text_box_size_for_metre_values, 
                                        style=wx.TE_RIGHT
                                        )
        self.label_max_units = wx.StaticText(
                                        win, 
                                        -1,
                                        "metres"
                                        )
        self.checkbox_flatten = wx.CheckBox(
                                            win,
                                            -1, 
                                            "Flatten all at"
                                            )
        self.text_ctrl_flatten = floatctrl.FloatCtrl(
                                            win, 
                                            -1, 
                                            size=text_box_size_for_metre_values,
                                            style=wx.TE_RIGHT
                                            )
        self.label_flatten_units = wx.StaticText(
                                            win, 
                                            -1, 
                                            "metres"
                                            )
        #set up the terrain materials group of controls
        self.staticbox_materials = wx.StaticBox(
                                                        win,
                                                        -1, 
                                                        "Materials and textures"
                                                        )
        self.label_weightmap = wx.StaticText(
                                            win, 
                                            -1, 
                                            "Weightmap resolution"
                                            )
        self.choice_weightmap = wx.Choice(
                                            win, 
                                            -1, 
                                            choices = ["64", "128", "256", "512"],
                                            size = (-1, -1)
                                            )
        self.label_weightmap_units = wx.StaticText(
                                                    win, 
                                                    -1, 
                                                    "pixels"
                                                    )
        self.label_global_res = wx.StaticText(
                                        win, 
                                        -1,
                                        "Global texture resolution"
                                        )
        self.choice_global_res = wx.Choice(
                                            win, 
                                            -1, 
                                            choices = ["256", "512", "1024", "2048"],
                                            size = (-1, -1)
                                            )
        self.label_global_res_units = wx.StaticText(
                                                win, 
                                                -1,
                                                "pixels"
                                                )
        #set up the vegetation group of controls
        self.staticbox_vegetation = wx.StaticBox(
                                                        win, 
                                                        -1, 
                                                        "Vegetation"
                                                        )
        self.label_veg_cells_per_block = wx.StaticText(
                                                        win, 
                                                        -1, 
                                                        "Cells per terrain block"
                                                        )
        self.choice_veg_cells_per_block = wx.Choice(
                                                        win,    
                                                        -1, 
                                                        choices = ["1x1", "2x2", "4x4", "8x8", "16x16", "32x32"],
                                                        size = (-1, -1)
                                                        )
        self.label_vegetation_res = wx.StaticText(
                                                win, 
                                                -1, 
                                                "Editor map resolution"
                                                )
        self.choice_vegetation_res = wx.Choice(
                                                    win, 
                                                    -1, 
                                                    choices = ["2", "4", "8", "16", "32", "64", "128", "256"],
                                                    size = (-1, -1)
                                                    )
        self.label_vegetation_res_units = wx.StaticText(
                                                        win, 
                                                        -1, 
                                                        "pixels per block"
                                                        )

        self.set_properties()
        win.Bind(
            wx.EVT_TEXT, 
            self.on_change_separation, 
            self.text_ctrl_separation
            )
        win.Bind(
            wx.EVT_CHOICE,
            self.__on_choice_resolution, 
            self.choice_resolution
            )
        win.Bind(
            wx.EVT_CHOICE,
            self.on_change_block_size, 
            self.choice_block
            )
        win.Bind(
            wx.EVT_CHECKBOX,
            self.on_checkbox_flatten, 
            self.checkbox_flatten
            )

    def set_properties(self, outdoor = None):
        #set terrain detail properties
        if outdoor is None:
            self.text_ctrl_separation.set_value(2)
            self.set_default_resolution()
            self.set_current_block_size(17)
            self.update_level_size_choices()
            self.set_default_level_size()
            #set height properties
            self.text_ctrl_min.set_value(-20)
            self.text_ctrl_max.set_value(100)
            self.checkbox_flatten.SetValue(True)
            self.text_ctrl_flatten.set_value(-20)
            #set material and texture properties
            self.choice_weightmap.SetStringSelection("512")
            self.choice_global_res.SetStringSelection("1024")
            #set vegetation properties
            self.choice_veg_cells_per_block.SetSelection(4)
            self.choice_vegetation_res.SetStringSelection("128")
        else:
            self.text_ctrl_separation.set_value(
                outdoor.getpointseparation() 
                )
            #set the terrain size from the outdoor
            resolution = outdoor.getheightmapsize()
            self.choice_resolution.Append(
                str(resolution) 
                )
            self.set_current_resolution(resolution)
            #set the block size from the outdoor
            block_size = outdoor.getblocksize()
            self.choice_block.Append(
                str(block_size) 
                )
            self.set_current_block_size(block_size)
            #set the level size from the outdoor
            level_bounding_box = outdoor.getoriginalbbox()
            #we can assume the size is double the extent of the bounding box
            level_size = level_bounding_box[3] * 2
            self.choice_level_size.Append(
                str(level_size) 
                )
            self.set_current_level_size(level_size)
            #set the height values from the outdoor
            self.text_ctrl_min.set_value( outdoor.getminheight() )
            self.text_ctrl_max.set_value( outdoor.getmaxheight() )
            #set the weightmap value from the outdoor
            weightmap_size_as_string = str(
                                                    outdoor.getweightmapsize() 
                                                    )
            self.choice_weightmap.Append(
                weightmap_size_as_string
                )
            self.choice_weightmap.SetStringSelection(
                weightmap_size_as_string
                )
            #set the global resolution from the outdoor
            global_texture_size_as_string = str( 
                                                        outdoor.getglobaltexturesize() 
                                                        )
            self.choice_global_res.Append(
                global_texture_size_as_string
                )
            self.choice_global_res.SetStringSelection(
                global_texture_size_as_string
                )
            #set vegetation properties from the outdoor
            veg_cells_per_block_index = outdoor.getvegetationcellsubdivision()
            self.choice_veg_cells_per_block.SetSelection(
                veg_cells_per_block_index
                )
            veg_cell_resolution_as_string = str(
                                                        outdoor.getgrowthmapsizebycell() 
                                                        )
            self.choice_vegetation_res.Append(
                veg_cell_resolution_as_string
                )
            self.choice_vegetation_res.SetStringSelection(
                veg_cell_resolution_as_string
                )

            # Disable all property controls since are read-only
            self.choice_resolution.Disable()
            self.choice_block.Disable()
            self.choice_level_size.Disable()
            self.choice_weightmap.Disable()
            self.choice_global_res.Disable()
            self.choice_veg_cells_per_block.Disable()
            self.choice_vegetation_res.Disable()
            self.text_ctrl_separation.Disable()
            self.text_ctrl_min.Disable()
            self.text_ctrl_max.Disable()
            self.checkbox_flatten.Hide()
            self.text_ctrl_flatten.Hide()
            self.label_flatten_units.Hide()

        #we always do this bit
        self.update_label_resolution_units()
        self.update_label_block_size_units()

    def set_default_resolution(self):
        self.set_current_resolution( self.get_default_terrain_size() )

    def set_default_level_size(self):
        self.choice_level_size.SetSelection(0)

    def do_layout(self, parent_sizer):
        sizer_terrain_details = wx.StaticBoxSizer(
                                        self.staticbox_terrain_details,
                                        wx.VERTICAL
                                        )
        flex_sizer_details = wx.FlexGridSizer(
                                        4, 
                                        3, 
                                        cfg.BORDER_WIDTH, 
                                        cfg.BORDER_WIDTH
                                        )
        flex_sizer_details.AddGrowableCol(2)
        flex_sizer_details.Add(
            self.label_separation,
            0,
            cfg.LABEL_ALIGN|wx.ALIGN_CENTER_VERTICAL|wx.FIXED_MINSIZE
            )
        flex_sizer_details.Add(
            self.text_ctrl_separation, 
            0, 
            wx.FIXED_MINSIZE|wx.EXPAND
            )
        flex_sizer_details.Add(
            self.label_separation_units, 
            0, 
            wx.ALIGN_CENTER_VERTICAL|wx.FIXED_MINSIZE
            )
        flex_sizer_details.Add(
            self.label_resolution,
            0, 
            cfg.LABEL_ALIGN|wx.ALIGN_CENTER_VERTICAL|wx.FIXED_MINSIZE
            )
        flex_sizer_details.Add(
            self.choice_resolution, 
            0, 
            wx.FIXED_MINSIZE|wx.EXPAND
            )
        flex_sizer_details.Add(
            self.label_resolution_units,
            0, 
            wx.ALIGN_CENTER_VERTICAL
            )
        flex_sizer_details.Add(
            self.label_block,
            0, 
            cfg.LABEL_ALIGN|wx.ALIGN_CENTER_VERTICAL|wx.FIXED_MINSIZE
            )
        flex_sizer_details.Add(
            self.choice_block, 
            0,
            wx.FIXED_MINSIZE|wx.EXPAND
            )
        flex_sizer_details.Add(
            self.label_block_units,
            0,
            wx.ALIGN_CENTER_VERTICAL
            )
        flex_sizer_details.Add(
            self.label_level_size, 
            0, 
            cfg.LABEL_ALIGN|wx.ALIGN_CENTER_VERTICAL|wx.FIXED_MINSIZE
            )
        flex_sizer_details.Add(
            self.choice_level_size, 
            0, 
            wx.FIXED_MINSIZE|wx.EXPAND
            )
        flex_sizer_details.Add(
            self.label_level_size_units, 
            0, 
            wx.ALIGN_CENTER_VERTICAL
            )
        sizer_terrain_details.Add(
            flex_sizer_details,
            0, 
            wx.ALL|wx.EXPAND,
            cfg.BORDER_WIDTH
            )
        parent_sizer.Add(
            sizer_terrain_details, 
            0, 
            wx.ALL|wx.EXPAND, 
            cfg.BORDER_WIDTH
            )
        #height group
        sizer_height = wx.StaticBoxSizer(
                                self.staticbox_height, 
                                wx.VERTICAL
                                )
        flex_sizer_height = wx.FlexGridSizer(
                                    3, 
                                    3, 
                                    cfg.BORDER_WIDTH, 
                                    cfg.BORDER_WIDTH
                                    )
        flex_sizer_height.Add(
            self.label_min, 
            0,
            cfg.LABEL_ALIGN|wx.ALIGN_CENTER_VERTICAL|wx.FIXED_MINSIZE
            )
        flex_sizer_height.Add(
            self.text_ctrl_min,
            0, 
            wx.FIXED_MINSIZE|wx.EXPAND
            )
        flex_sizer_height.Add(
            self.label_min_units,
            0,
            wx.ALIGN_CENTER_VERTICAL|wx.FIXED_MINSIZE
            )
        flex_sizer_height.Add(
            self.label_max, 
            0, 
            cfg.LABEL_ALIGN|wx.ALIGN_CENTER_VERTICAL|wx.FIXED_MINSIZE
            )
        flex_sizer_height.Add(
            self.text_ctrl_max, 
            0, 
            wx.FIXED_MINSIZE|wx.EXPAND
            )
        flex_sizer_height.Add(
            self.label_max_units,
            0, 
            wx.ALIGN_CENTER_VERTICAL|wx.FIXED_MINSIZE
            )
        flex_sizer_height.Add(
            self.checkbox_flatten, 
            0,
            cfg.LABEL_ALIGN|wx.ALIGN_CENTER_VERTICAL|wx.FIXED_MINSIZE
            )
        flex_sizer_height.Add(
            self.text_ctrl_flatten, 
            0, 
            wx.FIXED_MINSIZE|wx.EXPAND
            )
        flex_sizer_height.Add(
            self.label_flatten_units,
            0,
            wx.ALIGN_CENTER_VERTICAL|wx.FIXED_MINSIZE
            )
        sizer_height.Add(
            flex_sizer_height, 
            0, 
            wx.ALL|wx.EXPAND, 
            cfg.BORDER_WIDTH
            )
        parent_sizer.Add(
            sizer_height, 
            0,
            wx.ALL|wx.EXPAND, 
            cfg.BORDER_WIDTH
            )
        #materials group
        sizer_materials = wx.StaticBoxSizer(
                                    self.staticbox_materials, 
                                    wx.VERTICAL
                                    )
        flex_sizer_materials = wx.FlexGridSizer(
                                                    2, 
                                                    3, 
                                                    cfg.BORDER_WIDTH, 
                                                    cfg.BORDER_WIDTH
                                                    )
        flex_sizer_materials.Add(
            self.label_weightmap, 
            0, 
            cfg.LABEL_ALIGN|wx.ALIGN_CENTER_VERTICAL|wx.FIXED_MINSIZE
            )
        flex_sizer_materials.Add(
            self.choice_weightmap,
            0, 
            wx.FIXED_MINSIZE|wx.EXPAND
            )
        flex_sizer_materials.Add(
            self.label_weightmap_units, 
            0, 
            wx.ALIGN_CENTER_VERTICAL|wx.FIXED_MINSIZE
            )
        flex_sizer_materials.Add(
            self.label_global_res,
            0, 
            cfg.LABEL_ALIGN|wx.ALIGN_CENTER_VERTICAL|wx.FIXED_MINSIZE
            )
        flex_sizer_materials.Add(
            self.choice_global_res, 
            0, 
            wx.FIXED_MINSIZE|wx.EXPAND
            )
        flex_sizer_materials.Add(
            self.label_global_res_units,
            0, 
            wx.ALIGN_CENTER_VERTICAL|wx.FIXED_MINSIZE
            )
        sizer_materials.Add(
            flex_sizer_materials, 
            0, 
            wx.ALL|wx.EXPAND, 
            cfg.BORDER_WIDTH
            )
        parent_sizer.Add(
            sizer_materials, 
            0, 
            wx.ALL|wx.EXPAND, 
            cfg.BORDER_WIDTH
            )
        #vegetation group
        sizer_vegetation = wx.StaticBoxSizer(
                                    self.staticbox_vegetation,
                                    wx.VERTICAL
                                    )
        flex_sizer_vegetation = wx.FlexGridSizer(
                                            2, 
                                            3, 
                                            cfg.BORDER_WIDTH, 
                                            cfg.BORDER_WIDTH
                                            )
        flex_sizer_vegetation.Add(
            self.label_veg_cells_per_block, 
            0, 
            cfg.LABEL_ALIGN|wx.ALIGN_CENTER_VERTICAL|wx.FIXED_MINSIZE
            )
        flex_sizer_vegetation.Add(
            self.choice_veg_cells_per_block, 
            0, 
            wx.FIXED_MINSIZE|wx.EXPAND
            )
        flex_sizer_vegetation.AddSpacer( (0, 0) )
        flex_sizer_vegetation.Add(
            self.label_vegetation_res, 
            0,
            cfg.LABEL_ALIGN|wx.ALIGN_CENTER_VERTICAL|wx.FIXED_MINSIZE
            )
        flex_sizer_vegetation.Add(
            self.choice_vegetation_res, 
            0,
            wx.FIXED_MINSIZE|wx.EXPAND
            )
        flex_sizer_vegetation.Add(
            self.label_vegetation_res_units,
            0, 
            wx.ALIGN_CENTER_VERTICAL|wx.FIXED_MINSIZE
            )
        sizer_vegetation.Add(
            flex_sizer_vegetation, 0,
            wx.ALL|wx.EXPAND, 
            cfg.BORDER_WIDTH
            )
        parent_sizer.Add(
            sizer_vegetation, 
            0, 
            wx.ALL|wx.EXPAND, 
            cfg.BORDER_WIDTH
            )

    def on_change_separation(self, event):
        self.update_label_resolution_units()
        self.update_label_block_size_units()
        self.update_level_size_choices()
        event.Skip()

    def on_change_block_size(self, event):
        self.update_label_block_size_units()
        event.Skip()
   
    def __on_choice_resolution(self, event):
        self.update_block_size_choices()
        self.update_level_size_choices()
        self.update_label_resolution_units()
        event.Skip()

    def get_current_block_size(self):
        return int( self.choice_block.GetStringSelection() ) 
        
    def set_current_block_size(self, new_size):
        self.choice_block.SetStringSelection(
            str (new_size) 
            )

    def get_possible_block_sizes(self):
        list_of_sizes = []
        for each in self.block_size_choices:
            list_of_sizes.append( int(each) )
        return list_of_sizes

    def get_default_terrain_size(self):
        return 513

    def convert_pixels_to_metres(self, number_of_pixels):
        return (number_of_pixels - 1) * self.get_point_separation()

    def get_terrain_size_in_metres(self):
        resolution = self.get_current_resolution()
        return self.convert_pixels_to_metres(resolution)

    def get_block_size_in_metres(self):
        block_size = self.get_current_block_size()
        return self.convert_pixels_to_metres(block_size)

    def get_point_separation(self):
        value = self.text_ctrl_separation.GetValue() 
        if value:
            return float(value)
        else:
            return value

    def get_flatten_at_value(self):
        return self.text_ctrl_flatten.GetValue()

    def update_label_resolution_units(self):
        #update the label to show the equivalent value in metres too
        #the format will be "pixels  (equivalent to nnn metres)"
        value_in_metres = self.get_terrain_size_in_metres()
        label_string = "pixels  (equivalent to %.2f metres)" % value_in_metres
        self.label_resolution_units.SetLabel(label_string)

    def update_label_block_size_units(self):
        #update the label to show the equivalent value in metres too
        #the format will be "pixels  (equivalent to nnn metres)"
        value_in_metres = self.get_block_size_in_metres()
        label_string = "pixels  (equivalent to %.2f metres)" % value_in_metres
        self.label_block_units.SetLabel(label_string)

    def add_possible_block_size(self, new_size):
        self.choice_block.Append(
            str(new_size)
            )

    def update_block_size_choices(self):
        # Constrain max block size to new map resolution
        current_block_size = self.get_current_block_size()
        self.choice_block.Clear()
        max_value = self.get_current_resolution()
        for size in self.get_possible_block_sizes():
            if size > max_value:
                break
            else:
                self.add_possible_block_size( size )
        if current_block_size > max_value:
            current_block_size = max_value
        self.set_current_block_size(current_block_size)

    def get_current_resolution(self):
        return int( self.choice_resolution.GetStringSelection() )

    def set_current_resolution(self, resolution):
        self.choice_resolution.SetStringSelection(
            str(resolution)
            )

    def get_current_level_size(self):
        value = self.choice_level_size.GetStringSelection()
        if value == '':
            return None
        else:
            return float(value)

    def set_current_level_size(self, level_size):
        self.choice_level_size.SetStringSelection(
            str(level_size)
            )

    def add_possible_level_size(self, new_size):
        self.choice_level_size.Append(
            str(new_size) 
            )

    def calculate_possible_level_sizes(self):
        resolution = self.get_current_resolution()
        return self.calculate_possible_level_sizes_from_resolution(resolution)

    def calculate_possible_level_sizes_from_resolution(self, resolution):
        list_of_sizes = []
        for each_number in range(0, 3):
            list_of_sizes.append(
                self.convert_pixels_to_metres(resolution) * (2 ** each_number)
                )
        return list_of_sizes

    def update_level_size_choices(self):
        #Set list of possible level size values based on current map resolution
        current_level_size = self.get_current_level_size()
        self.choice_level_size.Clear()
        for size in self.calculate_possible_level_sizes():
            self.add_possible_level_size( size )
            if current_level_size == size:
                self.set_current_level_size(current_level_size)

    def on_checkbox_flatten(self, event):
        if self.is_flatten_all_selected():
            self.enable_flatten_controls()
        else:
            self.disable_flatten_controls()
            self.text_ctrl_flatten.Clear()
        event.Skip()
    
    def is_flatten_all_selected(self):
        return self.checkbox_flatten.IsChecked()

    def is_min_height_equal_to_max_height(self):
        return self.text_ctrl_min.get_value() == self.text_ctrl_max.get_value()

    def enable_flatten_controls(self):
        self.enable_disable_flatten_controls(True)

    def disable_flatten_controls(self):
        self.enable_disable_flatten_controls(False)

    def enable_disable_flatten_controls(self, enable):
        self.text_ctrl_flatten.Enable(enable)
        self.label_flatten_units.Enable(enable)

    def persist(self):
        data = {
            'resolution': self.choice_resolution.GetSelection(),
            'point separation': self.get_point_separation(),
            'min height': self.text_ctrl_min.get_value(),
            'max height': self.text_ctrl_max.get_value()
            }
        return data

    def restore(self, data):
        self.choice_resolution.SetSelection( data['resolution'] )
        self.text_ctrl_separation.set_value( data['point separation'] )
        self.text_ctrl_min.set_value( data['min height'] )
        self.text_ctrl_max.set_value( data['max height'] )


# NewTerrainDialog class
class NewTerrainClassDialog(wx.Dialog):
    """Dialog to create a new terrain class"""
    
    def __init__(self, parent):
        wx.Dialog.__init__(self, 
                                parent, 
                                -1, 
                                "New terrain class", 
                                style=wx.DEFAULT_DIALOG_STYLE|
                                            wx.RESIZE_BORDER|
                                            wx.TAB_TRAVERSAL|
                                            wx.MAXIMIZE_BOX
                                )

        self.label_classname = wx.StaticText(self, -1, "Class name")
        self.text_classname = wx.TextCtrl(self, -1, "")
        self.props_ctrls = PropertiesGroup(self)
        self.checkbox_make_terrain = wx.CheckBox(self, -1, "Create terrain")
        self.button_ok = wx.Button(self, -1, "&OK")
        self.button_cancel = wx.Button(self, wx.ID_CANCEL, "&Cancel")
        
        self.__set_properties()
        self.__do_layout()
        self.__bind_events()

    def __set_properties(self):
        self.text_classname.SetMaxLength(
            cfg.MAX_LENGTH_CLASS_NAME
            )
        self.checkbox_make_terrain.Enable(
            app.get_outdoor() is None 
            )
        self.button_ok.SetDefault()

    def __do_layout(self):
        outer_sizer = wx.BoxSizer(wx.VERTICAL)
        sizer_classname = wx.BoxSizer(wx.HORIZONTAL)
        sizer_classname.Add(
            self.label_classname,
            0, 
            wx.ALIGN_CENTER_VERTICAL|wx.FIXED_MINSIZE
            )
        sizer_classname.Add(
            self.text_classname, 
            1,
            wx.LEFT|wx.EXPAND,
            cfg.BORDER_WIDTH
            )
        outer_sizer.Add(
            sizer_classname,
            0,
            wx.ALL|wx.EXPAND, 
            cfg.BORDER_WIDTH
            )
        self.props_ctrls.do_layout(outer_sizer)
        outer_sizer.Add(
            self.checkbox_make_terrain,
            0, 
            wx.EXPAND|wx.ALL,
            cfg.BORDER_WIDTH
            )
        # this spacer ensures buttons stay pegged to bottom of dialog
        outer_sizer.Add(
            (-1, cfg.BORDER_WIDTH *2), 
            1
            ) 
        sizer_buttons = wx.BoxSizer(wx.HORIZONTAL)
        sizer_buttons.Add(
            self.button_ok, 
            0,
            wx.RIGHT, 
            cfg.BORDER_WIDTH
            )
        sizer_buttons.Add(
            self.button_cancel
            )
        outer_sizer.Add(
            sizer_buttons, 
            0, 
            wx.ALIGN_RIGHT|wx.LEFT|wx.RIGHT| wx.BOTTOM, 
            cfg.BORDER_WIDTH
            )
        self.SetSizerAndFit(outer_sizer)

    def __bind_events(self):
        self.Bind(wx.EVT_BUTTON, self.on_ok, self.button_ok)
        self.Bind(wx.EVT_BUTTON, self.on_cancel, self.button_cancel)

    def has_level_size(self):
        return self.props_ctrls.get_current_level_size()

    def are_min_and_max_height_values_equal(self):
        return self.props_ctrls.is_min_height_equal_to_max_height()

    def on_ok(self, event):
        ec_server = servers.get_entity_class_server()
        # Verify that has been given a valid class name
        name = str( self.text_classname.GetValue().capitalize() )
        if not ec_server.checkclassname( name ):
            cjr.show_error_message(
                "Please enter a valid class name."
                )
            return

        # Verify that there isn't another class with the given name
        outdoors = ec_server.getentityclass("neoutdoor")
        outdoor = outdoors.gethead()
        while outdoor is not None:
            if outdoor.getname() == name:
                msg = "There is an existing terrain class with " \
                            "the given name.\n\nPlease enter another."
                cjr.show_error_message(msg)
                return
            outdoor = outdoor.getsucc()

        # Check that a level size has been entered
        if not self.has_level_size():
            cjr.show_error_message(
                "Please select a level size."
                )
            return

        if self.are_min_and_max_height_values_equal():
            cjr.show_error_message(
                "Minimum and maximum height values cannot be the same."
                )
            return

        # Create the terrain class
        dlg = waitdlg.WaitDialog(
                    self.GetParent(), 
                    "Creating terrain class..." 
                    )
        try:
            terrain = trn.get_terrain_module()
            created_ok = terrain.createterrainclass(
                                name,
                                self.props_ctrls.get_current_resolution(),
                                self.props_ctrls.get_current_block_size(),
                                # N.B. we pass the terrain module the selection
                                # index of the "Cells per terrain block" choice
                                # Since the cells are in the format "2n x 2n", 
                                # we are in effect passing the value of n
                                int( self.props_ctrls.choice_veg_cells_per_block.GetSelection() ),
                                int( self.props_ctrls.choice_vegetation_res.GetStringSelection() ),
                                int( self.props_ctrls.choice_weightmap.GetStringSelection() ),
                                int( self.props_ctrls.choice_global_res.GetStringSelection() ),
                                self.props_ctrls.get_point_separation()
                                )
            if not created_ok:
                cjr.show_error_message(
                    "Unable to create the terrain class."
                    )
                return
            flatten_at_value = self.props_ctrls.get_flatten_at_value()
            if flatten_at_value == '':
                flatten_at_value = '0.0'
            set_params_ok = terrain.setterrainclassparams(
                                        name,
                                        float( self.props_ctrls.text_ctrl_min.get_value() ),
                                        float( self.props_ctrls.text_ctrl_max.get_value() ),
                                        bool( self.props_ctrls.is_flatten_all_selected() ),
                                        float(flatten_at_value)
                                        )
            if not set_params_ok:
                cjr.show_error_message(
                    "Unable to create the terrain class " \
                        "due to invalid parameters."
                    )
                return
            #set the level size
            centre_to_boundary_trn = (self.props_ctrls.get_terrain_size_in_metres() ) / 2
            centre_to_boundary_level = ( self.props_ctrls.get_current_level_size() ) / 2
            new_class = ec_server.getentityclass(name)
            new_class.setoriginalbbox(
                centre_to_boundary_trn, 
                0, 
                centre_to_boundary_trn, 
                centre_to_boundary_level, 
                centre_to_boundary_level, 
                centre_to_boundary_level
                )

            # Create a terrain instance, if enabled
            if self.checkbox_make_terrain.IsChecked():
                dlg.set_message( "Creating terrain instance..." )
                created_trn_ok = terrain.createterraininstance( name )
                if not created_trn_ok:
                    cjr.show_error_message(
                        "Unable to create the terrain instance."
                        )
        finally:
            # Closes dialog
            dlg.Destroy()

        #report OK
        self.EndModal(wx.ID_OK)

    def on_cancel(self, event):
        self.EndModal(wx.ID_CANCEL)


# PropertiesDialog class
class PropertiesDialog(wx.Dialog):
    """Dialog to modify the terrain properties"""
    
    def __init__(self, parent):
        wx.Dialog.__init__(
            self, 
            parent, 
            -1, 
            "Terrain properties", 
            style=wx.DEFAULT_DIALOG_STYLE|
                    wx.RESIZE_BORDER|
                    wx.TAB_TRAVERSAL|
                    wx.MAXIMIZE_BOX
            )

        self.label_classname = wx.StaticText(
                                        self, 
                                        -1, 
                                        "Class name",
                                        style=wx.ALIGN_RIGHT
                                        )
        self.text_classname = wx.TextCtrl(self, -1, "")
        self.props_ctrls = PropertiesGroup(self)
        self.button_close = wx.Button(self, wx.ID_CANCEL, "&Close")

        self.set_properties()
        self.__do_layout()
        self.__bind_events()
    
    def set_properties(self):
        self.button_close.SetDefault()
        # Fill all property controls
        outdoor = app.get_outdoor()
        self.props_ctrls.set_properties(outdoor)
        if outdoor is not None:
            self.text_classname.SetValue( outdoor.getname() )
        # Disable all property controls since are read-only
        self.text_classname.Disable()
    
    def __do_layout(self):
        outer_sizer = wx.BoxSizer(wx.VERTICAL)
        sizer_classname = wx.BoxSizer(wx.HORIZONTAL)
        sizer_classname.Add(
            self.label_classname, 
            0, 
            wx.ALIGN_CENTER_VERTICAL|wx.FIXED_MINSIZE
            )
        sizer_classname.Add(
            self.text_classname,
            1, 
            wx.LEFT|wx.EXPAND, 
            cfg.BORDER_WIDTH
            )
        outer_sizer.Add(
            sizer_classname, 
            0, 
            wx.LEFT|wx.RIGHT|wx.TOP|wx.EXPAND, 
            cfg.BORDER_WIDTH
            )
        self.props_ctrls.do_layout(outer_sizer)
        #this spacer ensures buttons stay pegged to bottom of dialog
        outer_sizer.Add(
            (-1, cfg.BORDER_WIDTH *2), 
            1
            ) 
        outer_sizer.Add(
            self.button_close, 
            0, 
            wx.LEFT|wx.RIGHT|wx.BOTTOM|wx.ALIGN_RIGHT|wx.FIXED_MINSIZE, 
            cfg.BORDER_WIDTH
            )
        self.SetSizerAndFit(outer_sizer)

    def __bind_events(self):
        self.Bind(wx.EVT_BUTTON, self.on_close, self.button_close)

    def on_close(self, event):
        self.EndModal(wx.ID_CANCEL)

    def persist(self):
        return [
            create_window, 
            (), # no parameters for create function
            self.props_ctrls.persist()
            ]

    def restore(self, data_list):
        self.props_ctrls.restore(data_list[0])


# create_window function
def create_window(parent):
    return PropertiesDialog(parent)
