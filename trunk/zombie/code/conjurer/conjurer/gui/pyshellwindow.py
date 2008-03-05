#-------------------------------------------------------------------------
# File: nebulagui.py
#-------------------------------------------------------------------------
# Conjurer Services, S.A. (2005)
#-------------------------------------------------------------------------
# Description: main window for the outer nebula gui.
#-------------------------------------------------------------------------
# Log:
#	- 01-02-2005: David Reyes:: Created File
#-------------------------------------------------------------------------

#-------------------------------------------------------------------------
# Imports
#-------------------------------------------------------------------------
import wx.py as py
import childdialoggui

#-------------------------------------------------------------------------
# Main Code
#-------------------------------------------------------------------------
class PythonShell(childdialoggui.childDialogGUI):
    def __init__(self, frame):
        childdialoggui.childDialogGUI.__init__(
            self,
            "Python Shell", 
            frame 
            )

    def persist(self):
        return [
            create_window,
            ()  #no parameters for create function
            ]

def create_window(frame):
    win = PythonShell(frame)
    win.Show(True)
    editor = py.shell.Shell(win, -1 )
    # set focus to editor so user can start typing straight away
    editor.SetFocus()
    return win