##\file infodlg.py
##\brief Generic tool information dialog

import wx

import pynebula

import app
import format
import mathex
import togwin
import toolscmds
import trn

import conjurerframework as cjr

# InfoDialog class
class InfoDialog(togwin.ChildToggableDialog):
    """Generic tool information dialog"""
    
    def __init__(self, parent):
        togwin.ChildToggableDialog.__init__(
            self, "Tool info", parent
            )
        
        # Number of decimals used in all float representations
        self.decimals = 2
        
        # Tool ids and paths
        self.ID_TerrainRaise = wx.NewId()
        self.ID_TerrainFlatten = wx.NewId()
        self.ID_TerrainSlope = wx.NewId()
        self.ID_TerrainSmooth = wx.NewId()
        self.ID_TerrainNoise = wx.NewId()
        self.ID_TerrainPaint = wx.NewId()
        self.ID_TerrainHole = wx.NewId()
        self.ID_Measure = wx.NewId()
        self.tool_paths = {
            toolscmds.ID_TranslateObjTool: app.get_translate_tool().getfullname(),
            toolscmds.ID_RotateObjTool: app.get_rotate_tool().getfullname(),
            toolscmds.ID_ScaleObjTool: app.get_scale_tool().getfullname(),
            self.ID_TerrainRaise: trn.get_terrain_tool_path('toolRaiseLow'),
            self.ID_TerrainFlatten: trn.get_terrain_tool_path('toolFlatten'),
            self.ID_TerrainSlope: trn.get_terrain_tool_path('toolSlope'),
            self.ID_TerrainSmooth: trn.get_terrain_tool_path('toolSmooth'),
            self.ID_TerrainNoise: trn.get_terrain_tool_path('toolNoise'),
            self.ID_TerrainPaint: trn.get_terrain_tool_path('toolPaint'),
            self.ID_TerrainHole: trn.get_terrain_tool_path('toolHole'),
            self.ID_Measure: app.get_measure_tool().getfullname()
            }
        self.current_tool = toolscmds.ID_NullTool
        
        # Label names and values. They're init with an empty string,
        # but with a width long enough to contain any name/value.
        # Each tool will then update the labels as it needs.
        self.label_names = []
        self.label_values = []
        for i in range(5):
            self.label_names.append( wx.StaticText(self, -1, "",
                size=(80,-1), style=wx.ALIGN_RIGHT) )
            self.label_values.append( wx.StaticText(self, -1, "",
                size=(125,-1)) )
        
        # Short help displayed while waiting for the first signal
        # (created last to be on top of other labels)
        self.label_help = wx.StaticText(self, -1, "For most tools, click and" \
            " drag to display\ninformation.\n\nSome tools doesn't give " \
            "information at all.", pos=(10,10))
        
        self.__do_layout()
        self.__bind_events()
    
    def __do_layout(self):
        sizer_border = wx.BoxSizer(wx.VERTICAL)
        sizer_layout = wx.BoxSizer(wx.VERTICAL)
        flags = wx.EXPAND
        for i in range(len(self.label_names)):
            sizer_field = wx.BoxSizer(wx.HORIZONTAL)
            sizer_field.Add(self.label_names[i], 0, wx.FIXED_MINSIZE, 0)
            sizer_field.Add(self.label_values[i], 0, wx.LEFT|wx.FIXED_MINSIZE, 5)
            sizer_layout.Add(sizer_field, 0, flags, 5)
            flags = wx.TOP|wx.EXPAND
        sizer_border.Add(sizer_layout, 1, wx.ALL|wx.EXPAND, 10)
        self.SetAutoLayout(True)
        self.SetSizer(sizer_border)
        sizer_border.Fit(self)
        sizer_border.SetSizeHints(self)
        self.Layout()
    
    def __bind_events(self):
        self.__bind_signal(toolscmds.ID_TranslateObjTool, 'refreshinfo', 'ontranslateobject')
        self.__bind_signal(toolscmds.ID_RotateObjTool, 'refreshinfo', 'onrotateobject')
        self.__bind_signal(toolscmds.ID_ScaleObjTool, 'refreshinfo', 'onscaleobject')
        self.__bind_signal(self.ID_TerrainRaise, 'refreshinfo', 'onterrainraise')
        self.__bind_signal(self.ID_TerrainFlatten, 'refreshinfo', 'onterrainflatten')
        self.__bind_signal(self.ID_TerrainSlope, 'refreshinfo', 'onterrainslope')
        self.__bind_signal(self.ID_TerrainSmooth, 'refreshinfo', 'onterrainsmooth')
        self.__bind_signal(self.ID_TerrainNoise, 'refreshinfo', 'onterrainnoise')
        self.__bind_signal(self.ID_TerrainPaint, 'refreshinfo', 'onterrainpaint')
        self.__bind_signal(self.ID_TerrainHole, 'refreshinfo', 'onterrainhole')
        self.__bind_signal(self.ID_Measure, 'refreshinfo', 'onmeasure')
    
    def __bind_signal(self, tool_id, signal_name, receiver_method):
        try:
            tool = pynebula.lookup( self.tool_paths[tool_id] )
        except:
            cjr.show_error_message(
                "Tool '" + self.tool_paths[tool_id] + "' not found"
                )
        else:
            pynebula.pyBindSignal(tool, signal_name, self, receiver_method, 0)
    
    def __unbind_signal(self, tool_id, signal_name):
        try:
            tool = pynebula.lookup( self.tool_paths[tool_id] )
        except:
            cjr.show_error_message(
                "Tool '" + self.tool_paths[tool_id] + "' not found"
                )
        else:
            pynebula.pyUnbindTargetObject(tool, signal_name, self)
    
    def __clear_labels(self):
        self.label_help.Hide()
        for i in range(len(self.label_names)):
            self.label_names[i].SetLabel("")
            self.label_values[i].SetLabel("")
    
    def __init_labels(self, tool_id, labels):
        # Init labels if tool has changed
        if self.current_tool != tool_id:
            self.__clear_labels()
            for i in range(len(labels)):
                self.label_names[i].SetLabel( labels[i] )
            self.Layout()
            self.current_tool = tool_id
    
    def __set_common_values(self, tool):
        ok, x0, y0, z0 = tool.getfirstposition()
        self.label_values[0].SetLabel( format.vector3_to_str(
            (x0,y0,z0), self.decimals) )
        ok, x1, y1, z1 = tool.getlastposition()
        self.label_values[1].SetLabel( format.vector3_to_str(
            (x1,y1,z1), self.decimals) )
        self.label_values[2].SetLabel( str( round(
            mathex.distance3D((x0,y0,z0), (x1,y1,z1)), self.decimals) ) )
    
    def ontranslateobject(self):
        # Set label names
        self.__init_labels( toolscmds.ID_TranslateObjTool,
            ["Start:", "End:", "Distance:"] )
        
        # Set new info values
        tool = pynebula.lookup( self.tool_paths[toolscmds.ID_TranslateObjTool] )
        self.__set_common_values(tool)
    
    def onrotateobject(self):
        # Set label names
        self.__init_labels( toolscmds.ID_RotateObjTool,
            ["Angle:"] )
        
        # Set new info values
        tool = pynebula.lookup( self.tool_paths[toolscmds.ID_RotateObjTool] )
        angle = tool.getcurrentrotangle()
        self.label_values[0].SetLabel( str( round(angle, self.decimals) ) )
    
    def onscaleobject(self):
        # Set label names
        self.__init_labels( toolscmds.ID_ScaleObjTool,
            ["Scale:"] )
        
        # Set new info values
        tool = pynebula.lookup( self.tool_paths[toolscmds.ID_ScaleObjTool] )
        scale = tool.getcurrentscaling()
        self.label_values[0].SetLabel( str( round(scale, self.decimals) ) )
    
    def onterrainraise(self):
        # Set label names
        self.__init_labels( self.ID_TerrainRaise,
            ["Start:", "End:", "Distance:"] )
        
        # Set new info values
        tool = pynebula.lookup( self.tool_paths[self.ID_TerrainRaise] )
        self.__set_common_values( tool )
    
    def onterrainflatten(self):
        # Set label names
        self.__init_labels( self.ID_TerrainFlatten,
            ["Start:", "End:", "Distance:"] )
        
        # Set new info values
        tool = pynebula.lookup( self.tool_paths[self.ID_TerrainFlatten] )
        self.__set_common_values( tool )
    
    def onterrainslope(self):
        # Set label names
        self.__init_labels( self.ID_TerrainSlope,
            ["Start:", "End:", "Distance:", "Slope:"] )
        
        # Set new info values
        tool = pynebula.lookup( self.tool_paths[self.ID_TerrainSlope] )
        self.__set_common_values( tool )
        slope = tool.getslope()
        self.label_values[3].SetLabel( str( round(slope, self.decimals) ) )
    
    def onterrainsmooth(self):
        # Set label names
        self.__init_labels( self.ID_TerrainSmooth,
            ["Start:", "End:", "Distance:"] )
        
        # Set new info values
        tool = pynebula.lookup( self.tool_paths[self.ID_TerrainSmooth] )
        self.__set_common_values( tool )
    
    def onterrainnoise(self):
        # Set label names
        self.__init_labels( self.ID_TerrainNoise,
            ["Start:", "End:", "Distance:"] )
        
        # Set new info values
        tool = pynebula.lookup( self.tool_paths[self.ID_TerrainNoise] )
        self.__set_common_values( tool )
    
    def onterrainpaint(self):
        # Set label names
        self.__init_labels( self.ID_TerrainPaint,
            ["Start:", "End:", "Distance:"] )
        
        # Set new info values
        tool = pynebula.lookup( self.tool_paths[self.ID_TerrainPaint] )
        self.__set_common_values( tool )
    
    def onterrainhole(self):
        # Set label names
        self.__init_labels( self.ID_TerrainHole,
            ["Start:", "End:", "Distance:"] )
        
        # Set new info values
        tool = pynebula.lookup( self.tool_paths[self.ID_TerrainHole] )
        self.__set_common_values( tool )

    def onmeasure(self):
        # Set label names
        self.__init_labels( self.ID_TerrainSlope,
            ["Start:", "End:", "Distance:", "Path distance:", "Slope:"] )

        # Set new info values
        tool = pynebula.lookup( self.tool_paths[self.ID_Measure] )
        self.__set_common_values( tool )
        terraindistance, slope = tool.getmeasureinfo()
        self.label_values[3].SetLabel( str( round(terraindistance, self.decimals) ) )
        self.label_values[4].SetLabel( str( round(slope, self.decimals) ) )

    def persist(self):
        return [
            create_window,
            (), # no parameters for create function
            ]

    def __del__(self):
        self.__unbind_signal(toolscmds.ID_TranslateObjTool, 'refreshinfo')
        self.__unbind_signal(toolscmds.ID_RotateObjTool, 'refreshinfo')
        self.__unbind_signal(toolscmds.ID_ScaleObjTool, 'refreshinfo')
        self.__unbind_signal(self.ID_TerrainRaise, 'refreshinfo')
        self.__unbind_signal(self.ID_TerrainFlatten, 'refreshinfo')
        self.__unbind_signal(self.ID_TerrainSlope, 'refreshinfo')
        self.__unbind_signal(self.ID_TerrainSmooth, 'refreshinfo')
        self.__unbind_signal(self.ID_TerrainNoise, 'refreshinfo')
        self.__unbind_signal(self.ID_TerrainPaint, 'refreshinfo')
        self.__unbind_signal(self.ID_TerrainHole, 'refreshinfo')
        self.__unbind_signal(self.ID_Measure, 'refreshinfo')

# create_window function
def create_window(parent):
    return InfoDialog(parent)
