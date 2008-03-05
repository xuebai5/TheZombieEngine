##\file trntoolpanel.py
##\brief Terrain tool panel

import wx

import pynebula

import app
import events
import floatslider
import format
import imgbox
import intctrl
import intslider
import math
import servers
import textslider
import togwin
import trn

import conjurerframework as cjr
import conjurerconfig as cfg


#------------------------------------------------------------------------------
# ToolPanel class
class ToolPanel(wx.Panel):
    """Common base panel for all terrain tools which have intensity"""

    def __init__(self, *args, **kwds):
        wx.Panel.__init__(self, *args, **kwds)
        
        self.slider_attenuation = floatslider.FloatSlider(
                                            self, 
                                            -1, 
                                            "Attenuation", 
                                            1.0,
                                            -0.5, 
                                            3.0, 
                                            precision=100.0
                                            )
        self.slider_intensity = floatslider.FloatSlider(
                                        self,
                                        -1, 
                                        "Intensity", 
                                        0.5,
                                        0.0, 
                                        1.0, 
                                        precision=1000.0
                                        )

    def _do_layout(self):
        border_sizer = wx.BoxSizer(wx.VERTICAL)
        sizer_controls = wx.BoxSizer(wx.VERTICAL)
        sizer_controls.Add(
            self.slider_attenuation, 
            0, 
            wx.EXPAND
            )
        sizer_controls.Add(
            self.slider_intensity,
            0, 
            wx.EXPAND
            )
        border_sizer.Add(
            sizer_controls,
            0, 
            wx.EXPAND, 
            )
        self.SetSizerAndFit(border_sizer)

    def _bind_events(self):
        self.Bind(
            events.EVT_CHANGING, 
            self.on_intensity,
            self.slider_attenuation
            )
        self.Bind(
            events.EVT_CHANGED, 
            self.on_intensity,
            self.slider_attenuation
            )
        self.Bind(
            events.EVT_CHANGING,
            self.on_intensity,
            self.slider_intensity
            )
        self.Bind(
            events.EVT_CHANGED, 
            self.on_intensity,
            self.slider_intensity
            )

    def on_intensity(self, event):
        att = self.slider_attenuation.get_value()
        intensity = self.slider_intensity.get_value()
        tool_intensity = pow( intensity, att + 1 )
        self.tool.setintensity( tool_intensity )

    def persist_ingui(self):
        data = {
            'attenuation': self.slider_attenuation.get_value(),
            'intensity': self.tool.getintensity()
            }
        return data

    def persist_outgui(self):
        return self.persist_ingui()

    def restore(self, data):
        if data.has_key('attenuation'):
            att = data['attenuation']
        else:
            att = 1.0
        self.slider_attenuation.set_value( att )
        inv_att = 1.0 / (att + 1)
        tool_int = data['intensity']
        int = math.pow( tool_int, inv_att )
        self.slider_intensity.set_value( int )


#------------------------------------------------------------------------------
# RaisePanel class
class RaisePanel(ToolPanel):
    """Panel with the controls for the raise/dig tool"""
    
    def __init__(self, *args, **kwds):
        ToolPanel.__init__(self, *args, **kwds)
        self.tool = trn.get_terrain_tool('toolRaiseLow')
        
        self.__set_properties()
        self._do_layout()
        self._bind_events()
    
    def __set_properties(self):
        textslider.align_sliders([
            self.slider_attenuation,
            self.slider_intensity
            ])


