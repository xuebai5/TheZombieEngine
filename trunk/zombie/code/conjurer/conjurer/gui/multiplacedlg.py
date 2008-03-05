##\file multiplacedlg.py
##\brief Multiplace tool settings dialog

import wx

import app
import events
import floatslider
import togwin

import conjurerconfig as cfg


# MultiPlaceDialog class
class MultiPlaceDialog(togwin.ChildToggableDialog):
    """Multiplace tool settings dialog"""

    def __init__(self, parent):
        togwin.ChildToggableDialog.__init__(
            self, "Multiplace object", parent
            )
        
        self.slider_density = floatslider.FloatSlider(
            self, -1, "Density", 0.5, 0.0, 1.0, precision=1000.0
            )
        self.slider_diameter = floatslider.FloatSlider(
            self, -1, "Diameter", 10.0, 1.0, 100.0, precision=10.0
            )
        self.checkbox_random_rotation = wx.CheckBox(self, -1, "Random rotation")
        self.slider_size_offset = floatslider.FloatSlider(
            self, -1, "Max. size offset (%)", 0.0, 0.0, 100.0, precision=10.0
            )
        
        self.__set_properties()
        self.__do_layout()
        self.__bind_events()

    def __set_properties(self):
        data = {
            'density': app.get_multiplace_tool().getintensity(),
            'diameter': app.get_multiplace_tool().getdiameter(),
            'random rotation': app.get_multiplace_tool().getrandomrotation(),
            'size offset': app.get_multiplace_tool().getsizevariation()
            }
        self.restore([data])

    def __do_layout(self):
        sizer_border = wx.BoxSizer(wx.VERTICAL)
        sizer_layout = wx.BoxSizer(wx.VERTICAL)
        sizer_layout.Add(
            self.slider_density,
            0, 
            wx.EXPAND
            )
        sizer_layout.Add(
            self.slider_diameter,
            0, 
            wx.TOP|wx.EXPAND, 
            cfg.BORDER_WIDTH
            )
        sizer_layout.Add(
            self.checkbox_random_rotation, 
            0,
            wx.TOP|wx.FIXED_MINSIZE, 
            cfg.BORDER_WIDTH
            )
        sizer_layout.Add(
            self.slider_size_offset, 
            0, 
            wx.TOP|wx.EXPAND,
            cfg.BORDER_WIDTH
            )
        sizer_border.Add(
            sizer_layout, 
            0,
            wx.ALL|wx.EXPAND, 
            cfg.BORDER_WIDTH
            )
        self.SetSizerAndFit(sizer_border)

    def __bind_events(self):
        self.Bind(
            events.EVT_CHANGING, 
            self.on_change_density, 
            self.slider_density
            )
        self.Bind(
            events.EVT_CHANGED, 
            self.on_change_density,
            self.slider_density
            )
        self.Bind(
            events.EVT_CHANGING, 
            self.on_change_diameter, 
            self.slider_diameter
            )
        self.Bind(
            events.EVT_CHANGED, 
            self.on_change_diameter,
            self.slider_diameter
            )
        self.Bind(
            wx.EVT_CHECKBOX,
            self.on_toggle_random_rotation,
            self.checkbox_random_rotation
            )
        self.Bind(
            events.EVT_CHANGING, 
            self.on_change_size_offset, 
            self.slider_size_offset
            )
        self.Bind(
            events.EVT_CHANGED, 
            self.on_change_size_offset, 
            self.slider_size_offset
            )

    def on_change_density(self, event):
        app.get_multiplace_tool().setintensity(
            self.slider_density.get_value() )

    def on_change_diameter(self, event):
        app.get_multiplace_tool().setdiameter(
            self.slider_diameter.get_value() )

    def on_toggle_random_rotation(self, event):
        app.get_multiplace_tool().setrandomrotation(
            self.checkbox_random_rotation.GetValue() )

    def on_change_size_offset(self, event):
        app.get_multiplace_tool().setsizevariation(
            self.slider_size_offset.get_value() / 100 )

    def persist(self):
        data = {
            'density': self.slider_density.get_value(),
            'diameter': self.slider_diameter.get_value(),
            'random rotation': self.checkbox_random_rotation.GetValue(),
            'size offset': self.slider_size_offset.get_value() / 100
            }
        return [
            create_window,
            (), # no parameters for create function
            data
            ]

    def restore(self, data_list):
        data = data_list[0]

        # Restore tool
        tool = app.get_multiplace_tool()
        tool.setintensity( data['density'] )
        tool.setdiameter( data['diameter'] )
        tool.setrandomrotation( data['random rotation'] )
        if data.has_key('size offset'):
            tool.setsizevariation( data['size offset'] )

        # Restore controls
        self.slider_density.set_value( tool.getintensity() )
        self.slider_diameter.set_value( tool.getdiameter() )
        self.checkbox_random_rotation.SetValue( tool.getrandomrotation() )
        self.slider_size_offset.set_value( tool.getsizevariation() * 100 )


# create_window function
def create_window(parent):
    return MultiPlaceDialog(parent)
