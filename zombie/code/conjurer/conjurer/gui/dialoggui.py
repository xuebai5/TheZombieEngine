#-------------------------------------------------------------------------
# File: dialoggui.py
#-------------------------------------------------------------------------
# Conjurer Services, S.A. (2005)
#-------------------------------------------------------------------------
# Description: base class to build a dialog.
#-------------------------------------------------------------------------
# Log:
#    - 04-02-2005: Zombie:: Created File
#-------------------------------------------------------------------------

#-------------------------------------------------------------------------
# Imports
#-------------------------------------------------------------------------
import wx
from pynebula import *


class guiDialog:
    def __init__(self, dialog_name):
        self.dialog_name = dialog_name
        
    def new_command( self, do, undo ):
        lookup( '/sys/servers/undo/' ).newcommand( do, undo )
        
    def name(self):
        return self.dialog_name
    
    # should be overwrited 
    def persist(self):
        return []
        
    # should be overwrited
    def restore(self, list ):
        pass
    
    # may be overwrited
    def is_restorable(self, data_list):
        return True
    
    # may be overwrited
    def refresh(self):
        pass
    
    # may be overwrited
    def save(self, overwrite=True):
        pass
    
    # may be overwrited
    def is_dirty(self):
        return False
    
    # must be overwrited to return a DocSaver if is_dirty is overwrited
    def get_saver(self):
        return None
    
    # may be overwrited
    def on_app_event(self, event):
        pass
    
    # the next handle_X methods are intended to tell if the window answers
    # to the tipycal shortcuts associated to each action X
    # if the window handles that action, then overwrite the method to return
    # true (and do the action there if the window hasn't binded to the
    # shortcut event)
    def handle_undo(self):
        return False
    def handle_redo(self):
        return False
    def handle_cut(self):
        return False
    def handle_copy(self):
        return False
    def handle_paste(self):
        return False
