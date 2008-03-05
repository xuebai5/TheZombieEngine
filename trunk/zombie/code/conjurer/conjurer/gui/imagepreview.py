##\file imagepreview.py
##\brief Dialog that shows an image at its full size

import wx

import childdialoggui
import format



# PreviewWindow class
class PreviewWindow(wx.PyScrolledWindow):
    """Scrolled window where a preview of a FSM is shown"""
    
    def __init__(self, parent, image_path=None):
        wx.PyScrolledWindow.__init__(self, parent, style=wx.SUNKEN_BORDER)
        
        self.image = wx.StaticBitmap( self, -1, wx.Bitmap(
            format.mangle_path(image_path)) )
        
        self.SetScrollRate(1,1)
        self.SetVirtualSize( self.image.GetSize() )


# PreviewDialog class
class PreviewDialog(childdialoggui.childDialogGUI):
    """Dialog that shows an image at its full size"""
    
    def __init__(self, parent, image_path=None, title=None):
        if title == None:
            title = image_path
        childdialoggui.childDialogGUI.__init__(self, title, parent)
        
        self.win_preview = PreviewWindow(self, image_path)
        
        self.__do_layout()
    
    def __do_layout(self):
        sizer_layout = wx.BoxSizer(wx.VERTICAL)
        sizer_layout.Add(self.win_preview, 1, wx.EXPAND, 0)
        self.SetSize((500, 500))
        self.SetAutoLayout(True)
        self.SetSizer(sizer_layout)
        self.Layout()
    
    def __on_size(self, event):
        # Needed to resize controls along with the window owning this control
        self.Layout()
        event.Skip()
