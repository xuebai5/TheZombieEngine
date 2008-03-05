##\file intsilder.py
##\brief Label-slider-text_box combined control for integer values

import wx

import intctrl
import slider
import textslider

import conjurerconfig as cfg


# IntSlider class
class IntSlider(textslider.TextSlider):
    """Label-slider-text_box combined control for integer values"""
    
    def __init__(
        self, 
        parent,
        id, 
        label="", 
        value=128, 
        min_value=0, 
        max_value=255,
        label_size=(-1,-1),
        int_ctrl_size=(30,-1)
        ):
        textslider.TextSlider.__init__(self, parent, id)
        
        label_ctrl = wx.StaticText(
                            self, 
                            -1, 
                            label, 
                            size=label_size,
                            style=cfg.SLIDER_LABEL_ALIGN
                            )
        slider_ctrl = slider.Slider(
                            self, 
                            -1, 
                            value, 
                            min_value,
                            max_value
                            )
        int_ctrl = intctrl.IntCtrl(
                        parent=self, 
                        limited=True, 
                        value=value,
                        min=min_value,
                        max=max_value,
                        size=int_ctrl_size, 
                        style=wx.TE_RIGHT
                        )
        self._create(value, label_ctrl, slider_ctrl, int_ctrl)

    def set_range(self, min_value, max_value):
        self.text_ctrl.SetMin(min_value)
        self.text_ctrl.SetMax(max_value)
        textslider.TextSlider.set_range(self, min_value, max_value)
