##\file textsilder.py
##\brief Base class for a label-slider-text_box combined control

import wx

import events


# TextSlider class
class TextSlider(wx.PyControl):
    """
    Base class for a label-slider-text_box combined control
    
    The control generates 'changing' events when dragging the slider's thumb
    with the mouse or when the text box throws them itself. In all other
    cases where the value changes a 'changed' event is generated.
    
    Inherited classes must create the label, slider and text control and then
    call to the _create function. The label must be a wx.StaticText or have an
    equivalent interface, while the slider and text control must be custom
    controls which throw 'changing' and 'changed' events and implement the
    set_value method.
    
    Also, the inherited classes must override the 4 conversion function to
    exchange values from slider to text to user value.
    """
    
    def __init__(self, parent, id):
        wx.PyControl.__init__(self, parent, id, style=wx.NO_BORDER)
    
    def _create(self, value, label, slider, text_ctrl):
        self.label = label
        self.slider = slider
        self.text_ctrl = text_ctrl
        
        self.throw_event = True # to control when to throw a value change event
        self.value = value
        self.old_value = self.get_value()
        
        self.__do_layout()
        self.__bind_events()

    def __do_layout(self):
        sizer = wx.BoxSizer(wx.HORIZONTAL)
        sizer.Add(self.label, 0, wx.FIXED_MINSIZE|wx.ALIGN_CENTER_VERTICAL)
        sizer.Add(self.slider, 1, wx.FIXED_MINSIZE|wx.ALIGN_CENTER_VERTICAL)
        sizer.Add(self.text_ctrl, 0, wx.FIXED_MINSIZE|wx.ALIGN_CENTER_VERTICAL)
        self.SetSizerAndFit(sizer)
        self.Layout()

    def __bind_events(self):
        self.Bind(events.EVT_CHANGING, self.__on_changing_slider, self.slider)
        self.Bind(events.EVT_CHANGED, self.__on_changed_slider, self.slider)
        self.Bind(events.EVT_CHANGING, self.__on_changing_text, self.text_ctrl)
        self.Bind(events.EVT_CHANGED, self.__on_changed_text, self.text_ctrl)
        wx.EVT_SET_FOCUS(self, self.__on_set_focus)
        wx.EVT_SIZE(self, self.__on_size)

    def __on_changing_slider(self, event):
        value = self._slider_to_value( event.get_value() )
        if value != self.value:
            self.value = value
            self.text_ctrl.set_value( self._value_to_text(value) )
            self.GetEventHandler().ProcessEvent(
                events.ChangingEvent( self.GetId(), value, self )
                )

    def __on_changed_slider(self, event):
        value = self._slider_to_value( event.get_value() )
        if self.value != value or self.value != self.old_value:
            self.value = value
            self.text_ctrl.set_value( self._value_to_text(value) )
            self.GetEventHandler().ProcessEvent(
                events.ChangedEvent( self.GetId(), value, self.old_value, self )
                )
            self.old_value = value

    def __on_changing_text(self, event):
        value = self._text_to_value( event.get_value() )
        if value != self.value:
            self.value = value
            self.slider.set_value( self._value_to_slider(self.value) )
            self.GetEventHandler().ProcessEvent(
                events.ChangingEvent( self.GetId(), self.value, self )
                )

    def __on_changed_text(self, event):
        value = self._text_to_value( event.get_value() )
        if self.value != value or self.value != self.old_value:
            self.value = value
            self.slider.set_value( self._value_to_slider(self.value) )
            self.GetEventHandler().ProcessEvent(
                events.ChangedEvent( self.GetId(), self.value, self.old_value, self )
                )
            self.old_value = value

    def __on_set_focus(self, event):
        self.text_ctrl.SetSelection(-1,-1)
        self.text_ctrl.SetFocus()

    def __on_size(self, evt):
        # Needed to resize the slider with the window owning this control
        self.Layout()
        evt.Skip()

    # Override these conversion functions
    def _slider_to_value(self, value):
        return value

    def _value_to_slider(self, value):
        return value

    def _text_to_value(self, value):
        return value

    def _value_to_text(self, value):
        return value

    def get_value(self):
        return self.value

    def set_value(self, value):
        self.old_value = value
        self.value = value
        self.slider.set_value( self._value_to_slider(value) )
        self.text_ctrl.set_value( self._value_to_text(value) )

    def set_range(self, min, max):
        self.slider.SetRange( self._value_to_slider(min),
            self._value_to_slider(max) )
        if self.value < min:
            self.set_value( min )
            self.old_value = self.value
        elif self.value > max:
            self.set_value( max )
            self.old_value = self.value

    def get_min(self):
        return self._slider_to_value( self.slider.GetMin() )

    def get_max(self):
        return self._slider_to_value( self.slider.GetMax() )

    def resize_label(self, size):
        self.label.SetSize(size)
        sizer = self.GetSizer()
        sizer.Detach(self.label)
        sizer.Insert(
            0, 
            self.label,
            0, 
            wx.FIXED_MINSIZE|wx.ALIGN_CENTER_VERTICAL
            )
        sizer.Fit(self)
        sizer.SetSizeHints(self)
        self.Layout()

# align_sliders function
def align_sliders(sliders):
        """
        Resize slider labels to the longest label so all sliders become aligned
        
        \param sliders TextSlider sequence to align
        """
        # get longest label width
        max_width = -1
        for slider in sliders:
            width = slider.label.GetSize().width
            max_width = max(max_width, width)
        
        # resize all labels to the longest width
        for slider in sliders:
            slider.resize_label(
                (max_width,-1)
                )
