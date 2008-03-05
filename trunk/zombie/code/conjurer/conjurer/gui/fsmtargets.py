##\file fsmtargets.py
##\brief Control to manage and edit the targets of a transition

import wx

import pynebula

import events
import fsm
import fsmevents
import fsmstates


# get_free_targets function
def get_free_targets(transition, allowed_target=None):
    """
    Return a paths list of those states which the given transition doesn't
    already have a target for.
    
    \param transition Transition (path) to gather the free targets for
    \param allowed_target Target state (path) allowed as a free target, even if it's already taken
    \return List of states paths not found as targets for the given transition
    """
    fsm_path = fsm.get_fsm_of_transition( transition )
    states_paths = fsm.get_states( fsm_path )
    trans_obj = pynebula.lookup( transition )
    for i in range( trans_obj.gettargetsnumber() ):
        state_path = trans_obj.gettargetstatebyindex(i).getfullname()
        if allowed_target != state_path:
            states_paths.remove( state_path )
    return states_paths


# TargetsListCtrl class
class TargetsListCtrl(wx.PyControl):
    """Control to manage the targets of a transition"""
    
    def __init__(self, parent):
        wx.PyControl.__init__(self, parent, style=wx.NO_BORDER|wx.TAB_TRAVERSAL)
        self.transition_path = None
        
        self.list_targets = wx.ListCtrl(self, -1, style=wx.LC_REPORT|wx.LC_SINGLE_SEL)
        self.label_total = wx.StaticText(self, -1, "Total probability: 100%")
        self.button_new = wx.Button(self, -1, "New")
        self.button_delete = wx.Button(self, -1, "Delete")
        
        self.__set_properties()
        self.__do_layout()
        self.__bind_events()
    
    def __set_properties(self):
        self.list_targets.InsertColumn(0, 'State')
        self.list_targets.InsertColumn(1, 'Probability', wx.LIST_FORMAT_RIGHT, wx.LIST_AUTOSIZE_USEHEADER)
        self.Enable(False)
        self.button_delete.Enable(False)
    
    def __do_layout(self):
        sizer_layout = wx.BoxSizer(wx.VERTICAL)
        sizer_buttons = wx.BoxSizer(wx.HORIZONTAL)
        sizer_layout.Add(self.list_targets, 1, wx.EXPAND, 0)
        sizer_layout.Add(self.label_total, 0, wx.TOP|wx.ALIGN_CENTER_HORIZONTAL|wx.FIXED_MINSIZE, 5)
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
        self.Bind(wx.EVT_LIST_ITEM_SELECTED, self.__on_select_target, self.list_targets)
        self.Bind(wx.EVT_LIST_ITEM_DESELECTED, self.__on_select_target, self.list_targets)
        self.Bind(wx.EVT_BUTTON, self.__on_new_target, self.button_new)
        self.Bind(wx.EVT_BUTTON, self.__on_delete_target, self.button_delete)
    
    def __get_transition(self):
        return pynebula.lookup( self.transition_path )
    
    def __get_current_target(self):
        return self.list_targets.GetNextItem( -1, wx.LIST_NEXT_ALL,
            wx.LIST_STATE_SELECTED )
    
    def __on_size(self, event):
        # Needed to resize controls along with the window owning this control
        self.Layout()
        event.Skip()
    
    def __on_new_target(self, event):
        """Create a new target with default values"""
        # Create a transition to the first free target state by default
        state = pynebula.lookup( get_free_targets(self.transition_path)[0] )
        probability = 100
        self.__get_transition().addtarget( state, probability )
        index = self.list_targets.GetItemCount()
        gui_name = fsmstates.get_state_gui_name( state.getfullname() )
        self.list_targets.InsertStringItem( index, gui_name )
        self.list_targets.SetStringItem( index, 1, str(probability) + "%" )
        self.__update_total_probability()
        self.__update_new_button()
        fsmevents.signal_fsm_change(self, fsmevents.ID_NewTarget)
    
    def __on_delete_target(self, event):
        """Delete the current selected target"""
        target_index = self.__get_current_target()
        self.__get_transition().removetargetbyindex( target_index )
        self.list_targets.DeleteItem( target_index )
        self.__on_select_target(None)
        self.__update_total_probability()
        self.__update_new_button()
        fsmevents.signal_fsm_change(self)
    
    def __on_select_target(self, event):
        # Enable/disable controls that works with a selected target
        enable = self.list_targets.GetSelectedItemCount() > 0
        self.button_delete.Enable(enable)
        # Signal a target selection change
        self.GetEventHandler().ProcessEvent(
            fsmevents.SelectionEvent( self.GetId(),
                (self.transition_path, self.__get_current_target()), self )
            )
    
    def __update_total_probability(self):
        total = 0
        transition = self.__get_transition()
        for i in range( transition.gettargetsnumber() ):
            total = total + transition.gettargetprobabilitybyindex(i)
        self.label_total.SetLabel( "Total probability: " + str(total) + "%" )
    
    def __update_new_button(self):
        if self.transition_path != None:
            # Enable the new button when there's some state not chosen yet
            # as a target for this transition
            self.button_new.Enable(
                len( get_free_targets(self.transition_path) ) > 0
                )
    
    def on_new_state(self):
        self.__update_new_button()
    
    def set_transition(self, transition_path):
        self.transition_path = transition_path
        
        self.list_targets.DeleteAllItems()
        if transition_path != None:
            # Update the targets list
            transition = self.__get_transition()
            for i in range( transition.gettargetsnumber() ):
                state = transition.gettargetstatebyindex(i)
                gui_name = fsmstates.get_state_gui_name( state.getfullname() )
                probability = transition.gettargetprobabilitybyindex(i)
                self.list_targets.InsertStringItem( i, gui_name )
                self.list_targets.SetStringItem( i, 1, str(probability) + "%" )
            
            # Update the total probability
            self.__update_total_probability()
        
        # Enable/disable this control
        self.Enable( self.transition_path != None )
        self.__update_new_button()
        self.button_delete.Enable( False )
    
    def select_target(self, target_index):
        if target_index == None:
            target_index = wx.NOT_FOUND
        self.list_targets.Select( target_index )
        self.button_delete.Enable( target_index != wx.NOT_FOUND )
    
    def refresh_current_target(self, target_index):
        transition = self.__get_transition()
        state_gui_name = fsmstates.get_state_gui_name(
            transition.gettargetstatebyindex( target_index ).getfullname() )
        probability = transition.gettargetprobabilitybyindex( target_index )
        self.list_targets.SetStringItem( target_index, 0, state_gui_name )
        self.list_targets.SetStringItem( target_index, 1, str(probability) + "%" )
        self.__update_total_probability()


