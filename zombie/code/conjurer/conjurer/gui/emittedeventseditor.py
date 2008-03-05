##\file emittedeventseditor.py
##\brief Specific editor to set the events that an entity is emitting

import wx
import wx.lib.ticker

import editorpanel
import events
import propgrid
import servers
import triggerinput
import conjurerconfig as cfg

import conjurerframework as cjr


# EmittedEventsPanel class
class EmittedEventsPanel(editorpanel.EditorPanel):
    """Specific editor to set the events that an entity is emitting"""
    
    def __init__(self, *args, **kwds):
        editorpanel.EditorPanel.__init__(self, *args, **kwds)
        self.entity = None
        self.event_ids = []
        self.pids = {'source': None, 'duration': None, 'priority': None}
        
        self.checklist_events = wx.CheckListBox(
                                            self,
                                            style=wx.LB_SORT
                                            )
        self.props_event = propgrid.PropertyGrid(self)
        self.text_info = wx.lib.ticker.Ticker(
                                self, 
                                -1,
                                "Source = 0 -> self entity; Duration = 0 -> "\
                                "infinite duration; " \
                                "Low priority numbers -> Higher priority",
                                bgcolor=wx.SystemSettings.GetColour(
                                                wx.SYS_COLOUR_BTNFACE
                                            ),
                                ppf=1,
                                fps=50
                                )
        
        self.__set_properties()
        self.__do_layout()
        self.__bind_events()
    
    def __set_properties(self):
        self.SetScrollRate(10, 10)
        
        # Available events
        trigger_server = self.__get_trigger_server()
        for i in range( trigger_server.geteventtypesnumber() ):
            if trigger_server.isanareatriggerinevent(i):
                self.checklist_events.Append( trigger_server.geteventlabel(i) )
                self.event_ids.append( i )
        # Properties of an event
        self.props_event.begin()
        self.pids['source'] = self.props_event.append_property(
                                        None, 
                                        'Source entity', 
                                        propgrid.Type_Int 
                                        )
        self.pids['duration'] = self.props_event.append_property(
                                            None, 
                                            'Duration', 
                                            propgrid.Type_Int 
                                            )
        self.pids['priority'] = self.props_event.append_property(
                                            None, 
                                            'Priority', 
                                            propgrid.Type_Int 
                                            )
        self.props_event.end()

        self.__update_props()
    
    def __do_layout(self):
        sizer_border = wx.BoxSizer(wx.VERTICAL)
        sizer_layout = wx.BoxSizer(wx.VERTICAL)
        sizer_layout.Add(self.checklist_events, 1, wx.EXPAND|wx.ADJUST_MINSIZE)
        sizer_layout.Add(
            self.props_event, 
            0, 
            wx.TOP|wx.EXPAND|wx.ADJUST_MINSIZE, 
            cfg.BORDER_WIDTH
            )
        sizer_layout.Add(
            self.text_info, 
            0, 
            wx.TOP|wx.EXPAND|wx.ADJUST_MINSIZE, 
            cfg.BORDER_WIDTH
            )
        sizer_border.Add(sizer_layout, 1, wx.ALL|wx.EXPAND, cfg.BORDER_WIDTH)
        self.SetAutoLayout(True)
        self.SetSizer(sizer_border)
        sizer_border.Fit(self)
        sizer_border.SetSizeHints(self)
        self.Layout()

    def __bind_events(self):
        self.Bind(
            wx.EVT_LISTBOX, 
            self.on_select_event, 
            self.checklist_events
            )
        self.Bind(
            wx.EVT_CHECKLISTBOX, 
            self.on_toggle_event, 
            self.checklist_events
            )
        self.Bind(
            events.EVT_CHANGED, 
            self.on_change_event, 
            self.props_event
            )

    def __update_props(self):
        i = self.checklist_events.GetSelection()
        if i != wx.NOT_FOUND:
            event_type = self.event_ids[i]
            index = self.__get_event_index( event_type )
            if index != wx.NOT_FOUND:
                trigger_server = self.__get_trigger_server()
                event_id = self.entity.geteventidbyindex(index)
                source_id = trigger_server.geteventsource(event_id)
                if source_id == self.entity.getid():
                    source_id = 0
                duration = trigger_server.geteventduration(event_id)
                priority = trigger_server.geteventpriority(event_id)
                self.props_event.set_property_value(
                    self.pids['source'], 
                    source_id 
                    )
                self.props_event.set_property_value(
                    self.pids['duration'], 
                    duration 
                    )
                self.props_event.set_property_value(
                    self.pids['priority'], 
                    priority 
                    )
        self.props_event.Refresh()
    
    def __get_event_index(self, event_type):
        trigger_server = self.__get_trigger_server()
        for i in range( self.entity.geteventscount() ):
            event_id = self.entity.geteventidbyindex(i)
            if trigger_server.geteventtype(event_id) == event_type:
                return i
        return wx.NOT_FOUND
    
    def __get_prop_values(self):
        source_id = self.props_event.get_property_value( self.pids['source'] )
        if source_id == 0:
            source_id = self.entity.getid()
        source = servers.get_entity_object_server().getentityobject( source_id )
        if source is None:
            cjr.show_warning_message(
                "There's no valid source entity for the specified entity id"
                )
            source = self.entity
            self.props_event.set_property_value( self.pids['source'], 0 )
            self.props_event.Refresh()
        duration = self.props_event.get_property_value( self.pids['duration'] )
        priority = self.props_event.get_property_value( self.pids['priority'] )
        return {'source': source, 'duration': duration, 'priority': priority}
    
    def on_select_event(self, event):
        self.__update_props()
    
    def on_toggle_event(self, event):
        event_type = self.event_ids[ event.GetSelection() ]
        index = self.__get_event_index( event_type )
        if index == wx.NOT_FOUND:
            # Add the event
            values = self.__get_prop_values()
            persist_type = self.__get_trigger_server().geteventpersistentid(
                                        event_type
                                        )
            self.entity.emitgameevent(
                persist_type, 
                values['source'],
                values['duration'], 
                values['priority'] 
                )
        else:
            # Delete the event
            event_id = self.entity.geteventidbyindex( index )
            self.entity.stopgameevent( event_id )
    
    def on_change_event(self, event):
        i = self.checklist_events.GetSelection()
        if i != wx.NOT_FOUND:
            if self.checklist_events.IsChecked(i):
                event_type = self.event_ids[i]
                persist_type = self.__get_trigger_server().geteventpersistentid(
                                            event_type
                                        )
                index = self.__get_event_index( event_type )
                event_id = self.entity.geteventidbyindex( index )
                values = self.__get_prop_values()
                self.entity.stopgameevent( event_id )
                self.entity.emitgameevent(
                    persist_type, 
                    values['source'],
                    values['duration'], 
                    values['priority'] 
                    )

    def __get_trigger_server(self):
        return servers.get_trigger_server()

    def set_entity(self, entity):
        self.entity = entity

        # Emitted events
        trigger_server = self.__get_trigger_server()
        for i in range( self.entity.geteventscount() ):
            event_type = trigger_server.geteventtype(
                                    self.entity.geteventidbyindex(i)
                                )
            self.checklist_events.Check( self.event_ids.index(event_type) )


# EmittedEventsClassPanel class
class EmittedEventsClassPanel(editorpanel.EditorPanel):
    
    def __init__(self, *args, **kwds):
        editorpanel.EditorPanel.__init__(self, *args, **kwds)
        self.entity = None

        self.input = triggerinput.TriggerInputCtrl(self, 'Emitted events')

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
    editor = EmittedEventsPanel(parent, -1)
    editor.set_entity(entity)
    return [ ('Emitted events', editor) ]

# create_all_class_editors function
def create_class_editors(entity, parent):
    editor = EmittedEventsClassPanel(parent, -1)
    editor.set_entity(entity)
    return [ ('Emitted events', editor) ]
