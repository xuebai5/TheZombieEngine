##\file fsmtransitiontype.py
##\brief Control to edit the type of a transition and its type dependend fields

import wx

import pynebula

import events
import format
import fsm
import fsmevents
import servers

import conjurerframework as cjr


# Transition type ids
ID_EventTransition = 0
ID_ScriptTransition = 1


# get_default_condition function
##def get_default_condition(condition_type):
##    """
##    Get the default condition for the given transition type.
##    
##    The condition is created if it doesn't exist yet.
##    """
##    if condition_type == ID_EventTransition:
##        path = fsm.get_event_conditions_lib()
##        type = 'neventcondition'
##    else:
##        path = fsm.get_script_conditions_lib()
##        type = 'nscriptcondition'
##    path = format.append_to_path( path, "default" )
##    try:
##        return pynebula.lookup( path )
##    except:
##        return pynebula.new( type, path )


# get_event_condition function
def get_event_condition(event):
    cond = pynebula.lookup( fsm.get_event_conditions_lib() ).gethead()
    while cond is not None:
        if cond.getevent() == event:
            return cond.getfullname()
        cond = cond.getsucc()
    raise "Error: shared event " + str(event) + " not found"
##    path = format.append_to_path( fsm.get_event_conditions_lib(), "event" + str(event) )
##    try:
##        pynebula.lookup( path )
##        return path
##    except:
##        raise "Error: shared event condition " + str(event) + " not found"


# get_condition_gui_name function
def get_condition_gui_name(condition_path):
    condition = pynebula.lookup(condition_path)
##    shared_event_prefix = format.append_to_path( fsm.get_event_conditions_lib(), "event" )
##    if condition_path.startswith( shared_event_prefix ):
    if condition.isa('neventcondition'):
        return servers.get_trigger_server().geteventlabel( condition.getevent() )
    else:
        return condition.getname()


# get_free_event_conditions function
def get_free_event_conditions(state, allowed_condition=None):
    """
    Return a paths list of those event conditions which the given state doesn't
    already have a transition for.
    
    \param state State (path) to gather the free event conditions for
    \param allowed_condition Condition (path) allowed as a free condition, even if it's already taken
    \return List of event conditions paths not found in any transition of the given state
    """
    allowed_event = None
    if allowed_condition != None:
        cond = pynebula.lookup( allowed_condition )
        if cond.isa('neventcondition'):
            allowed_event = cond.getevent()
    conds_paths = fsm.get_event_conditions()
##    conds_paths = []
##    cond = pynebula.lookup( fsm.get_event_conditions_lib() ).gethead()
##    while cond is not None:
##        if cond.getname().startswith( "event" ):
##            conds_paths.append( cond.getfullname() )
##        cond = cond.getsucc()
    trans_paths = fsm.get_transitions( state )
    for trans_path in trans_paths:
        trans_obj = pynebula.lookup( trans_path )
        cond_obj = trans_obj.getcondition()
        if cond_obj.isa('neventcondition'):
            if cond_obj.getevent() != allowed_event:
                cond_path = get_event_condition( cond_obj.getevent() )
##                if cond_path == fsm.get_filteredcondition_of_transition( trans_path ):
##                    conds_paths.remove( get_event_condition( cond_obj.getevent() ) )
##                else:
                conds_paths.remove( cond_path )
    return conds_paths

# get_free_script_conditions function
def get_free_script_conditions(state, allowed_condition=None):
    """
    Return a paths list of those script conditions which the given state
    doesn't already have a transition for.
    
    \param state State (path) to gather the free script conditions for
    \param allowed_condition Condition (path) allowed as a free condition, even if it's already taken
    \return List of script conditions paths not found in any transition of the given state
    """
    conds_paths = fsm.get_script_conditions()
    trans_paths = fsm.get_transitions( state )
    for trans_path in trans_paths:
        trans_obj = pynebula.lookup( trans_path )
        cond_obj = trans_obj.getcondition()
        cond_path = cond_obj.getfullname()
        if cond_obj.isa('nscriptcondition') and cond_path != allowed_condition:
            conds_paths.remove( cond_path )
    return conds_paths

# get_free_conditions function
def get_free_conditions(condition_type, state):
    if condition_type == ID_EventTransition:
        return get_free_event_conditions( state )
    else:
        return get_free_script_conditions( state )