# TargetEditCtrl class
class TargetEditCtrl(wx.PyControl):
    """Control to edit a target of a transition"""
    
    def __init__(self, parent):
        wx.PyControl.__init__(self, parent, style=wx.NO_BORDER|wx.TAB_TRAVERSAL)
        self.transition_path = None
        self.target_index = wx.NOT_FOUND
        self.skip_fsm_change_event = False
        
        self.label_state = wx.StaticText(self, -1, "State:")
        self.choice_state = wx.Choice(self, -1, choices=[])
        self.label_probability = wx.StaticText(self, -1, "Probability:")
        self.spin_probability = wx.SpinCtrl(self, -1, "", min=0, max=100)
        
        self.__set_properties()
        self.__do_layout()
        self.__bind_events()
    
    def __set_properties(self):
        self.Enable(False)
    
    def __do_layout(self):
        sizer_layout = wx.BoxSizer(wx.VERTICAL)
        sizer_probability = wx.BoxSizer(wx.HORIZONTAL)
        sizer_state = wx.BoxSizer(wx.HORIZONTAL)
        sizer_state.Add(self.label_state, 0, wx.ALIGN_CENTER_VERTICAL|wx.FIXED_MINSIZE, 0)
        sizer_state.Add(self.choice_state, 1, wx.LEFT|wx.ALIGN_CENTER_VERTICAL|wx.FIXED_MINSIZE, 5)
        sizer_layout.Add(sizer_state, 0, wx.EXPAND, 0)
        sizer_probability.Add(self.label_probability, 0, wx.ALIGN_CENTER_VERTICAL|wx.FIXED_MINSIZE, 0)
        sizer_probability.Add(self.spin_probability, 1, wx.LEFT|wx.FIXED_MINSIZE, 5)
        sizer_layout.Add(sizer_probability, 0, wx.TOP|wx.EXPAND, 5)
        self.SetAutoLayout(True)
        self.SetSizer(sizer_layout)
        sizer_layout.Fit(self)
        sizer_layout.SetSizeHints(self)
        self.Layout()
    
    def __bind_events(self):
        self.Bind(wx.EVT_SIZE, self.__on_size)
        self.Bind(wx.EVT_CHOICE, self.__on_change_state, self.choice_state)
        self.Bind(wx.EVT_SPINCTRL, self.__on_change_probability, self.spin_probability)
        self.Bind(wx.EVT_TEXT, self.__on_change_probability, self.spin_probability)
    
    def __get_transition(self):
        return pynebula.lookup( self.transition_path )
    
    def __on_size(self, event):
        # Needed to resize controls along with the window owning this control
        self.Layout()
        event.Skip()
    
    def __on_change_state(self, event):
        state_path = fsm.find_path(
            fsm.get_fsm_of_transition( self.transition_path ),
            self.choice_state.GetStringSelection(),
            fsmstates.get_state_gui_name
            )
        if self.__get_transition().gettargetstatebyindex( self.target_index ).getfullname() != state_path:
            self.__get_transition().settargetstatebyindex(
                self.target_index, pynebula.lookup( state_path ) )
            fsmevents.signal_fsm_change(self, self.target_index)
    
    def __on_change_probability(self, event):
        self.__get_transition().settargetprobabilitybyindex(
            self.target_index, self.spin_probability.GetValue() )
        if not self.skip_fsm_change_event:
            fsmevents.signal_fsm_change(self, self.target_index)
    
    def __update_choice_state(self):
        # Get current target state path
        transition = self.__get_transition()
        target_state_path = transition.gettargetstatebyindex(
            self.target_index ).getfullname()
        
        # Fill the state choicer with all the states in the fsm available
        # for the target's transition and select the current target state
        self.choice_state.Clear()
        states_paths = get_free_targets( self.transition_path,
            target_state_path )
        for state_path in states_paths:
            gui_name = fsmstates.get_state_gui_name( state_path )
            index = self.choice_state.Append( gui_name )
            if state_path == target_state_path:
                self.choice_state.Select( index )
    
    def __update_spin_probability(self):
        probability = self.__get_transition().gettargetprobabilitybyindex(
            self.target_index )
        self.skip_fsm_change_event = True
        self.spin_probability.SetValue( probability )
        self.skip_fsm_change_event = False
    
    def on_new_state(self):
        if self.target_index != wx.NOT_FOUND:
            self.__update_choice_state()
    
    def on_new_target(self):
        if self.target_index != wx.NOT_FOUND:
            self.__update_choice_state()
    
    def set_target( self, target_info ):
        self.transition_path = target_info[0]
        self.target_index = target_info[1]
        
        # Update controls
        if self.target_index != wx.NOT_FOUND:
            self.__update_choice_state()
            self.__update_spin_probability()
        
        # Enable/Disable the whole control
        self.Enable( self.target_index != wx.NOT_FOUND )


