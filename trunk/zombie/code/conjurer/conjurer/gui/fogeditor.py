##\file fogeditor.py
##\brief Specific fog editor panel

import wx

import choice
import colorsel
import editorpanel
import events
import format
import servers
import conjurerconfig as cfg


# Nebula fog attribute IDs
# IMPORTANT: Update fog type choice control if IDs changes
ID_NoFog = 0
ID_LinearFog = 1
ID_LayeredFog = 2
ID_ExpFog = 3


# FogPanel class
class FogPanel(editorpanel.EditorPanel):
    def __init__(self, *args, **kwds):
        editorpanel.EditorPanel.__init__(self, *args, **kwds)
        self.fog = None
        
        self.label_type = wx.StaticText(self, -1, "Fog type")
        self.choice_type = choice.Choice(
            self, -1, choices=["None", "Linear", "Layered", "Exponential"]
            )
        self.color = colorsel.ColorSelector(self, -1, "Color")
        
        self.__set_properties()
        self.__do_layout()
        self.__bind_events()

    def __set_properties(self):
        self.SetScrollRate(10, 10)

    def __do_layout(self):
        border_width = cfg.BORDER_WIDTH
        sizer = wx.BoxSizer(wx.VERTICAL)
        grid_sizer_type = wx.FlexGridSizer(1, 2, border_width, border_width * 2)
        grid_sizer_type.Add(self.label_type, 0, cfg.LABEL_ALIGN | wx.ALIGN_CENTER_VERTICAL | wx.ADJUST_MINSIZE)
        grid_sizer_type.Add(self.choice_type, 0, wx.ADJUST_MINSIZE | wx.EXPAND)
        sizer.Add(grid_sizer_type, 0, wx.ALL, border_width)
        sizer.Add(self.color, 0, wx.LEFT|wx.RIGHT|wx.BOTTOM|wx.EXPAND, border_width)
        self.SetAutoLayout(True)
        self.SetSizer(sizer)
        sizer.Fit(self)
        sizer.SetSizeHints(self)
        self.Layout()
    
    def __bind_events(self):
        self.Bind(events.EVT_CHANGING, self.on_changing_color, self.color)
        self.Bind(events.EVT_CHANGED, self.on_changed_fog_type, self.choice_type)
        self.Bind(events.EVT_CHANGED, self.on_changed_color, self.color)
    
    def __update_color(self, attribute, new_color):
        rgba = format.byte_rgb_2_unit_rgba(new_color)
        self.get_fog().setvectoroverride(attribute, rgba[0], rgba[1], rgba[2], rgba[3])
    
    def __do_int_command(self, attribute, new_value, old_value):
        # redo
        redo = self.cmd_init
        redo = redo + "noreport = fog.setintvariable('" + attribute + "', "
        redo = redo + str(new_value) + ");del fog"
        # undo
        undo = self.cmd_init
        undo = undo + "noreport = fog.setintvariable('" + attribute + "', "
        undo = undo + str(old_value) + ");del fog"
        # new command
        servers.get_command_server().newcommand(redo,undo)
    
    def __do_color_command(self, attribute, new_color, old_color):
        # redo
        rgba = format.byte_rgb_2_unit_rgba(new_color)
        redo = self.cmd_init
        redo = redo + "noreport = fog.setvectoroverride('" + attribute + "', "
        redo = redo + str(rgba[0]) + ", "
        redo = redo + str(rgba[1]) + ", "
        redo = redo + str(rgba[2]) + ", "
        redo = redo + str(rgba[3]) + ");del fog"
        # undo
        rgba = format.byte_rgb_2_unit_rgba(old_color)
        undo = self.cmd_init
        undo = undo + "noreport = fog.setvectoroverride('" + attribute + "', "
        undo = undo + str(rgba[0]) + ", "
        undo = undo + str(rgba[1]) + ", "
        undo = undo + str(rgba[2]) + ", "   
        undo = undo + str(rgba[3]) + ");del fog"
        # new command
        servers.get_command_server().newcommand(redo,undo)
    
    def on_changing_color(self, event):
        self.__update_color('fogColor', event.get_value())
    
    def on_changed_fog_type(self, event):
        self.__do_int_command('fogindex', event.get_value(), event.get_old_value())
    
    def on_changed_color(self, event):
        self.__do_color_command('fogColor', event.get_value(), event.get_old_value())
    
    def set_fog(self, fog):
        self.fog = fog
        self.cmd_init = "fog = pynebula.lookup('/sys/servers/entityobject')" \
            ".getentityobject(" + str(fog.getid()) + ");"
        self.choice_type.SetSelection( fog.getintvariable('fogindex') )
        r, g, b, a = fog.getvectoroverride('fogColor')
        self.color.set_value( format.unit_rgb_2_byte_rgb((r,g,b)) )
    
    def get_fog(self):
        return self.fog
    
    def refresh(self):
        self.set_fog(self.get_fog())


# create_all_editors function
def create_all_editors(fog, parent):
    editor = FogPanel(parent, -1)
    editor.set_fog(fog)
    return [ ('Fog', editor) ]
