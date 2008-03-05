##\file lighteditor.py
##\brief Specific light editor panel

import wx

import choice
import colorsel
import editorpanel
import events
import floatslider
import format
import servers
import conjurerconfig as cfg


# Nebula light attribute IDs
# IMPORTANT: Update light type choice control if IDs changes
ID_Point = 0
ID_Spot = 1
ID_Directional = 2
ID_Bulb = 3

# LightPanel class
class LightPanel(editorpanel.EditorPanel):
    def __init__(self, *args, **kwds):
        editorpanel.EditorPanel.__init__(self, *args, **kwds)
        self.light = None
        
        self.label_type = wx.StaticText(self, -1, "Light type")
        self.choice_type = choice.Choice(
            self, -1, choices=["Point", "Spot", "Directional", "Bulb"]
            )
        self.csel_ambient = colorsel.ColorSelector(self, -1, "Ambient")
        self.csel_diffuse = colorsel.ColorSelector(self, -1, "Diffuse")
        self.csel_specular = colorsel.ColorSelector(self, -1, "Specular")
        self.staticbox_spot = wx.StaticBox(self, -1, "Spot")
        self.staticbox_static = wx.StaticBox(self, -1, "Static light")
        self.slider_spot_burn = floatslider.FloatSlider(
            self, -1, "Spot burn", 0.5, 0.0, 1.0, precision=1000.0
            )
        self.slider_lightmapambientfactor = floatslider.FloatSlider(
            self, -1, "Lightmap ambient factor", 0.2, 0.0, 2.0, precision=1000.0
            )
        self.slider_lightmapdiffusefactor = floatslider.FloatSlider(
            self, -1, "Lightmap diffuse factor", 0.2, 0.0, 2.0, precision=1000.0
            )
        self.slider_terrainambientfactor = floatslider.FloatSlider(
            self, -1, "Terrain ambient factor", 1.0, 0.0, 2.0, precision=1000.0
            )
        self.slider_terraindiffusefactor = floatslider.FloatSlider(
            self, -1, "Terrain diffuse factor", 1.0, 0.0, 2.0, precision=1000.0
            )                           
        self.__set_properties()
        self.__do_layout()
        self.__bind_events()

    def __set_properties(self):
        self.SetScrollRate(10, 10)

    def __do_layout(self):
        border_width = cfg.BORDER_WIDTH
        sizer = wx.BoxSizer(wx.VERTICAL)
        sizer_spot_group = wx.StaticBoxSizer(self.staticbox_spot, wx.VERTICAL)
        sizer_static_group = wx.StaticBoxSizer(self.staticbox_static, wx.VERTICAL)
        grid_sizer_type = wx.FlexGridSizer(1, 2, border_width, border_width * 2)
        grid_sizer_type.Add(self.label_type, 0, cfg.LABEL_ALIGN | wx.ALIGN_CENTER_VERTICAL | wx.ADJUST_MINSIZE)
        grid_sizer_type.Add(self.choice_type, 0, wx.ADJUST_MINSIZE | wx.EXPAND)
        sizer.Add(grid_sizer_type, 0, wx.ALL, border_width)
        sizer.Add(self.csel_ambient, 0, wx.LEFT|wx.RIGHT|wx.BOTTOM|wx.EXPAND, border_width)
        sizer.Add(self.csel_diffuse, 0, wx.LEFT|wx.RIGHT|wx.BOTTOM|wx.EXPAND, border_width)
        sizer.Add(self.csel_specular, 0, wx.LEFT|wx.RIGHT|wx.BOTTOM|wx.EXPAND, border_width)
        sizer_spot_group.Add(self.slider_spot_burn, 1, wx.ALL|wx.EXPAND, border_width)
        sizer.Add(sizer_spot_group, 0, wx.LEFT|wx.RIGHT|wx.BOTTOM|wx.EXPAND, border_width)
        sizer_static_group.Add(self.slider_lightmapambientfactor, 1, wx.ALL|wx.EXPAND, border_width)
        sizer_static_group.Add(self.slider_lightmapdiffusefactor, 1, wx.ALL|wx.EXPAND, border_width)
        sizer_static_group.Add(self.slider_terrainambientfactor, 1, wx.ALL|wx.EXPAND, border_width)   
        sizer_static_group.Add(self.slider_terraindiffusefactor, 1, wx.ALL|wx.EXPAND, border_width)              
        sizer.Add(sizer_static_group, 0, wx.LEFT|wx.RIGHT|wx.BOTTOM|wx.EXPAND, border_width)
        self.SetAutoLayout(True)
        self.SetSizer(sizer)
        sizer.Fit(self)
        sizer.SetSizeHints(self)
        self.Layout()
    
    def __bind_events(self):
        self.Bind(events.EVT_CHANGING, self.on_changing_ambient, self.csel_ambient)
        self.Bind(events.EVT_CHANGING, self.on_changing_diffuse, self.csel_diffuse)
        self.Bind(events.EVT_CHANGING, self.on_changing_specular, self.csel_specular)
        self.Bind(events.EVT_CHANGING, self.on_changing_spot_burn, self.slider_spot_burn)
        self.Bind(events.EVT_CHANGING, self.on_changing_lightmapambientfactor, self.slider_lightmapambientfactor)
        self.Bind(events.EVT_CHANGING, self.on_changing_terrainambientfactor, self.slider_terrainambientfactor)
        self.Bind(events.EVT_CHANGING, self.on_changing_terraindiffusefactor, self.slider_terraindiffusefactor)
        self.Bind(events.EVT_CHANGING, self.on_changing_lightmapdiffusefactor, self.slider_lightmapdiffusefactor)
        self.Bind(events.EVT_CHANGED, self.on_changed_light_type, self.choice_type)
        self.Bind(events.EVT_CHANGED, self.on_changed_ambient, self.csel_ambient)
        self.Bind(events.EVT_CHANGED, self.on_changed_diffuse, self.csel_diffuse)
        self.Bind(events.EVT_CHANGED, self.on_changed_specular, self.csel_specular)
        self.Bind(events.EVT_CHANGED, self.on_changed_spot_burn, self.slider_spot_burn)
        self.Bind(events.EVT_CHANGED, self.on_changed_lightmapambientfactor, self.slider_lightmapambientfactor)
        self.Bind(events.EVT_CHANGED, self.on_changed_terrainambientfactor, self.slider_terrainambientfactor)
        self.Bind(events.EVT_CHANGED, self.on_changed_terraindiffusefactor, self.slider_terraindiffusefactor)
        self.Bind(events.EVT_CHANGED, self.on_changed_lightmapdiffusefactor, self.slider_lightmapdiffusefactor)

    
    def __update_specific_controls(self):
        light = self.get_light()
        if light.getintvariable('lightindex') == ID_Spot:
            self.slider_spot_burn.set_value( light.getfloatvariable('Intensity0') )
    
    def __show_specific_controls(self):
        if self.choice_type.GetSelection() == ID_Spot:
            self.staticbox_spot.Enable()
            self.slider_spot_burn.Enable()