# EventTransitionPanel class
class EventTransitionPanel(wx.Panel):
    """Panel with the controls specifics for an event transition"""
    
    def __init__(self, *args, **kwds):
        wx.Panel.__init__(self, *args, **kwds)
        self.transition_path = None
        self.event_ids = []
        
        self.label_event = wx.StaticText(self, -1, "Event:", style=wx.TE_RIGHT)
        self.choice_event = wx.Choice(self, -1)
        self.label_filter = wx.StaticText(self, -1, "Filter:", style=wx.TE_RIGHT)
        self.choice_filter = wx.Choice(self, -1)
        
        self.__set_properties()
        self.__do_layout()
        self.__bind_events()
    
    def __set_properties(self):
        format.align_labels( [self.label_event, self.label_filter] )
        self.__update_filter_list()
    
    def __do_layout(self):
        sizer_layout = wx.BoxSizer(wx.VERTICAL)
        sizer_event = wx.BoxSizer(wx.HORIZONTAL)
        sizer_filter = wx.BoxSizer(wx.HORIZONTAL)
        sizer_event.Add(self.label_event, 0, wx.ALIGN_CENTER_VERTICAL|wx.FIXED_MINSIZE, 0)
        sizer_event.Add(self.choice_event, 1, wx.LEFT|wx.ALIGN_CENTER_VERTICAL|wx.FIXED_MINSIZE, 5)
        sizer_layout.Add(sizer_event, 0, wx.EXPAND, 0)
        sizer_filter.Add(self.label_filter, 0, wx.ALIGN_CENTER_VERTICAL|wx.FIXED_MINSIZE, 0)
        sizer_filter.Add(self.choice_filter, 1, wx.LEFT|wx.ALIGN_CENTER_VERTICAL|wx.FIXED_MINSIZE, 5)
        sizer_layout.Add(sizer_filter, 0, wx.TOP|wx.EXPAND, 5)
        self.SetAutoLayout(True)
        self.SetSizer(sizer_layout)
        sizer_layout.Fit(self)
        sizer_layout.SetSizeHints(self)
        self.Layout()
    
    def __bind_events(self):
        self.Bind(wx.EVT_CHOICE, self.__on_update_condition, self.choice_event)
        self.Bind(wx.EVT_CHOICE, self.__on_update_condition, self.choice_filter)
    
    def __get_transition(self):
        return pynebula.lookup( self.transition_path )
    
    def __update_event_list(self):
        self.choice_event.Clear()
        self.event_ids = []
        if self.transition_path != None:
            cond = self.__get_transition().getcondition()
            cond_path = get_event_condition( cond.getevent() )
##            if cond_path == fsm.get_filteredcondition_of_transition( self.transition_path ):
##                cond_path = get_event_condition( pynebula.lookup(cond_path).getevent() )
            state_path = fsm.get_state_of_transition( self.transition_path )
            paths = get_free_event_conditions( state_path, cond_path )
            for path in paths:
                index = self.choice_event.Append( get_condition_gui_name(path) )
                self.event_ids.append( pynebula.lookup(path).getevent() )
                if path == cond_path:
                    self.choice_event.Select( index )
    
    def __update_filter_list(self):
        self.choice_filter.Clear()
        self.choice_filter.Append("<none>")
        scripts = fsm.get_script_conditions()
        for script in scripts:
            self.choice_filter.Append( format.get_name(script) )
        self.__select_filter()
    
    def __select_filter(self):
        if self.transition_path != None:
            cond_obj = self.__get_transition().getcondition()
            if cond_obj != None:
                filter_obj = cond_obj.getfiltercondition()
                if filter_obj == None:
                    name = "<none>"
                else:
                    name = filter_obj.getname()
                self.choice_filter.SetStringSelection( name )
    
    def __on_update_condition(self, event):
        # Remove transition from state
        transition = self.__get_transition()
        state_path = fsm.get_state_of_transition( self.transition_path )
        state = pynebula.lookup( state_path )
        state.removetransition( transition )
        # Get old event and filter
        old_cond_obj = transition.getcondition()
        old_event_id = old_cond_obj.getevent()
        old_filter_obj = old_cond_obj.getfiltercondition()
        # Get new event and filter
