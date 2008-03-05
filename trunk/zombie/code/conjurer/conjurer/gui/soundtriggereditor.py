##\file soundtriggereditor.py
##\brief Specific sound source trigger editor panel

import wx

import editorpanel
import triggerinput
import conjurerconfig as cfg


# SoundTriggerPanel class
class SoundTriggerPanel(editorpanel.EditorPanel):
    
    def __init__(self, *args, **kwds):
        editorpanel.EditorPanel.__init__(self, *args, **kwds)
        self.entity = None
        
        self.input = triggerinput.TriggerInputCtrl(self)
        
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
        self.input.set_entity( entity, triggerinput.OutEvents )
    
    def refresh(self):
        self.set_entity( self.entity )


# create_all_editors function
def create_all_editors(entity, parent):
    editor = SoundTriggerPanel(parent, -1)
    editor.set_entity(entity)
    return [ ('Trigger input', editor) ]
