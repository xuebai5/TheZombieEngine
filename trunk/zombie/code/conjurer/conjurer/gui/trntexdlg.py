##\file trntexdlg.py
##\brief Terrain texture dialog

import wx

import floatslider
import imgbox
import servers
import textslider
import togwin


# TextureDialog class
class TextureDialog(togwin.ChildToggableDialog):
    """Dialog to select and modify the terrain texture brush"""
    
    def __init__(self, parent):
        togwin.ChildToggableDialog.__init__(
            self, "Terrain texture brush", parent
            )
        
        # texture selector control
        # TODO: Retrieve the texture images from Nebula terrain object
        self.imgbox_textures = imgbox.ImageBox(self, image_size=wx.Size(80,80))
        fileserver = servers.get_file_server()
        self.imgbox_textures.append_image(wx.Bitmap(fileserver.manglepath("outgui:images/001.png"), wx.BITMAP_TYPE_ANY))
        self.imgbox_textures.append_image(wx.Bitmap(fileserver.manglepath("outgui:images/007.png"), wx.BITMAP_TYPE_ANY))
        self.imgbox_textures.append_image(wx.Bitmap(fileserver.manglepath("outgui:images/011.png"), wx.BITMAP_TYPE_ANY))
        
        self.sizer_filter_group_staticbox = wx.StaticBox(self, -1, "Filter")
        self.slider_size = floatslider.FloatSlider(
            self, -1, "Size:", 0.5, 0.0, 1.0, precision=1000.0
            )
        self.slider_blend = floatslider.FloatSlider(
            self, -1, "Blend:", 0.5, 0.0, 1.0, precision=1000.0
            )
        self.slider_intensity = floatslider.FloatSlider(
            self, -1, "Intensity:", 0.5, 0.0, 1.0, precision=1000.0
            )
        self.checkbox_slope = wx.CheckBox(self, -1, "Filter by slope")
        self.slider_min_slope = floatslider.FloatSlider(
            self, -1, "Min. slope:", 15.0, 0.0, 90.0, precision=10.0
            )
        self.slider_max_slope = floatslider.FloatSlider(
            self, -1, "Max. slope:", 45.0, 0.0, 90.0, precision=10.0
            )
        self.static_line_filter = wx.StaticLine(self, -1)
        self.checkbox_height = wx.CheckBox(self, -1, "Filter by height")
        self.slider_min_height = floatslider.FloatSlider(
            self, -1, "Min. height:", 20.0, 0.0, 100.0, precision=10.0
            )
        self.slider_max_height = floatslider.FloatSlider(
            self, -1, "Max. height:", 60.0, 0.0, 100.0, precision=10.0
            )
        
        self.__set_properties()
        self.__do_layout()

    def __set_properties(self):
        textslider.align_sliders([
            self.slider_size,
            self.slider_blend,
            self.slider_intensity
            ])
        textslider.align_sliders([
            self.slider_min_slope,
            self.slider_max_slope
            ])
        textslider.align_sliders([
            self.slider_min_height,
            self.slider_max_height
            ])

    def __do_layout(self):
        border_sizer = wx.BoxSizer(wx.VERTICAL)
        sizer_controls = wx.BoxSizer(wx.VERTICAL)
        sizer_filter_group = wx.StaticBoxSizer(self.sizer_filter_group_staticbox, wx.VERTICAL)
        sizer_controls.Add(self.imgbox_textures, 1, wx.EXPAND, 0)
        sizer_controls.Add(self.slider_size, 0, wx.TOP|wx.BOTTOM|wx.EXPAND, 5)
        sizer_controls.Add(self.slider_blend, 0, wx.BOTTOM|wx.EXPAND, 5)
        sizer_controls.Add(self.slider_intensity, 0, wx.BOTTOM|wx.EXPAND, 5)
        sizer_filter_group.Add(self.checkbox_slope, 0, wx.ALL|wx.FIXED_MINSIZE, 5)
        sizer_filter_group.Add(self.slider_min_slope, 0, wx.ALL|wx.EXPAND, 5)
        sizer_filter_group.Add(self.slider_max_slope, 0, wx.LEFT|wx.RIGHT|wx.BOTTOM|wx.EXPAND, 5)
        sizer_filter_group.Add(self.static_line_filter, 0, wx.BOTTOM|wx.EXPAND, 5)
        sizer_filter_group.Add(self.checkbox_height, 0, wx.ALL|wx.FIXED_MINSIZE, 5)
        sizer_filter_group.Add(self.slider_min_height, 0, wx.ALL|wx.EXPAND, 5)
        sizer_filter_group.Add(self.slider_max_height, 0, wx.LEFT|wx.RIGHT|wx.BOTTOM|wx.EXPAND, 5)
        sizer_controls.Add(sizer_filter_group, 0, wx.EXPAND, 0)
        border_sizer.Add(sizer_controls, 1, wx.ALL|wx.EXPAND, 10)
        self.SetAutoLayout(True)
        self.SetSizer(border_sizer)
        border_sizer.Fit(self)
        border_sizer.SetSizeHints(self)
        self.Layout()

    def persist(self):
        # TODO
        return [
            create_window,
            ()  # no parameters for create
            ]

    def restore(self, data_list):
        # TODO
        pass


# create_window function
def create_window(parent):
    return TextureDialog(parent)
