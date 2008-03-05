##\file silder.py
##\brief Slider control with custom events

import wx

import events


# Slider class
class Slider(wx.Slider):
    """Slider control with custom events"""
    
    def __init__(self, *args, **kwds):
        wx.Slider.__init__(self, *args, **kwds)
        
        self.old_value = self.GetValue()
        self.slider_held = False
        
        wx.EVT_SCROLL(self, self.__on_generic_event)
        wx.EVT_SCROLL_THUMBTRACK(self, self.__on_track_thumb)
        wx.EVT_SCROLL_THUMBRELEASE(self, self.__on_release_thumb)
    
    def __on_generic_event(self, event):
        if not self.slider_held and self.GetValue() != self.old_value:
            self.GetEventHandler().ProcessEvent(
                events.ChangedEvent( self.GetId(), self.GetValue(),
                    self.old_value, self )
                )
            self.old_value = self.GetValue()
        event.Skip()
    
    def __on_track_thumb(self, event):
        self.slider_held = True
        self.GetEventHandler().ProcessEvent(
            events.ChangingEvent( self.GetId(), self.GetValue(), self )
            )
        event.Skip()
    
    def __on_release_thumb(self, event):
        self.slider_held = False
        event.Skip()
    
    def get_value(self):
        return self.GetValue()
    
    def set_value(self, value):
        self.SetValue(value)
    
    def SetValue(self, value):
        self.old_value = value
        wx.Slider.SetValue(self, value)
