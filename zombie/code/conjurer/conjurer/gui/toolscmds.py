##\file toolscmds.py
##\brief Tools menu commands

import wx

import pynebula

import app
import infodlg
import menu
import multiplacedlg
import placedlg
import servers
import trncmds
import rnsgame
import waypointpathstool
import sndsrcselectdlg

# Tools menu IDs
ID_NullTool = wx.NewId()
ID_SelectObjTool = wx.NewId()
ID_TranslateObjTool = wx.NewId()
ID_RotateObjTool = wx.NewId()
ID_ScaleObjTool = wx.NewId()
ID_PlaceObjTool = wx.NewId()
ID_MultiplaceObjTool = wx.NewId()
ID_TrnEditTool = wx.NewId()
ID_WaypointsTool = wx.NewId()
ID_MeasureTool = wx.NewId()
ID_PlaceSettings = wx.NewId()
ID_MultiplaceSettings = wx.NewId()
ID_CircleTriggerTool = wx.NewId()
ID_PolygonTriggerTool = wx.NewId()
ID_CircleSoundSourceTool = wx.NewId()
ID_PolygonSoundSourceTool = wx.NewId()
ID_ToolInfo = wx.NewId()
ID_GameTool = wx.NewId()
ID_WaypointsPathTool = wx.NewId()

# Add tool ids here too (only those in the radio group)
tools_ids = [
    ID_NullTool,
    ID_SelectObjTool,
    ID_TranslateObjTool,
    ID_RotateObjTool,
    ID_ScaleObjTool,
    ID_PlaceObjTool,
    ID_MultiplaceObjTool,
    ID_TrnEditTool,
    ID_WaypointsTool,
    ID_CircleTriggerTool,
    ID_PolygonTriggerTool,
    ID_CircleSoundSourceTool,
    ID_PolygonSoundSourceTool,
    ID_MeasureTool,
    ID_WaypointsPathTool
    ]

# Ids understood by the object state for each of its tools
object_tool_ids = {
    ID_TranslateObjTool: 0,
    ID_RotateObjTool: 1,
    ID_ScaleObjTool: 2,
    ID_SelectObjTool: 3,
    ID_PlaceObjTool: 4,
    ID_MultiplaceObjTool: 5,
    ID_WaypointsTool: 6,
    ID_CircleTriggerTool: 7,
    ID_PolygonTriggerTool: 8,
    ID_CircleSoundSourceTool: 9,
    ID_PolygonSoundSourceTool: 10,
    ID_MeasureTool: 11,
    ID_WaypointsPathTool: 12
    }


