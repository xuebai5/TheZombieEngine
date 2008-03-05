import inspectorstatetab
import inspectorcmdstab

import childdialoggui
import wx

class __object_inspector(childdialoggui.childDialogGUI):
    def __init__(self, parent, object):
        childdialoggui.childDialogGUI.__init__(self, "Object inspector", parent)
        
        self.SetTitle("Inspector:%s" % object.getfullname())
        
        self.notebook = wx.Notebook(self, -1)
        self.class_tab = inspectorcmdstab.ClassCommandsTab(self.notebook, object)
        self.state_tab = inspectorstatetab.StateTab(self.notebook, object)
    
        self.notebook.AddPage(self.state_tab, "State")
        self.notebook.AddPage(self.class_tab, "Commands")
        
        self.__set_properties()
        self.__do_layout()
        
    def __set_properties(self):
        self.SetSize((300,400))
    
    def __do_layout(self):
        sizer = wx.BoxSizer(wx.HORIZONTAL)
        sizer.Add(self.notebook, 1, wx.EXPAND, 0)
        self.SetAutoLayout(True)
        self.SetSizer(sizer)
        self.Layout()
    
def create_window(frame, object):
    win = __object_inspector( frame, object )
    return win

