##\file conjurerspinctrl.py
##\brief spinctrl that allows silent setting of value

import wx

class ConjurerSpinCtrl(wx.SpinCtrl):
    def __init__(self, *args, **kwds):
        wx.SpinCtrl.__init__(self, *args, **kwds)
        
        self.suppress_text_event = False
        self.Bind(
            wx.EVT_TEXT,
            self.__on_text
            )

    def __on_text(self, event):
    # don't allow the EVT_TEXT event to propagate if 
    # the flag to suppress it has been set
        if not self.suppress_text_event:
            event.Skip()

    def silent_set_value(self, value):
        """ set the value on the control without signalling EVT_TEXT """
        old_suppress_value = self.suppress_text_event
        self.suppress_text_event = True
        self.SetValue(value)
        self.suppress_text_event = old_suppress_value