#------------------------------------------------------------------------------
# FlattenPanel class
class FlattenPanel(ToolPanel):
    """Panel with the controls for the flatten tool"""

    def __init__(self, *args, **kwds):
        ToolPanel.__init__(self, *args, **kwds)
        self.tool = trn.get_terrain_tool('toolFlatten')
        
        # controls
        # Needed to set the slider range here because using the
        # __update_slider_range doesn't update the slider range if called
        # during dialog contruction.
        outdoor = app.get_outdoor()
        if outdoor is None:
            min_value = 0.0
            max_value = 100.0
        else:
            min_value = outdoor.getminheight()
            max_value = outdoor.getmaxheight()
        value = (max_value - min_value) / 2
        self.slider_height = floatslider.FloatSlider(
            self, -1, "Height", value, min_value, max_value, precision=10.0
            )
        self.checkbox_adaptive = wx.CheckBox(self, -1, "")
        self.slider_adaptive = floatslider.FloatSlider(
            self, -1, "Adaptive", 0.5, 0.0, 1.0, precision=1000.0
            )
        
        self.__set_properties()
        self.__do_layout()
        self._bind_events()

    def __set_properties(self):
        textslider.align_sliders([
            self.slider_attenuation,
            self.slider_intensity,
            self.slider_height,
            self.slider_adaptive
            ])

    def __do_layout(self):
        border_sizer = wx.BoxSizer(wx.VERTICAL)
        sizer_controls = wx.BoxSizer(wx.VERTICAL)
        sizer_attenuation = wx.BoxSizer(wx.HORIZONTAL)
        sizer_intensity = wx.BoxSizer(wx.HORIZONTAL)
        sizer_height = wx.BoxSizer(wx.HORIZONTAL)
        sizer_adaptive = wx.BoxSizer(wx.HORIZONTAL)
        separation_size = wx.Size(cfg.BORDER_WIDTH, cfg.BORDER_WIDTH)
        align_size = self.checkbox_adaptive.GetSize() + (separation_size.x, 0)
        sizer_attenuation.Add(align_size)
        sizer_attenuation.Add(
            self.slider_attenuation, 
            1, 
            0, 
            0
            )
        sizer_controls.Add(
            sizer_attenuation, 
            1, 
            wx.EXPAND
            )
        sizer_intensity.Add(
            align_size
            )
        sizer_intensity.Add(
            self.slider_intensity, 
            1, 
            0, 
            0
            )
        sizer_controls.Add(
            sizer_intensity,
            1,
            wx.EXPAND
            )
        sizer_height.Add(align_size)
        sizer_height.Add(
            self.slider_height, 
            1, 
            0,
            0
            )
        sizer_controls.Add(
            sizer_height, 
            1, 
            wx.EXPAND
            )
        sizer_adaptive.Add(
            self.checkbox_adaptive,
            0, 
            wx.FIXED_MINSIZE|wx.ALIGN_CENTER_VERTICAL
            )
        sizer_adaptive.Add(separation_size)
        sizer_adaptive.Add(
            self.slider_adaptive, 
            1, 
            0, 
            0
            )
        sizer_controls.Add(
            sizer_adaptive, 
            1, 
            wx.EXPAND
            )
        border_sizer.Add(
            sizer_controls,
            0,
            wx.EXPAND
            )
        self.SetSizerAndFit(border_sizer)

    def _bind_events(self):
        ToolPanel._bind_events(self)
        self.Bind(
            events.EVT_CHANGING, 
            self.on_height, 
            self.slider_height
            )
        self.Bind(
            events.EVT_CHANGED, 
            self.on_height, 
            self.slider_height
            )
        self.Bind(
            wx.EVT_CHECKBOX, 
            self.on_check_adaptive,
            self.checkbox_adaptive
            )
        self.Bind(
            events.EVT_CHANGING, 
            self.on_adaptive,
            self.slider_adaptive
            )
        self.Bind(
            events.EVT_CHANGED, 
            self.on_adaptive,
            self.slider_adaptive
            )
        
        # Nebula signals
        pynebula.pyBindSignal(
            self.tool, 
            'refreshflattenheight', 
            self,
            'onheightsignal', 
            0
            )

    def __del__(self):
        pynebula.pyUnbindTargetObject(
            self.tool, 
            'refreshflattenheight', 
            self
            )

    def __update_slider_height(self):
        # Update height slider's range
        outdoor = app.get_outdoor()
        if outdoor is not None:
            self.slider_height.set_range( outdoor.getminheight(),
                outdoor.getmaxheight() )

    def onheightsignal(self):
        self.slider_height.set_value( self.tool.getheight() )

    def on_height(self, event):
        self.tool.setheight( self.slider_height.get_value() )

    def is_adaptive_checked(self):
        return self.checkbox_adaptive.IsChecked()

    def on_check_adaptive(self, event):
        is_checked = self.is_adaptive_checked()
        self.slider_adaptive.Enable(is_checked)
        if is_checked:
            self.tool.setadaptiveintensity(
                self.slider_adaptive.get_value() 
                )
        else:
            self.tool.setadaptiveintensity(0)

    def on_adaptive(self, event):
        if self.is_adaptive_checked():
            self.tool.setadaptiveintensity( self.slider_adaptive.get_value() )

    def refresh(self):
        self.__update_slider_height()

    def persist_ingui(self):
        data = ToolPanel.persist_ingui(self)
        data['height'] = self.tool.getheight()
        data['adaptive enabled'] = True
        data['adaptive'] = self.tool.getadaptiveintensity()
        return data

    def persist_outgui(self):
        data = self.persist_ingui(self)
        data['adaptive enabled'] = self.checkbox_adaptive.GetValue()
        data['adaptive'] = self.slider_adaptive.get_value()
        return data

    def restore(self, data):
        ToolPanel.restore(self, data)
        
        # restore conjurer tool
        self.tool.setheight( data['height'] )
        if data['adaptive enabled']:
            self.tool.setadaptiveintensity( data['adaptive'] )
        else:
            self.tool.setadaptiveintensity(0)
        
        # restore controls
        self.slider_height.set_value( self.tool.getheight() )
        adaptive_enabled =  data['adaptive enabled'] 
        self.checkbox_adaptive.SetValue(adaptive_enabled)
        self.slider_adaptive.Enable(adaptive_enabled)
        if adaptive_enabled:
            self.slider_adaptive.set_value( self.tool.getadaptiveintensity() )
        else:
            self.slider_adaptive.set_value( data['adaptive'] )


