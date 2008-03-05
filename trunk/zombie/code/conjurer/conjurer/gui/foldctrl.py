##\file foldctrl.py
##\brief Control to fold/unfold another child control

import wx


# FoldCtrl class
class FoldCtrl(wx.ToggleButton):
    """Control to fold/unfold another control"""
    
    def __init__(self, parent, caption, target_ctrl=None):
        wx.ToggleButton.__init__(self, parent, -1, caption, style=wx.SUNKEN_BORDER)
        
        self.target_ctrl = target_ctrl
        self.expanded = True
        self.SetValue(False)
        
        self.Bind(wx.EVT_TOGGLEBUTTON, self.__on_toggle_ctrl, self)
    
    def __on_toggle_ctrl(self, event):
        if self.target_ctrl != None:
            self.SetValue(False)
            self.expanded = not self.expanded
            parent = self.target_ctrl.GetParent()
            parent.GetSizer().Show( self.target_ctrl, self.expanded, True )
#            parent.GetSizer().Show( self.target_ctrl, self.GetValue(), True )
            parent.Layout()
        event.Skip()
    
    def set_target_ctrl(self, target_ctrl):
        self.target_ctrl = target_ctrl


# FoldChildCtrl class
class FoldChildCtrl(wx.PyControl):
    """Control to fold/unfold another control that's child of this one"""
    
    def __init__(self, parent, caption):
        wx.PyControl.__init__(self, parent, style=wx.NO_BORDER|wx.TAB_TRAVERSAL)
        
        self.button_caption = wx.ToggleButton(self, -1, caption)
        self.ctrl_contents = None
        
        self.button_caption.SetValue(True)
    
    def __do_layout(self):
        sizer_layout = wx.BoxSizer(wx.VERTICAL)
        sizer_layout.Add(self.button_caption, 0, wx.EXPAND, 0)
        sizer_layout.Add(self.ctrl_contents, 1, wx.EXPAND, 0)
        self.SetAutoLayout(True)
        self.SetSizer(sizer_layout)
        sizer_layout.Fit(self)
        sizer_layout.SetSizeHints(self)
        self.Layout()
    
    def __bind_events(self):
        self.Bind(wx.EVT_SIZE, self.__on_size)
        self.Bind(wx.EVT_TOGGLEBUTTON, self.__on_toggle_fold, self.button_caption)
    
    def __on_size(self, event):
        # Needed to resize controls along with the window owning this control
        self.Layout()
        event.Skip()
    
    def __on_toggle_fold(self, event):
        self.GetSizer().Show( self.ctrl_contents,
            self.button_caption.GetValue(), True )
        self.GetParent().Layout()
    
    def set_contents_ctrl(self, contents_ctrl):
        """
        Set the control to be shown/hid when toggling the caption button.
        
        Call just once and before setting the parent window layout.
        """
        self.ctrl_contents = contents_ctrl
        self.__do_layout()
        self.__bind_events()
    
    def show(self, show):
        """Show/hide the contents control"""
        # TODO: Test this function (and if it's enough to just call to SetValue
        #       due to it maybe triggering an wx.EVT_TOGGLEBUTTON)
        self.SetValue( show )
        self.__on_toggle_fold( None )
