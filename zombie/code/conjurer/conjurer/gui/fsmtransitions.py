##\file fsmtransitions.py
##\brief Control to manage and edit the transitions of a state

import wx

import pynebula

import events
import format
import fsm
import fsmevents
import fsmtargets
import fsmtransitiontype

import conjurerframework as cjr


# get_transition_gui_name function
def get_transition_gui_name(transition_path):
    """Return the transition's name shown in the GUI"""
    condition = pynebula.lookup(transition_path).getcondition()
    if condition.isa('neventcondition'):
        type = 'Event'
    else:
        type = 'Script'
    if condition.getfullname() == fsm.get_filteredcondition_of_transition( transition_path ):
        # Find homologous shared event
        shared_cond = fsmtransitiontype.get_event_condition( condition.getevent() )
        value = fsmtransitiontype.get_condition_gui_name( shared_cond ) + "+"
    else:
        value = fsmtransitiontype.get_condition_gui_name( condition.getfullname() )
    return type + " " + value


# TransitionsListCtrl class
class TransitionsListCtrl(wx.PyControl):
    """Control to manage the transitions of a state"""
    
    def __init__(self, parent):
        wx.PyControl.__init__(self, parent, style=wx.NO_BORDER|wx.TAB_TRAVERSAL)
        self.state_path = None
        
        self.list_transitions = wx.ListBox(self, -1, style=wx.LB_SORT)
        self.button_new = wx.Button(self, -1, "New")
        self.button_delete = wx.Button(self, -1, "Delete")
        
        self.__set_properties()
        self.__do_layout()
        self.__bind_events()
    
    def __set_properties(self):
        self.Enable(False)
        self.button_delete.Enable(False)
    
    def __do_layout(self):
        sizer_layout = wx.BoxSizer(wx.VERTICAL)
        sizer_buttons = wx.BoxSizer(wx.HORIZONTAL)
        sizer_layout.Add(self.list_transitions, 1, wx.EXPAND, 0)
        sizer_buttons.Add(self.button_new, 1, wx.FIXED_MINSIZE, 0)
        sizer_buttons.Add(self.button_delete, 1, wx.LEFT|wx.FIXED_MINSIZE, 5)
        sizer_layout.Add(sizer_buttons, 0, wx.TOP|wx.EXPAND, 5)
        self.SetAutoLayout(True)
        self.SetSizer(sizer_layout)
        sizer_layout.Fit(self)
        sizer_layout.SetSizeHints(self)
        self.Layout()
    
    def __bind_events(self):
        self.Bind(wx.EVT_SIZE, self.__on_size)
        self.Bind(wx.EVT_LISTBOX, self.__on_select_transition, self.list_transitions)
        self.Bind(wx.EVT_BUTTON, self.__on_new_transition, self.button_new)
        self.Bind(wx.EVT_BUTTON, self.__on_delete_transition, self.button_delete)
    
    def __get_state(self):
        return pynebula.lookup( self.state_path )
    
    def __get_transitions_path(self):
        return format.append_to_path( self.state_path, 'transitions' )
    
    def __get_transitions_obj(self):
        try:
            return pynebula.lookup( self.__get_transitions_path() )
        except:
            return pynebula.new( 'nroot', self.__get_transitions_path() )
    
    def __get_current_transition_path(self):
        gui_name = self.list_transitions.GetStringSelection()
        if gui_name == "":
            return None
        else:
            return fsm.find_path( self.__get_transitions_path(), gui_name,
                get_transition_gui_name )
    
    def __get_current_transition(self):
        return pynebula.lookup( self.__get_current_transition_path() )
    
    def __on_size(self, event):
        # Needed to resize controls along with the window owning this control
        self.Layout()
        event.Skip()
    
    def __on_new_transition(self, event):
        """Create a new transition with default values"""
        # Get the first free condition by default
        cond_paths = fsmtransitiontype.get_free_event_conditions( self.state_path )
        if len(cond_paths) == 0:
            cond_paths = fsmtransitiontype.get_free_script_conditions( self.state_path )
            if len(cond_paths) == 0:
                cjr.show_information_message(
                    "This state already has transitions for all " \
                    "the available conditions."
                    )
                return
        condition = pynebula.lookup( cond_paths[0] )
        # Create the transition with a free name
        transition_path = fsm.get_free_name( self.__get_transitions_path(),
            'transition' )
        transition = pynebula.new( "ntransition", transition_path )
        transition.setcondition( condition )
        self.__get_state().addtransition( transition )
        self.list_transitions.Append( get_transition_gui_name( transition.getfullname() ) )
        self.__update_new_button()
        fsmevents.signal_fsm_change(self, fsmevents.ID_NewTransition)

    def __on_delete_transition(self, event):
        """Ask to confirm the transition deletion and delete it if so"""
        transition_name = self.list_transitions.GetStringSelection()
        if transition_name != "":
            msg = "Deleting a transition cannot be undone.\n"
            msg = msg + "Are you sure that you want to delete the "
            msg = msg + "'" + transition_name + "' transition?"
            result = cjr.warn_yes_no(self, msg)
            if dlg.ShowModal() == wx.ID_YES:
                transition = self.__get_current_transition()
                self.__get_state().deletetransition( transition )
                transition = None
                self.list_transitions.Delete(
                    self.list_transitions.GetSelection() )
                self.__on_select_transition(None)
                self.__update_new_button()
                fsmevents.signal_fsm_change(self)

    def __on_select_transition(self, event):
        # Enable/disable controls that works with a selected transition
        enable = self.list_transitions.GetSelection() != wx.NOT_FOUND
        self.button_delete.Enable(enable)
        # Signal a transition selection change
        self.GetEventHandler().ProcessEvent(
            fsmevents.SelectionEvent( self.GetId(),
                self.__get_current_transition_path(), self )
            )

    def __update_new_button(self):
        if self.state_path != None:
            # Enable the new button when there's some transition condition not
            # chosen yet for this state
            self.button_new.Enable(
                len( fsmtransitiontype.get_free_event_conditions(self.state_path) ) + \
                len( fsmtransitiontype.get_free_script_conditions(self.state_path) ) > 0
                )

    def set_state(self, state_path):
        self.state_path = state_path

        # Update the transitions list
        self.list_transitions.Clear()
        if state_path != None:
            transition = self.__get_transitions_obj().gethead()
            while transition is not None:
                self.list_transitions.Append( get_transition_gui_name(
                    transition.getfullname() ) )
                transition = transition.getsucc()

        # Enable/disable this control
        self.Enable( self.state_path != None )
        self.__update_new_button()
        self.button_delete.Enable( False )

    def select_transition(self, transition_path):
        gui_name = get_transition_gui_name( transition_path )
        self.list_transitions.SetStringSelection( gui_name )
        self.button_delete.Enable( True )

    def refresh_current_transition_name(self, transition_path):
        self.list_transitions.SetString( self.list_transitions.GetSelection(),
            get_transition_gui_name( transition_path ) )


