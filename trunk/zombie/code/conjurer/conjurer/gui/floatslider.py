##\file floatsilder.py
##\brief Label-slider-text_box combined control for float values

import wx

import floatctrl
import slider
import textslider

import conjurerconfig as cfg


# FloatSlider class
class FloatSlider(textslider.TextSlider):
    """Label-slider-text_box combined control for float values"""
    
    def __init__(
        self, 
        parent, 
        id, 
        label="", 
        value=0.0,
        min_value=-1.0, 
        max_value=1.0,
        precision=1000.0, 
        round_decimals=5, 
        float_ctrl_size=(45,-1)
        ):
        textslider.TextSlider.__init__(self, parent, id)
        
        self.precision = precision
        self.round_decimals = 5
        label_ctrl = wx.StaticText(self, -1, label, style=cfg.SLIDER_LABEL_ALIGN)
        slider_ctrl = slider.Slider(
                            self, 
                            -1, 
                            self._value_to_slider(value),
                            self._value_to_slider(min_value), 
                            self._value_to_slider(max_value)
                            )
        float_ctrl = floatctrl.FloatCtrl(
                            parent=self,
                            limited=True, 
                            value=self._value_to_text(value),
                            min=self._value_to_text(min_value), 
                            max=self._value_to_text(max_value),
                            size=float_ctrl_size, 
                            style=wx.TE_RIGHT,
                            round_decimals=round_decimals
                            )
        self._create(value, label_ctrl, slider_ctrl, float_ctrl)

    def _slider_to_value(self, value):
        return self.slider.GetValue() / self.precision
    
    def _value_to_slider(self, value):
        return int(value * self.precision)
    
    def _text_to_value(self, value):
        return value
    
    def _value_to_text(self, value):
        return value
