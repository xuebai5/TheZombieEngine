##\file scriptriggereditor.py
##\brief Specific script trigger editor panel

import wx

import pynebula

import editorpanel
import format
import servers
import triggerlib


# ScriptTriggerPanel class
class ScriptTriggerPanel(editorpanel.EditorPanel):
    
    def __init__(self, *args, **kwds):
        editorpanel.EditorPanel.__init__(self, *args, **kwds)
        self.entity = None
        self.getting_values = False
        self.event_ids = []
        
        self.sizer_input_staticbox = wx.StaticBox(self, -1, "Input")
        self.sizer_output_staticbox = wx.StaticBox(self, -1, "Output")
        self.checklist_input = wx.CheckListBox(self, -1)
        self.label_script = wx.StaticText(self, -1, "Script:")
        self.choice_script = wx.Choice(self, -1)
        
        self.__set_properties()
        self.__do_layout()
        self.__bind_events()
    
    def __set_properties(self):
        self.SetScrollRate(10, 10)
        
        # Available input events
        ts = servers.get_trigger_server()
        for i in range( ts.geteventtypesnumber() ):
            if ts.isagenerictriggerinevent(i):
                self.checklist_input.Append( ts.geteventlabel(i) )
                self.event_ids.append( i )
    
    def __do_layout(self):
        sizer_layout = wx.BoxSizer(wx.VERTICAL)
        sizer_output = wx.StaticBoxSizer(self.sizer_output_staticbox, wx.VERTICAL)
        sizer_script = wx.BoxSizer(wx.HORIZONTAL)
        sizer_input = wx.StaticBoxSizer(self.sizer_input_staticbox, wx.VERTICAL)
        sizer_input.Add(self.checklist_input, 1, wx.ALL|wx.EXPAND, 5)
        sizer_layout.Add(sizer_input, 1, wx.ALL|wx.EXPAND, 10)
        sizer_script.Add(self.label_script, 0, wx.ALIGN_CENTER_VERTICAL|wx.FIXED_MINSIZE, 0)
        sizer_script.Add(self.choice_script, 1, wx.LEFT|wx.ALIGN_CENTER_VERTICAL|wx.FIXED_MINSIZE, 5)
        sizer_output.Add(sizer_script, 0, wx.ALL|wx.EXPAND, 5)
        sizer_layout.Add(sizer_output, 0, wx.LEFT|wx.RIGHT|wx.BOTTOM|wx.EXPAND, 10)
        self.SetAutoLayout(True)
        self.SetSizer(sizer_layout)
        sizer_layout.Fit(self)
        sizer_layout.SetSizeHints(self)
        self.Layout()
    
    def __bind_events(self):
        self.Bind(wx.EVT_CHECKLISTBOX, self.on_change_input, self.checklist_input)
        self.Bind(wx.EVT_CHOICE, self.on_select_script, self.choice_script)
    
    def __update_choice_scripts(self):
        # Fill choice control
        self.choice_script.Clear()
        self.choice_script.Append("<none>")
        script_list = triggerlib.get_trigger_scripts()
        script_list.sort()
        self.choice_script.AppendItems( script_list )
        # Select current script
        if self.entity != None:
            script = self.entity.gettriggerscript()
            if script is None:
                self.choice_script.SetStringSelection("<none>")
            else:
                self.choice_script.SetStringSelection( script.getname() )
    
    def on_change_input(self, event):
        if not self.getting_values:
            for i in range( self.checklist_input.GetCount() ):
                self.entity.seteventflag( self.event_ids[i], self.checklist_input.IsChecked(i) )
            servers.get_entity_object_server().setentityobjectdirty( self.entity, True )
    
    def on_select_script(self, event):
        if not self.getting_values:
            # Get the selected trigger script
            name = self.choice_script.GetStringSelection()
            if name == "<none>":
                # Unset trigger script
                self.entity.settriggerscript( None )
            else:
                # Set the trigger script
                self.entity.settriggerscriptbyname( str(name) )
            servers.get_entity_object_server().setentityobjectdirty( self.entity, True )
    
    def set_entity(self, entity):
        self.entity = entity
        self.getting_values = True
        
        # Input
        for i in range( self.checklist_input.GetCount() ):
            self.checklist_input.Check( i, self.entity.geteventflag( self.event_ids[i] ) )
        
        # Output
        self.__update_choice_scripts()
        
        self.getting_values = False
    
    def refresh(self):
        self.set_entity( self.entity )


# create_all_editors function
def create_all_editors(entity, parent):
    editor = ScriptTriggerPanel(parent, -1)
    editor.set_entity(entity)
    return [ ('Generic trigger', editor) ]