# TargetsCtrl class
class TargetsCtrl(wx.PyControl):
    """Control to manage and edit the targets of a transition"""
    
    def __init__(self, parent):
        wx.PyControl.__init__(self, parent, style=wx.NO_BORDER|wx.TAB_TRAVERSAL)
        self.transition_path = None
        
        self.list_ctrl = TargetsListCtrl(self)
        self.edit_ctrl = TargetEditCtrl(self)
        
        self.__do_layout()
        self.__bind_events()
    
    def __do_layout(self):
        sizer_layout = wx.BoxSizer(wx.VERTICAL)
        sizer_layout.Add(self.list_ctrl, 1, wx.EXPAND, 0)
        sizer_layout.Add(self.edit_ctrl, 0, wx.TOP|wx.EXPAND, 5)
        self.SetAutoLayout(True)
        self.SetSizer(sizer_layout)
        sizer_layout.Fit(self)
        sizer_layout.SetSizeHints(self)
        self.Layout()
    
    def __bind_events(self):
        self.Bind(wx.EVT_SIZE, self.__on_size)
        self.Bind(fsmevents.EVT_SELECTION, self.__on_select_target, self.list_ctrl)
        self.Bind(events.EVT_CHANGED, self.__on_change_transition, self.list_ctrl)
        self.Bind(events.EVT_CHANGED, self.__on_change_current_target, self.edit_ctrl)
    
    def __on_size(self, event):
        # Needed to resize controls along with the window owning this control
        self.Layout()
        event.Skip()
    
    def __on_select_target(self, event):
        self.edit_ctrl.set_target( event.get_selection() )
    
    def __on_change_transition(self, event):
        fsmevents.signal_fsm_change(self)
        if event.get_value() != None:
            if event.get_value() == fsmevents.ID_NewTarget:
                self.edit_ctrl.on_new_target()
    
    def __on_change_current_target(self, event):
        self.list_ctrl.refresh_current_target( event.get_value() )
        fsmevents.signal_fsm_change(self)
    
    def on_new_state(self):
        self.list_ctrl.on_new_state()
        self.edit_ctrl.on_new_state()
    
    def set_transition(self, transition):
        self.list_ctrl.set_transition( transition )
        self.edit_ctrl.set_target( (None,wx.NOT_FOUND) )
    
    def select_target(self, target_index):
        self.list_ctrl.select_target( target_index )
        self.edit_ctrl.set_target( (self.list_ctrl.transition_path, target_index) )