#        new_event_name = self.choice_event.GetStringSelection()
#        new_event_id = servers.get_fsm_server().geteventcondition( str(new_event_name) ).getevent()
        new_event_id = self.event_ids[ self.choice_event.GetSelection() ]
        new_filter_name = self.choice_filter.GetStringSelection()
        new_filter_obj = None
        if new_filter_name != "<none>":
            new_filter_obj = servers.get_fsm_server().getscriptcondition( str(new_filter_name) )
        # Act depending on filter condition existance and selection
        local_cond_path = fsm.get_filteredcondition_of_transition( self.transition_path )
        if old_filter_obj == None:
            if new_filter_obj == None:
                # Neither the old or the new conditions have a filter
                # Just select a shared condition
                new_cond_obj = get_event_condition( new_event_id ) #servers.get_fsm_server().geteventcondition( str(new_event_name) )
            else:
                # The old condition doesn't have a filter, but the new one does
                # Create a local condition, with the same event and new filter
                new_cond_obj = pynebula.new( 'neventcondition', local_cond_path )
                new_cond_obj.setevent( new_event_id )
                new_cond_obj.setfiltercondition( new_filter_obj )
        else:
            if new_filter_obj == None:
                # The old condition has a filter, but the new one doesn't
                # Remove the local condition and select a shared condition
                old_cond_obj = None
                pynebula.delete( str(local_cond_path) )
                new_cond_obj = get_event_condition( new_event_id ) #servers.get_fsm_server().geteventcondition( str(new_event_name) )
            else:
                # Both the old and the new conditions have a filter
                # Update the local condition with the new event
                new_cond_obj = old_cond_obj
                new_cond_obj.setevent( new_event_id )
        # Reinsert the transition into the state
        transition.setcondition( new_cond_obj )
        state.addtransition( transition )
        # Signal the change
        fsmevents.signal_fsm_change( self, self.transition_path )
    
    def on_new_transition(self):
        self.__update_event_list()
        self.__select_filter()
    
    def set_transition(self, transition_path):
        self.transition_path = transition_path
        self.__update_event_list()
        self.__select_filter()
    
    def quiet_refresh(self):
        self.__update_filter_list()


# ScriptTransitionPanel class
class ScriptTransitionPanel(wx.Panel):
    """Panel with the controls specifics for a script transition"""
    
    def __init__(self, *args, **kwds):
        wx.Panel.__init__(self, *args, **kwds)
        self.transition_path = None
        
        self.label_script = wx.StaticText(self, -1, "Script:")
        self.choice_script = wx.Choice(self, -1)
        
        self.__do_layout()
        self.__bind_events()
    
    def __do_layout(self):
        sizer_layout = wx.BoxSizer(wx.VERTICAL)
        sizer_script = wx.BoxSizer(wx.HORIZONTAL)
        sizer_script.Add(self.label_script, 0, wx.ALIGN_CENTER_VERTICAL|wx.FIXED_MINSIZE, 0)
        sizer_script.Add(self.choice_script, 1, wx.LEFT|wx.ALIGN_CENTER_VERTICAL|wx.FIXED_MINSIZE, 5)
        sizer_layout.Add(sizer_script, 0, wx.EXPAND, 0)
        self.SetAutoLayout(True)
        self.SetSizer(sizer_layout)
        sizer_layout.Fit(self)
        sizer_layout.SetSizeHints(self)
        self.Layout()
    
    def __bind_events(self):
        self.Bind(wx.EVT_CHOICE, self.__on_select_script, self.choice_script)
    
    def __update_script_list(self):
        self.choice_script.Clear()
        if self.transition_path != None:
            cond_path = pynebula.lookup( self.transition_path ).getcondition().getfullname()
            state_path = fsm.get_state_of_transition( self.transition_path )
            paths = get_free_script_conditions( state_path, cond_path )
            for path in paths:
                index = self.choice_script.Append( format.get_name(path) )
                if path == cond_path:
                    self.choice_script.Select( index )
    
    def __on_select_script(self, event):
        transition = pynebula.lookup( self.transition_path )
        state_path = fsm.get_state_of_transition( self.transition_path )
        state = pynebula.lookup( state_path )
        state.removetransition( transition )
        condition = servers.get_fsm_server().getscriptcondition(
            str(self.choice_script.GetStringSelection()) )
        transition.setcondition( condition )
        state.addtransition( transition )
        fsmevents.signal_fsm_change( self, self.transition_path )
    
    def on_new_transition(self):
        self.__update_script_list()
    
    def set_transition(self, transition_path):
        self.transition_path = transition_path
        self.__update_script_list()
    
    def quiet_refresh(self):
        self.__update_script_list()