#------------------------------------------------------------------------------
# SlopePanel class
class SlopePanel(ToolPanel):
    """Panel with the controls for the slope tool"""

    def __init__(self, *args, **kwds):
        ToolPanel.__init__(self, *args, **kwds)
        self.tool = trn.get_terrain_tool('toolSlope')
        
        self.slider_slope = floatslider.FloatSlider(
            self, -1, "Slope", 45.0, 0.0, 90.0, precision=10.0
            )
        
        self.__set_properties()
        self.__do_layout()
        self._bind_events()

    def _bind_events(self):
        ToolPanel._bind_events(self)
        self.Bind(events.EVT_CHANGING, self.on_slope, self.slider_slope)
        self.Bind(events.EVT_CHANGED, self.on_slope, self.slider_slope)

    def on_slope(self, event):
        self.tool.setslope( self.slider_slope.get_value() )

    def __set_properties(self):
        textslider.align_sliders([
            self.slider_attenuation,
            self.slider_intensity,
            self.slider_slope
            ])

    def __do_layout(self):
        border_sizer = wx.BoxSizer(wx.VERTICAL)
        sizer_controls = wx.BoxSizer(wx.VERTICAL)
        sizer_controls.Add(
            self.slider_attenuation,
            0, 
            wx.EXPAND
            )
        sizer_controls.Add(
            self.slider_intensity,
            0, 
            wx.EXPAND
            )
        sizer_controls.Add(
            self.slider_slope, 
            0, 
            wx.EXPAND
            )
        border_sizer.Add(
            sizer_controls,
            0, 
            wx.EXPAND
            )
        self.SetSizerAndFit(border_sizer)

    def persist_ingui(self):
        data = ToolPanel.persist_ingui(self)
        data['slope'] = self.tool.getslope()
        return data

    def restore(self, data):
        ToolPanel.restore(self, data)
        self.tool.setslope( data['slope'] )
        self.slider_slope.set_value( self.tool.getslope() )


#------------------------------------------------------------------------------
# SmoothPanel class
class SmoothPanel(ToolPanel):
    """Panel with the controls for the smooth tool"""
    
    def __init__(self, *args, **kwds):
        ToolPanel.__init__(self, *args, **kwds)
        self.tool = trn.get_terrain_tool('toolSmooth')
        
        self.__set_properties()
        self._do_layout()
        self._bind_events()
    
    def __set_properties(self):
        textslider.align_sliders([
            self.slider_attenuation,
            self.slider_intensity
            ])


#------------------------------------------------------------------------------
# NoisePanel class
class NoisePanel(ToolPanel):
    """Panel with the controls for the noise tool"""
    
    def __init__(self, *args, **kwds):
        ToolPanel.__init__(self, *args, **kwds)
        self.tool = trn.get_terrain_tool('toolNoise')
        
        self.__set_properties()
        self._do_layout()
        self._bind_events()

    def __set_properties(self):
        textslider.align_sliders([
            self.slider_attenuation,
            self.slider_intensity
            ])


