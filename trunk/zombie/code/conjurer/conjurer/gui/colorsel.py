##\file colorsel.py
##\brief Color selector control

import wx
import wx.lib.colourselect

import events
import intslider
import slider
import textslider
import conjurerconfig as cfg


# ColorSelector class
class ColorSelector(wx.PyControl):
    """Control that groups several controls to easyly set a color"""
    
    def __init__(self, parent, id, label="", color=(128, 128, 128)):
        wx.PyControl.__init__(
            self, 
            parent, 
            id, 
            style=wx.NO_BORDER|wx.TAB_TRAVERSAL
            )
        self.color = color
        # Also used to remember the initial color when using the 'slider all'
        # to darken/lighten a color. This avoids trunkating a color between
        # scroll changes and 'forgetting' the initial color hue.
        self.old_color = color
        
        self.staticbox = wx.StaticBox(self, -1, label)
        self.slider_R = intslider.IntSlider(self, -1, "R:", color[0])
        self.slider_G = intslider.IntSlider(self, -1, "G:", color[1])
        self.slider_B = intslider.IntSlider(self, -1, "B:", color[2])
        average = self.__get_average(self.color)
        self.slider_all = slider.Slider(self, -1, average, 0, 255)
        self.color_sel = wx.lib.colourselect.ColourSelect(self, -1)
        
        self.__set_properties()
        self.__do_layout()
        self.__bind_events()
    
    def __set_properties(self):
        # resize slider labels to the largest label to align them
        textslider.align_sliders([
            self.slider_R,
            self.slider_G,
            self.slider_B
            ])
        
        # resize color selector button to match width of numeric text boxes,
        # but keeping it square
        x = self.slider_R.text_ctrl.GetSize().x
        self.color_sel.SetSize((x, x))
        
        # set controls to reflect current color
        self.__update_controls()
    
    def __do_layout(self):
        group_sizer = wx.StaticBoxSizer(self.staticbox, wx.HORIZONTAL)
        sizer_3 = wx.BoxSizer(wx.VERTICAL)
        sizer_4 = wx.BoxSizer(wx.HORIZONTAL)
        sizer_3.Add(
            self.slider_R, 
            0, 
            wx.LEFT|wx.RIGHT|wx.TOP|wx.EXPAND|wx.ALIGN_RIGHT, 
            cfg.BORDER_WIDTH
            )
        sizer_3.Add(
            self.slider_G, 
            0, 
            wx.LEFT|wx.RIGHT|wx.EXPAND|wx.ALIGN_RIGHT, 
            cfg.BORDER_WIDTH
            )
        sizer_3.Add(
            self.slider_B, 
            0, 
            wx.LEFT|wx.RIGHT|wx.EXPAND|wx.ALIGN_RIGHT, 
            cfg.BORDER_WIDTH
            )
        sizer_4.Add(
            self.slider_R.label.GetSize(), 
            0, 
            wx.FIXED_MINSIZE
            )
        sizer_4.Add(
            self.slider_all, 
            1, 
            wx.FIXED_MINSIZE|wx.ALIGN_CENTER_VERTICAL
            )
        sizer_4.Add(
            self.color_sel, 
            0, 
            wx.FIXED_MINSIZE|wx.ALIGN_CENTER_VERTICAL
            )
        sizer_3.Add(
            sizer_4, 
            1, 
            wx.LEFT|wx.RIGHT|wx.BOTTOM|wx.EXPAND, 
            cfg.BORDER_WIDTH
            )
        group_sizer.Add(sizer_3, 1, wx.EXPAND, 0)
        self.SetAutoLayout(True)
        self.SetSizer(group_sizer)
        group_sizer.Fit(self)
        group_sizer.SetSizeHints(self)
        self.Layout()
    
    def __bind_events(self):
        self.Bind(wx.EVT_SET_FOCUS, self.__on_set_focus)
        self.Bind(wx.EVT_SIZE, self.__on_size)
        self.Bind(events.EVT_CHANGING, self.__on_changing_R, self.slider_R)
        self.Bind(events.EVT_CHANGING, self.__on_changing_G, self.slider_G)
        self.Bind(events.EVT_CHANGING, self.__on_changing_B, self.slider_B)
        self.Bind(events.EVT_CHANGING, self.__on_changing_all, self.slider_all)
        self.Bind(events.EVT_CHANGED, self.__on_changed_R, self.slider_R)
        self.Bind(events.EVT_CHANGED, self.__on_changed_G, self.slider_G)
        self.Bind(events.EVT_CHANGED, self.__on_changed_B, self.slider_B)
        self.Bind(events.EVT_CHANGED, self.__on_changed_all, self.slider_all)
        self.Bind(
            wx.lib.colourselect.EVT_COLOURSELECT, 
            self.__on_select_color, 
            self.color_sel
            )

    def __on_set_focus(self, event):
        self.slider_R.SetFocus()
    
    def __on_size(self, evt):
        # Needed to resize sliders with the window owning this control
        self.Layout()
        evt.Skip()
    
    def __changing_color(self, color):
        self.color = color
        self.GetEventHandler().ProcessEvent(
            events.ChangingEvent( self.GetId(), color, self )
            )
    
    def __changed_color(self, color):
        self.color = color
        self.GetEventHandler().ProcessEvent(
            events.ChangedEvent( self.GetId(), color, self.old_color, self )
            )
        self.old_color = color
    
    def __on_changing_R(self, event):
        self.__changing_color((event.get_value(), self.color[1], self.color[2]))
        self.__update_controls(update_rgb=False)
    
    def __on_changing_G(self, event):
        self.__changing_color((self.color[0], event.get_value(), self.color[2]))
        self.__update_controls(update_rgb=False)
    
    def __on_changing_B(self, event):
        self.__changing_color((self.color[0], self.color[1], event.get_value()))
        self.__update_controls(update_rgb=False)
    
    def __on_changing_all(self, event):
        self.__lighten_color()
        self.__changing_color(self.color)
        self.__update_controls(update_average=False)
    
    def __on_changed_R(self, event):
        self.__changed_color((event.get_value(), self.color[1], self.color[2]))
        self.__update_controls(update_rgb=False)
    
    def __on_changed_G(self, event):
        self.__changed_color((self.color[0], event.get_value(), self.color[2]))
        self.__update_controls(update_rgb=False)
    
    def __on_changed_B(self, event):
        self.__changed_color((self.color[0], self.color[1], event.get_value()))
        self.__update_controls(update_rgb=False)
    
    def __on_changed_all(self, event):
        self.__lighten_color()
        self.__changed_color(self.color)
        self.__update_controls(update_average=False)
    
    def __on_select_color(self, evt):
        self.__changed_color( evt.GetValue().Get() )
        self.__update_controls()
    
    def __lighten_color(self):
        # Use 'slider all' to darken/lighten color
        old_average = self.__get_average(self.old_color)
        new_average = self.slider_all.get_value()
        if old_average != 0:
            new_R = (new_average * self.old_color[0]) / old_average
            new_G = (new_average * self.old_color[1]) / old_average
            new_B = (new_average * self.old_color[2]) / old_average
        else:
            new_R = new_average
            new_G = new_average
            new_B = new_average
        self.color = self.__get_trunkated_color( (new_R, new_G, new_B) )
    
    def __update_controls(self, update_rgb=True, update_average=True):
        color = self.__get_trunkated_color(self.color)
        if update_rgb:
            self.slider_R.set_value(color[0])
            self.slider_G.set_value(color[1])
            self.slider_B.set_value(color[2])
        self.color_sel.SetValue(color)
        if update_average:
            self.slider_all.set_value( self.__get_average(color) )
    
    def __get_average(self, color):
        return ( color[0] + color[1] + color[2] ) / 3
    
    def __get_trunkated_color(self, color):
        return ( min(255, color[0]), min(255, color[1]), min(255, color[2]) )
    
    def get_value(self):
        return self.__get_trunkated_color(self.color)
    
    def set_value(self, color):
        self.color = color
        self.old_color = color
        self.__update_controls()
