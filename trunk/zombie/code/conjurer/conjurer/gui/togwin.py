##\file togwin.py
##\brief Helpers to create windows toggable by the user (shown/hided)

import wx

import childdialoggui


# ChildToggableDialog class
class ChildToggableDialog(childdialoggui.childDialogGUI):
    """Inherit your ChildDialogGUI from this if its display is toggable"""
    
    def __init__(self, title, parent):
        """
        Constructor
        
        \param title Window title
        \param parent Parent window
        """
        childdialoggui.childDialogGUI.__init__(self, title, parent)
    
    def on_close(self, event):
        """
        Close event handler
        
        Override it when you need to do clean up stuff or control if the window
        can be closed. If you override it you must return true when the window
        must be closed and its associated menu item unchecked.
        
        \return True if the dialog must be closed, false otherwise
        """
        self.Destroy()
        return True


# ToggableWindowMgr class
class ToggableWindowMgr:
    """
    Manage show/hide state of toggable windows
    
    Usage, for each toggable window:
     1) Inherit a window from childToggableDialog.
     2) Add the window with the add_window method.
    
    A window is shown/hided each time its associated menu item is
    checked/unchecked. When the user closes the window the menu item's
    checkmark is automatically cleared.
    
    Internally the window is created/destroyed each time it's shown/hided.
    """
    
    def __init__(self, parent):
        """
        Constructor
        
        \param parent Parent window for all toggable windows
        """
        self.parent = parent
        self.windows = {}
    
    # When checking/unchecking a menu item
    def __on_menu(self, event):
        win = self.windows[ event.GetId() ]
        if win['window'] is None:
            win['window'] = win['create_function'](self.parent)
            win['window'].Bind(wx.EVT_CLOSE, self.__on_close)
        else:
            win['window'].Close()
    
    # When closing a window
    def __on_close(self, event):
        for key in self.windows.keys():
            win = self.windows[key]
            if win['window'] is event.GetEventObject():
                if win['window'].on_close(event):
                    win['window'].Unbind(wx.EVT_CLOSE)
                    win['menu_item'].Check(False)
                    win['window'].Destroy()
                    win['window'] = None
                else:
                    win['menu_item'].Check(True)
                break
    
    def add_window(self, create_function, menu_item, window=None):
        """
        Add an entry to handle a toggable window
        
        If a window is given, the associated menu item is checked. Otherwise
        it's unchecked.
        
        \param create_function Function that creates and returns a ChildToggableDialog
        \param menu_item Checkable menu item associated to the window
        \param window Window to handle, if it's already opened, or None otherwise
        """
        self.windows[menu_item.GetId()] = {
            'create_function': create_function,
            'menu_item': menu_item,
            'window': window
            }
        self.parent.Bind(wx.EVT_MENU, self.__on_menu, menu_item)
        if window is not None:
            menu_item.Check(True)
            window.Bind(wx.EVT_CLOSE, self.__on_close)
        else:
            menu_item.Check(False)
    
    def set_window(self, create_function, window):
        """
        Attach a window with a create_function
        
        The create function must have been added previously (with add_window).
        The associated menu item is checked.
        
        \param create_function Function used to create the window
        \param window Window to attach to the creation function
        """
        for key in self.windows.keys():
            win = self.windows[key]
            if win['create_function'] == create_function:
                win['window'] = window
                win['menu_item'].Check(True)
                window.Bind(wx.EVT_CLOSE, self.__on_close)
                break
    
    def show_window(self, menu_id):
        """
        Show a window and set the checkmark ot its associated menu item
        
        Nothing happens if the window is already displayed.
        
        \param menu_id Menu item's id associated to the window to show
        """
        win = self.windows[menu_id]
        if win['window'] == None:
            win['window'] = win['create_function'](self.parent)
            win['window'].Bind(wx.EVT_CLOSE, self.__on_close)
            win['menu_item'].Check(True)