#            self.GetSizer().Show(4, True)
        else:
            self.staticbox_spot.Disable()
            self.slider_spot_burn.Disable()
#            self.GetSizer().Show(4, False)
#        self.GetSizer().SetVirtualSizeHints(self)
#        self.Layout()
    
    def __update_color(self, attribute, new_color):
        rgba = format.byte_rgb_2_unit_rgba(new_color)
        self.get_light().setvectoroverride(attribute, rgba[0], rgba[1], rgba[2], rgba[3])
    
    def __do_int_command(self, attribute, new_value, old_value):
        # redo
        redo = self.cmd_init
        redo = redo + "noreport = light.setintvariable('" + attribute + "', "
        redo = redo + str(new_value) + ");del light"
        # undo
        undo = self.cmd_init
        undo = undo + "noreport = light.setintvariable('" + attribute + "', "
        undo = undo + str(old_value) + ");del light"
        # new command
        servers.get_command_server().newcommand(redo,undo)
    
    def __do_float_command(self, attribute, new_value, old_value):
        # redo
        redo = self.cmd_init
        redo = redo + "noreport = light.setfloatoverride('" + attribute + "', "
        redo = redo + str(new_value) + ");del light"
        # undo
        undo = self.cmd_init
        undo = undo + "noreport = light.setfloatoverride('" + attribute + "', "
        undo = undo + str(old_value) + ");del light"
        # new command
        servers.get_command_server().newcommand(redo,undo)
    
    def __do_color_command(self, attribute, new_color, old_color):
        # redo
        rgba = format.byte_rgb_2_unit_rgba(new_color)
        redo = self.cmd_init
        redo = redo + "noreport = light.setvectoroverride('" + attribute + "', "
        redo = redo + str(rgba[0]) + ", "
        redo = redo + str(rgba[1]) + ", "
        redo = redo + str(rgba[2]) + ", "
        redo = redo + str(rgba[3]) + ");del light"
        # undo
        rgba = format.byte_rgb_2_unit_rgba(old_color)
        undo = self.cmd_init
        undo = undo + "noreport = light.setvectoroverride('" + attribute + "', "
        undo = undo + str(rgba[0]) + ", "
        undo = undo + str(rgba[1]) + ", "
        undo = undo + str(rgba[2]) + ", "   
        undo = undo + str(rgba[3]) + ");del light"
        # new command
        servers.get_command_server().newcommand(redo,undo)
    
    def on_changing_ambient(self, event):
        self.__update_color('LightAmbient', event.get_value())
    
    def on_changing_diffuse(self, event):
        self.__update_color('LightDiffuse', event.get_value())
    
    def on_changing_specular(self, event):
        self.__update_color('LightSpecular', event.get_value())
    
    def on_changing_spot_burn(self, event):
        self.get_light().setfloatoverride('Intensity0', event.get_value())
        
    def on_changing_lightmapambientfactor(self, event):
        self.get_light().setfloatoverride('LightMapAmbientFactor', event.get_value())
        
    def on_changing_terrainambientfactor(self, event):
        self.get_light().setfloatoverride('TerrainAmbientFactor', event.get_value())     
    
    def on_changing_terraindiffusefactor(self, event):
        self.get_light().setfloatoverride('TerrainDiffuseFactor', event.get_value()) 
        
    def on_changing_lightmapdiffusefactor(self, event):
        self.get_light().setfloatoverride('LightMapDiffuseFactor', event.get_value())         
    
    def on_changed_light_type(self, event):
        self.__do_int_command('lightindex', event.get_value(), event.get_old_value())
        self.__update_specific_controls()
        self.__show_specific_controls()
    
    def on_changed_ambient(self, event):
        self.__do_color_command('LightAmbient', event.get_value(), event.get_old_value())
    
    def on_changed_diffuse(self, event):
        self.__do_color_command('LightDiffuse', event.get_value(), event.get_old_value())
    
    def on_changed_specular(self, event):
        self.__do_color_command('LightSpecular', event.get_value(), event.get_old_value())
    
    def on_changed_spot_burn(self, event):
        self.__do_float_command('Intensity0', event.get_value(), event.get_old_value())
        
    def on_changed_lightmapambientfactor(self, event):
        self.__do_float_command('LightMapAmbientFactor', event.get_value(), event.get_old_value())
        
    def on_changed_terrainambientfactor(self, event):
        self.__do_float_command('TerrainAmbientFactor', event.get_value(), event.get_old_value())        
        
    def on_changed_terraindiffusefactor(self, event):
        self.__do_float_command('TerrainDiffuseFactor', event.get_value(), event.get_old_value()) 
                
    def on_changed_lightmapdiffusefactor(self, event):
        self.__do_float_command('LightMapDiffuseFactor', event.get_value(), event.get_old_value())         
    
    def set_light(self, light):
        self.light = light
        self.cmd_init = "light = pynebula.lookup('/sys/servers/entityobject')" \
            ".getentityobject(" + str(light.getid()) + ");"
        self.choice_type.SetSelection( light.getintvariable('lightindex') )
        r, g, b, a = light.getvectoroverride('LightAmbient')
        self.csel_ambient.set_value( format.unit_rgb_2_byte_rgb((r,g,b)) )
        r, g, b, a = light.getvectoroverride('LightDiffuse')
        self.csel_diffuse.set_value( format.unit_rgb_2_byte_rgb((r,g,b)) )
        r, g, b, a = light.getvectoroverride('LightSpecular')
        self.csel_specular.set_value( format.unit_rgb_2_byte_rgb((r,g,b)) )
        self.__update_specific_controls()
        self.__show_specific_controls()
    
    def get_light(self):
        return self.light
    
    def refresh(self):
        self.set_light(self.get_light())


# create_all_editors function
def create_all_editors(light, parent):
    editor = LightPanel(parent, -1)
    editor.set_light(light)
    return [ ('Light', editor) ]
