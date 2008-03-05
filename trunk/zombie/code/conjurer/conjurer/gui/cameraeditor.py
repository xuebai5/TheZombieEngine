##\file cameraeditor.py
##\brief Specific camera editor panel

import wx

import editorpanel
import events
import floatslider
import objdlg
import servers
import textslider

import conjurerconfig as cfg


# CameraPanel class
class CameraPanel(editorpanel.EditorPanel):
    def __init__(self, *args, **kwds):
        editorpanel.EditorPanel.__init__(self, *args, **kwds)
        
        self.fslider_fov = floatslider.FloatSlider(
            self, -1, "FOV", 60.0, 1.0, 179.0, precision=10.0
            )
        self.fslider_near = floatslider.FloatSlider(
            self, -1, "Near plane", 0.1, 0.0, 1.0, precision=1000.0
            )
        self.fslider_far = floatslider.FloatSlider(
            self, -1, "Far plane", 5000.0, 100.0, 20000.0, precision=0.1
            )
        self.fslider_speed = floatslider.FloatSlider(
            self, -1, "Speed", 6.0, 0.0, 1000.0, precision=1.0
            )
        self.fslider_mousex = floatslider.FloatSlider(
            self, -1, "Mouse Sensitivity X", 1.0, 0.0, 5.0, precision=10.0
            )
        self.fslider_mousey = floatslider.FloatSlider(
            self, -1, "Mouse Sensitivity Y", 1.0, 0.0, 5.0, precision=10.0
            )
        self.mouse_invert = wx.CheckBox( self, -1, "Invert mouse vertical" )
        self.button_camera = wx.Button(self, -1, "Edit camera")
        
        self.__set_properties()
        self.__do_layout()
        self.__bind_events()
    
    def __set_properties(self):
        # enable scrolls
        self.SetScrollRate(10, 10)
        
        # resize slider labels to the largest label to align them
        textslider.align_sliders([
            self.fslider_fov,
            self.fslider_near,
            self.fslider_far,
            self.fslider_speed,
            self.fslider_mousex,
            self.fslider_mousey
            ])
    
    def __bind_events(self):
        self.Bind(
            events.EVT_CHANGING, 
            self.on_changing_fov, 
            self.fslider_fov
            )
        self.Bind(
            events.EVT_CHANGING,
            self.on_changing_near, 
            self.fslider_near
            )
        self.Bind(
            events.EVT_CHANGING, 
            self.on_changing_far,
            self.fslider_far
            )
        self.Bind(
            events.EVT_CHANGING, 
            self.on_changing_speed, 
            self.fslider_speed
            )
        self.Bind(
            events.EVT_CHANGING, 
            self.on_changing_mousex, 
            self.fslider_mousex
            )
        self.Bind(
            events.EVT_CHANGING, 
            self.on_changing_mousey, 
            self.fslider_mousey
            )
        self.Bind(
            events.EVT_CHANGED, 
            self.on_changed_fov, 
            self.fslider_fov
            )
        self.Bind(
            events.EVT_CHANGED, 
            self.on_changed_near, 
            self.fslider_near
            )
        self.Bind(
            events.EVT_CHANGED,
            self.on_changed_far,
            self.fslider_far
            )
        self.Bind(
            events.EVT_CHANGED, 
            self.on_changed_speed, 
            self.fslider_speed
            )
        self.Bind(
            events.EVT_CHANGED, 
            self.on_changed_mousex, 
            self.fslider_mousex
            )
        self.Bind(
            events.EVT_CHANGED, 
            self.on_changed_mousey,
            self.fslider_mousey
            )
        self.Bind(
            wx.EVT_CHECKBOX, 
            self.on_mouseinvert, 
            self.mouse_invert 
            )
        self.Bind(
            wx.EVT_BUTTON, 
            self.on_edit_camera,
            self.button_camera
            )

    def __do_layout(self):
        border_sizer = wx.BoxSizer(wx.VERTICAL)
        sizer_controls = wx.BoxSizer(wx.VERTICAL)
        sizer_controls.Add(
            self.fslider_fov, 
            0, 
            wx.EXPAND
            )
        sizer_controls.Add(
            self.fslider_near, 
            0, 
            wx.TOP|wx.EXPAND, 
            cfg.BORDER_WIDTH
            )
        sizer_controls.Add(
            self.fslider_far, 
            0, 
            wx.TOP|wx.EXPAND, 
            cfg.BORDER_WIDTH
            )
        sizer_controls.Add(
            self.fslider_speed, 
            0, 
            wx.TOP|wx.EXPAND, 
            cfg.BORDER_WIDTH
            )
        sizer_controls.Add(
            self.fslider_mousex, 
            0, 
            wx.TOP|wx.EXPAND,
            cfg.BORDER_WIDTH
            )
        sizer_controls.Add(
            self.fslider_mousey, 
            0, 
            wx.TOP|wx.EXPAND, 
            cfg.BORDER_WIDTH
            )
        sizer_controls.Add(
            self.mouse_invert,
            0, 
            wx.TOP|wx.EXPAND, 
            cfg.BORDER_WIDTH
            )
        sizer_controls.Add(
            (0, 0),
            1,
            wx.EXPAND
            )
        horizontal_line = wx.StaticLine(self)
        sizer_controls.Add(
            horizontal_line, 
            0, 
            wx.EXPAND|wx.TOP, 
            cfg.BORDER_WIDTH
            )
        sizer_controls.Add(
            self.button_camera, 
            0,
            wx.TOP|wx.ALIGN_RIGHT, 
            cfg.BORDER_WIDTH
            )
        border_sizer.Add(
            sizer_controls,
            1, 
            wx.ALL|wx.EXPAND, 
            cfg.BORDER_WIDTH
            )
        self.SetSizerAndFit(border_sizer)

    def on_changing_fov(self, event):
        self.viewport.setangleofview( event.get_value() )

    def on_changing_near(self, event):
        self.viewport.setnearplane( event.get_value() )

    def on_changing_far(self, event):
        self.viewport.setfarplane( event.get_value() )

    def on_changing_speed(self, event):
        self.viewport.setviewervelocity( event.get_value() )

    def on_changing_mousex(self, event):
        self.viewport.setmousexfactor( event.get_value() )

    def on_changing_mousey(self, event):
        self.viewport.setmouseyfactor( event.get_value() )

    def on_edit_camera(self, event):
        win = objdlg.create_window(
                    wx.GetApp().GetTopWindow(), 
                    self.viewport.getviewportentity().getid()
                    )
        win.display()

    def __do_command(self, function_name, new_value, old_value):
        # redo
        redo = self.cmd_init
        redo = redo + "noreport = viewport." + function_name
        redo = redo + "(" + str(new_value) + "); del viewport"
        # undo
        undo = self.cmd_init
        undo = undo + "noreport = viewport." + function_name
        undo = undo + "(" + str(old_value) + "); del viewport"
        # new command
        servers.get_command_server().newcommand(redo, undo)

    def on_changed_fov(self, event):
        self.__do_command(
            'setangleofview', 
            event.get_value(), 
            event.get_old_value()
            )

    def on_changed_near(self, event):
        self.__do_command(
            'setnearplane', 
            event.get_value(),
            event.get_old_value()
            )

    def on_changed_far(self, event):
        self.__do_command(
            'setfarplane', 
            event.get_value(), 
            event.get_old_value()
            )

    def on_changed_speed(self, event):
        self.__do_command(
            'setviewervelocity', 
            event.get_value(), 
            event.get_old_value()
            )

    def on_changed_mousex(self, event):
        self.__do_command(
            'setmousexfactor', 
            event.get_value(),
            event.get_old_value()
            )

    def on_changed_mousey(self, event):
        self.__do_command(
            'setmouseyfactor', 
            event.get_value(), 
            event.get_old_value()
            )

    def on_mouseinvert( self, evt ):
        self.viewport.setmouseyinvert( self.mouse_invert.GetValue() )

    def set_viewport(self, viewport):
        self.viewport = viewport
        self.cmd_init = "viewport = pynebula.lookup" \
                                "('%s');" % self.viewport.getfullname()
        try:
            self.fslider_fov.set_value( viewport.getangleofview() )
        except:
            self.fslider_fov.Enable( False )
        try:
            self.fslider_near.set_value( viewport.getnearplane() )
        except:
            self.fslider_near.Enable( False )
        try:
            self.fslider_far.set_value( viewport.getfarplane() )
        except:
            self.fslider_far.Enable( False )
        try:
            self.fslider_speed.set_value( viewport.getviewervelocity() )
        except:
            self.fslider_speed.Enable( False )
        try:
            self.fslider_mousey.set_value( viewport.getmouseyfactor() )
        except:
            self.fslider_mousey.Enable( False )
        try:
            self.fslider_mousex.set_value( viewport.getmousexfactor() )
        except:
            self.fslider_mousex.Enable( False )
        try:
            self.mouse_invert.SetValue( viewport.getmouseyinvert() )
        except:
            self.mouse_invert.Enable( False )
            
    def refresh(self):
        self.set_viewport(self.viewport)


# create_all_editors function
def create_all_editors(viewport, parent):
    editor1 = CameraPanel(parent, -1)
    editor1.set_viewport(viewport)
    return [ ('Viewport', editor1) ]
