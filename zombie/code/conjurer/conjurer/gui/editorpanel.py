##\file editorpanel.py
##\brief Panel class where all editor panels inherit from

import wx


# EditorPanel class
class EditorPanel(wx.ScrolledWindow):
    def __init__(self, *args, **kwds):
        kwds["style"] = wx.TAB_TRAVERSAL
        wx.ScrolledWindow.__init__(self, *args, **kwds)
    
    def persist(self):
        return []
    
    def restore(self, data):
        pass
    
    def refresh(self):
        pass
    
    def on_app_event(self, event):
        pass
