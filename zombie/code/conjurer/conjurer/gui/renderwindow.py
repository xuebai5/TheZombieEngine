import wx

import childdialoggui

class RenderWindow(childdialoggui.childDialogGUI):

    def __init__ (self,parent):
        childdialoggui.childDialogGUI.__init__(self,"Render Window", parent )
        
        self.panel = wx.Panel(self)

    def GetPanelHandle(self):
        return self.panel.GetHandle()


def CreateWindow(parent):
    return RenderWindow(parent)


