##\file fsmeditpanel.py
##\brief Scrolled window that contains the FSM edit controls

import wx

import pynebula

import app
import events
import foldctrl
import format
import fsm
import fsmevents
import fsmstates
import fsmstatetype
import fsmtransitions
import servers
import vtextctrl

import conjurerframework as cjr


# validate_fsm_name function
def validate_fsm_name(name):
    path = format.append_to_path( fsm.get_fsms_lib(), name )
    try:
        # Lookup will fail if it's a new name (name not found)
        pynebula.lookup( path )
        msg = "Another finite state machine called '%s' already exists." % name
        cjr.show_error_message(msg)
        return False
    except:
        msg = "Renaming this FSM will invalidate any references to it that " \
            "any agent has.\nAre you sure that you want to rename it?"
        result = cjr.confirm_yes_no(None, msg)
        return result == wx.ID_YES


# EditPanel class
class EditPanel(wx.PyScrolledWindow):
    """
    Scrolled window that contains the FSM edit controls
    
    The controls are vertically layout. The window doesn't show scrollbars,
    its scroll is managed externally (this makes easy to avoid the scrollbar
    overlapping the window contents).
    """
    
    def __init__(self, parent, fsm_path=None):
        wx.PyScrolledWindow.__init__(self, parent, style=wx.TAB_TRAVERSAL)
        self.fsm_path = fsm_path
        
        self.text_name = vtextctrl.ValidatedTextCtrl(self, -1, "Name:",
            validator=validate_fsm_name)
        self.foldbutton_states = foldctrl.FoldCtrl(self, "States")
        self.ctrl_states = fsmstates.StatesListCtrl(self)
        self.foldbutton_statetype = foldctrl.FoldCtrl(self, "State type")
        self.ctrl_statetype = fsmstatetype.StateTypeCtrl(self)
        self.foldbutton_transitions = foldctrl.FoldCtrl(self, "Transitions")
        self.ctrl_transitions = fsmtransitions.TransitionsCtrl(self)
#        self.foldbutton_emotactions = foldctrl.FoldCtrl(self, "Emotional actions")
#        self.ctrl_emotactions = wx.ListBox(self, -1)
#        self.ctrl_emotactions = fsmemotactions.EmotionalActionsListCtrl(self)
        
        self.__set_properties()
        self.__do_layout()
        self.__bind_events()
    
    def __set_properties(self):
        self.SetScrollRate(0,10)
        self.foldbutton_states.set_target_ctrl( self.ctrl_states )
        self.foldbutton_statetype.set_target_ctrl( self.ctrl_statetype )
        self.foldbutton_transitions.set_target_ctrl( self.ctrl_transitions )
#        self.foldbutton_emotactions.set_target_ctrl( self.ctrl_emotactions )
    
    def __do_layout(self):
        sizer_border = wx.BoxSizer(wx.VERTICAL)
        sizer_layout = wx.BoxSizer(wx.VERTICAL)
        sizer_layout.Add(self.text_name, 0, wx.EXPAND, 0)
        sizer_layout.Add(self.foldbutton_states, 0, wx.TOP|wx.EXPAND, 5)
        sizer_layout.Add(self.ctrl_states, 0, wx.TOP|wx.EXPAND, 5)
        sizer_layout.Add(self.foldbutton_statetype, 0, wx.TOP|wx.EXPAND, 5)
        sizer_layout.Add(self.ctrl_statetype, 0, wx.TOP|wx.EXPAND, 5)
        sizer_layout.Add(self.foldbutton_transitions, 0, wx.TOP|wx.EXPAND, 5)
        sizer_layout.Add(self.ctrl_transitions, 0, wx.TOP|wx.EXPAND, 5)
#        sizer_layout.Add(self.foldbutton_emotactions, 0, wx.TOP|wx.EXPAND, 5)
#        sizer_layout.Add(self.ctrl_emotactions, 0, wx.TOP|wx.EXPAND, 5)
        sizer_border.Add(sizer_layout, 1, wx.ALL|wx.EXPAND, 10)
        self.SetAutoLayout(True)
        self.SetSizer(sizer_border)
        sizer_border.Fit(self)
        sizer_border.SetVirtualSizeHints(self)
        self.Layout()
    
    def __bind_events(self):
        self.Bind(events.EVT_CHANGED, self.__on_change_name, self.text_name)
        self.Bind(fsmevents.EVT_SELECTION, self.__on_select_state, self.ctrl_states)
        self.Bind(events.EVT_CHANGED, self.__on_change_fsm, self.ctrl_states)
        self.Bind(events.EVT_CHANGED, self.__on_change_current_state_type, self.ctrl_statetype)
        self.Bind(events.EVT_CHANGED, self.__on_change_fsm, self.ctrl_transitions)
    
    def __on_change_name(self, event):
        # Change the FSM file and object names
        fsm = pynebula.lookup( self.fsm_path )
        old_name = str( format.get_name( self.fsm_path ) )
        new_name = str( self.text_name.get_value() )
        fsm.setname( new_name )
        servers.get_fsm_server().savefsm( fsm )
        fsm.setname( old_name )
        servers.get_fsm_server().erasefsm( fsm )
        fsm.setname( new_name )
        # Update the controls
        self.fsm_path = fsm.getfullname()
        self.ctrl_states.set_fsm( self.fsm_path )
        self.ctrl_statetype.set_state(None)
        self.ctrl_transitions.set_state(None)
#        self.ctrl_emotactions.set_state(None)
        # Notify change to update the name in the FSM library
        app.get_top_window(self).emit_app_event( events.FSMNameChanged(
            new_name=new_name, old_name=old_name ) )
        fsmevents.signal_fsm_change( self, fsmevents.ID_FSMNameChanged, self.fsm_path )
    
    def __on_select_state(self, event):
        self.ctrl_statetype.set_state( event.get_selection() )
        self.ctrl_transitions.set_state( event.get_selection() )
    
    def __on_change_fsm(self, event):
        fsmevents.signal_fsm_change( self )
        if event.get_value() != None:
            if event.get_value() == fsmevents.ID_NewState:
                self.ctrl_transitions.on_new_state()
    
    def __on_change_current_state_type(self, event):
        self.ctrl_states.refresh_current_state_name( event.get_value() )
        fsmevents.signal_fsm_change( self )
    
    def set_fsm(self, fsm_path):
        self.fsm_path = fsm_path
        if self.fsm_path is None:
            self.text_name.set_value("")
        else:
            self.text_name.set_value( format.get_name(self.fsm_path) )
        self.ctrl_states.set_fsm(fsm_path)
    
    def select_state(self, state_path):
        self.ctrl_states.select_state( state_path )
        self.ctrl_statetype.set_state( state_path )
        self.ctrl_transitions.set_state( state_path )
    
    def select_transition(self, transition_path, target_index):
        state_path = fsm.get_state_of_transition( transition_path )
        self.select_state( state_path )
        self.ctrl_transitions.select_transition( transition_path, target_index )
    
    def quiet_refresh(self):
        self.ctrl_statetype.quiet_refresh()
        self.ctrl_transitions.quiet_refresh()
