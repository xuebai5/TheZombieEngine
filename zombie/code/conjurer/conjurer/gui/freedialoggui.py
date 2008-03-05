#-------------------------------------------------------------------------
# File: freedialoggui.py
#-------------------------------------------------------------------------
# Conjurer Services, S.A. (2005)
#-------------------------------------------------------------------------
# Description: base class to build a dialog.
#-------------------------------------------------------------------------
# Log:
#    - 04-02-2005: David Reyes:: Created File
#-------------------------------------------------------------------------

#-------------------------------------------------------------------------
# Imports
#-------------------------------------------------------------------------
from dialoggui import *

import wx

class freeDialogGUI( guiDialog, wx.Dialog ):
    def __init__( self, name, parent ):
        guiDialog.__init__(self, name )
        wx.Dialog.__init__(self, parent, -1, name )
        self.Bind(wx.EVT_CLOSE, self.on_close)
    
    # should be overwrited (and rebinded)
    def on_close(self, event):
        self.Destroy()
