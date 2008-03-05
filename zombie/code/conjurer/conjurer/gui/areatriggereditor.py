##\file areatriggereditor.py
##\brief Specific area trigger editor panel

import wx
import wx.lib.ticker

import editorpanel
import floatctrl
import format
import servers
import triggerinput
import triggeroutput
import conjurerconfig as cfg


# InputPanel class
class InputPanel(editorpanel.EditorPanel):
    
    # Shape panel indices
    ID_Circle = 0
    ID_Rectangle = 1
    
    def __init__(self, *args, **kwds):
        editorpanel.EditorPanel.__init__(self, *args, **kwds)
        self.entity = None
        self.getting_values = False
        
        self.choicebook_shape = wx.Choicebook(self, -1)
        self.panel_rectangle = wx.Panel(self.choicebook_shape, -1)
        self.sizer_shape_staticbox = wx.StaticBox(self, -1, "Shape")
        self.panel_circle = wx.Panel(self.choicebook_shape, -1)
        self.label_radius = wx.StaticText(self.panel_circle, -1, "Radius",
            style=wx.TE_RIGHT)
        self.float_radius = floatctrl.FloatCtrl(
            self.panel_circle,
            limited=True, value=5.0, min=1.0, max=10000.0,
            size=wx.Size(60,-1), style=wx.TE_RIGHT
            )
        self.label_circle_height = wx.StaticText(self.panel_circle, -1, "Height",
            style=wx.TE_RIGHT)
        self.float_circle_height = floatctrl.FloatCtrl(
            self.panel_circle,
            limited=True, value=0.0, min=0.0, max=10000.0,
            size=wx.Size(60,-1), style=wx.TE_RIGHT
            )
        self.label_length = wx.StaticText(self.panel_rectangle, -1, "Length",
            style=wx.TE_RIGHT)
        self.float_length = floatctrl.FloatCtrl(
            self.panel_rectangle,
            limited=True, value=5.0, min=1.0, max=10000.0,
            size=wx.Size(60,-1), style=wx.TE_RIGHT
            )
        self.label_width = wx.StaticText(self.panel_rectangle, -1, "Width",
            style=wx.TE_RIGHT)
        self.float_width = floatctrl.FloatCtrl(
            self.panel_rectangle,
            limited=True, value=5.0, min=1.0, max=10000.0,
            size=wx.Size(60,-1), style=wx.TE_RIGHT
            )
        self.label_polygon_height = wx.StaticText(self.panel_rectangle, -1, "Height",
            style=wx.TE_RIGHT)
        self.float_polygon_height = floatctrl.FloatCtrl(
            self.panel_rectangle,
            limited=True, value=0.0, min=0.0, max=10000.0,
            size=wx.Size(60,-1), style=wx.TE_RIGHT
            )
        self.text_info = wx.lib.ticker.Ticker(self, -1,
            "Set a 0 height for an infinite height. " \
            "Polygon length and width may not display the real size, " \
            "use them only to make the polygon a rectangle.",
            bgcolor=wx.SystemSettings.GetColour(wx.SYS_COLOUR_BTNFACE),
            ppf=1, fps=50)
        self.input = triggerinput.TriggerInputCtrl(self, 'Perceivable events')
        
        self.__set_properties()
        self.__do_layout()
        self.__bind_events()
    
    def __set_properties(self):
        self.SetScrollRate(10, 10)
        
        # Circle shape labels
        format.align_labels([
            self.label_radius,
            self.label_circle_height
            ])
        
        # Polygon shape labels
        format.align_labels([
            self.label_length,
            self.label_width,
            self.label_polygon_height
            ])
    
    def __do_layout(self):
        border_width = cfg.BORDER_WIDTH
        sizer_layout = wx.BoxSizer(wx.VERTICAL)
        sizer_shape = wx.StaticBoxSizer(self.sizer_shape_staticbox, wx.VERTICAL)
        sizer_rectangle = wx.BoxSizer(wx.VERTICAL)
        sizer_polygon_height = wx.BoxSizer(wx.HORIZONTAL)
        sizer_length = wx.BoxSizer(wx.HORIZONTAL)
        sizer_circle = wx.BoxSizer(wx.VERTICAL)
        sizer_circle_height = wx.BoxSizer(wx.HORIZONTAL)
        sizer_radius = wx.BoxSizer(wx.HORIZONTAL)
        sizer_radius.Add(self.label_radius, 0, wx.ALIGN_CENTER_VERTICAL|wx.FIXED_MINSIZE, 0)
        sizer_radius.Add(self.float_radius, 0, wx.LEFT|wx.ALIGN_CENTER_VERTICAL|wx.FIXED_MINSIZE, border_width)
        sizer_circle.Add(sizer_radius, 0, wx.EXPAND, 0)
        sizer_circle_height.Add(self.label_circle_height, 0, wx.ALIGN_CENTER_VERTICAL|wx.FIXED_MINSIZE, 0)
        sizer_circle_height.Add(self.float_circle_height, 0, wx.LEFT|wx.ALIGN_CENTER_VERTICAL|wx.FIXED_MINSIZE, border_width)
        sizer_circle.Add(sizer_circle_height, 0, wx.TOP|wx.EXPAND, border_width)
        self.panel_circle.SetAutoLayout(True)
        self.panel_circle.SetSizer(sizer_circle)
        sizer_circle.Fit(self.panel_circle)
        sizer_circle.SetSizeHints(self.panel_circle)
        sizer_length.Add(self.label_length, 0, wx.ALIGN_CENTER_VERTICAL|wx.FIXED_MINSIZE, 0)
        sizer_length.Add(self.float_length, 0, wx.LEFT|wx.ALIGN_CENTER_VERTICAL|wx.FIXED_MINSIZE, border_width)
        sizer_length.Add(self.label_width, 0, wx.LEFT|wx.ALIGN_CENTER_VERTICAL|wx.FIXED_MINSIZE, border_width)
        sizer_length.Add(self.float_width, 0, wx.LEFT|wx.ALIGN_CENTER_VERTICAL|wx.FIXED_MINSIZE, border_width)
        sizer_rectangle.Add(sizer_length, 0, wx.EXPAND, 0)
        sizer_polygon_height.Add(self.label_polygon_height, 0, wx.ALIGN_CENTER_VERTICAL|wx.FIXED_MINSIZE, 0)
        sizer_polygon_height.Add(self.float_polygon_height, 0, wx.LEFT|wx.ALIGN_CENTER_VERTICAL|wx.FIXED_MINSIZE, border_width)
        sizer_rectangle.Add(sizer_polygon_height, 0, wx.TOP|wx.EXPAND, border_width)
        self.panel_rectangle.SetAutoLayout(True)
        self.panel_rectangle.SetSizer(sizer_rectangle)
        sizer_rectangle.Fit(self.panel_rectangle)
        sizer_rectangle.SetSizeHints(self.panel_rectangle)
        self.choicebook_shape.AddPage(self.panel_circle, "Circle")
        self.choicebook_shape.AddPage(self.panel_rectangle, "Polygon")
        sizer_shape.Add(self.choicebook_shape, 1, wx.ALL|wx.EXPAND, border_width)
        sizer_shape.Add(self.text_info, 0, wx.LEFT|wx.RIGHT|wx.BOTTOM|wx.EXPAND, border_width)
        sizer_layout.Add(sizer_shape, 0, wx.ALL|wx.EXPAND, border_width)
        sizer_layout.Add(self.input, 1, wx.LEFT|wx.RIGHT|wx.BOTTOM|wx.EXPAND, border_width)
        self.SetAutoLayout(True)
        self.SetSizer(sizer_layout)
        sizer_layout.Fit(self)
        sizer_layout.SetSizeHints(self)
        self.Layout()
    
    def __bind_events(self):
        self.Bind(wx.EVT_CHOICEBOOK_PAGE_CHANGED, self.on_change_shape, self.choicebook_shape)
        self.Bind(floatctrl.EVT_FLOAT, self.on_change_circle, self.float_radius)
        self.Bind(floatctrl.EVT_FLOAT, self.on_change_circle_height, self.float_circle_height)
        self.Bind(floatctrl.EVT_FLOAT, self.on_change_rectangle, self.float_length)
        self.Bind(floatctrl.EVT_FLOAT, self.on_change_rectangle, self.float_width)
        self.Bind(floatctrl.EVT_FLOAT, self.on_change_polygon_height, self.float_polygon_height)
    
    def on_change_shape(self, event):
        if not self.getting_values:
            radius = 5.0
            if self.choicebook_shape.GetSelection() == self.ID_Circle:
                self.float_radius.set_value( radius )
                self.on_change_circle( None )
                self.float_circle_height.set_value( self.entity.getheight() )
            else:
                self.float_length.set_value( radius )
                self.float_width.set_value( radius )
                self.on_change_rectangle( None )
                self.float_polygon_height.set_value( self.entity.getheight() )
            servers.get_entity_object_server().setentityobjectdirty( self.entity, True )
    
    def on_change_circle(self, event):
        if not self.getting_values:
            r = self.float_radius.get_value()
            self.entity.setcircle( r )
            servers.get_entity_object_server().setentityobjectdirty( self.entity, True )
    
    def on_change_circle_height(self, event):
        if not self.getting_values:
            self.entity.setheight( self.float_circle_height.get_value() )
            servers.get_entity_object_server().setentityobjectdirty( self.entity, True )
    
    def on_change_rectangle(self, event):
        if not self.getting_values:
            hx = self.float_width.get_value() / 2
            hz = self.float_length.get_value() / 2
            self.entity.setpolygon(
                -hx, 0, -hz,
                 hx, 0, -hz,
                 hx, 0,  hz
                )
            self.entity.addvertextopolygon(
                -hx, 0, hz
                )
            servers.get_entity_object_server().setentityobjectdirty( self.entity, True )
    
    def on_change_polygon_height(self, event):
        if not self.getting_values:
            self.entity.setheight( self.float_polygon_height.get_value() )
            servers.get_entity_object_server().setentityobjectdirty( self.entity, True )
    
    def set_entity(self, entity):
        self.entity = entity
        self.getting_values = True
        
        # Shape
        if self.entity.getshapetype() == 'circle':
            self.choicebook_shape.SetSelection(0)
            self.float_radius.set_value( self.entity.getcircle() )
            self.float_circle_height.set_value( self.entity.getheight() )
        else:
            self.choicebook_shape.SetSelection(1)
            self.float_polygon_height.set_value( self.entity.getheight() )
        
        # Input
        self.input.set_entity( entity, triggerinput.OutEvents )
        
        self.getting_values = False
    
    def refresh(self):
        self.set_entity( self.entity, triggerinput.OutEvents )