#------------------------------------------------------------------------------
# PaintPanel class
class PaintPanel(ToolPanel):
    """Panel with the controls for the paint tool"""
    
    def __init__(self, *args, **kwds):
        ToolPanel.__init__(self, *args, **kwds)
        self.tool = trn.get_terrain_tool('toolPaint')
        
        self.slider_blend = floatslider.FloatSlider(
            self, -1, "Blend", 0.5, 0.0, 1.0, precision=1000.0
            )
        self.staticbox_filter = wx.StaticBox(self, -1, "Filter")
        self.checkbox_slope = wx.CheckBox(self, -1, "Filter by slope")
        self.slider_min_slope = floatslider.FloatSlider(
            self, -1, "Min. slope", 15.0, 0.0, 90.0, precision=10.0
            )
        self.slider_max_slope = floatslider.FloatSlider(
            self, -1, "Max. slope", 45.0, 0.0, 90.0, precision=10.0
            )
        self.staticline_filter = wx.StaticLine(self, -1)
        self.checkbox_height = wx.CheckBox(self, -1, "Filter by height")
        # Needed to set the height sliders range here because using the
        # __update_sliders_height doesn't update the height sliders range if
        # called during dialog contruction.
        outdoor = app.get_outdoor()
        if outdoor is None:
            min_value = 0.0
            max_value = 100.0
        else:
            min_value = outdoor.getminheight()
            max_value = outdoor.getmaxheight()
        value = 0.8 * min_value + 0.2 * max_value
        self.slider_min_height = floatslider.FloatSlider(
            self, -1, "Min. height", value, min_value, max_value, precision=10.0
            )
        value = 0.2 * min_value + 0.8 * max_value
        self.slider_max_height = floatslider.FloatSlider(
            self, -1, "Max. height", value, min_value, max_value, precision=10.0
            )
        
        self.__set_properties()
        self._do_layout()
        self._bind_events()

    def __set_properties(self):
        textslider.align_sliders([
            self.slider_attenuation,
            self.slider_intensity,
            self.slider_blend
            ])
        textslider.align_sliders([
            self.slider_min_slope,
            self.slider_max_slope
            ])
        textslider.align_sliders([
            self.slider_min_height,
            self.slider_max_height
            ])

    def _do_layout(self):
        border_sizer = wx.BoxSizer(wx.VERTICAL)
        sizer_controls = wx.BoxSizer(wx.VERTICAL)
        sizer_filter_group = wx.StaticBoxSizer(
                                        self.staticbox_filter, 
                                        wx.VERTICAL
                                        )
        sizer_controls.Add(
            self.slider_attenuation,
            0,
            wx.EXPAND
            )
        sizer_controls.Add(
            self.slider_intensity, 
            0, 
            wx.EXPAND
            )
        sizer_controls.Add(
            self.slider_blend, 
            0, 
            wx.EXPAND
            )
        sizer_filter_group.Add(
            self.checkbox_slope, 
            0, 
            wx.ALL|wx.FIXED_MINSIZE, 
            cfg.BORDER_WIDTH
            )
        sizer_filter_group.Add(
            self.slider_min_slope, 
            0, 
            wx.LEFT|wx.RIGHT|wx.TOP|wx.EXPAND,
            cfg.BORDER_WIDTH
            )
        sizer_filter_group.Add(
            self.slider_max_slope,
            0,
            wx.LEFT|wx.RIGHT|wx.BOTTOM|wx.EXPAND, 
            cfg.BORDER_WIDTH
            )
        sizer_filter_group.Add(
            self.staticline_filter, 
            0, 
            wx.BOTTOM|wx.EXPAND, 
            cfg.BORDER_WIDTH
            )
        sizer_filter_group.Add(
            self.checkbox_height,
            0, 
            wx.ALL|wx.FIXED_MINSIZE, 
            cfg.BORDER_WIDTH
            )
        sizer_filter_group.Add(
            self.slider_min_height, 
            0,
            wx.LEFT|wx.RIGHT|wx.TOP|wx.EXPAND, 
            cfg.BORDER_WIDTH
            )
        sizer_filter_group.Add(
            self.slider_max_height,
            0, 
            wx.LEFT|wx.RIGHT|wx.BOTTOM|wx.EXPAND, 
            cfg.BORDER_WIDTH
            )
        sizer_controls.Add(
            sizer_filter_group, 
            0, 
            wx.TOP|wx.EXPAND,
            cfg.BORDER_WIDTH
            )
        border_sizer.Add(
            sizer_controls,
            1, 
            wx.EXPAND
            )
        self.SetSizerAndFit(border_sizer)

    def _bind_events(self):
        # 'Changing' events for the filters' sliders aren't catched because
        # it's expensive to update the paint tool for this parameters.
        ToolPanel._bind_events(self)
        self.Bind(wx.EVT_CHECKBOX, self.on_enable_slope, self.checkbox_slope)
        self.Bind(events.EVT_CHANGING, self.on_blend, self.slider_blend)
        self.Bind(events.EVT_CHANGED, self.on_blend, self.slider_blend)
        self.Bind(events.EVT_CHANGED, self.on_min_slope, self.slider_min_slope)
        self.Bind(events.EVT_CHANGED, self.on_max_slope, self.slider_max_slope)
        self.Bind(wx.EVT_CHECKBOX, self.on_enable_height, self.checkbox_height)
        self.Bind(
            events.EVT_CHANGED, 
            self.on_min_height, 
            self.slider_min_height
            )
        self.Bind(
            events.EVT_CHANGED, 
            self.on_max_height, 
            self.slider_max_height
            )

    def __update_sliders_height(self):
        # Update height sliders' range
        outdoor = app.get_outdoor()
        if outdoor is not None:
            self.slider_min_height.set_range( outdoor.getminheight(),
                outdoor.getmaxheight() )
            self.slider_max_height.set_range( outdoor.getminheight(),
                outdoor.getmaxheight() )

    def on_enable_slope(self, event):
        self.tool.setfilterslopeenabled( self.checkbox_slope.IsChecked() )

    def on_blend(self, event):
        self.tool.setblendintensity( self.slider_blend.get_value() )

    def on_min_slope(self, event):
        self.tool.setminfilterslope( self.slider_min_slope.get_value() )

    def on_max_slope(self, event):
        self.tool.setmaxfilterslope( self.slider_max_slope.get_value() )

    def on_enable_height(self, event):
        self.tool.setfilterheightenabled( self.checkbox_height.IsChecked() )

    def on_min_height(self, event):
        self.tool.setminfilterheight( self.slider_min_height.get_value() )

    def on_max_height(self, event):
        self.tool.setmaxfilterheight( self.slider_max_height.get_value() )

    def refresh(self):
        self.__update_sliders_height()

    def persist_ingui(self):
        data = ToolPanel.persist_ingui(self)
        data['blend'] = self.tool.getblendintensity()
        data['filter by slope'] = self.tool.getfilterslopeenabled()
        data['min slope'] = self.tool.getminfilterslope()
        data['max slope'] = self.tool.getmaxfilterslope()
        data['filter by height'] = self.tool.getfilterheightenabled()
        data['min height'] = self.tool.getminfilterheight()
        data['max height'] = self.tool.getmaxfilterheight()
        return data

    def restore(self, data):
        ToolPanel.restore(self, data)
        try:
            self.tool.setblendintensity( data['blend'] )
            self.slider_blend.set_value( self.tool.getblendintensity() )
        except:
            pass
        self.tool.setfilterslopeenabled( data['filter by slope'] )
        self.checkbox_slope.SetValue( self.tool.getfilterslopeenabled() )
        self.tool.setminfilterslope( data['min slope'] )
        self.slider_min_slope.set_value( self.tool.getminfilterslope() )
        self.tool.setmaxfilterslope( data['max slope'] )
        self.slider_max_slope.set_value( self.tool.getmaxfilterslope() )
        self.tool.setfilterheightenabled( data['filter by height'] )
        self.checkbox_height.SetValue( self.tool.getfilterheightenabled() )
        self.tool.setminfilterheight( data['min height'] )
        self.slider_min_height.set_value( self.tool.getminfilterheight() )
        self.tool.setmaxfilterheight( data['max height'] )
        self.slider_max_height.set_value( self.tool.getmaxfilterheight() )


