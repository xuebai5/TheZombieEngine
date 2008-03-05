##\file triggeroutput.py
##\brief Control used to set perceivable events for a trigger

import wx

import servers
import conjurerconfig as cfg


# Events sets/filters to display only some group of events
InEvents = 0
OutEvents = 1


# TriggerInputCtrl class
class TriggerInputCtrl(wx.PyControl):
    """Control used to set perceivable events for a trigger"""
    
    def __init__(self, parent, caption='Events'):
        wx.PyControl.__init__(self, parent, style=wx.NO_BORDER|wx.TAB_TRAVERSAL)
        self.entity = None
        self.event_ids = {}
        self.getting_values = False
        
        self.staticbox_input = wx.StaticBox(self, -1, caption)
        self.checklist_input = wx.CheckListBox(self, -1, style=wx.LB_SORT)
        
        self.__do_layout()
        self.__bind_events()
    
    def __do_layout(self):
        sizer_border = wx.StaticBoxSizer(self.staticbox_input, wx.VERTICAL)
        sizer_border.Add(
            self.checklist_input, 
            1, 
            wx.ALL|wx.EXPAND, 
            cfg.BORDER_WIDTH
            )
        self.SetAutoLayout(True)
        self.SetSizer(sizer_border)
        sizer_border.Fit(self)
        sizer_border.SetSizeHints(self)
        self.Layout()
    
    def __bind_events(self):
        self.Bind(wx.EVT_SIZE, self.__on_size)
        self.Bind(
            wx.EVT_CHECKLISTBOX, 
            self.on_change_input, 
            self.checklist_input
            )
    
    def __on_size(self, event):
        # Needed to resize controls along with the window owning this control
        self.Layout()
        event.Skip()
    
    def __update_input_ctrl(self):
        self.checklist_input.Clear()
        self.event_ids = {}
        if self.entity is None:
            return

        # Available input events
        trigger_server = servers.get_trigger_server()
        for index in range( trigger_server.geteventtypesnumber() ):
            if trigger_server.isanareatriggerinevent(index):
                # since the list box is sorted we can't assume the position 
                # of any item won't change, so we need to store the id of 
                # each group against its name
                event_label =  trigger_server.geteventlabel(index) 
                self.event_ids[event_label] = index
                j = self.checklist_input.Append(event_label)
                # Enabled input events
                if self.get_event_flag( index ):
                    self.checklist_input.Check( j )
    
    def on_change_input(self, event):
        if not self.getting_values:
            selection_index = event.GetSelection()
            event_name = self.checklist_input.GetString(selection_index)
            index = self.event_ids[event_name]
            enabled = self.checklist_input.IsChecked( event.GetSelection() )
            self.set_event_flag( index, enabled )
    
    def set_event_flag(self, index, enabled):
        if self.entity.isa('nentityclass'):
            if self.events_set == InEvents:
                self.entity.setperceivableevent( index, enabled )
            else:
                self.entity.setemittedevent( index, enabled )
            servers.get_entity_class_server().setentityclassdirty(
                self.entity, 
                True
                )
        else:
            self.entity.seteventflag( index, enabled )
            servers.get_entity_object_server().setentityobjectdirty(
                self.entity, 
                True 
                )
    
    def get_event_flag(self, index):
        if self.entity.isa('nentityclass'):
            if self.events_set == InEvents:
                return self.entity.getperceivableevent( index )
            else:
                return self.entity.getemittedevent( index )
        else:
            return self.entity.geteventflag( index )
    
    def set_entity(self, entity, events_set):
        self.entity = entity
        self.events_set = events_set
        self.getting_values = True
        self.__update_input_ctrl()
        self.getting_values = False