# TransitionTypeCtrl class
class TransitionTypeCtrl(wx.Choicebook):
    """Choicebook with specific transition type controls"""
    
    def __init__(self, parent):
        wx.Choicebook.__init__(self, parent, -1)
        self.transition_path = None
        self.skip_change_type_event = False
        
        self.panel_event = EventTransitionPanel(self, -1)
        self.panel_script = ScriptTransitionPanel(self, -1)
        self.AddPage( self.panel_event, "Event transition" )
        self.AddPage( self.panel_script, "Script transition" )
        
        self.Bind(wx.EVT_CHOICEBOOK_PAGE_CHANGING, self.__on_changing_type, self)
        self.Bind(wx.EVT_CHOICEBOOK_PAGE_CHANGED, self.__on_change_type, self)
        self.Bind(events.EVT_CHANGED, self.__on_change_condition, self.panel_event)
        self.Bind(events.EVT_CHANGED, self.__on_change_condition, self.panel_script)
        self.Enable(False)
    
    def __get_transition(self):
        return pynebula.lookup( self.transition_path )
    
    def __get_transitions_path(self):
        return self.transition_path.rsplit('/',1)[0]
    
    def __get_transitions_obj(self):
        return pynebula.lookup( self.__get_transitions_path() )
    
    def __get_state_path(self):
        return self.transition_path.rsplit('/',2)[0]
    
    def __get_state(self):
        return pynebula.lookup( self.__get_state_path() )
    
    def __on_changing_type(self, event):
        if self.transition_path != None and not self.skip_change_type_event:
            # Forbid change type if there isn't any condition available
            # for the new transition type
            state_path = fsm.get_state_of_transition( self.transition_path )
            conds_paths = get_free_conditions( event.GetSelection(), state_path )
            if len(conds_paths) == 0:
                if event.GetSelection() == ID_EventTransition:
                    name1 = "an event"
                    name2 = "events"
                else:
                    name1 = "a script"
                    name2 = "condition scripts"
                cjr.show_error_message(
                    "Cannot transform this transition in " \
                    + name1 + " transition.\nThis state has already " \
                    "transitions for all the available " + name2 + "."
                    )
                event.Veto()
                return

    def __on_change_type(self, event):
        if self.transition_path != None and not self.skip_change_type_event:
            # Remove transition from state
            transition = self.__get_transition()
            state = self.__get_state()
            state.removetransition( transition )
            # Get free conditions for the selected transition type
            state_path = fsm.get_state_of_transition( self.transition_path )
            conds_paths = get_free_conditions( event.GetSelection(), state_path )
            # Delete condition if it's an event condition with filter
            cond_path = fsm.get_filteredcondition_of_transition( self.transition_path )
            try:
                pynebula.lookup( cond_path )
                pynebula.delete( cond_path )
            except:
                pass
            # Set first free condition for the selected transition type
            transition.setcondition( pynebula.lookup(conds_paths[0]) )
            # Add transition to state
            state.addtransition( transition )
            # Refresh condition parameters panel
            if self.GetSelection() == 0:
                self.panel_event.set_transition( self.transition_path )
            else:
                self.panel_script.set_transition( self.transition_path )
            # Signal change
            fsmevents.signal_fsm_change( self, self.transition_path )
    
    def __on_change_condition(self, event):
        fsmevents.signal_fsm_change( self, event.get_value() )
    
    def on_new_transition(self):
        self.GetPage( self.GetSelection() ).on_new_transition()
    
    def set_transition(self, transition_path):
        self.transition_path = transition_path
        if self.transition_path != None:
            transition = self.__get_transition()
            condition = transition.getcondition()
            self.skip_change_type_event = True
            if condition.isa("neventcondition"):
                self.SetSelection(ID_EventTransition)
                self.panel_event.set_transition( self.transition_path )
            else:
                self.SetSelection(ID_ScriptTransition)
                self.panel_script.set_transition( self.transition_path )
            self.skip_change_type_event = False
        self.Enable( self.transition_path != None )
    
    def quiet_refresh(self):
        self.GetPage( self.GetSelection() ).quiet_refresh()