#------------------------------------------------------------------------------
# HolePanel class
class HolePanel(wx.Panel):
    """Panel for the hole tool"""

    def __init__(self, *args, **kwds):
        wx.Panel.__init__(self, *args, **kwds)
        self.tool = trn.get_terrain_tool('toolHole')

    def persist_ingui(self):
        return None

    def persist_outgui(self):
        return self.persist_ingui()

    def restore(self, data):
        pass


#------------------------------------------------------------------------------
# GrassPanel class
class GrassPanel(wx.Panel):
    """Panel for the grass tool"""

    def __init__(self, *args, **kwds):
        wx.Panel.__init__(self, *args, **kwds)
        self.tool = trn.get_terrain_tool('toolGrass')
        
        self.slider_alpha = floatslider.FloatSlider(
            self, -1, "Alpha", 1.0, 0.0, 1.0, precision=100.0
            )
        
        self._do_layout()
        self._bind_events()

    def _do_layout(self):
        border_sizer = wx.BoxSizer(wx.VERTICAL)
        sizer_controls = wx.BoxSizer(wx.VERTICAL)
        sizer_controls.Add(
            self.slider_alpha,
            0, 
            wx.EXPAND, 
            )
        border_sizer.Add(
            sizer_controls,
            0,
            wx.EXPAND
            )
        self.SetSizerAndFit(border_sizer)

    def _bind_events(self):
        self.Bind(events.EVT_CHANGING, self.on_alpha, self.slider_alpha)
        self.Bind(events.EVT_CHANGED, self.on_alpha, self.slider_alpha)

    def on_alpha(self, event):
        alpha = self.slider_alpha.get_value()
        terrain = app.get_outdoor()
        terrain.setgrowtheditionalpha( alpha )

    def persist_ingui(self):
        data = {
            'alpha': self.slider_alpha.get_value(),
            }
        return data

    def persist_outgui(self):
        return self.persist_ingui()

    def restore(self, data):
        if data.has_key('alpha'):
            alpha = data['alpha']
        self.slider_alpha.set_value( alpha )
        terrain = app.get_outdoor()
        if terrain is not None:
            terrain.setgrowtheditionalpha( alpha )
        # TODO: if terrain is None, disable alpha and bind a signal to enable
        # it when a terrain is created, so there's no chance of having
        # a GUI value of alpha different from the one in the terrain


#------------------------------------------------------------------------------
# ToolChoicebook class
class ToolChoicebook(wx.Choicebook):
    """Choicebook with controls of a terrain tool in each page"""

    def __init__(self, *args, **kwds):
        wx.Choicebook.__init__(self, *args, **kwds)
        self.terrain = trn.get_terrain_module()
        
        self.raise_panel = RaisePanel(self, -1)
        self.flatten_panel = FlattenPanel(self, -1)
        self.slope_panel = SlopePanel(self, -1)
        self.smooth_panel = SmoothPanel(self, -1)
        self.noise_panel = NoisePanel(self, -1)
        self.paint_panel = PaintPanel(self, -1)
        self.grass_panel = GrassPanel(self, -1)
        self.hole_panel = HolePanel(self, -1)
        self.AddPage( self.raise_panel, "Raise/Dig" )
        self.AddPage( self.flatten_panel, "Flatten" )
        self.AddPage( self.slope_panel, "Slope" )
        self.AddPage( self.smooth_panel, "Smooth" )
        self.AddPage( self.noise_panel, "Noise" )
        self.AddPage( self.paint_panel, "Paint" )
        self.AddPage( self.grass_panel, "Grass" )
        self.AddPage( self.hole_panel, "Hole" )
        
        self.Bind(wx.EVT_CHOICEBOOK_PAGE_CHANGED, self.on_select_tool, self)

    def select_current_tool(self):
        self.terrain.selecttool( self.GetSelection() )
        self.terrain.setpaintbrushsize(
            self.GetParent().slider_size.get_value() 
            )
        app.get_top_window(self).get_menubar().get_menu('T&ools').on_terrain_tool(None)

    def on_select_tool(self, event):
        self.GetParent().imgbox_tools.set_selection( self.GetSelection() )
        self.select_current_tool()

    def set_tools_brush_size(self, size):
        # set the size just for the current tool
        self.terrain.setpaintbrushsize(size)

    def refresh(self):
        self.flatten_panel.refresh()
        self.paint_panel.refresh()

    def persist_ingui(self):
        tools_data = []
        for i in range(self.GetPageCount()):
            tools_data.append( self.GetPage(i).persist_ingui() )
        
        data = {
            'selected tool': self.terrain.getselectedtool(),
            'tools': tools_data
            }
        return data

    def persist_outgui(self):
        return self.persist_ingui()

    def restore(self, data):
        # restore conjurer terrain object
        self.terrain.selecttool( data['selected tool'] )
        
        # restore controls
        self.SetSelection( self.terrain.getselectedtool() )
        
        # restore children
        for i in range(self.GetPageCount()):
            self.GetPage(i).restore( data['tools'][i] )


