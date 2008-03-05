##\file aitestdlg.py
##\brief Dialog to configure the AI test mode

import wx

import pynebula

import app
import events
import intslider
import togwin


# AITestDialog class
class AITestDialog(togwin.ChildToggableDialog):
    """Dialog to configure the AI test mode"""
    
    def __init__(self, parent):
        togwin.ChildToggableDialog.__init__(
            self, "AI test settings", parent
            )
        self.timer = None
        
        self.slider_speed = intslider.IntSlider(
            self, -1, "Update speed:", 1, -10, 10
            )
        self.staticbox_debug = wx.StaticBox(self, -1, "Event transitions debugger")
        self.button_debug = wx.ToggleButton(self, -1, "Pause event transitions")
        self.button_apply = wx.Button(self, -1, "Apply pending event")
        self.button_discard = wx.Button(self, -1, "Discard pending event")
        self.label_event = wx.StaticText(self, -1, "Pending event:")
        self.text_event = wx.TextCtrl(self, value="Squad member", style=wx.TE_READONLY|wx.NO_BORDER)
        self.label_entity = wx.StaticText(self, -1, "Pending entity:")
        self.text_entity = wx.TextCtrl(self, style=wx.TE_READONLY|wx.NO_BORDER)
        
        self.__set_properties()
        self.__do_layout()
        self.__bind_events()
    
    def __set_properties(self):
        data = {
            'speed': -app.get_ai_tester().getupdatefrequency()
            }
        self.restore([data])
        
        self.text_event.SetBackgroundColour( wx.SystemSettings.GetColour(wx.SYS_COLOUR_BTNFACE) )
        self.text_entity.SetBackgroundColour( wx.SystemSettings.GetColour(wx.SYS_COLOUR_BTNFACE) )
        self.__update_pending_event()
    
    def __do_layout(self):
        sizer_border = wx.BoxSizer(wx.VERTICAL)
        sizer_layout = wx.BoxSizer(wx.VERTICAL)
        sizer_debug = wx.StaticBoxSizer(self.staticbox_debug, wx.VERTICAL)
        sizer_debug_layout = wx.BoxSizer(wx.VERTICAL)
        sizer_event = wx.BoxSizer(wx.HORIZONTAL)
        sizer_entity = wx.BoxSizer(wx.HORIZONTAL)
        sizer_layout.Add(self.slider_speed, 0, wx.EXPAND, 0)
        sizer_debug_layout.Add(self.button_debug, 0, wx.EXPAND, 0)
        sizer_debug_layout.Add(self.button_apply, 0, wx.TOP|wx.EXPAND, 5)
        sizer_debug_layout.Add(self.button_discard, 0, wx.TOP|wx.EXPAND, 5)
        sizer_event.Add(self.label_event, 0, wx.FIXED_MINSIZE, 0)
        sizer_event.Add(self.text_event, 1, wx.LEFT|wx.FIXED_MINSIZE, 5)
        sizer_debug_layout.Add(sizer_event, 0, wx.TOP|wx.EXPAND, 8)
        sizer_entity.Add(self.label_entity, 0, wx.FIXED_MINSIZE, 0)
        sizer_entity.Add(self.text_entity, 1, wx.LEFT|wx.FIXED_MINSIZE, 5)
        sizer_debug_layout.Add(sizer_entity, 0, wx.TOP|wx.EXPAND, 0)
        sizer_debug.Add(sizer_debug_layout, 0, wx.ALL|wx.EXPAND, 5)
        sizer_layout.Add(sizer_debug, 0, wx.TOP|wx.EXPAND, 5)
        sizer_border.Add(sizer_layout, 0, wx.ALL|wx.EXPAND, 10)
        self.SetAutoLayout(True)
        self.SetSizer(sizer_border)
        sizer_border.Fit(self)
        sizer_border.SetSizeHints(self)
        self.Layout()
    
    def __bind_events(self):
        self.Bind(events.EVT_CHANGING, self.on_change_speed, self.slider_speed)
        self.Bind(events.EVT_CHANGED, self.on_change_speed, self.slider_speed)
        self.Bind(wx.EVT_TOGGLEBUTTON, self.on_toggle_pause, self.button_debug)
        self.Bind(wx.EVT_BUTTON, self.on_apply_event, self.button_apply)
        self.Bind(wx.EVT_BUTTON, self.on_discard_event, self.button_discard)
        pynebula.pyBindSignal( app.get_ai_tester(), 'pendingeventchanged',
            self, 'onpendingeventchanged', 0 )
    
    def __del__(self):
        pynebula.pyUnbindTargetObject( app.get_ai_tester(),
            'pendingeventchanged', self)
    
    def __update_pending_event(self):
        tester = app.get_ai_tester()
        entity, event = tester.getpendingevent()
        debug = tester.isgameplaydebuggingenabled()
        pending = debug and entity != 0
        self.button_debug.SetValue( debug )
        self.button_apply.Enable( pending )
        self.button_discard.Enable( pending )
        if pending:
            self.text_event.SetValue( event )
            self.text_entity.SetValue( str(entity) )
        else:
            self.text_event.SetValue( "" )
            self.text_entity.SetValue( "" )

    def on_change_speed(self, event):
        app.get_ai_tester().setupdatefrequency( -self.slider_speed.get_value() )

    def on_toggle_pause(self, event):
        if self.button_debug.GetValue():
            app.get_ai_tester().enablegameplaydebugging()
        else:
            app.get_ai_tester().disablegameplaydebugging()
        self.__update_pending_event()

    def on_apply_event(self, event):
        app.get_ai_tester().applypendingevent()

    def on_discard_event(self, event):
        app.get_ai_tester().discardpendingevent()

    def onpendingeventchanged(self):
        self.__update_pending_event()

    def persist(self):
        data = {
            'speed': self.slider_speed.get_value()
            }
        return [
            create_window, 
            (), # no parameters for create function
            data
            ]

    def restore(self, data_list):
        data = data_list[0]

        app.get_ai_tester().setupdatefrequency( -data['speed'] )
        self.slider_speed.set_value( data['speed'] )


# create_window function
def create_window(parent):
    return AITestDialog(parent)
