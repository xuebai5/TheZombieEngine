##\file vtextctrl.py
##\brief Text control with an OK button to accept changes

import wx

import events
import format


# ValidatedTextCtrl class
class ValidatedTextCtrl(wx.PyControl):
    """Text control with an OK button to accept changes"""
    
    def __init__(self, parent, id=-1, label="", value="", size=wx.DefaultSize,
                 validator=None, allow_empty=False):
        wx.PyControl.__init__(self, parent, id, style=wx.NO_BORDER)
        
        self.label = wx.StaticText(self, -1, label, style=wx.TE_RIGHT)
        self.text = wx.TextCtrl(self, id, value, size=size, style=wx.TE_PROCESS_ENTER)
        bmap = wx.Bitmap( format.mangle_path("outgui:images/tools/ok.bmp") )
        self.button = wx.BitmapButton(self, -1, bmap, style=wx.BU_AUTODRAW, size=(21,21))
        
        self.old_value = value
        self.validator = validator
        self.allow_empty = allow_empty
        
        self.__do_layout()
        self.__bind_events()
    
    def __do_layout(self):
        sizer = wx.BoxSizer(wx.HORIZONTAL)
        sizer.Add(self.label, 0, wx.FIXED_MINSIZE|wx.ALIGN_CENTER_VERTICAL, 0)
        sizer.Add(self.text, 1, wx.LEFT|wx.FIXED_MINSIZE|wx.ALIGN_CENTER_VERTICAL, 5)
        sizer.Add(self.button, 0, wx.FIXED_MINSIZE|wx.ALIGN_CENTER_VERTICAL, 0)
        self.SetAutoLayout(True)
        self.SetSizer(sizer)
        sizer.Fit(self)
        sizer.SetSizeHints(self)
        self.Layout()
    
    def __bind_events(self):
        self.Bind(wx.EVT_TEXT, self.__on_text_changes, self.text)
        self.Bind(wx.EVT_TEXT_ENTER, self.__on_accept_changes, self.text)
        self.Bind(wx.EVT_BUTTON, self.__on_accept_changes, self.button)
        self.Bind(wx.EVT_SIZE, self.__on_size)
    
    def __is_new_value(self):
        return self.text.GetValue() != self.old_value and \
            (self.allow_empty or self.text.GetValue() != "")
    
    def __update_ok_button(self):
        self.button.Enable( self.__is_new_value() )
    
    def __on_size(self, event):
        # Needed to resize the slider with the window owning this control
        self.Layout()
        event.Skip()
    
    def __on_text_changes(self, event):
        self.__update_ok_button()
    
    def __on_accept_changes(self, event):
        if self.__is_new_value():
            new_value = self.text.GetValue()
            old_value = self.old_value
            if self.validator != None:
                if not self.validator(new_value):
                    self.text.SetValue( self.old_value )
                    self.__update_ok_button()
                    return
            self.old_value = new_value
            self.__update_ok_button()
            self.GetEventHandler().ProcessEvent(
                events.ChangedEvent( self.GetId(), new_value, old_value, self )
                )
    
    def get_value(self):
        return self.text.GetValue()
    
    def set_value(self, value, permanent=True):
        if permanent:
            self.old_value = value
        self.text.SetValue( value )
        self.__update_ok_button()
