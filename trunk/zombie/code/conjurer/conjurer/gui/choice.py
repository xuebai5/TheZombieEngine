##\file choice.py
##\brief Choice control with custom events

import wx

import events


# Choice class
class Choice(wx.Choice):
    """Choice control with custom events"""
    
    def __init__(self, *args, **kwds):
        wx.Choice.__init__(self, *args, **kwds)
        
        self.old_selection = self.GetSelection()
        
        self.Bind(wx.EVT_CHOICE, self.__on_choice, self)
    
    def __on_choice(self, event):
        self.GetEventHandler().ProcessEvent(
            events.ChangedEvent( self.GetId(), self.GetSelection(),
                self.old_selection, self )
            )
        self.old_selection = self.GetSelection()
        event.Skip()
    
    def SetSelection(self, index):
        wx.Choice.SetSelection(self, index)
        self.old_selection = self.GetSelection()
    
    def SetStringSelection(self, str):
        result = wx.Choice.SetStringSelection(self, str)
        self.old_selection = self.GetSelection()
        return result