#------------------------------------------------------------------------------
# ToolPanel class
class ToolDialog(togwin.ChildToggableDialog):
    """Dialog to select and modify the terrain working tool"""

    def __init__(self, parent):
        togwin.ChildToggableDialog.__init__(self, "Terrain edit tool", parent)
        self.terrain = trn.get_terrain_module()
        
        self.imgbox_brushes = imgbox.ImageBox(
                                        self, 
                                        image_size=wx.Size(64, 64),
                                        text_size=8
                                        )
        self.imgbox_tools = imgbox.ImageBox(
                                    self, 
                                    image_size=wx.Size(32, 32),
                                    text_size=8
                                    )
        self.sizer_brush_group_staticbox = wx.StaticBox(self, -1, "Brush")
        self.sizer_tool_group_staticbox = wx.StaticBox(self, -1, "Tool")
        self.slider_size = intslider.IntSlider(
                                self,
                                -1, 
                                "Size", 
                                10, 
                                1, 
                                20
                                )
        self.button_range = wx.Button(self, -1, "[1,20]", style=wx.BU_EXACTFIT)
        self.choicebook_tool = ToolChoicebook(self, -1)
        
        self.__set_properties()
        self.__do_layout()
        self.__bind_events()

    def __set_properties(self):
        data = {
            'selected brush': self.terrain.getselectedpaintbrush(),
            'brush size': self.terrain.getpaintbrushsize(),
            'tools': self.choicebook_tool.persist_ingui()
            }
        self.restored = False
        self.restore([data])

    def __do_layout(self):
        outer_sizer = wx.BoxSizer(wx.VERTICAL)
        sizer_tool_group = wx.StaticBoxSizer(
                                    self.sizer_tool_group_staticbox, 
                                    wx.VERTICAL
                                    )
        sizer_brush_group = wx.StaticBoxSizer(
                                        self.sizer_brush_group_staticbox,
                                        wx.VERTICAL
                                        )
        sizer_brush_size = wx.BoxSizer(wx.HORIZONTAL)
        sizer_brush_group.Add(
            self.imgbox_brushes,
            1, 
            wx.ALL|wx.EXPAND, 
            cfg.BORDER_WIDTH
            )
        sizer_brush_size.Add(
            self.slider_size,
            1,
            wx.FIXED_MINSIZE
            )
        sizer_brush_size.Add(
            self.button_range,
            0, 
            wx.LEFT|wx.FIXED_MINSIZE, 
            cfg.BORDER_WIDTH
            )
        sizer_brush_group.Add(
            sizer_brush_size, 
            0, 
            wx.ALL|wx.EXPAND,
            cfg.BORDER_WIDTH
            )
        outer_sizer.Add(
            sizer_brush_group, 
            1, 
            wx.ALL|wx.EXPAND, 
            cfg.BORDER_WIDTH
            )
        sizer_tool_group.Add(
            self.imgbox_tools, 
            0,
            wx.ALL|wx.EXPAND, 
            cfg.BORDER_WIDTH
            )
        sizer_tool_group.Add(
            self.choicebook_tool,
            1, 
            wx.LEFT|wx.RIGHT|wx.BOTTOM|wx.EXPAND, 
            cfg.BORDER_WIDTH
            )
        outer_sizer.Add(
            sizer_tool_group, 
            0, 
            wx.LEFT|wx.RIGHT|wx.BOTTOM|wx.EXPAND,
            cfg.BORDER_WIDTH
            )
        self.SetSizerAndFit(outer_sizer)

    def __bind_events(self):
        self.Bind(imgbox.EVT_IMAGE, self.on_select_brush, self.imgbox_brushes)
        self.Bind(events.EVT_CHANGING, self.on_brush_size, self.slider_size)
        self.Bind(events.EVT_CHANGED, self.on_brush_size, self.slider_size)
        self.Bind(wx.EVT_BUTTON, self.on_brush_size_range, self.button_range)
        self.Bind(imgbox.EVT_IMAGE, self.on_select_tool, self.imgbox_tools)
        pynebula.pyBindSignal(
            self.terrain,
            'paintbrushsizechanged',
            self, 
            'onpaintbrushsizechanged', 
            0
            )

    def __del__(self):
        pynebula.pyUnbindTargetObject(
            self.terrain, 
            'paintbrushsizechanged', 
            self
            )

    def onpaintbrushsizechanged(self, brush_size):
        min_value = min(
                            self.slider_size.get_min(),
                            brush_size
                            )
        max_value = max(
                            self.slider_size.get_max(),
                            brush_size
                            )
        self.slider_size.set_range( min_value, max_value )
        self.slider_size.set_value(brush_size)
        self.button_range.SetLabel(
            "[%s,%s]" % (min_value, max_value)
            )

    def on_select_brush(self, event):
        selection_index = self.imgbox_brushes.get_selection()
        self.enable_disable_brush_size_range(
            self.terrain.issizeablepaintbrush(selection_index)
            )
        self.terrain.selectpaintbrush(selection_index)

    def on_brush_size(self, event):
        self.choicebook_tool.set_tools_brush_size(
            self.slider_size.get_value()
            )

    def enable_disable_brush_size_range(self, should_enable):
        self.slider_size.Enable(should_enable)
        self.button_range.Enable(should_enable)

    def on_brush_size_range(self, event):
        dlg = RangeDialog(
                    self,
                    self.slider_size.get_min(),
                    self.slider_size.get_max()
                    )
        if dlg.ShowModal() == wx.ID_OK:
            min_value = dlg.get_min()
            max_value = dlg.get_max()
            self.slider_size.set_range( min_value, max_value )
            self.button_range.SetLabel(
                "[%s,%s]" % (min_value, max_value)
                )
        dlg.Destroy()

    def on_select_tool(self, event):
        self.choicebook_tool.SetSelection( self.imgbox_tools.get_selection() )
        self.choicebook_tool.select_current_tool()

    def refresh(self):
        self.choicebook_tool.refresh()

    def persist(self):
        data = {
            'selected brush': self.imgbox_brushes.get_selection(),
            'brush size': self.slider_size.get_value(),
            'brush min size': self.slider_size.get_min(),
            'brush max size': self.slider_size.get_max(),
            'tools': self.choicebook_tool.persist_outgui()
            }
        return [
            create_window,
            (),  # no parameters for create function
            data
            ]

    def restore(self, data_list):
        # set brush and tool images
        # load images only once to speed things up
        if not self.restored:
            # brushes
            fileserver = servers.get_file_server()
            brushes_nb = self.terrain.paintbrushcount()
            for i in range(brushes_nb):
                path = self.terrain.getpaintbrushthumbnail(i)
                path = fileserver.manglepath(path)
                bmp = wx.Bitmap(path, wx.BITMAP_TYPE_ANY)
                if self.terrain.issizeablepaintbrush(i):
                    img_name = 'Resizable'
                else:
                    img_name = str( self.terrain.getpaintbrushsizebyindex(i) )
                self.imgbox_brushes.append_image(bmp, name=img_name)
            
            # tools
            tool_names = ['Raise', 'Flatten', 'Slope', 'Smooth', 'Noise',
                'Paint', 'Grass', 'Hole']
            for name in tool_names:
                path = "outgui:images/terrain/%s.bmp" % name.lower()
                path = format.mangle_path( path )
                bmp = wx.Bitmap(path)
                self.imgbox_tools.append_image( bmp, name=name )
            
            self.restored = True
        
        # restore brush size range
        data = data_list[0]
        min_value = 1
        if data.has_key('brush min size'):
            min_value = data['brush min size']
        max_value = 20
        if data.has_key('brush max size'):
            max_value = data['brush max size']
        self.slider_size.set_range(min_value, max_value )
        self.button_range.SetLabel(
            "[%s,%s]" % (min_value, max_value)
            )
        
        # restore conjurer terrain object
        self.terrain.selectpaintbrush( data['selected brush'] )
        size = data['brush size']
        if size < min_value:
            size = min_value
        elif size > max_value:
            size = max_value
        self.choicebook_tool.set_tools_brush_size( size )
        
        # restore controls
        if self.terrain.getselectedpaintbrush() != wx.NOT_FOUND:
            self.imgbox_brushes.set_selection(
                self.terrain.getselectedpaintbrush() 
                )
            selection_index = self.imgbox_brushes.get_selection()
            self.enable_disable_brush_size_range(
                self.terrain.issizeablepaintbrush(
                    selection_index
                    )
                )    
        self.slider_size.set_value( self.terrain.getpaintbrushsize() )
        self.imgbox_tools.set_selection( self.terrain.getselectedtool() )
        
        # restore children
        self.choicebook_tool.restore( data['tools'] )

    def is_restorable(self, data_list):
        data = data_list[0]
        return data['selected brush'] < self.terrain.paintbrushcount()


