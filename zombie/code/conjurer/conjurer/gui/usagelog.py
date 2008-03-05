##\file usagelog.py
##\brief Dialogs and functions to report who is referencing some object

import wx

import pynebula

import childdialoggui
import fsm
import fsmstates
import fsmtransitions

import conjurerconfig as cfg


# find_behaviour_action function
def find_behaviour_action(action_class):
    columns = ['FSM', 'State']
    rows = []
    
    # Look for the action in all the FSMs
    fsm_paths = fsm.get_fsms()
    for fsm_path in fsm_paths:
        state_mc = pynebula.lookup( fsm_path )
        
        # Look for the action in all the leaf nodes of the FSM
        state_paths = fsm.get_states( state_mc.getfullname() )
        for state_path in state_paths:
            state = pynebula.lookup( state_path )
            if state.isa('nleafstate'):
                action = state.getbehaviouralaction()
                if action.getactionclass() == action_class:
                    rows.append( [
                        state_mc.getname(),
                        fsmstates.get_state_gui_name( state_path )
                        ] )
    
    return (columns, rows)


# find_transition_condition function
def find_transition_condition(condition_class):
    columns = ['FSM', 'State', 'Transition']
    rows = []
    
    # Look for the condition in all the FSMs
    fsm_paths = fsm.get_fsms()
    for fsm_path in fsm_paths:
        state_mc = pynebula.lookup( fsm_path )
        
        # Look for the condition in all the script transitions of 
        # the FSM (and event filters)
        state_paths = fsm.get_states( state_mc.getfullname() )
        for state_path in state_paths:
            trans_paths = fsm.get_transitions( state_path )
            for trans_path in trans_paths:
                trans = pynebula.lookup( trans_path )
                trans_cond = trans.getcondition()
                if trans_cond.isa( str(condition_class) ):
                    rows.append( [
                        state_mc.getname(),
                        fsmstates.get_state_gui_name(
                            state_path 
                            ),
                        fsmtransitions.get_transition_gui_name(
                            trans_path 
                            )
                        ] )
                elif trans_cond.isa('neventcondition'):
                    filter_cond = trans_cond.getfiltercondition()
                    if filter_cond is not None:
                        if filter_cond.isa( str(condition_class) ):
                            rows.append( [
                                state_mc.getname(),
                                fsmstates.get_state_gui_name(
                                    state_path 
                                    ),
                                fsmtransitions.get_transition_gui_name(
                                    trans_path 
                                    )
                                ] )
    
    return (columns, rows)


# find_fsm_selector function
def find_fsm_selector(selector_class):
    columns = ['FSM', 'State']
    rows = []
    
    # Look for the selector in all the FSMs
    fsm_paths = fsm.get_fsms()
    for fsm_path in fsm_paths:
        state_mc = pynebula.lookup( fsm_path )
        
        # Look for the selector in all the state nodes of the FSM
        state_paths = fsm.get_states( state_mc.getfullname() )
        for state_path in state_paths:
            state = pynebula.lookup( state_path )
            if state.isa('nnodestate'):
                selector = state.getfsmselector()
                if selector.isa( str(selector_class) ):
                    rows.append( [
                        state_mc.getname(),
                        fsmstates.get_state_gui_name( state_path )
                        ] )
    
    return (columns, rows)


# UsageLogDialog
class UsageLogDialog(childdialoggui.childDialogGUI):
    """Show the objects that have some reference to another specific object"""
    
    def __init__(self, parent, caption, message, log):
        childdialoggui.childDialogGUI.__init__( self, caption, parent )
        
        self.label = wx.StaticText(self, -1, message)
        self.list = wx.ListCtrl(self, -1, style=wx.LC_REPORT)
        
        for i in range( len( log[0] ) ):
            self.list.InsertColumn(i, log[0][i])
        for row in log[1]:
            self.list.Append( row )
        
        self.__do_layout()
    
    def __do_layout(self):
        sizer = wx.BoxSizer(wx.VERTICAL)
        sizer_list = wx.BoxSizer(wx.VERTICAL)
        sizer_list.Add(
            self.label,
            0, 
            wx.FIXED_MINSIZE
            )
        sizer_list.Add(
            self.list,
            1, 
            wx.TOP|wx.EXPAND, 
            cfg.BORDER_WIDTH
            )
        sizer.Add(
            sizer_list,
            1, 
            wx.ALL|wx.EXPAND, 
            cfg.BORDER_WIDTH
            )
        self.SetSizerAndFit(sizer)


# DeleteErrorDialog
class DeleteErrorDialog(UsageLogDialog):
    """Show the objects that have caused a deletion error"""

    def __init__(self, parent, name, log):
        caption = "Error deleting the %s" % name
        message = "Unable to delete the %s because it is " \
                        "still being used by: " % name
        UsageLogDialog.__init__( self, parent, caption, message, log )
        
        wx.Bell()
