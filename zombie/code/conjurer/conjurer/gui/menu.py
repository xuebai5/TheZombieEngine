##\file menu.py
##\brief Base menu class for all menus

import wx


# Menu class
class Menu(wx.Menu):
    """Base menu class for all menus"""
    def __init__(self, frame):
        wx.Menu.__init__(self)
        self.frame = frame
        self.funcs = {}
    
    def get_frame(self):
        return self.frame
    
    def bind_function(self, cmd_id, function):
        """Bind a function to a menu item"""
        self.frame.Bind(wx.EVT_MENU, function, id=cmd_id)
        self.funcs[cmd_id] = function
    
    def unbind_function(self, cmd_id):
        """Unbind a function from a menu item"""
        if self.funcs.has_key(cmd_id):
            self.funcs.pop(cmd_id)
            self.Unbind(wx.EVT_MENU, id=cmd_id)
    
    def get_function(self, cmd_id):
        """Return the function bound to a menu item, or None if no function
        is found"""
        if self.funcs.has_key(cmd_id):
            return self.funcs[cmd_id]
        else:
            return None
    
    # Override it when needed to rebuild a dynamic menu
    def refresh(self):
        pass