#------------------------------------------------------------------------------
# RangeDialog class
class RangeDialog(wx.Dialog):
    def __init__(self, parent, min_value, max_value):
        wx.Dialog.__init__(
            self, 
            parent, 
            1,
            "Brush size range",
            style = wx.DEFAULT_DIALOG_STYLE|wx.RESIZE_BORDER|wx.TAB_TRAVERSAL
            )
        self.label_enter = wx.StaticText(
                                    self, 
                                    -1, 
                                    "Enter the desired brush size range"
                                    )
        self.label_min = wx.StaticText(
                                self, 
                                -1, 
                                "Mininum size",
                                style=wx.ALIGN_RIGHT
                                )
        self.int_min = intctrl.IntCtrl(
                                self, 
                                value=min_value, 
                                limited=True,
                                min=1,
                                max=999, 
                                size=(40,-1), 
                                style=wx.TE_RIGHT
                                )
        self.label_max = wx.StaticText(
                                self, 
                                -1, 
                                "Maximum size", 
                                style=wx.ALIGN_RIGHT
                                )
        self.int_max = intctrl.IntCtrl(
                                self, 
                                value=max_value, 
                                limited=True,
                                min=1, 
                                max=999, 
                                size=(40,-1),
                                style=wx.TE_RIGHT
                                )
        self.button_ok = wx.Button(self, -1, "&OK")
        self.button_cancel = wx.Button(self, wx.ID_CANCEL, "&Cancel")

        self.__set_properties()
        self.__do_layout()
        self.__bind_events()

    def __set_properties(self):
        format.align_labels( [self.label_min, self.label_max] )
        self.button_ok.SetDefault()

    def __do_layout(self):
        sizer_layout = wx.BoxSizer(wx.VERTICAL)
        sizer_buttons = wx.BoxSizer(wx.HORIZONTAL)
        sizer_range = wx.BoxSizer(wx.HORIZONTAL)
        sizer_range.Add(
            self.label_enter,
            0, 
            wx.ADJUST_MINSIZE
            )
        sizer_layout.Add(
            sizer_range,
            0,
            wx.ALL,
            cfg.BORDER_WIDTH
            )
        grid_sizer_min_max = wx.FlexGridSizer(
                                        2, 
                                        2, 
                                        cfg.BORDER_WIDTH, 
                                        cfg.BORDER_WIDTH * 2
                                        )
        grid_sizer_min_max.AddGrowableCol(1)
        grid_sizer_min_max.Add(
            self.label_min, 
            0, 
            cfg.LABEL_ALIGN|wx.ALIGN_CENTER_VERTICAL
            )
        grid_sizer_min_max.Add(
            self.int_min
            )
        grid_sizer_min_max.Add(
            self.label_max, 
            0, 
            cfg.LABEL_ALIGN|wx.ALIGN_CENTER_VERTICAL
            )
        grid_sizer_min_max.Add(
            self.int_max
            )
        sizer_layout.Add(
            grid_sizer_min_max,
            1,
            wx.ALL|wx.EXPAND,
            cfg.BORDER_WIDTH
            )
        horizontal_line = wx.StaticLine(
                                self, 
                                -1
                                )
        sizer_layout.Add(
            horizontal_line,
            0, 
            wx.EXPAND|wx.LEFT|wx.RIGHT,
            cfg.BORDER_WIDTH
            )
        sizer_buttons.Add(
            self.button_ok, 
            0, 
            wx.ADJUST_MINSIZE
            )
        sizer_buttons.Add(
            self.button_cancel,
            0, 
            wx.ADJUST_MINSIZE|wx.LEFT,
            cfg.BORDER_WIDTH
            )
        sizer_layout.Add(
            sizer_buttons, 
            0, 
            wx.ALL|wx.ALIGN_RIGHT, 
            cfg.BORDER_WIDTH
            )
        self.SetSizerAndFit(sizer_layout)

    def __bind_events(self):
        self.Bind(wx.EVT_BUTTON, self.on_ok, self.button_ok)
        self.Bind(wx.EVT_BUTTON, self.on_cancel, self.button_cancel)

    def on_ok(self, event):
        if self.int_min.get_value() > self.int_max.get_value():
            cjr.show_error_message("Invalid range")
        else:
            self.EndModal( wx.ID_OK )

    def on_cancel(self, event):
        self.EndModal( wx.ID_CANCEL )

    def get_min(self):
        return self.int_min.get_value()

    def get_max(self):
        return self.int_max.get_value()


#------------------------------------------------------------------------------
# create_window function
def create_window(parent):
    try:
        try:
            wx.BeginBusyCursor()
            win = ToolDialog(
                        parent
                        )
        finally:
            wx.EndBusyCursor()
    except:
        # make sure any errors are not hidden
        raise
    return win