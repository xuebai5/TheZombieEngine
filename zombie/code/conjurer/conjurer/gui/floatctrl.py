##\file floatctrl.py
##\brief Text control that only accepts float values

import string
import types
import wx

import events


#------------------------------------------------------------------------------
# EVT_FLOAT event

# Event used to notify changes in the value of the float control
EVT_FLOAT_TYPE = wx.NewEventType()
EVT_FLOAT = wx.PyEventBinder(EVT_FLOAT_TYPE, 1)

# FloatUpdatedEvent class
class FloatUpdatedEvent(wx.PyCommandEvent):
    """Event class for an EVT_FLOAT event"""
    
    def __init__(self, id, value, object):
        wx.PyCommandEvent.__init__(self, EVT_FLOAT_TYPE, id)
        self.value = value
        self.SetEventObject(object)
    
    def get_value(self):
        """Retrieve the value of the float control at the time this event was generated"""
        return self.value


#------------------------------------------------------------------------------
# FloatValidator class
class FloatValidator(wx.PyValidator):
    """Validator for float values, optionally within a range"""
    
    def __init__(self, limited=False, min=0.0, max=1.0):
        wx.PyValidator.__init__(self)
        self.limited = limited
        self.min = min
        self.max = max
        self.Bind(wx.EVT_CHAR, self.OnChar)
    
    def Clone(self):
        return self.__class__(self.limited, self.min, self.max)
    
    def __is_valid(self, value):
        try:
            fvalue = float(value)
            return (not self.limited) or (fvalue >= self.min and fvalue <= self.max)
        except ValueError:
            return False
    
    def Validate(self, win):
        # The value is validated in OnChar, so this value is always valid
        return True
    
    # TODO: Catch Cut and Paste combination keys
    def OnChar(self, event):
        key = event.KeyCode()
        ctrl = event.GetEventObject()
        
        value = ctrl.get_value()
        textval = wx.TextCtrl.GetValue(ctrl)
        pos = ctrl.GetInsertionPoint()
        sel_start, sel_to = ctrl.GetSelection()
        select_len = sel_to - sel_start
        
        # Predict resulting value and validate it.
        
        if key in (wx.WXK_DELETE, wx.WXK_BACK):
            if select_len > 0:
                new_text = textval[:sel_start] + textval[sel_to:]
            elif key == wx.WXK_DELETE and pos < len(textval):
                new_text = textval[:pos] + textval[pos+1:]
            elif key == wx.WXK_BACK and pos > 0:
                new_text = textval[:pos-1] + textval[pos:]
            else:
                new_text = textval
            
            if self.__is_valid(new_text):
                event.Skip()
                return
            else:
                # Text box is empty, so fill it with "0.0" or the minimum
                # allowed value and select it for quick replacing by the
                # user if desired.
                # The selection must be done after the TextCtrl responses to
                # this event, so use CallAfter function.
                if self.__is_valid('0.0'):
                    new_text = '0.0'
                else:
                    new_text = str(self.min)
                wx.CallAfter(ctrl.SetValue, new_text)
                wx.CallAfter(ctrl.SetInsertionPoint, 0)
                wx.CallAfter(ctrl.SetSelection, 0, len(new_text))
                event.Skip()
        
        elif key < wx.WXK_SPACE or key > 255:
            event.Skip()
            return
        
        elif chr(key) in (string.digits + '-.'):
            new_text = textval[:sel_start] + chr(key) + textval[sel_to:]
            if self.__is_valid(new_text):
                event.Skip()
                return
        
        if not wx.Validator_IsSilent():
            wx.Bell()
        
        # Returning without calling even.Skip eats the event before it
        # gets to the text control
        return

    def TransferToWindow(self):
        """ Transfer data from validator to window.
            
            The default implementation returns False, indicating that an error
            occurred.  We simply return True, as we don't do any data transfer.
        """
        return True # Prevent wxDialog from complaining.

    def TransferFromWindow(self):
        """ Transfer data from window to validator.
            
            The default implementation returns False, indicating that an error
            occurred.  We simply return True, as we don't do any data transfer.
        """
        return True # Prevent wxDialog from complaining.


#------------------------------------------------------------------------------
# _FloatCtrl class
class _FloatCtrl(wx.TextCtrl):
    """
    Text control that only accepts float values
    
    Bind a function to EVT_FLOAT to catch float value changes.
    """
    
    def __init__( self, parent, id=-1, limited=False, value=0.0, min=0.0,
                  max=1.0, pos=wx.DefaultPosition, size=wx.DefaultSize,
                  style=0, validator=wx.DefaultValidator, name="floatCtrl",
                  round_decimals=5 ):
        """
        Contructor
        
        \param limited If true, the value is kept within the given range
        \param value Initial value
        \param min Minimum value allowed
        \param max Maximum value allowed
        \see wx.TextCtrl for other parameter descriptions
        """
        if validator == wx.DefaultValidator:
            validator = FloatValidator(limited, min, max)
        wx.TextCtrl.__init__(
            self, parent, id, str(value), pos, size,
            style, validator, name
            )
        self.skip_float_event = False
        self.round_decimals = round_decimals
        self.Bind(wx.EVT_TEXT, self.on_text)
    
    def on_text(self, event):
        """Handle the event for text changes and throw an EVT_FLOAT event."""
        if not self.skip_float_event:
            try:
                self.GetEventHandler().ProcessEvent(
                    FloatUpdatedEvent( self.GetId(), self.get_value(), self )
                    )
            except ValueError:
                return
        # let normal processing of the text continue
        event.Skip()
    
    def get_value(self):
        return float( self.GetValue() )
    
    def set_value(self, value, throw_event=True):
        if self.GetValidator().limited:
            value = max( value, self.GetValidator().min )
            value = min( value, self.GetValidator().max )
        self.skip_float_event = not throw_event
        self.SetValue( str(round(value, self.round_decimals)) )
        self.skip_float_event = False


#------------------------------------------------------------------------------
# FloatCtrl class
class FloatCtrl(_FloatCtrl):
    def __init__( # Same params as _FloatCtrl
                  self, parent, id=-1, limited=False, value=0.0, min=0.0,
                  max=1.0, pos=wx.DefaultPosition, size=wx.DefaultSize,
                  style=0, validator=wx.DefaultValidator, name="floatCtrl",
                  round_decimals=5 ):
        _FloatCtrl.__init__(self, parent, id, limited, value, min, max, pos,
            size, style, validator, name, round_decimals)
        
        self.old_value = self.get_value()
        
        self.Bind(EVT_FLOAT, self.__on_change_float, self)
        wx.EVT_KILL_FOCUS(self, self.__on_kill_focus)
    
    def __on_change_float(self, event):
        self.GetEventHandler().ProcessEvent(
            events.ChangingEvent( self.GetId(), self.get_value(), self )
            )
        event.Skip()
    
    def __on_kill_focus(self, event):
        if self.get_value() != self.old_value:
            self.GetEventHandler().ProcessEvent(
                events.ChangedEvent( self.GetId(), self.get_value(),
                    self.old_value, self )
                )
            self.old_value = self.get_value()
        event.Skip()
    
    def set_value(self, value, throw_event=True):
        if self.FindFocus() != self:
            self.old_value = value
        _FloatCtrl.set_value(self, value, throw_event)
