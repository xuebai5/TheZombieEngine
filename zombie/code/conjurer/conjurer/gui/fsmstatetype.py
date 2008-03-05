##\file fsmstatetype.py
##\brief Control to edit the type of a state and its type dependend fields

import wx

import pynebula

import events
import format
import fsm
import fsmaction
import fsmevents

import conjurerframework as cjr

# get_selector_gui_name function
def get_selector_gui_name( selector_path ):
    return pynebula.lookup( selector_path ).getname()


# create_default_state function
def create_default_state( path, type = 'nleafstate' ):
    """Create a state with default parameters"""
    state = pynebula.new( type, path )
    if type == 'nleafstate':
        action_path = fsm.get_behaction_of_state( path )
        action = fsmaction.create_default_action( action_path )
        if action != None:
            state.setbehaviouralaction( action )
        else:
            state = None
            pynebula.delete( path )
            cjr.show_error_message(
                "There isn't any action available.\n" \
                "Couldn't create a default leaf state."
                )
    elif type == 'nnodestate':
        selector_list = fsm.get_fsm_selectors()
        if len(selector_list) > 0:
            state.setfsmselectorbyname( str(selector_list[0]) )
        else:
            state = None
            pynebula.delete( path )
            cjr.show_error_message(
                "There isn't any FSM selection script available.\n" \
                "Couldn't create a default node state."
                )
    return state


# NodeStatePanel class
class NodeStatePanel(wx.Panel):
    """Panel with the controls specifics for a node state"""
    
    def __init__(self, *args, **kwds):
        wx.Panel.__init__(self, *args, **kwds)
        self.state_path = None
        
        self.label_selector = wx.StaticText(self, -1, "FSM selector:")
        self.choice_selector = wx.Choice(self, -1)
        
        self.__do_layout()
        self.__bind_events()
    
    def __do_layout(self):
        sizer_layout = wx.BoxSizer(wx.VERTICAL)
        sizer_selector = wx.BoxSizer(wx.HORIZONTAL)
        sizer_selector.Add(self.label_selector, 0, wx.ALIGN_CENTER_VERTICAL|wx.FIXED_MINSIZE, 0)
        sizer_selector.Add(self.choice_selector, 1, wx.LEFT|wx.ALIGN_CENTER_VERTICAL|wx.FIXED_MINSIZE, 5)
        sizer_layout.Add(sizer_selector, 0, wx.EXPAND, 0)
        self.SetAutoLayout(True)
        self.SetSizer(sizer_layout)
        sizer_layout.Fit(self)
        sizer_layout.SetSizeHints(self)
        self.Layout()
    
    def __bind_events(self):
        self.Bind(wx.EVT_CHOICE, self.__on_choice_selector, self.choice_selector)
    
    def __get_state(self):
        return pynebula.lookup( self.state_path )
    
    def __on_choice_selector(self, event):
        selector_name = str( self.choice_selector.GetStringSelection() )
        self.__get_state().setfsmselectorbyname( str(selector_name) )
        fsmevents.signal_fsm_change( self, self.state_path )
    
    def __update_choice_selector(self):
        # Fill choice control
        self.choice_selector.Clear()
        selector_list = fsm.get_fsm_selectors()
        selector_list.sort()
        self.choice_selector.AppendItems( selector_list )
        # Select current selector
        current_selector_name = self.__get_state().getfsmselector().getname()
        self.choice_selector.SetStringSelection( current_selector_name )
    
    def set_state(self, state_path):
        self.state_path = state_path
        if self.state_path != None:
            self.__update_choice_selector()
    
    def quiet_refresh(self):
        if self.state_path != None:
            self.__update_choice_selector()


# LeafStatePanel class
class LeafStatePanel(wx.Panel):
    """Panel with the controls specifics for a leaf state"""
    
    def __init__(self, *args, **kwds):
        wx.Panel.__init__(self, *args, **kwds)
        self.state_path = None
        
        self.action = fsmaction.ActionCtrl(self, -1, "Behavioural action")
        
        self.__do_layout()
        self.__bind_events()
    
    def __do_layout(self):
        sizer = wx.BoxSizer(wx.VERTICAL)
        sizer.Add(self.action, 0, wx.EXPAND, 0)
        self.SetAutoLayout(True)
        self.SetSizer(sizer)
        sizer.Fit(self)
        sizer.SetSizeHints(self)
        self.Layout()
    
    def __bind_events(self):
        self.Bind(events.EVT_CHANGED, self.__on_change_action, self.action)
    
    def __on_change_action(self, event):
        fsmevents.signal_fsm_change( self, self.state_path )
    
    def set_state(self, state_path):
        self.state_path = state_path
        if self.state_path != None:
            self.action.set_action( fsm.get_behaction_of_state(self.state_path) )
    
    def quiet_refresh(self):
        if self.state_path != None:
            self.action.quiet_refresh()


