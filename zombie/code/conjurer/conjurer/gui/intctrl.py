##\file intctrl.py
##\brief Text control that only accepts int values

import wx.lib.intctrl

import events


# IntCtrl class
class IntCtrl(wx.lib.intctrl.IntCtrl):
    """
    Text control that only accepts int values
    
    It's just a wx.lib.intctrl.IntCtrl that throws custom events.
    
    The control generates 'changing' events while keeping the focus, ending
    with a 'changed' event when loses the focus and the value has changed.
    """
    
    def __init__(
                # Same params as wx.lib.intctrl.IntCtrl
                self, parent, id=-1, value = 0,
                pos = wx.DefaultPosition, size = wx.DefaultSize,
                style = 0, validator = wx.DefaultValidator,
                name = "integer",
                min=None, max=None,
                limited = 0, allow_none = 0, allow_long = 0,
                default_color = wx.BLACK, oob_color = wx.RED,
                ):
        wx.lib.intctrl.IntCtrl.__init__( self, parent, id, value, pos, size,
                style, validator, name, min, max, limited, allow_none,
                allow_long, default_color, oob_color )
        
        self.old_value = self.GetValue()
        
        self.Bind(wx.lib.intctrl.EVT_INT, self.__on_change_int, self)
        wx.EVT_KILL_FOCUS(self, self.__on_kill_focus)
    
    def __on_change_int(self, event):
        self.GetEventHandler().ProcessEvent(
            events.ChangingEvent( self.GetId(), self.GetValue(), self )
            )
        event.Skip()
    
    def __on_kill_focus(self, event):
        if self.GetValue() != self.old_value:
            self.GetEventHandler().ProcessEvent(
                events.ChangedEvent( self.GetId(), self.GetValue(),
                    self.old_value, self )
                )
            self.old_value = self.GetValue()
        event.Skip()
    
    def get_value(self):
        return self.GetValue()
    
    def set_value(self, value):
        self.SetValue(value)
    
    def SetValue(self, value):
        if self.__limited:
            value = max( value, self.__min )
            value = min( value, self.__max )
        if self.FindFocus() != self:
            self.old_value = value
        wx.lib.intctrl.IntCtrl.SetValue(self, value)
