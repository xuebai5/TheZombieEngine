#-------------------------------------------------------------------------
# File: iehelpwindow.py
#-------------------------------------------------------------------------
# Conjurer Services, S.A. (2005)
#-------------------------------------------------------------------------
# Description: Windows with an ie integrated and search engine
#-------------------------------------------------------------------------
# Log:
#    - 09-02-2005: Javier Ferrer:: Created File
#-------------------------------------------------------------------------

#-------------------------------------------------------------------------
# TODO:
#    - Integration with nebula2 doxygen documentation.
#-------------------------------------------------------------------------

#-------------------------------------------------------------------------
# Imports
#-------------------------------------------------------------------------
import wx
import wx.py as py
from childdialoggui import *

# Conjurer
import pynebula

if wx.Platform == '__WXMSW__':
    import  wx.lib.iewin    as  iewin


class ChildWindow(childDialogGUI):
    def __init__(self, parent, prefix, sufix, query, home):
        childDialogGUI.__init__(self,"Help Browser",parent)

        self.home = home
        self.prefix = prefix
        self.sufix = sufix            
        self.query = query

        self.parent = parent
        if parent:
            self.titleBase = "Conjurer GUI"

        # Creating Boxer
        sizer = wx.BoxSizer(wx.VERTICAL)
        btnSizer = wx.BoxSizer(wx.HORIZONTAL)
        
        self.ie = iewin.IEHtmlWindow(self, -1, 
                                     style = wx.NO_FULL_REPAINT_ON_RESIZE)

        btn = wx.Button(self, -1, "Home", style=wx.BU_EXACTFIT)
        self.Bind(wx.EVT_BUTTON, self.OnHomeButton, btn)
        btnSizer.Add(btn, 0, wx.EXPAND|wx.ALL, 2)

        btn = wx.Button(self, -1, "<--", style=wx.BU_EXACTFIT)
        self.Bind(wx.EVT_BUTTON, self.OnPrevPageButton, btn)
        btnSizer.Add(btn, 0, wx.EXPAND|wx.ALL, 2)

        btn = wx.Button(self, -1, "-->", style=wx.BU_EXACTFIT)
        self.Bind(wx.EVT_BUTTON, self.OnNextPageButton, btn)
        btnSizer.Add(btn, 0, wx.EXPAND|wx.ALL, 2)

        btn = wx.Button(self, -1, "Stop", style=wx.BU_EXACTFIT)
        self.Bind(wx.EVT_BUTTON, self.OnStopButton, btn)
        btnSizer.Add(btn, 0, wx.EXPAND|wx.ALL, 2)

        btn = wx.Button(self, -1, "Refresh", style=wx.BU_EXACTFIT)
        self.Bind(wx.EVT_BUTTON, self.OnRefreshPageButton, btn)
        btnSizer.Add(btn, 0, wx.EXPAND|wx.ALL, 2)

        txt = wx.StaticText(self, -1, "Location:")
        btnSizer.Add(txt, 0, wx.CENTER|wx.ALL, 2)

        self.location = wx.ComboBox(
                            self, -1, "", style=wx.CB_DROPDOWN|wx.PROCESS_ENTER
                            )
        
        self.Bind(wx.EVT_COMBOBOX, self.OnLocationSelect, self.location)
        self.location.Bind(wx.EVT_KEY_UP, self.OnLocationKey)
        self.location.Bind(wx.EVT_CHAR, self.IgnoreReturn)
        btnSizer.Add(self.location, 1, wx.EXPAND|wx.ALL, 2)

        txt = wx.StaticText(self, -1, "Search:")
        btnSizer.Add(txt, 0, wx.CENTER|wx.ALL, 2)

        self.search = wx.ComboBox(
                          self, -1, "", style=wx.CB_DROPDOWN|wx.PROCESS_ENTER
                            )
        self.Bind(wx.EVT_COMBOBOX, self.OnSearchSelect, self.search)
        self.search.Bind(wx.EVT_KEY_UP, self.OnSearchKey)
        self.search.Bind(wx.EVT_CHAR, self.IgnoreReturn)
        btnSizer.Add(self.search, 1, wx.EXPAND|wx.ALL, 2)

        sizer.Add(btnSizer, 0, wx.EXPAND)
        sizer.Add(self.ie, 1, wx.EXPAND)

        self.current = self.prefix + self.query + self.sufix
        
        self.ie.LoadUrl(self.current)
        self.location.Append(self.current)
        
        # Setting this window main sizer
        self.SetSizer(sizer)

        # Since this is a wxWindow we have to call Layout ourselves
        self.Bind(wx.EVT_SIZE, self.OnSize)
        
        # Hook up the event handlers for the IE window
        self.Bind(iewin.EVT_DocumentComplete, self.OnDocumentComplete, self.ie)
        self.Bind(iewin.EVT_StatusTextChange, self.OnStatusTextChange, self.ie)
        self.Bind(iewin.EVT_TitleChange, self.OnTitleChange, self.ie)
        
        # self.tree.SetSelectionChangedCallback(self.OnNOHSelectionChanged)
        self.Bind(wx.EVT_CLOSE, self.OnCloseWindow)

    def OnSize(self, evt):
        self.Layout()
        
    def OnLocationSelect(self, evt):
        self.current = self.location.GetStringSelection()
        self.ie.Navigate(self.current)

    def OnSearchSelect(self, evt):
        word = self.search.GetStringSelection()
        self.ie.Navigate(self.prefix + word + self.sufix)        

    def OnLocationKey(self, evt):
        if evt.KeyCode() == wx.WXK_RETURN:
            URL = self.location.GetValue()
            self.location.Append(URL)
            self.ie.Navigate(URL)
        else:
            evt.Skip()

    def OnSearchKey(self, evt):
        if evt.KeyCode() == wx.WXK_RETURN:
            word = self.search.GetValue()
            self.search.Append(word)
            self.ie.Navigate(self.prefix + word + self.sufix)
        else:
            evt.Skip()

    def IgnoreReturn(self, evt):
        if evt.GetKeyCode() != wx.WXK_RETURN:
            evt.Skip()
        
    def OnCloseWindow(self, event):
        self.Destroy()

    def OnHomeButton(self, event):
        self.ie.Navigate(self.home)

    def OnPrevPageButton(self, event):
        self.ie.GoBack()

    def OnNextPageButton(self, event):
        self.ie.GoForward()

    def OnStopButton(self, evt):
        self.ie.Stop()

    def OnSearchPageButton(self, evt):
        self.ie.GoSearch()

    def OnRefreshPageButton(self, evt):
        self.ie.Refresh(iewin.REFRESH_COMPLETELY)

    def OnDocumentComplete(self, evt):
        self.current = evt.URL
        self.location.SetValue(self.current)

    def OnTitleChange(self, evt):
        if self.parent:
            self.parent.SetTitle(self.titleBase + ' -- ' + evt.Text)

    def OnStatusTextChange(self, evt):
        if self.parent:
            self.parent.SetStatusText(evt.Text)
    
def CreateWindow(frame, prefix, sufix="", query="", home="http://thezombieengine.sourceforge.net/"):
    # Creating lua shell window
    win = ChildWindow( frame, prefix, sufix, query, home )
