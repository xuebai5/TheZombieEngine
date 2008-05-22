#-------------------------------------------------------------------------
# File: childdialoggui.py
#-------------------------------------------------------------------------
# Conjurer Services, S.A. (2005)
#-------------------------------------------------------------------------
# Description: base classes for Conjurer dialogs
#-------------------------------------------------------------------------
# Log:
#    - 04-02-2005: Zombie:: Created File
#-------------------------------------------------------------------------

#-------------------------------------------------------------------------
# Imports
#-------------------------------------------------------------------------
from dialoggui import *

import wx

import weakref


class childDialogGUI( guiDialog, wx.MDIChildFrame ):
    def __init__( self, name, parent ):
        guiDialog.__init__(self, name )
        wx.MDIChildFrame.__init__(self, parent, -1, name )
        self.SetBackgroundStyle( wx.BG_STYLE_COLOUR )
        self.SetBackgroundColour(
            wx.SystemSettings.GetColour(wx.SYS_COLOUR_BTNFACE)
            )
        self.SetWindowStyleFlag(wx.TAB_TRAVERSAL)
        self.Bind(wx.EVT_CLOSE, self.on_close)
    
    # should be overwritten (and rebound)
    def on_close(self, event):
        self.Destroy()


class InstanceTrackingChildDialogGUI(childDialogGUI):
    """Abstract class - subclass it to get child dialog gui classes 
    that keep track of their instances and prevent duplicate
    views being created.
    New instances are added to the __window_dictionary__ 
    class variable, with the parameters passed to the create 
    function as the dictionary key.
    When a new instance is requested, the class checks if there
    is already an instance of the class in the dictionary that 
    matches the create function parameters. If there is one, and 
    it has not been destroyed, this is returned instead.
    If no match is found, a new instance is created, intialized and
    added to the dictionary.
    The dictionary has weak value references, so automatically 
    removes redundant entries.
    IMPORTANT: To create subclasses:
    a) make this class the parent class
    b) do not implement the #__init__ method in the child class:
    instead put the behaviour you would include here in the #init 
    method - this ensures that the initialization step is run only 
    for new instances.
    """

    __window_dictionary__ = weakref.WeakValueDictionary()

    def __remove_from_window_dictionary(self):
        instance_dictionary = self.__class__.__window_dictionary__
        for each_key, each_value in instance_dictionary.items():
            if each_value == self:
                del instance_dictionary[each_key]

    def __new__(cls, *args, **kwds):
        window_instance = None
        if cls.__window_dictionary__.has_key(args):
            window_instance = cls.__window_dictionary__[args]
        if window_instance:
            return window_instance
        else:
            new_instance = object.__new__(cls, *args, **kwds)
            cls.__window_dictionary__[args] = new_instance
            new_instance.init(*args, **kwds)
            return new_instance

    def __init__(self, *args, **kwds):
        pass

    def init(self, *args, **kwds):
        childDialogGUI.__init__(self, *args, **kwds)

    def display(self):
        """Display me - if I have been iconized, restore me.
        If I am hidden, show me.
        Finally, set focus to me."""
        if self.IsIconized():
            self.Restore()
        if not self.IsShown():
            self.Show()
        self.SetFocus()

    def on_close(self, event):
        # remove myself from the class window dictionary
        # before doing the on_close processing in the superclass
        self.__remove_from_window_dictionary()
        childDialogGUI.on_close(self, event)

