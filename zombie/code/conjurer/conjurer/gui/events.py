##\file events.py
##\brief Events used commonly by all custom controls
##
## Two events are provided. Any control that uses them is expected to throw
## them when its value changes, but one is thrown when the value is expected
## to continue changing soon, while the other not. This is intented to avoid
## generating multiple undo commands for small changes, which will be annoying
## for a user. The idea is that a dialog only generates an undo command when
## a 'changed' event is thrown, while making direct changes to the
## scene when receiving 'changing' events.
##
## An example is a user dragging a slider's thumb. While dragging the thumb
## 'changing' events are thrown, but when releasing the thumb a 'changed' event
## is thrown.

import wx


#------------------------------------------------------------------------------
# Event thrown while the value of a control is changing and it's expected to
# continue to do so soon
EVT_CHANGING_TYPE = wx.NewEventType()
EVT_CHANGING = wx.PyEventBinder(EVT_CHANGING_TYPE, 1)

# ChangingEvent class
class ChangingEvent(wx.PyCommandEvent):
    """Event class for an EVT_CHANGING event"""
    
    def __init__(self, id, value, object):
        wx.PyCommandEvent.__init__(self, EVT_CHANGING_TYPE, id)
        self.value = value
        self.SetEventObject(object)
    
    def get_value(self):
        """Retrieve the value of the control at the time this event was generated"""
        return self.value


#------------------------------------------------------------------------------
# Event thrown when the value of a control is changed and it isn't expected to
# continue to do so soon
EVT_CHANGED_TYPE = wx.NewEventType()
EVT_CHANGED = wx.PyEventBinder(EVT_CHANGED_TYPE, 1)

# ChangedEvent class
class ChangedEvent(wx.PyCommandEvent):
    """Event class for an EVT_CHANGED event"""
    
    def __init__(self, id, new_value, old_value, object):
        wx.PyCommandEvent.__init__(self, EVT_CHANGED_TYPE, id)
        self.new_value = new_value
        self.old_value = old_value
        self.SetEventObject(object)
    
    def get_value(self):
        """Retrieve the value of the control at the time this event was generated"""
        return self.new_value
    
    def get_old_value(self):
        """
        Retrieve the value of the control just before the last changes happened
        
        'Last changes' mean the last 'changing' + 'changed' event sequence.
        
        This value is needed by the undo command, or otherwise the dialog must
        keep track of the old value by itself.
        """
        return self.old_value


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

EVT_RIGHT_CLICK_TYPE = wx.NewEventType()
EVT_RIGHT_CLICK = wx.PyEventBinder(EVT_RIGHT_CLICK_TYPE, 1)

# ChangedEvent class
class RightClickEvent(wx.PyCommandEvent):
    """Event class for an EVT_RIGHT_CLICK event"""
    
    def __init__(self, id, value, object):
        wx.PyCommandEvent.__init__(self, EVT_RIGHT_CLICK_TYPE, id)
        self.value = value
        self.SetEventObject(object)
    
    def get_value(self):
        """Retrieve the selection value associated to this event"""
        return self.value

#------------------------------------------------------------------------------
# AppEvents

# FSMNameChanged class
class FSMNameChanged:
    def __init__(self, new_name, old_name):
        self.new_name = new_name
        self.old_name = old_name
    def get_name(self):
        return self.new_name
    def get_old_name(self):
        return self.old_name

# FSMSaved class
class FSMSaved:
    def __init__(self, fsm_path):
        self.fsm_path = fsm_path
    def get_fsm_path(self):
        return self.fsm_path

# ActionAdded class
class ActionAdded:
    def __init__(self, action_name):
        self.action_name = action_name
    def get_action_name(self):
        return self.action_name

# ConditionAdded class
class ConditionAdded:
    def __init__(self, condition_name):
        self.condition_name = condition_name
    def get_condition_name(self):
        return self.condition_name

# FSMSelectorAdded class
class FSMSelectorAdded:
    def __init__(self, selector_name):
        self.selector_name = selector_name
    def get_selector_name(self):
        return self.selector_name

# SoundLibSaved class
class SoundLibSaved:
    pass

# TriggerScriptAdded class
class TriggerScriptAdded:
    def __init__(self, trigger_name):
        self.trigger_name = trigger_name
    def get_trigger_name(self):
        return self.trigger_name