# ToolsMenu class
class ToolsMenu(menu.Menu):
    """Err... The tools menu"""

    def __init__(self, frame):
        menu.Menu.__init__(self, frame)
        self.first_refresh = True

    def create(self):
        # menu items
        self.AppendRadioItem( ID_NullTool, "&Null tool" )
        self.AppendRadioItem( ID_SelectObjTool, "&Select object tool" )
        self.AppendRadioItem( ID_TranslateObjTool, "&Translate object tool" )
        self.AppendRadioItem( ID_RotateObjTool, "&Rotate object tool" )
        self.AppendRadioItem( ID_ScaleObjTool, "&Scale object tool" )
        self.AppendRadioItem( ID_PlaceObjTool, "&Place object tool" )
        self.AppendRadioItem( ID_MultiplaceObjTool, "&Multiplace object tool" )
        self.AppendRadioItem( ID_TrnEditTool, "Terrain &edit tool" )
        self.AppendRadioItem( ID_WaypointsTool, "&Waypoints creation tool" )
        self.AppendRadioItem(
            ID_WaypointsPathTool, 
            "&Waypoint Paths creation tool" 
            )
        self.AppendRadioItem(
            ID_CircleTriggerTool, 
            "Circle trigger creation tool" 
            )
        self.AppendRadioItem(
            ID_PolygonTriggerTool, 
            "Polygon trigger creation tool" 
            )
        self.AppendRadioItem(
            ID_CircleSoundSourceTool, 
            "Circle sound source creation tool"
            )
        self.AppendRadioItem(
            ID_PolygonSoundSourceTool, 
            "Polygon sound source creation tool"
            )
        self.AppendRadioItem( ID_MeasureTool, "Terrain distance measure tool" )
        self.AppendSeparator()
        self.AppendCheckItem(
            ID_PlaceSettings, 
            "Place object settings" 
            )
        self.AppendCheckItem(
            ID_MultiplaceSettings, 
            "Multiplace object settings" 
            )
        self.Append(
            ID_GameTool, 
            "Renaissance Game Init" 
            )
        self.AppendSeparator()
        self.AppendCheckItem(
            ID_ToolInfo, 
            "Tool &info" 
            )

        # bindings
        self.bind_function(ID_NullTool, self.on_null_tool)
        self.bind_function(ID_NullTool, self.on_null_tool)
        self.bind_function(ID_SelectObjTool, self.on_select_object_tool)
        self.bind_function(ID_TranslateObjTool, self.on_translate_object_tool)
        self.bind_function(ID_RotateObjTool, self.on_rotate_object_tool)
        self.bind_function(ID_ScaleObjTool, self.on_scale_object_tool)
        self.bind_function(ID_PlaceObjTool, self.on_place_object_tool)
        self.bind_function(ID_MultiplaceObjTool, self.on_multiplace_object_tool)
        self.bind_function(ID_TrnEditTool, self.on_terrain_tool)
        self.bind_function(ID_WaypointsTool, self.on_waypoints_tool)
        self.bind_function(ID_WaypointsPathTool, self.on_waypointspath_tool)
        self.bind_function(ID_CircleTriggerTool, self.on_circle_trigger_tool)
        self.bind_function(ID_PolygonTriggerTool, self.on_polygon_trigger_tool)
        self.bind_function(
            ID_CircleSoundSourceTool, 
            self.on_circle_sound_source_tool
            )
        self.bind_function(
            ID_PolygonSoundSourceTool, 
            self.on_polygon_sound_source_tool
            )
        self.bind_function(ID_MeasureTool, self.on_measure_tool)
        self.bind_function(ID_GameTool, self.on_game_tool)
        togwinmgr = self.__get_togwinmgr()
        togwinmgr.add_window(
            placedlg.create_window,
            self.FindItemById(ID_PlaceSettings)
            )
        togwinmgr.add_window(
            multiplacedlg.create_window,
            self.FindItemById(ID_MultiplaceSettings)
            )
        togwinmgr.add_window(
            infodlg.create_window,
            self.FindItemById(ID_ToolInfo)
            )
        togwinmgr.add_window(
            waypointpathstool.create_window,
            self.FindItemById(ID_WaypointsPathTool)
            )

    def refresh(self):
        if self.first_refresh:
            # Do the binding here because the object state doesn't exist yet
            # when this menu is created
            pynebula.pyBindSignal(
                app.get_object_state(), 
                'toolselected',
                self, 
                'ontoolselectedsignal', 
                0
                )
            self.first_refresh = False

    def ontoolselectedsignal(self):
        """Check the menu option and tool button for the new selected tool"""
        tool_id = app.get_object_state().getselectedtoolindex()
        for menu_id in object_tool_ids.keys():
            if object_tool_ids[menu_id] == tool_id:
                self.Check( menu_id, True )
                self.__toggle_tool( menu_id, True )

    def on_null_tool(self, evt):
        self.__select_null_tool()

    def __select_null_tool(self):
        """Select the null tool"""
        self.Check( ID_NullTool, True )
        self.__toggle_tool( ID_NullTool, True )
        self.__set_conjurer_state("editor")

    def on_select_object_tool(self, evt):
        """Select the object selection tool"""
        self.Check( ID_SelectObjTool, True )
        self.__toggle_tool( ID_SelectObjTool, True )
        self.__set_conjurer_state("object")
        self.__select_tool(
            object_tool_ids[ID_SelectObjTool] 
            )

    def on_translate_object_tool(self, evt):
        """Select the object translation tool"""
        self.Check( ID_TranslateObjTool, True )
        self.__toggle_tool( ID_TranslateObjTool, True )
        self.__set_conjurer_state("object")
        self.__select_tool(
            object_tool_ids[ID_TranslateObjTool] 
            )

    def on_rotate_object_tool(self, evt):
        """Select the object rotation tool"""
        self.Check( ID_RotateObjTool, True )
        self.__toggle_tool( ID_RotateObjTool, True )
        self.__set_conjurer_state("object")
        self.__select_tool(
            object_tool_ids[ID_RotateObjTool] 
            )

    def on_scale_object_tool(self, evt):
        """Select the object scaling tool"""
        self.Check( ID_ScaleObjTool, True )
        self.__toggle_tool( ID_ScaleObjTool, True )
        self.__set_conjurer_state("object")
        self.__select_tool(
            object_tool_ids[ID_ScaleObjTool] 
            )

    def on_place_object_tool(self, evt):
        """Select the object placing tool"""
        self.Check( ID_PlaceObjTool, True )
        self.__toggle_tool( ID_PlaceObjTool, True )
        self.__set_conjurer_state("object")
        self.__select_tool(
            object_tool_ids[ID_PlaceObjTool] 
            )
        self.__show_toggable_window(ID_PlaceSettings)

    def on_multiplace_object_tool(self, evt):
        """Select the object multiplacing tool"""
        self.Check( ID_MultiplaceObjTool, True )
        self.__toggle_tool( ID_MultiplaceObjTool, True )
        self.__set_conjurer_state("object")
        self.__select_tool(
            object_tool_ids[ID_MultiplaceObjTool] 
            )
        self.__show_toggable_window(ID_MultiplaceSettings)

    def on_terrain_tool(self, evt):
        """Select the terrain edit tool"""
        self.Check( ID_TrnEditTool, True )
        self.__toggle_tool( ID_TrnEditTool, True )
        self.__set_conjurer_state("terrain")
        if evt is not None:
            self.__show_toggable_window(trncmds.ID_EditTool)
            self.__show_toggable_window(trncmds.ID_TextureLayers)

    def on_waypoints_tool(self, evt):
        """Select the waypoints creation tool"""
        self.Check( ID_WaypointsTool, True )
        self.__toggle_tool( ID_WaypointsTool, True )
        self.__set_conjurer_state("object")
        self.__select_tool(
            object_tool_ids[ID_WaypointsTool] 
            )

    def on_waypointspath_tool(self, evt):
        """Select the waypoints path creation tool"""
        self.Check( ID_WaypointsPathTool, True )
        self.__toggle_tool( ID_WaypointsPathTool, True )
        self.__set_conjurer_state("object")
        self.__select_tool(
            object_tool_ids[ID_WaypointsPathTool] 
            )

    def on_circle_trigger_tool(self, evt):
        """Select the circular area trigger creation tool"""
        self.Check( ID_CircleTriggerTool, True )
        self.__toggle_tool( ID_CircleTriggerTool, True )
        self.__set_conjurer_state("object")
        self.__select_tool(
            object_tool_ids[ID_CircleTriggerTool] 
            )

    def on_polygon_trigger_tool(self, evt):
        """Select the polygonal area trigger creation tool"""
        self.Check( ID_PolygonTriggerTool, True )
        self.__toggle_tool( ID_PolygonTriggerTool, True )
        self.__set_conjurer_state("object")
        self.__select_tool(
            object_tool_ids[ID_PolygonTriggerTool] 
            )

    def __prompt_user_for_sound_source_class(self):
        try:
            try:
                wx.BeginBusyCursor()
                dlg  = sndsrcselectdlg.SoundSourceSelectionDialog( 
                            self.get_frame() 
                            )    
                result = dlg.ShowModal()
                class_name = None
                if result == wx.ID_OK:
                    class_name = str(dlg.sound_source_class)
                dlg.Destroy()
            finally:
                wx.EndBusyCursor()
        except:
            # make sure any errors are not hidden
            raise
        return class_name

    def on_circle_sound_source_tool(self, evt):
        """Select the circular sound source creation tool"""
        class_name = self.__prompt_user_for_sound_source_class()
        if class_name is None:
            self.__select_null_tool()
        else:
            self.Check(ID_CircleSoundSourceTool, True)
            self.__toggle_tool(
                ID_CircleSoundSourceTool, 
                True
                )
            self.__set_conjurer_state("object")
            self.__select_tool_with_parameter(
                object_tool_ids[ID_CircleSoundSourceTool],
                class_name
                )

    def __is_polygonal_sound_source(self, entity):
        return entity.isa('nesoundsource') and \
            entity.hascomponent('ncTriggerShape')

    def on_polygon_sound_source_tool(self, evt):
        """Select the polygonal sound source creation tool
        If the selected item in the ingui is a polygonal sound source, 
        do NOT prompt the user for a source source class since 
        the tool will be used to edit the selected sound source area."""
        class_name = None
        state = app.get_object_state()
        num_entities = state.getselectioncount()
        if num_entities == 1:
            entity = state.getselectedentity(0)
            if self.__is_polygonal_sound_source(entity):
                class_name = entity.getclass()
        if class_name is None:
            class_name = self.__prompt_user_for_sound_source_class()
        if class_name is None:
            self.__select_null_tool()
        else:
            self.Check( ID_PolygonSoundSourceTool, True )
            self.__toggle_tool(
                ID_PolygonSoundSourceTool, 
                True
                )
            self.__set_conjurer_state("object")
            self.__select_tool_with_parameter(
                object_tool_ids[ID_PolygonSoundSourceTool],
                class_name
                )   

    def on_measure_tool(self, evt):
        """Select the terrain distance measure tool"""
        self.Check( ID_MeasureTool, True )
        self.__toggle_tool( ID_MeasureTool, True )
        self.__set_conjurer_state("object")
        self.__select_tool(
            object_tool_ids[ID_MeasureTool] 
            )
        self.__show_toggable_window(ID_ToolInfo)

    def on_game_tool(self, evt):
        """Init a renaissance game"""
        game_win = rnsgame.create_window( self.GetParent() )
        game_win.Show()

    def __get_toolbar(self):
        return self.get_frame().get_toolbar()

    def __show_toggable_window(self, window_id):
        self.__get_togwinmgr().show_window(window_id)

    def __get_togwinmgr(self):
        return self.get_frame().get_togwinmgr()

    def __select_tool(self, tool_id):
        app.get_object_state().selecttool(tool_id)

    def __select_tool_with_parameter(self, tool_id, parameter_one):
        app.get_object_state().selecttoolwithparameter(
            tool_id, 
            parameter_one
            )

    def __toggle_tool(self, tool_id, boolean):
        self.__get_toolbar().ToggleTool(
            tool_id, boolean
            )

    def __set_conjurer_state(self, state_string):
        servers.get_conjurer().setstate(state_string)
