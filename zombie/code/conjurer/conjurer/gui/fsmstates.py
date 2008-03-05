##\file fsmstates.py
##\brief Control to manage the states of a FSM

import wx

import pynebula

import fsm
import fsmaction
import fsmevents
import fsmstatetype

import conjurerframework as cjr

# get_state_gui_name function
def get_state_gui_name(state_path):
    """Return the state's name shown in the GUI"""
    state = pynebula.lookup(state_path)
    if state.isa('nnodestate'):
#        brief =  'Node state'
        brief = fsmstatetype.get_selector_gui_name( state.getfsmselector().getfullname() )
    elif state.isa('nleafstate'):
#        brief = 'Leaf state'
        brief = fsmaction.get_action_gui_name( state.getbehaviouralaction().getfullname() )
    else:
        brief = 'End state'
    return fsm.get_index(state_path, 'state') + ": " + brief


# get_index function
##def get_index(state_gui_name):
##    """Return the index associated to a state's GUI name"""
##    return state_gui_name.split(':',1)[0]


# StatesListCtrl class
class StatesListCtrl(wx.PyControl):
    """
    Control to manage the states of a FSM.
    
    Events thrown:
    
        - events.EVT_CHANGED: Whenever a change in the FSM is made (when a
          state is added/deleted or set as the FSM's initial state).
        - wx.EVT_LISTBOX: Whenever a state is selected/deselected.
    """
    
    def __init__(self, parent):
        wx.PyControl.__init__(self, parent, style=wx.NO_BORDER|wx.TAB_TRAVERSAL)
        self.fsm_path = None
        
        self.list_states = wx.ListBox(self, -1, style=wx.LB_SORT)
        self.button_new = wx.Button(self, -1, "New")
        self.button_delete = wx.Button(self, -1, "Delete")
        self.button_initial_state = wx.Button(self, -1, "Set as initial state")
        
        self.__set_properties()
        self.__do_layout()
        self.__bind_events()
    
    def __set_properties(self):
        self.__on_select_state(None)
    
    def __do_layout(self):
        sizer_layout = wx.BoxSizer(wx.VERTICAL)
        sizer_buttons = wx.BoxSizer(wx.HORIZONTAL)
        sizer_layout.Add(self.list_states, 1, wx.EXPAND, 0)
        sizer_buttons.Add(self.button_new, 1, wx.FIXED_MINSIZE, 0)
        sizer_buttons.Add(self.button_delete, 1, wx.LEFT|wx.FIXED_MINSIZE, 5)
        sizer_layout.Add(sizer_buttons, 0, wx.TOP|wx.EXPAND, 5)
        sizer_layout.Add(self.button_initial_state, 0, wx.TOP|wx.EXPAND, 5)
        self.SetAutoLayout(True)
        self.SetSizer(sizer_layout)
        sizer_layout.Fit(self)
        sizer_layout.SetSizeHints(self)
        self.Layout()
    
    def __bind_events(self):
        self.Bind(wx.EVT_SIZE, self.__on_size)
        self.Bind(wx.EVT_LISTBOX, self.__on_select_state, self.list_states)
        self.Bind(wx.EVT_BUTTON, self.__on_new_state, self.button_new)
        self.Bind(wx.EVT_BUTTON, self.__on_delete_state, self.button_delete)
        self.Bind(wx.EVT_BUTTON, self.__on_set_initial_state, self.button_initial_state)
    
    def __get_fsm(self):
        return pynebula.lookup( self.fsm_path )
    
    def __get_current_state_path(self):
        gui_name = self.list_states.GetStringSelection()
        if gui_name == "":
            return None
        else:
            return fsm.find_path( self.fsm_path, gui_name, get_state_gui_name )
##            return fsm.get_path( self.fsm_path, 'state', get_index(gui_name) )
    
    def __get_current_state(self):
        return pynebula.lookup( self.__get_current_state_path() )

    def __on_size(self, event):
        # Needed to resize controls along with the window owning this control
        self.Layout()
        event.Skip()

    def __on_new_state(self, event):
        """Create a new state with default values"""
        state_path = fsm.get_free_name( self.fsm_path, 'state' )
        state = fsmstatetype.create_default_state( state_path )
        if state != None:
            self.__get_fsm().addstate( state )
            self.list_states.Append( get_state_gui_name(state_path) )
            fsmevents.signal_fsm_change(self, fsmevents.ID_NewState)

    def __on_delete_state(self, event):
        """Ask to confirm the state deletion and delete it if so"""
        state_name = self.list_states.GetStringSelection()
        if state_name != "":
            msg = "Deleting a state cannot be undone.\n"
            msg = msg + "Are you sure that you want to delete the "
            msg = msg + "'" + state_name + "' state?"
            result = cjr.warn_yes_no(self, msg)
            if result == wx.ID_YES:
                # Delete all transition targets pointing to the state to be deleted
                state = self.__get_current_state()
                fsm_ = fsm.get_fsm_of_state( state.getfullname() )
                states = fsm.get_states( fsm_ )
                for s in states:
                    transitions = fsm.get_transitions(s)
                    for t in transitions:
                        t_obj = pynebula.lookup(t)
                        t_obj.removetarget(state)
                        # Delete the transition if it's emmpty after deleting the target
                        if t_obj.gettargetsnumber() == 0:
                            s_obj = pynebula.lookup(s)
                            s_obj.deletetransition(t_obj)
                            t_obj = None
                
                # Delete the state
                self.__get_fsm().deletestate( state )
                state = None
                self.list_states.Delete( self.list_states.GetSelection() )
                self.__on_select_state(None)
                fsmevents.signal_fsm_change(self)

    def __on_select_state(self, event):
        # Enable/disable controls that works with a selected state
        enable = self.list_states.GetSelection() != wx.NOT_FOUND
        self.button_delete.Enable(enable)
        self.button_initial_state.Enable(enable)
        # Signal a state selection change
        self.GetEventHandler().ProcessEvent(
            fsmevents.SelectionEvent( self.GetId(),
                self.__get_current_state_path(), self )
            )

    def __on_set_initial_state(self, event):
        state = self.__get_current_state()
        self.__get_fsm().setinitialstate( state )
        fsmevents.signal_fsm_change(self)

    def set_fsm(self, fsm_path):
        self.fsm_path = fsm_path

        # Update the state list
        self.list_states.Clear()
        state = self.__get_fsm().gethead()
        while state is not None:
            self.list_states.Append( get_state_gui_name( state.getfullname() ) )
            state = state.getsucc()
    
    def select_state(self, state_path):
        gui_name = get_state_gui_name( state_path )
        self.list_states.SetStringSelection( gui_name )
        self.button_delete.Enable( True )
        self.button_initial_state.Enable( True )
    
    def refresh_current_state_name(self, state_path):
        self.list_states.SetString( self.list_states.GetSelection(),
            get_state_gui_name( state_path ) )
##        index = get_index( self.list_states.GetStringSelection() )
##        path = fsm.get_path( self.fsm_path, 'state', index )
##        gui_name = get_state_gui_name( path )
##        self.list_states.SetString( self.list_states.GetSelection(), gui_name )
