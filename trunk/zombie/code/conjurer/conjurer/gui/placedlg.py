##\file placedlg.py
##\brief Place tool settings dialog

import wx

import app
import events
import floatslider
import togwin

import conjurerconfig as cfg


# PlaceDialog class
class PlaceDialog(togwin.ChildToggableDialog):
    """Place tool settings dialog"""
    
    def __init__(self, parent):
        togwin.ChildToggableDialog.__init__(
            self, "Place object", parent
            )
        
        self.checkbox_random_rotation = wx.CheckBox(self, -1, "Random rotation")
        self.slider_size_offset = floatslider.FloatSlider(
                                            self, 
                                            -1, 
                                            "Max. size offset (%)", 
                                            0.0,
                                            0.0, 
                                            100.0, 
                                            precision=10.0
                                            )
        
        self.__set_properties()
        self.__do_layout()
        self.__bind_events()
    
    def __set_properties(self):
        data = {
            'random rotation': app.get_place_tool().getrandomrotation(),
            'size offset': app.get_place_tool().getsizevariation()
            }
        self.restore([data])
    
    def __do_layout(self):
        sizer_border = wx.BoxSizer(wx.VERTICAL)
        sizer_layout = wx.BoxSizer(wx.VERTICAL)
        sizer_layout.Add(
            self.checkbox_random_rotation,
            0, 
            wx.FIXED_MINSIZE
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

    def on_toggle_random_rotation(self, event):
        app.get_place_tool().setrandomrotation(
            self.checkbox_random_rotation.GetValue() )

    def on_change_size_offset(self, event):
        app.get_place_tool().setsizevariation(
            self.slider_size_offset.get_value() / 100 )

    def persist(self):
        data = {
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
        tool = app.get_place_tool()
        tool.setrandomrotation( data['random rotation'] )
        if data.has_key('size offset'):
            tool.setsizevariation( data['size offset'] )

        # Restore controls
        self.checkbox_random_rotation.SetValue( tool.getrandomrotation() )
        self.slider_size_offset.set_value( tool.getsizevariation() * 100 )


# create_window function
def create_window(parent):
    return PlaceDialog(parent)
