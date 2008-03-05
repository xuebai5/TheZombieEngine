##\file toolbar.py
##\brief Tool bar for the main window

import wx

import app
import debugcmds
import editcmds
import filecmds
import levelcmds
import nebulaguisettings as guisettings
import scriptmgrdlg
import servers
import toolscmds
import viewcmds

import conjurerframework as cjr


# ToolBar class
class ToolBar(wx.ToolBar):
    """Tool bar for the main window"""

    def __init__(self, frame):
        wx.ToolBar.__init__(
            self, 
            frame, 
            -1,
            style=wx.TB_HORIZONTAL|wx.TB_DOCKABLE
            )
        self.custom_tools = {}
        self.SetToolBitmapSize( (16, 16) )

    def create(self):
        """Create the application toolbar"""
        fileserver = servers.get_file_server()
        self.AddLabelTool(
            filecmds.ID_New, 
            "new", 
            wx.Bitmap(
                fileserver.manglepath("outgui:images/tools/new.bmp"),
                wx.BITMAP_TYPE_ANY
                ), 
            wx.NullBitmap, 
            wx.ITEM_NORMAL,
            "New File", 
            ""
            )
        self.AddLabelTool(
            filecmds.ID_Open, 
            "open", 
            wx.Bitmap(
                fileserver.manglepath("outgui:images/tools/open.bmp"),
                wx.BITMAP_TYPE_ANY
                ), 
            wx.NullBitmap, 
            wx.ITEM_NORMAL,
            "Open File", 
            ""
            )
        self.AddLabelTool(
            filecmds.ID_Save, 
            "save", 
            wx.Bitmap(
                fileserver.manglepath("outgui:images/tools/save.bmp"),
                wx.BITMAP_TYPE_ANY
                ), 
            wx.NullBitmap, 
            wx.ITEM_NORMAL,
            "Save File", 
            ""
            )
        self.AddLabelTool(
            levelcmds.ID_Save, 
            "save level", 
            wx.Bitmap(
                fileserver.manglepath("outgui:images/tools/save_level.bmp"),
                wx.BITMAP_TYPE_ANY
                ), 
            wx.NullBitmap, 
            wx.ITEM_NORMAL,
            "Save Level", 
            ""
            )
        self.AddLabelTool(
            filecmds.ID_SaveAll, 
            "saveall", 
            wx.Bitmap(
                fileserver.manglepath("outgui:images/tools/saveall.bmp"),
                wx.BITMAP_TYPE_ANY
                ), 
            wx.NullBitmap, 
            wx.ITEM_NORMAL,
            "Save All Files", 
            ""
            )
        self.AddSeparator()
        self.AddLabelTool(
            editcmds.ID_Undo, 
            "undo", 
            wx.Bitmap(
                fileserver.manglepath("outgui:images/tools/undo.bmp"),
                wx.BITMAP_TYPE_ANY
                ), 
            wx.NullBitmap, 
            wx.ITEM_NORMAL, 
            "Undo", 
            ""
            )
        self.AddLabelTool(
            editcmds.ID_Redo, 
            "redo", 
            wx.Bitmap(
                fileserver.manglepath("outgui:images/tools/redo.bmp"),
                wx.BITMAP_TYPE_ANY
                ),
            wx.NullBitmap, 
            wx.ITEM_NORMAL, 
            "Redo", 
            ""
            )
        self.AddSeparator()
        self.AddLabelTool(
            toolscmds.ID_NullTool, 
            "null tool", 
            wx.Bitmap(
                fileserver.manglepath("outgui:images/tools/null.bmp"),
                wx.BITMAP_TYPE_ANY
                ), 
            wx.NullBitmap, 
            wx.ITEM_RADIO,
            "Do Nothing Tool", 
            ""
            )
        self.AddLabelTool(
            toolscmds.ID_SelectObjTool, 
            "select object",
            wx.Bitmap(
                fileserver.manglepath("outgui:images/tools/select.bmp"),
                wx.BITMAP_TYPE_ANY
                ), 
            wx.NullBitmap, 
            wx.ITEM_RADIO,
            "Select Object Tool", 
            ""
            )
        self.AddLabelTool(
            toolscmds.ID_TranslateObjTool, 
            "translate object",
            wx.Bitmap(
                fileserver.manglepath("outgui:images/tools/translate.bmp"),
                wx.BITMAP_TYPE_ANY
                ), 
            wx.NullBitmap, 
            wx.ITEM_RADIO,
            "Translate Object Tool", 
            ""
            )
        self.AddLabelTool(
            toolscmds.ID_RotateObjTool, 
            "rotate object",
            wx.Bitmap(
                fileserver.manglepath("outgui:images/tools/rotate.bmp"),
                wx.BITMAP_TYPE_ANY
                ), 
            wx.NullBitmap, 
            wx.ITEM_RADIO,
            "Rotate Object Tool", 
            ""
            )
        self.AddLabelTool(
            toolscmds.ID_ScaleObjTool, 
            "scale object",
            wx.Bitmap(
                fileserver.manglepath("outgui:images/tools/scale.bmp"),
                wx.BITMAP_TYPE_ANY
                ), 
            wx.NullBitmap, 
            wx.ITEM_RADIO,
            "Scale Object Tool", 
            ""
            )
        self.AddLabelTool(
            toolscmds.ID_PlaceObjTool, 
            "place object",
            wx.Bitmap(
                fileserver.manglepath("outgui:images/tools/place.bmp"),
                wx.BITMAP_TYPE_ANY), 
            wx.NullBitmap, 
            wx.ITEM_RADIO,
            "Place Object Tool", 
            ""
            )
        self.AddLabelTool(
            toolscmds.ID_MultiplaceObjTool, 
            "multiplace object",
            wx.Bitmap(
                fileserver.manglepath("outgui:images/tools/multiplace.bmp"),
                wx.BITMAP_TYPE_ANY
                ), 
            wx.NullBitmap, 
            wx.ITEM_RADIO,
            "Multiplace Object Tool", 
            ""
            )
        self.AddLabelTool(
            toolscmds.ID_TrnEditTool, 
            "terrain edit", 
            wx.Bitmap(
                fileserver.manglepath("outgui:images/tools/terrain_edit.bmp"),
                wx.BITMAP_TYPE_ANY
                ), 
            wx.NullBitmap, 
            wx.ITEM_RADIO,
            "Terrain Edit Tool", 
            ""
            )
        self.AddLabelTool(
            toolscmds.ID_WaypointsTool, 
            "create waypoints", 
            wx.Bitmap(
                fileserver.manglepath("outgui:images/tools/waypoints.bmp"),
                wx.BITMAP_TYPE_ANY
                ), 
            wx.NullBitmap, 
            wx.ITEM_RADIO,
            "Waypoints Creation Tool", 
            ""
            )
        self.AddLabelTool(
            toolscmds.ID_CircleTriggerTool, 
            "circle triggers", 
            wx.Bitmap(
                fileserver.manglepath("outgui:images/tools/circle_trigger.bmp"),
                wx.BITMAP_TYPE_ANY
                ), 
            wx.NullBitmap, 
            wx.ITEM_RADIO,
            "Circle Trigger Creation Tool", 
            ""
            )
        self.AddLabelTool(
            toolscmds.ID_PolygonTriggerTool, 
            "polygon triggers", 
            wx.Bitmap(
                fileserver.manglepath(
                    "outgui:images/tools/polygon_trigger.bmp"
                    ),
                wx.BITMAP_TYPE_ANY
                ), 
            wx.NullBitmap, 
            wx.ITEM_RADIO,
            "Polygon Trigger Creation Tool", 
            ""
            )
        self.AddLabelTool(
            toolscmds.ID_CircleSoundSourceTool, 
            "circle sound sources", 
            wx.Bitmap(
                fileserver.manglepath(
                    "outgui:images/tools/circle_sound_source.bmp"
                    ),
                wx.BITMAP_TYPE_ANY
                ), 
            wx.NullBitmap, 
            wx.ITEM_RADIO,
            "Circle Sound Source Creation Tool",
            ""
            )
        self.AddLabelTool(
            toolscmds.ID_PolygonSoundSourceTool, 
            "polygon sound sources", 
            wx.Bitmap(
                fileserver.manglepath(
                    "outgui:images/tools/polygon_sound_source.bmp"
                    ),
                wx.BITMAP_TYPE_ANY
                ), 
            wx.NullBitmap, 
            wx.ITEM_RADIO,
            "Polygon Sound Source Creation Tool",
            ""
            )
        self.AddLabelTool(
            toolscmds.ID_MeasureTool, 
            "measure", 
            wx.Bitmap(
                fileserver.manglepath(
                    "outgui:images/tools/measure.bmp"
                    ),
                wx.BITMAP_TYPE_ANY), 
            wx.NullBitmap, wx.ITEM_RADIO,
            "Terrain Distance Measure Tool", 
            ""
            )
        self.AddSeparator()
        self.AddLabelTool(
            viewcmds.ID_LocalTransform, 
            "local/world transform",
            wx.Bitmap(
                fileserver.manglepath(
                    "outgui:images/tools/world_axis.bmp"
                    ), 
                wx.BITMAP_TYPE_ANY
                ),
            wx.Bitmap(
                fileserver.manglepath(
                    "outgui:images/tools/local_axis.bmp"
                    ), 
                wx.BITMAP_TYPE_ANY
                ),
            wx.ITEM_CHECK,
            "Switch between local and world transform",
            ""
            )
        self.choice_move_mode = wx.Choice(
                                                self, 
                                                -1, 
                                                choices = [
                                                    'No terrain collision', 
                                                    'Terrain collision', 
                                                    'Snap to terrain'
                                                    ]
                                                )
        self.AddControl( self.choice_move_mode )
        self.choice_selection_mode = wx.Choice(
                                                    self, 
                                                    -1, 
                                                    choices = [
                                                        'Objects', 
                                                        'Subentities', 
                                                        'Terrain cells'
                                                        ]
                                                    )
        self.AddControl( self.choice_selection_mode )
        self.AddLabelTool(
            viewcmds.ID_LockSelection, 
            "lock selection",
            wx.Bitmap(
                fileserver.manglepath(
                    "outgui:images/tools/lock2.bmp"
                    ),
                wx.BITMAP_TYPE_ANY
                ), 
            wx.NullBitmap, 
            wx.ITEM_CHECK,
            "Lock/Unlock current selection",
            ""
            )
        self.AddSeparator()
        self.AddLabelTool(
            debugcmds.ID_ToggleAI, 
            "toggle AI", 
            wx.Bitmap(
                fileserver.manglepath(
                    "outgui:images/tools/ai.bmp"
                    ),
                wx.BITMAP_TYPE_ANY
                ), 
            wx.NullBitmap, 
            wx.ITEM_CHECK,
            "Turn on/off triggers and agents",
            ""
            )
        self.AddLabelTool(
            debugcmds.ID_ToggleSound,
            "toggle sound",
            wx.Bitmap(
                fileserver.manglepath(
                    "outgui:images/tools/sound.bmp"
                    ),
                wx.BITMAP_TYPE_ANY
                ), 
            wx.NullBitmap, 
            wx.ITEM_CHECK,
            "Turn on/off the sound sources",
            ""
            )
        self.AddSeparator()
        self.AddLabelTool(
            viewcmds.ID_ObjBrowser, 
            "object browser",
            wx.Bitmap(
                fileserver.manglepath("outgui:images/tools/browser.bmp"),
                wx.BITMAP_TYPE_ANY
                ), 
            wx.NullBitmap, 
            wx.ITEM_NORMAL,
            "Open an Object Browser", 
            ""
            )
        self.AddSeparator()
        self.AddLabelTool(
            toolscmds.ID_WaypointsPathTool, 
            "create waypoint paths", 
            wx.Bitmap(
                fileserver.manglepath("outgui:images/tools/waypoints.bmp"),
                wx.BITMAP_TYPE_ANY
                ), 
            wx.NullBitmap, 
            wx.ITEM_NORMAL,
            "Waypoint Paths Creation Tool", 
            ""
            )

        # initial state
        self.toggle_axis_transform(True)
        self.choice_move_mode.SetSelection(0)
        self.choice_selection_mode.SetSelection(0)

        # bindings
        self.Bind(
            wx.EVT_CHOICE, 
            self.__on_move_mode, 
            self.choice_move_mode
            )
        self.Bind(
            wx.EVT_CHOICE, 
            self.__on_selection_mode, 
            self.choice_selection_mode
            )

    def __on_move_mode(self, event):
        mode = self.choice_move_mode.GetSelection()
        menu = self.GetParent().get_menubar().get_menu('&View')
        if mode == 0:
            id_move = viewcmds.ID_MoveThroughTerrain
        elif mode == 1:
            id_move = viewcmds.ID_MoveToTerrain
        else:
            id_move = viewcmds.ID_MoveOnTerrain
        menu.Check( id_move, True )
        app.get_object_state().setcanmovethroughterrain( mode )

    def __on_selection_mode(self, event):
        mode = self.choice_selection_mode.GetSelection()
        menu = self.GetParent().get_menubar().get_menu('&View')
        succeed = False
        if mode == 0 and app.is_subentity_mode_active():
            result = cjr.confirm_yes_no(
                            app.get_top_window(),
                            "Do you want to save subentity changes?"
                            )
            save = (result == wx.ID_YES)
            succeed = app.get_object_state().setselectionmode( mode, save )
        else:
            succeed = app.get_object_state().setselectionmode( mode, False )

        if succeed:
            if mode == 0:
                id_selection = viewcmds.ID_SelectObjects
            elif mode == 1:
                id_selection = viewcmds.ID_SelectWaypoints
            else:
                id_selection = viewcmds.ID_SelectTerrainCells
            menu.Check( id_selection, True )

    def select_move_mode(self, mode):
        self.choice_move_mode.SetSelection(mode)

    def select_selection_mode(self, mode):
        self.choice_selection_mode.SetSelection(mode)

    def toggle_axis_transform(self, local):
        # Second bitmap for a check tool is used different in wxPython than
        # its C counterpart. In Python is used when the tool is disabled and
        # in C when pushed, the desired behaviour here. So this function
        # simulates the C behaviour.
        self.ToggleTool( viewcmds.ID_LocalTransform, local )
        tool = self.FindById( viewcmds.ID_LocalTransform )
        if local:
            bmp_filename = "local_axis.bmp"
        else:
            bmp_filename = "world_axis.bmp"
        tool.SetNormalBitmap(
            wx.Bitmap(
                servers.get_file_server().manglepath(
                    "outgui:images/tools/%s" % bmp_filename
                    )
                )
            )

    def Realize(self):
        # Another nice bug? When call to Realize, the first tool of a radio
        # tools group gets automatically selected (keeping the old selected
        # tool selected too). So find the selected tool before calling to
        # Realize, and restore it afterwards.
        selected_tool_id = None
        for tool_id in toolscmds.tools_ids:
            if self.GetToolState( tool_id ):
                selected_tool_id = tool_id
                break
        wx.ToolBar.Realize(self)
        if selected_tool_id is not None:
            self.ToggleTool( toolscmds.ID_NullTool, False )
            self.ToggleTool( selected_tool_id, True )

    def refresh(self):
        """
        Rebuild toolbar

        For all custom scripts defined in the script manager a tool button
        (either image button or text button) if added to the toolbar if that
        script has been marked to be shown in the toolbar by the user.
        """
        # Remove old custom tools
        for key in self.custom_tools.keys():
            button = self.custom_tools[key]['button']
            if button is None:
                self.Unbind(wx.EVT_MENU, id=key)
            else:
                self.Unbind(wx.EVT_BUTTON, button)
                button.Destroy()
            self.custom_tools[key]['button'] = None
            self.DeleteTool(key)
        self.custom_tools = {}

        # Add tool buttons for custom tools
        fileserver = servers.get_file_server()
        scripts = guisettings.Repository.getsettingvalue(
                        guisettings.ID_ScriptList
                        )
        for i in range(len(scripts)):
            script = scripts[i]
            tool_id = wx.NewId()
            self.custom_tools[tool_id] = {'index': i, 'button': None}

            # Tool
            if script['show button']:
                if script['button type'] == 'image':
                    # Add image button (normal toolbar tool)
                    self.AddLabelTool(tool_id, script['description'],
                        wx.Bitmap(fileserver.manglepath(
                        str(script['button image'])), wx.BITMAP_TYPE_ANY),
                        wx.NullBitmap, wx.ITEM_NORMAL, script['tooltip text'],
                        script['statusbar text'])
                    self.Bind(wx.EVT_MENU, self.on_custom_tool, id=tool_id)
                else:
                    # Add text button
                    button = wx.Button(self, tool_id, script['button text'],
                        style=wx.BU_EXACTFIT)
                    self.AddControl(button)
                    button.SetToolTip(wx.ToolTip(script['tooltip text']))
                    self.Bind(wx.EVT_BUTTON, self.on_custom_tool, button)
                    self.custom_tools[tool_id]['button'] = button

        # Refresh toolbar
        self.Realize()

    def on_custom_tool(self, event):
        scriptmgrdlg.run_script( self.custom_tools[event.GetId()]['index'] )
