##\file agenteditor.py
##\brief Specific agent trigger editor panel

import wx
import wx.lib.ticker

import editorpanel
import triggerinput
import conjurerconfig as cfg


# AgentTriggerPanel class
class AgentTriggerPanel(editorpanel.EditorPanel):
    
    def __init__(self, *args, **kwds):
        editorpanel.EditorPanel.__init__(self, *args, **kwds)
        self.entity = None
        
        self.input = triggerinput.TriggerInputCtrl(self, 'Perceivable events')
        
        self.__set_properties()
        self.__do_layout()
    
    def __set_properties(self):
        self.SetScrollRate(10, 10)
    
    def __do_layout(self):
        sizer_layout = wx.BoxSizer(wx.VERTICAL)
        sizer_layout.Add(self.input, 1, wx.ALL|wx.EXPAND, cfg.BORDER_WIDTH)
        self.SetAutoLayout(True)
        self.SetSizer(sizer_layout)
        sizer_layout.Fit(self)
        sizer_layout.SetSizeHints(self)
        self.Layout()
    
    def set_entity(self, entity):
        self.entity = entity
        self.input.set_entity( entity, triggerinput.InEvents )
    
    def refresh(self):
        self.set_entity( self.entity )


# create_all_editors function
def create_all_editors(entity, parent):
    editor = AgentTriggerPanel(parent, -1)
    editor.set_entity(entity)
    return [ ('Perception', editor) ]