# OutputPanel class
class OutputPanel(editorpanel.EditorPanel):
    def __init__(self, *args, **kwds):
        editorpanel.EditorPanel.__init__(self, *args, **kwds)
        
        self.output_enter = triggeroutput.TriggerOutputCtrl(self, 'enter_area', None)
        self.output_exit = triggeroutput.TriggerOutputCtrl(self, 'exit_area', None)
        
        self.__set_properties()
        self.__do_layout()
        self.__bind_events()
    
    def __set_properties(self):
        self.SetScrollRate(10, 10)
    
    def __do_layout(self):
        sizer_layout = wx.BoxSizer(wx.VERTICAL)
        sizer_layout.Add(self.output_enter, 1, wx.ALL|wx.EXPAND, cfg.BORDER_WIDTH)
        sizer_layout.Add(self.output_exit, 1, wx.LEFT|wx.RIGHT|wx.BOTTOM|wx.EXPAND, cfg.BORDER_WIDTH)
        self.SetAutoLayout(True)
        self.SetSizer(sizer_layout)
        sizer_layout.Fit(self)
        sizer_layout.SetSizeHints(self)
        self.Layout()
    
    def __bind_events(self):
        pass
    
    def set_entity(self, entity):
        self.output_enter.set_entity( entity )
        self.output_exit.set_entity( entity )


# create_all_editors function
def create_all_editors(entity, parent):
    input = InputPanel(parent, -1)
    input.set_entity( entity )
    output = OutputPanel(parent, -1)
    output.set_entity( entity )
    return [ ('Output', output), ('Input', input) ]
