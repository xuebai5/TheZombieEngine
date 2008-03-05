##\file fsmevents.py
##\brief Events used commonly by all FSM related controls

import wx

import events


# Event used to notify item selections
EVT_SELECTION_TYPE = wx.NewEventType()
EVT_SELECTION = wx.PyEventBinder(EVT_SELECTION_TYPE, 1)

# SelectionEvent class
class SelectionEvent(wx.PyCommandEvent):
    """Event class for an EVT_SELECTION event"""
    
    def __init__(self, id, value, object):
        wx.PyCommandEvent.__init__(self, EVT_SELECTION_TYPE, id)
        self.value = value
        self.SetEventObject(object)
    
    def get_selection(self):
        """Retrieve the selection value associated to this event"""
        return self.value


# Ids to indicate the cause of a fsm change event
ID_NewState = 1
ID_NewTransition = 2
ID_NewTarget = 3
ID_FSMNameChanged = 4

# signal_fsm_change function
def signal_fsm_change(emitter, value=None, oldvalue=None):
    """Throw an events.EVT_CHANGED event coming from the given object"""
    emitter.GetEventHandler().ProcessEvent(
        events.ChangedEvent( emitter.GetId(), value, oldvalue, emitter )
        )