# StateTypeCtrl class
class StateTypeCtrl(wx.Choicebook):
    """Choicebook with specific state type controls"""
    
    def __init__(self, parent):
        wx.Choicebook.__init__(self, parent, -1)
        self.state_path = None
        self.skip_change_type_event = False
        
        self.panel_node = NodeStatePanel(self, -1)
        self.panel_leaf = LeafStatePanel(self, -1)
        self.panel_end = wx.Panel(self, -1)
        self.AddPage( self.panel_node, "Node state" )
        self.AddPage( self.panel_leaf, "Leaf state" )
        self.AddPage( self.panel_end, "End state" )
        self.ID_NodeStatePanel = 0
        self.ID_LeafStatePanel = 1
        self.ID_EndStatePanel = 2
        
        self.Bind(wx.EVT_CHOICEBOOK_PAGE_CHANGED, self.__on_change_type, self)
        self.Bind(events.EVT_CHANGED, self.__on_change_state, self.panel_node)
        self.Bind(events.EVT_CHANGED, self.__on_change_state, self.panel_leaf)
        self.Enable(False)
    
    def __get_state(self):
        return pynebula.lookup( self.state_path )
    
    def __get_fsm(self):
        return pynebula.lookup( fsm.get_fsm_of_state( self.state_path ) )
    
    def __on_change_type(self, event):
        if self.state_path != None and not self.skip_change_type_event:
            # Rename the old state to keep it while building the new one
            old_state = self.__get_state()
            old_state.setname( '_tmp' )
            
            # Create new state for the new state type
            if self.GetSelection() == self.ID_NodeStatePanel:
                new_state = create_default_state( self.state_path, 'nnodestate' )
            elif self.GetSelection() == self.ID_LeafStatePanel:
                new_state = create_default_state( self.state_path, 'nleafstate' )
            else:
                new_state = create_default_state( self.state_path, 'nendstate' )
            if new_state == None:
                # If creating a default state fails abort and restore old state
                old_state.setname( str( format.get_name(self.state_path) ) )
                self.skip_change_type_event = True
                self.SetSelection( event.GetOldSelection() )
                self.skip_change_type_event = False
                return
            fsm_ = self.__get_fsm()
            fsm_.addstate( new_state )
            
            # Copy info that's independent of the state type
            old_trans_path = fsm.get_transitions_dir_of_state(
                old_state.getfullname() )
            new_trans_path = fsm.get_transitions_dir_of_state(
                new_state.getfullname(), False )
            pynebula.lookup( str(old_trans_path) ).clone( str(new_trans_path) )
            transitions = fsm.get_transitions( new_state.getfullname() )
            for t in transitions:
                t_obj = pynebula.lookup(t)
                new_state.addtransition( t_obj )
            if fsm_.getinitialstate() == old_state:
                fsm_.setinitialstate( new_state )
            
            # Update all transitions to point to the new state if pointing the old one
            states = fsm.get_states( fsm_.getfullname() )
            for s in states:
                transitions = fsm.get_transitions( s )
                for t in transitions:
                    t_obj = pynebula.lookup(t)
                    for i in range( t_obj.gettargetsnumber() ):
                        if t_obj.gettargetstatebyindex(i) == old_state:
                            t_obj.settargetstatebyindex(i, new_state)
            
            # Delete old state
            fsm_.deletestate( old_state )
            old_state = None # Python, release that #$%&/)?! reference NOW!
            
            # Update type specific controls
            self.__update_controls()
            
            # Signal change
            fsmevents.signal_fsm_change(self, self.state_path)
    
    def __on_change_state(self, event):
        fsmevents.signal_fsm_change( self, event.get_value() )
    
    def __update_controls(self):
        if self.state_path != None:
            state = self.__get_state()
            # Update selected panel
            if self.GetSelection() == self.ID_NodeStatePanel:
                self.panel_node.set_state( self.state_path )
            elif self.GetSelection() == self.ID_LeafStatePanel:
                self.panel_leaf.set_state( self.state_path )
        # Enable the whole control when there's some state to edit
        self.Enable( self.state_path != None )
    
    def set_state(self, state_path):
        self.state_path = state_path
        if state_path != None:
            state = self.__get_state()
            self.skip_change_type_event = True
            if state.isa("nnodestate"):
                self.SetSelection(self.ID_NodeStatePanel)
            elif state.isa("nleafstate"):
                self.SetSelection(self.ID_LeafStatePanel)
            else:
                self.SetSelection(self.ID_EndStatePanel)
            self.skip_change_type_event = False
        self.__update_controls()
    
    def quiet_refresh(self):
        if self.state_path != None:
            # Update selected panel
            if self.GetSelection() == self.ID_NodeStatePanel:
                self.panel_node.quiet_refresh()
            elif self.GetSelection() == self.ID_LeafStatePanel:
                self.panel_leaf.quiet_refresh()
