##\file statusbar.py
##\brief Status bar for the main window

import wx

import pynebula

import format
import servers


# ID of each field
ID_ToolBrief = 0
ID_UndoMemory = 1


# StatusBar class
class StatusBar(wx.StatusBar):
    """Status bar for the main window"""
    
    def __init__(self, frame):
        wx.StatusBar.__init__(self, frame, -1)
    
    def create(self):
        # Fields
        self.SetFieldsCount(2)
        self.SetStatusWidths([-1,175])
        self.SetStatusText("Magician, welcome to Conjurer. Ready to accept your magic.", ID_ToolBrief)
        self.update_undo_memory()
        
        # Timer to update the undo memory usage
        self.undo_timer = wx.PyTimer(self.update_undo_memory)
        self.undo_timer.Start(5000)
    
    def __del__(self):
        if self.undo_timer is not None:
            self.undo_timer.Stop()
            self.undo_timer = None
    
    def refresh(self):
        self.update_undo_memory()
    
    def update_undo_memory(self):
        mem_used = servers.get_command_server().getmemoryused()
        mem_limit = servers.get_command_server().getmemorylimit()
        
        # Format to ###.# Xb
        mem_used = format.bytes2best_str(mem_used)
        mem_limit = format.bytes2best_str(mem_limit)
        
        self.SetStatusText("Undo memory: " + mem_used + "/" + mem_limit, ID_UndoMemory)
    
    def destroy(self):
        self.undo_timer.Stop()
        self.undo_timer = None
