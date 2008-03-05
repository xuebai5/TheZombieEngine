##\file imgdlg.py
##\brief Image browser dialog


import os
import wx

import imgbox
import servers


# ImageFileDialog class
class ImageFileDialog(wx.FileDialog):
    """File dialog with a wildcard to chose among image files"""
    def __init__(self, *args, **kwds):
        wildcard, all_images_index = self.get_images_wildcard()
        kwds["wildcard"] = wildcard
        wx.FileDialog.__init__(self, *args, **kwds)
        self.SetFilterIndex(all_images_index)
    
    @staticmethod
    def get_images_wildcard():
        """
        Return a wildcard ready to be used by a file dialog for images
        
        It's also returned the index where the "all images" filter is found
        in the wildcard
        """
        # Get all image extensions supported
        all_wildcard = wx.Image.GetImageExtWildcard().split("|")[1]
        extensions = all_wildcard.split(";")
        
        # For each extension build a wildcard with the extension name in uppercase
        # Example: "*.jpg" -> "JPG images|*.jpg|"
        # Extensions are sorted so the user can find them easyly
        extensions.sort()
        wildcard = ""
        for extension in extensions:
            wildcard = wildcard + extension.strip("*.").upper() + " images|" + \
                       extension + "|"
        
        # Add an "all images" wildcard for all supported extensions
        wildcard = wildcard + "All image files|" + all_wildcard
        
        return wildcard, len(extensions)


# ImagesDialog class
class ImagesDialog(wx.Dialog):
    """Dialog used to select an image among a directory"""
    
    def __init__(self, parent, title, dir, image_size=wx.Size(32,32),
        scale_images=True, win_size=wx.Size(300,250)):
        wx.Dialog.__init__(self, parent, title=title,
            style=wx.DEFAULT_DIALOG_STYLE|wx.RESIZE_BORDER)
        self.win_size = win_size
        self.images_dir = dir
        
        self.label = wx.StaticText(self, -1, "Images from '" + \
            self.images_dir + "' directory:")
        self.imgbox = imgbox.ImageBox(self, image_size=image_size,
            scale_images=scale_images)
        self.button_ok = wx.Button(self, -1, "&OK")
        self.button_cancel = wx.Button(self, -1, "&Cancel")
        
        self.__set_properties()
        self.__do_layout()
        self.__bind_events()
    
    def __set_properties(self):
        # browse supported images from directory
        fileserver = servers.get_file_server()
        filenames = os.listdir( fileserver.manglepath(str(self.images_dir)) )
        dir = fileserver.manglepath(str(self.images_dir)) + "/"
        for filename in filenames:
            if not filename.startswith("."):
                path = dir + filename
                try:
                    if wx.Image.CanRead(path):
                        self.imgbox.append_image(wx.Bitmap(path), path)
                except:
                    pass
    
    def __do_layout(self):
        border_sizer = wx.BoxSizer(wx.VERTICAL)
        buttons_sizer = wx.BoxSizer(wx.HORIZONTAL)
        layout_sizer = wx.BoxSizer(wx.VERTICAL)
        layout_sizer.Add(self.label, 0, wx.BOTTOM|wx.FIXED_MINSIZE, 5)
        layout_sizer.Add(self.imgbox, 1, wx.EXPAND, 0)
        buttons_sizer.Add(self.button_ok, 1, wx.FIXED_MINSIZE, 0)
        buttons_sizer.Add(self.button_cancel, 1, wx.LEFT|wx.FIXED_MINSIZE, 5)
        layout_sizer.Add(buttons_sizer, 0, wx.TOP|wx.EXPAND, 10)
        border_sizer.Add(layout_sizer, 1, wx.ALL|wx.EXPAND, 10)
        self.SetAutoLayout(True)
        self.SetSizer(border_sizer)
        border_sizer.SetSizeHints(self)
        self.SetSize(self.win_size)
        self.Layout()
    
    def __bind_events(self):
        self.Bind(wx.EVT_BUTTON, self.on_ok, self.button_ok)
        self.Bind(wx.EVT_BUTTON, self.on_cancel, self.button_cancel)
    
    def on_ok(self, event):
        self.EndModal(wx.ID_OK)
    
    def on_cancel(self, event):
        self.EndModal(wx.ID_CANCEL)
    
    def get_path(self):
        """
        Return the path of the selected image
        
        An empty string is returned if there isn't any image selected
        """
        return self.imgbox.get_name_selection()
