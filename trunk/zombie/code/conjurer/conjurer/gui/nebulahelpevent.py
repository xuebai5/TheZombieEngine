import wx

# Used to trap events alerting that the help
# will be called
wxEVT_CONJURER_HELP = wx.NewEventType()
EVT_CONJURER_HELP = wx.PyEventBinder(wxEVT_CONJURER_HELP, 1)

KEY_HELP = wx.WXK_F1

# Class used for put a value into the event
class HelpEvent(wx.PyCommandEvent):
    def __init__(self, id, value = 0, object=None):
        wx.PyCommandEvent.__init__(self, wxEVT_CONJURER_HELP, id)

        self.__value = value
        self.SetEventObject(object)
        self.ShouldPropagate()

    def GetValue(self):
        """Retrieve the value of the control at the time
        this event was generated."""
        return self.__value