# TransitionsCtrl class
class TransitionsCtrl(wx.PyControl):
    """Control to manage and edit the transitions of a state"""

    def __init__(self, parent):
        wx.PyControl.__init__(self, parent, style=wx.NO_BORDER|wx.TAB_TRAVERSAL)

        self.ctrl_list = TransitionsListCtrl(self)
        self.staticbox_type = wx.StaticBox(self, -1, "Transition type")
        self.ctrl_type = fsmtransitiontype.TransitionTypeCtrl(self)
        self.staticbox_targets = wx.StaticBox(self, -1, "Target states")
        self.ctrl_targets = fsmtargets.TargetsCtrl(self)

        self.__do_layout()
        self.__bind_events()

    def __do_layout(self):
        sizer_layout = wx.BoxSizer(wx.VERTICAL)
        sizer_type = wx.StaticBoxSizer(self.staticbox_type, wx.VERTICAL)
        sizer_targets = wx.StaticBoxSizer(self.staticbox_targets, wx.VERTICAL)
        sizer_layout.Add(self.ctrl_list, 0, wx.EXPAND, 0)
        sizer_type.Add(self.ctrl_type, 0, wx.ALL|wx.EXPAND, 5)
        sizer_layout.Add(sizer_type, 0, wx.TOP|wx.EXPAND, 5)
        sizer_targets.Add(self.ctrl_targets, 1, wx.ALL|wx.EXPAND, 5)
        sizer_layout.Add(sizer_targets, 1, wx.TOP|wx.EXPAND, 5)
        self.SetAutoLayout(True)
        self.SetSizer(sizer_layout)
        sizer_layout.Fit(self)
        sizer_layout.SetSizeHints(self)
        self.Layout()

    def __bind_events(self):
        self.Bind(wx.EVT_SIZE, self.__on_size)
        self.Bind(fsmevents.EVT_SELECTION, self.__on_select_transition, self.ctrl_list)
        self.Bind(events.EVT_CHANGED, self.__on_change_state, self.ctrl_list)
        self.Bind(events.EVT_CHANGED, self.__on_change_current_transition_type, self.ctrl_type)
        self.Bind(events.EVT_CHANGED, self.__on_change_state, self.ctrl_targets)

    def __on_size(self, event):
        # Needed to resize controls along with the window owning this control
        self.Layout()
        event.Skip()

    def __on_select_transition(self, event):
        self.ctrl_type.set_transition( event.get_selection() )
        self.ctrl_targets.set_transition( event.get_selection() )

    def __on_change_state(self, event):
        fsmevents.signal_fsm_change(self)
        if event.get_value() != None:
            if event.get_value() == fsmevents.ID_NewTransition:
                self.ctrl_type.on_new_transition()

    def __on_change_current_transition_type(self, event):
        self.ctrl_list.refresh_current_transition_name( event.get_value() )
        fsmevents.signal_fsm_change(self)

    def on_new_state(self):
        self.ctrl_targets.on_new_state()

    def set_state(self, state):
        self.ctrl_list.set_state( state )
        self.ctrl_type.set_transition( None )
        self.ctrl_targets.set_transition( None )

    def select_transition(self, transition_path, target_index):
        self.ctrl_list.select_transition( transition_path )
        self.ctrl_type.set_transition( transition_path )
        self.ctrl_targets.set_transition( transition_path )
        self.ctrl_targets.select_target( target_index )

    def quiet_refresh(self):
        self.ctrl_type.quiet_refresh()
