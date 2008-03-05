##\file waitdlg.py
##\brief Dialog displayed to show a wait message

import wx


# WaitDialog class
class WaitDialog(wx.Dialog):
    """
    Dialog displayed to show a wait message
    
    This dialog is intended to be used to show a message to the user while
    Conjurer is performing some long work. The normal use is to show it just
    before beginning the long work and destroy it when the work finishes.
    If the work can be splitted in several steps it's good politics to update
    the displayed message between work steps.
    
    wx.MessageBox and wx.MessageDialog are useless for this purpose since
    they only work in modal mode. wx.ProgressDialog could serve, but it's ugly
    to show an empty progress bar when it cannot be updated.
    """
    
    def __init__(self, parent, message="Please wait, I'm working really hard..."):
        """
        Create and show the dialog
        
        A call to Update is done at the end to immediately paint the dialog
        contents, without having to wait for the next paint event. This is
        needed since this dialog is usually followed by a temporary blocking
        call to a Conjurer script command, which will make the paint event
        to happen after the call most of times.
        """
        wx.Dialog.__init__(self, parent, -1, style = 0)
        
        self.label = wx.StaticText(self, -1, message)
        
        self.__do_layout()
        
        self.Show(True)
        # Repaint dialog now, don't wait for the next Paint event
        self.Update()
    
    def __do_layout(self):
        sizer = wx.BoxSizer(wx.HORIZONTAL)
        sizer.Add(self.label, 0, wx.ALL|wx.FIXED_MINSIZE, 20)
        self.SetAutoLayout(True)
        self.SetSizer(sizer)
        sizer.Fit(self)
        sizer.SetSizeHints(self)
        self.Layout()
    
    def set_message(self, message):
        self.label.SetLabel(message)
        # Resize dialog to fit new label size
        self.SetSizer(None)
        self.__do_layout()
        # Repaint dialog now, don't wait for the next Paint event
        self.Update()
