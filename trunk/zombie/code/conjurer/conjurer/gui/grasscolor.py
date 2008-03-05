##\file agrasscolor.py
##\brief Dialog to edit the color of a grass material

import wx

import colorsel
import format
import conjurerconfig as cfg


# MaterialColorDialog
class MaterialColorDialog(wx.Dialog):
    """Dialog to edit the color of a grass material"""
    
    def __init__(self, parent, material):
        wx.Dialog.__init__(
            self, 
            parent, 
            -1, 
            "Material '" + material.getname() + "'",
            style=wx.DEFAULT_DIALOG_STYLE|wx.RESIZE_BORDER|wx.TAB_TRAVERSAL
            )
        self.material = material
        
        self.color = colorsel.ColorSelector(self, -1, "Material color",
            format.unit_rgb_2_byte_rgb( material.getcolor() ) )
        self.button_ok = wx.Button(self, -1, "&OK")
        self.button_cancel = wx.Button(self, wx.ID_CANCEL, "&Cancel")
        
        self.__set_properties()
        self.__do_layout()
        self.__bind_events()

    def __set_properties(self):
        self.button_ok.SetDefault()
    
    def __do_layout(self):
        border_width = cfg.BORDER_WIDTH
        sizer_layout = wx.BoxSizer(wx.VERTICAL)
        sizer_buttons = wx.BoxSizer(wx.HORIZONTAL)
        sizer_layout.Add(
            self.color, 
            1, 
            wx.ALL|wx.EXPAND|wx.ADJUST_MINSIZE, 
            border_width
            )
        sizer_buttons.Add(self.button_ok, 0, wx.ADJUST_MINSIZE)
        sizer_buttons.Add(
            self.button_cancel, 
            0, 
            wx.LEFT|wx.ADJUST_MINSIZE, 
            border_width
            )
        sizer_layout.Add(
            sizer_buttons, 
            0, 
            wx.LEFT|wx.RIGHT|wx.BOTTOM|wx.ALIGN_RIGHT, 
            border_width
            )
        self.SetAutoLayout(True)
        self.SetSizer(sizer_layout)
        sizer_layout.Fit(self)
        sizer_layout.SetSizeHints(self)
        self.Layout()
    
    def __bind_events(self):
        self.Bind(wx.EVT_BUTTON, self.on_ok, self.button_ok)
        self.Bind(wx.EVT_BUTTON, self.on_cancel, self.button_cancel)
    
    def on_ok(self, event):
        r, g, b, a = format.byte_rgb_2_unit_rgba( self.color.get_value() )
        self.material.setcolor( r, g, b )
        self.EndModal( wx.ID_OK )
    
    def on_cancel(self, event):
        self.EndModal( wx.ID_CANCEL )
