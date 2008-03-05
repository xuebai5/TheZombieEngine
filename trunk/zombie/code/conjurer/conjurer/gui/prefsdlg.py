##\file prefsdlg.py
##\brief Preferences dialog

import wx
import wx.lib.colourselect
import wx.lib.intctrl

import app
import floatctrl
import format
import intctrl
import nebulaguisettings as guisettings
import servers
import togwin
import nohtree
import conjurerconfig as cfg

#------------------------------------------------------------------------------
# restore function
def restore():
    """Restore user preferences"""
    restore_undo()
    restore_grid()
    restore_angle_snapping()
    restore_size_snapping()

# restore_undo function
def restore_undo():
    """Restore undo preferences"""
    undo_memory = get_repository_setting_value('undo memory')
    servers.get_command_server().setmemorylimit( format.mb2bytes(undo_memory) )

# restore_grid function
def restore_grid():
    """Restore grid preferences"""
    grid = get_repository_setting_value(guisettings.ID_Grid)
    grid_obj = app.get_grid()
    grid_obj.setspacing( grid['line spacing'] )
    grid_obj.setnsubdivision( grid['line grouping'] )
    color = format.byte_rgb_2_unit_rgba( grid['minor color'] )
    grid_obj.setlightcolor(color[0], color[1], color[2], color[3])
    color = format.byte_rgb_2_unit_rgba( grid['major color'] )
    grid_obj.setdarkcolor(color[0], color[1], color[2], color[3])
    grid_obj.setsnapenabled( grid['snap enabled'] )

# restore_angle_snapping function
def restore_angle_snapping():
    """Restore angle snapping preferences"""
    angle_snap = get_repository_setting_value(
                            guisettings.ID_AngleSnapping
                            )
    rotate = app.get_rotate_tool()
    if angle_snap['enabled']:
        rotate.setsnap( angle_snap['angle'] )
    else:
        rotate.setsnap(0)

# restore_size_snapping function
def restore_size_snapping():
    """Restore size snapping preferences"""
    size_snap = get_repository_setting_value(
                        guisettings.ID_SizeSnapping
                        )
    scale = app.get_scale_tool()
    if size_snap['enabled']:
        scale.setsnap( size_snap['size'] )
    else:
        scale.setsnap(0)

# return the value of the given repository setting
def get_repository_setting_value(setting_name):
    return guisettings.Repository.getsettingvalue(setting_name)
    
# set the value of the given repository setting
def set_repository_setting_value(setting_name, value):
    guisettings.Repository.setsettingvalue(setting_name, value)

#------------------------------------------------------------------------------
# PrefsPanel class
class PrefsPanel(wx.Panel):
    """Common base panel for all preferences groups"""
    
    def __init__(self, *args, **kwds):
        wx.Panel.__init__(self, *args, **kwds)
    
    def _do_layout(self, sizer):
        self.SetAutoLayout(True)
        self.SetSizer(sizer)
        sizer.Fit(self)
        sizer.SetSizeHints(self)
        self.Layout()


#------------------------------------------------------------------------------
# GeneralPanel class
class GeneralPanel(PrefsPanel):
    """Panel with non groupable preferences"""
    
    def __init__(self, *args, **kwds):
        PrefsPanel.__init__(self, *args, **kwds)
        
        # Undo memory limit
        self.label_undo = wx.StaticText(self, -1, "Undo memory (Mb)")
        self.int_ctrl_undo = wx.lib.intctrl.IntCtrl(
            self, -1, limited=True, value=1000, min=0, max=2000,
            size=(40, -1), style=wx.TE_RIGHT 
            )
        
        # Temporary working copy path
        self.label_temporary = wx.StaticText(self, -1, "Temporary working path")
        self.text_temporary = wx.TextCtrl(self, -1)
        self.button_temporary = wx.Button(self, -1, "...", style=wx.BU_EXACTFIT)
        
        # dot path
        self.label_dot = wx.StaticText(self, -1, "Dot path")
        self.text_dot = wx.TextCtrl(self, -1)
        self.button_dot = wx.Button(self, -1, "...", style=wx.BU_EXACTFIT)

        # default export entity path
        self.label_entity_export = wx.StaticText(
                                                self, 
                                                -1, 
                                                "Default entity export path"
                                                )
        self.text_entity_export = wx.TextCtrl(self, -1)
        self.button_entity_export = wx.Button(
                                                self, 
                                                -1, 
                                                "...", 
                                                style=wx.BU_EXACTFIT
                                                )
                                                
        # default particle system library path
        self.label_particle_system = wx.StaticText(
                                                self, 
                                                -1, 
                                                "Default particle system library path"
                                                )
        self.text_particle_system = wx.TextCtrl(self, -1)
        self.button_particle_system = wx.Button(
                                                self, 
                                                -1, 
                                                "...", 
                                                style=wx.BU_EXACTFIT
                                                )

        # presets path
        self.label_presets = wx.StaticText(self, -1, "Presets path")
        self.text_presets = wx.TextCtrl(self, -1)
        self.button_presets = wx.Button(self, -1, "...", style=wx.BU_EXACTFIT)

        # Save preset enable
        self.check_save_preset = wx.CheckBox(self, -1, "Save preset on exit")

        self.__set_properties()
        self.__do_layout()
        self.__bind_events()

    def __set_properties(self):
        # Undo
        mem_mb = get_repository_setting_value(
                            guisettings.ID_UndoMemory
                            )
        self.int_ctrl_undo.SetValue(mem_mb)
        
        # Temporary working path
        self.text_temporary.SetValue(
            get_repository_setting_value(
                guisettings.ID_TemporaryWorkingPath
                )
            )

        # dot path
        self.text_dot.SetValue(
            get_repository_setting_value(
                guisettings.ID_DotPath
                )
            )
        
        # Default entity export path
        self.text_entity_export.SetValue(
            get_repository_setting_value(
                guisettings.ID_DefaultEntityExportPath
                )
        )

        # Default library location for particle systems
        self.text_particle_system.SetValue(
            get_repository_setting_value(
                guisettings.ID_DefaultParticleSystemLibraryPath
                )
        )

        # Presets path
        self.text_presets.SetValue(
            get_repository_setting_value(
                guisettings.ID_PresetsPath
                )
            )
        
        # Save preset
        self.check_save_preset.SetValue(
            get_repository_setting_value(
                guisettings.ID_SavePreset
                )
            )

    def __do_layout(self):
        sizer_border = wx.BoxSizer(wx.VERTICAL)
        sizer_layout = wx.BoxSizer(wx.VERTICAL)
        grid_sizer = wx.FlexGridSizer(
                            6, 
                            3, 
                            cfg.BORDER_WIDTH, 
                            cfg.BORDER_WIDTH * 2
                            )
        grid_sizer.AddGrowableCol(1)
        grid_sizer.Add(
            self.label_undo, 
            0, 
            cfg.LABEL_ALIGN|wx.ALIGN_CENTER_VERTICAL
            )
        grid_sizer.Add(self.int_ctrl_undo)
        grid_sizer.AddSpacer( (0, 0) )
        grid_sizer.Add(
            self.label_temporary, 
            1,
            cfg.LABEL_ALIGN|wx.ALIGN_CENTER_VERTICAL
            )
        grid_sizer.Add(
            self.text_temporary, 
            1, 
            wx.EXPAND|wx.FIXED_MINSIZE|wx.ALIGN_CENTER_VERTICAL
            )
        grid_sizer.Add(self.button_temporary)
        grid_sizer.Add(
            self.label_dot, 
            1, 
            cfg.LABEL_ALIGN|wx.ALIGN_CENTER_VERTICAL
            )
        grid_sizer.Add(
            self.text_dot, 
            1, 
            wx.EXPAND|wx.FIXED_MINSIZE|wx.ALIGN_CENTER_VERTICAL 
            )
        grid_sizer.Add(self.button_dot)
        grid_sizer.Add(
            self.label_entity_export, 
            1, 
            cfg.LABEL_ALIGN|wx.ALIGN_CENTER_VERTICAL
            )
        grid_sizer.Add(
            self.text_entity_export, 
            1, 
            wx.EXPAND|wx.FIXED_MINSIZE|wx.ALIGN_CENTER_VERTICAL
            )
        grid_sizer.Add(self.button_entity_export)
        grid_sizer.Add(
            self.label_particle_system, 
            1, 
            cfg.LABEL_ALIGN|wx.ALIGN_CENTER_VERTICAL
            )
        grid_sizer.Add(
            self.text_particle_system, 
            1, 
            wx.EXPAND|wx.FIXED_MINSIZE|wx.ALIGN_CENTER_VERTICAL
            )
        grid_sizer.Add(self.button_particle_system)
        grid_sizer.Add(
            self.label_presets, 
            1,
            cfg.LABEL_ALIGN|wx.ALIGN_CENTER_VERTICAL 
            )
        grid_sizer.Add(
            self.text_presets, 
            1, 
            wx.EXPAND|wx.FIXED_MINSIZE|wx.ALIGN_CENTER_VERTICAL 
            )
        grid_sizer.Add(self.button_presets)
        grid_sizer.AddSpacer( (0, 0) )
        grid_sizer.Add(
            self.check_save_preset, 
            1, 
            wx.EXPAND|wx.ALIGN_CENTER_VERTICAL 
            )
        sizer_layout.Add(grid_sizer, 0, wx.EXPAND)
        sizer_border.Add(
            sizer_layout, 
            0, 
            wx.ALL|wx.EXPAND, 
            cfg.BORDER_WIDTH
            )
        self.SetSizer(sizer_border)
        sizer_border.SetSizeHints(self)
        self.Layout()

    def __bind_events(self):
        self.Bind(
            wx.lib.intctrl.EVT_INT, 
            self.on_change_undo, 
            self.int_ctrl_undo
            )
        self.Bind(
            wx.EVT_TEXT,    
            self.on_change_temporary_path, 
            self.text_temporary
            )
        self.Bind(
            wx.EVT_BUTTON, 
            self.on_choose_temporary_path, 
            self.button_temporary
            )
        self.Bind(
            wx.EVT_TEXT, 
            self.on_change_dot_path, 
            self.text_dot
            )
        self.Bind(
            wx.EVT_BUTTON, 
            self.on_choose_dot_path, 
            self.button_dot
            )
        self.Bind(
            wx.EVT_TEXT, 
            self.on_change_entity_export_path, 
            self.text_entity_export
            )
        self.Bind(
            wx.EVT_BUTTON, 
            self.on_choose_entity_export_path, 
            self.button_entity_export
            )
        self.Bind(
            wx.EVT_TEXT, 
            self.on_change_particle_system_library_path, 
            self.text_particle_system
            )
        self.Bind(
            wx.EVT_BUTTON, 
            self.on_choose_particle_system_library_path, 
            self.button_particle_system
            )
        self.Bind(
            wx.EVT_TEXT, 
            self.on_change_presets_path, 
            self.text_presets
            )
        self.Bind(
            wx.EVT_BUTTON, 
            self.on_choose_presets_path, 
            self.button_presets
            )
        self.Bind(
            wx.EVT_CHECKBOX, 
            self.on_change_save_preset, 
            self.check_save_preset
            )

    def on_change_undo(self, event):
        mem_mb = self.int_ctrl_undo.GetValue()
        mem_bytes = format.mb2bytes(mem_mb)
        servers.get_command_server().setmemorylimit(mem_bytes)
        set_repository_setting_value(
            guisettings.ID_UndoMemory, 
            mem_mb
            )

    def on_change_temporary_path(self, event):
        set_repository_setting_value(
            guisettings.ID_TemporaryWorkingPath,
            self.text_temporary.GetValue() 
            )

    def on_choose_temporary_path(self, event):
        dlg = wx.DirDialog(
            self,
            "Choose a directory",
            defaultPath = get_repository_setting_value(
                                    guisettings.ID_TemporaryWorkingPath
                ),
            style = wx.DD_NEW_DIR_BUTTON
            )
        if dlg.ShowModal() == wx.ID_OK:
            self.text_temporary.SetValue( str(dlg.GetPath()) )
            # Change is persisted due to text control triggering the EVT_TEXT

    def on_change_dot_path(self, event):
        set_repository_setting_value(
            guisettings.ID_DotPath,
            self.text_dot.GetValue() 
            )

    def on_choose_dot_path(self, event):
        dlg = wx.FileDialog(
            self,
            "Choose the dot executable file",
            wildcard="dot executable (dot.exe)|dot.exe|" \
                            "All files (*)|*",
            style=wx.OPEN
            )
        dlg.SetPath(
            get_repository_setting_value(guisettings.ID_DotPath)
            )
        if dlg.ShowModal() == wx.ID_OK:
            self.text_dot.SetValue( str(dlg.GetPath()) )
            # Change is persisted due to text control triggering the EVT_TEXT

    def on_choose_entity_export_path(self, event):
        dlg = wx.DirDialog(
            self,
            "Choose a directory",
            defaultPath = get_repository_setting_value(
                                    guisettings.ID_DefaultEntityExportPath
                                    ),
            style = wx.DD_NEW_DIR_BUTTON
            )
        if dlg.ShowModal() == wx.ID_OK:
            self.text_entity_export.SetValue( str(dlg.GetPath()) )
            # Change is persisted due to text control triggering the EVT_TEXT
    
    def on_change_entity_export_path(self, event):
        set_repository_setting_value(
            guisettings.ID_DefaultEntityExportPath,
            self.text_entity_export.GetValue() 
            )

    def on_choose_particle_system_library_path(self, event):
        # Get the location in the library
        dlg = nohtree.create_selection_dialog(
                    self
                    )
        try:
            if dlg.ShowModal() == wx.ID_OK:
                self.text_particle_system.SetValue(
                    dlg.target_library
                    )
        finally:
            dlg.Destroy()
        
    def on_change_particle_system_library_path(self, event):
        set_repository_setting_value(
            guisettings.ID_DefaultParticleSystemLibraryPath,
            self.text_particle_system.GetValue() 
            )

    def on_choose_presets_path(self, event):
        dlg = wx.DirDialog(
            self,
            "Choose a directory",
            defaultPath = get_repository_setting_value(
                                    guisettings.ID_PresetsPath
                                    ),
            style = wx.DD_NEW_DIR_BUTTON
            )
        if dlg.ShowModal() == wx.ID_OK:
            self.text_presets.SetValue( str(dlg.GetPath()) )
            # Change is persisted due to text control triggering the EVT_TEXT
    
    def on_change_presets_path(self, event):
        set_repository_setting_value( guisettings.ID_PresetsPath,
            self.text_presets.GetValue() )
    
    def on_change_save_preset(self, event):
        set_repository_setting_value( guisettings.ID_SavePreset,
            self.check_save_preset.GetValue() )



#------------------------------------------------------------------------------
# FontGroup class
class FontGroup:
    """Helper class to group and layout controls for a single font"""

    def __init__(self, parent, setting_id, label_name):
        self.parent = parent
        self.setting_id = setting_id
        self.label_font_id = wx.StaticText(parent, -1, label_name)
        self.label_font_sample = wx.StaticText(
                                            parent, 
                                            -1, 
                                            "<font name and size>"
                                            )
        self.button_font = wx.Button(parent, -1, "Select font")
        
        self.__update_font_sample()

    def __update_font_sample(self):
        label = self.label_font_sample
        font = guisettings.Repository.get_font(self.setting_id)
        label.SetLabel( font.GetFaceName() + ", " + str(font.GetPointSize()) )
        label.SetFont(font)
        self.parent.Layout()

    def do_layout(self, sizer):
        sizer.Add(
            self.label_font_id, 
            0, 
            cfg.LABEL_ALIGN|wx.ALIGN_CENTER_VERTICAL|wx.FIXED_MINSIZE
            )
        sizer.Add(
            self.label_font_sample, 
            1, 
            wx.LEFT|wx.RIGHT|wx.ALIGN_CENTER_VERTICAL, 
            cfg.BORDER_WIDTH
            )
        sizer.Add(
            self.button_font, 
            0, 
            wx.ALIGN_CENTER_VERTICAL|wx.FIXED_MINSIZE
            )

    def bind_events(self):
        self.parent.Bind(wx.EVT_BUTTON, self.on_select_font, self.button_font)

    def on_select_font(self, event):
        # open font dialog
        fontdata = wx.FontData()
        fontdata.SetInitialFont(
            guisettings.Repository.get_font(self.setting_id)
            )
        dlg = wx.FontDialog(self.parent, fontdata)
        
        if dlg.ShowModal() == wx.ID_OK:
            # update repository
            guisettings.Repository.set_font( self.setting_id,
                dlg.GetFontData().GetChosenFont() )
            
            # update display
            self.__update_font_sample()
            self.parent.GetParent().GetGrandParent().refresh_all()


# FontsPanel class
class FontsPanel(PrefsPanel):
    """Panel to adjust all configurable fonts"""
    
    def __init__(self, *args, **kwds):
        PrefsPanel.__init__(self, *args, **kwds)
        
        # Fonts controls groups
        self.font_groups = [
            FontGroup(self, guisettings.ID_Font_ScriptEditor, "Script editor")
            ]

        self.__do_layout()
        self.__bind_events()

    def __do_layout(self):
        number_of_font_groups = len(self.font_groups)
        sizer_main = wx.BoxSizer(wx.VERTICAL)
        for font_group in self.font_groups:
            sizer_font = wx.FlexGridSizer(
                                number_of_font_groups, 
                                3, 
                                cfg.BORDER_WIDTH, 
                                cfg.BORDER_WIDTH * 2
                                )
            sizer_font.AddGrowableCol(1)
            font_group.do_layout(sizer_font)
            sizer_main.Add(
                sizer_font, 
                0, 
                wx.LEFT|wx.TOP|wx.RIGHT|wx.EXPAND, 
                cfg.BORDER_WIDTH
                )
        sizer_main.Add( (cfg.BORDER_WIDTH, cfg.BORDER_WIDTH) ) # bottom margin
        self._do_layout(sizer_main)

    def __bind_events(self):
        for font_group in self.font_groups:
            font_group.bind_events()


#------------------------------------------------------------------------------
# SnapGridPanel class
class SnapGridPanel(PrefsPanel):
    """Panel to configure the InGUI snap and grid"""
    
    def __init__(self, *args, **kwds):
        PrefsPanel.__init__(self, *args, **kwds)
        
        self.staticbox_angle_group = wx.StaticBox(self, -1, "Angle snapping")
        self.staticbox_size_group = wx.StaticBox(self, -1, "Size snapping")
        self.staticbox_grid_group = wx.StaticBox(self, -1, "Grid")
        self.label_grid_1 = wx.StaticText(self, -1, "Minor grid line every")
        self.float_grid_cell_size = floatctrl.FloatCtrl( self,
            size=wx.Size(40,-1), style=wx.TE_RIGHT )
        self.label_grid_2 = wx.StaticText(self, -1, "world units")
        self.label_grid_3 = wx.StaticText(self, -1, "Major grid line every")
        self.int_grid_line_grouping = intctrl.IntCtrl( self,
            size = wx.Size(40,-1), style=wx.TE_RIGHT )
        self.label_grid_4 = wx.StaticText(self, -1, "minor grid lines")
        self.label_grid_5 = wx.StaticText(self, -1, "Minor grid line color")
        self.color_grid_minor = wx.lib.colourselect.ColourSelect(self, -1)
        self.label_grid_6 = wx.StaticText(self, -1, "Major grid line color")
        self.color_grid_major = wx.lib.colourselect.ColourSelect(self, -1)
        self.label_angle_1 = wx.StaticText(self, -1, "Angle snap")
        self.float_angle = floatctrl.FloatCtrl( self,
            size=wx.Size(40,-1), style=wx.TE_RIGHT )
        self.label_angle_2 = wx.StaticText(self, -1, "degrees")
        self.label_size_1 = wx.StaticText(self, -1, "Size snap")
        self.float_size = floatctrl.FloatCtrl( self,
            size=wx.Size(40,-1), style=wx.TE_RIGHT )
        self.label_size_2 = wx.StaticText(self, -1, "world units")
        
        self.__set_properties()
        self.__do_layout()
        self.__bind_events()

    def __set_properties(self):
        # Grid
        grid = app.get_grid()
        self.float_grid_cell_size.set_value( grid.getspacing() )
        self.int_grid_line_grouping.set_value( grid.getnsubdivision() )
        self.color_grid_minor.SetColour(
            format.unit_rgb_2_byte_rgb(grid.getlightcolor()) )
        self.color_grid_major.SetColour(
            format.unit_rgb_2_byte_rgb(grid.getdarkcolor()) )

        # Angle snapping
        angle_snap = get_repository_setting_value(
                                guisettings.ID_AngleSnapping
                                )
        self.float_angle.set_value( angle_snap['angle'] )

        # Size snapping
        size_snap = get_repository_setting_value(
                            guisettings.ID_SizeSnapping
                            )
        self.float_size.set_value( size_snap['size'] )

    def __do_layout(self):
        sizer_main = wx.BoxSizer(wx.VERTICAL)
        sizer_size_group = wx.StaticBoxSizer(
                                    self.staticbox_size_group, 
                                    wx.VERTICAL
                                    )
        sizer_size = wx.FlexGridSizer(
                            1, 
                            3, 
                            cfg.BORDER_WIDTH, 
                            cfg.BORDER_WIDTH * 2
                            )
        sizer_angle_group = wx.StaticBoxSizer(
                                        self.staticbox_angle_group, 
                                        wx.VERTICAL
                                        )
        sizer_angle = wx.FlexGridSizer(
                            1, 
                            3, 
                            cfg.BORDER_WIDTH, 
                            cfg.BORDER_WIDTH * 2
                            )
        sizer_grid = wx.FlexGridSizer(
                            1, 
                            3, 
                            cfg.BORDER_WIDTH, 
                            cfg.BORDER_WIDTH * 2
                            )
        sizer_grid_group = wx.StaticBoxSizer(
                                    self.staticbox_grid_group, 
                                    wx.VERTICAL
                                    )
        sizer_grid.Add(
            self.label_grid_1, 
            0, 
            cfg.LABEL_ALIGN|wx.ALIGN_CENTER_VERTICAL|wx.FIXED_MINSIZE,
            )
        sizer_grid.Add(
            self.float_grid_cell_size, 
            0, 
            wx.LEFT|wx.RIGHT|wx.ALIGN_CENTER_VERTICAL|wx.FIXED_MINSIZE, 
            cfg.BORDER_WIDTH
            )
        sizer_grid.Add(
            self.label_grid_2, 
            0, 
            wx.ALIGN_CENTER_VERTICAL|wx.FIXED_MINSIZE
            )
        sizer_grid.Add(
            self.label_grid_3, 
            0, 
            cfg.LABEL_ALIGN|wx.ALIGN_CENTER_VERTICAL|wx.FIXED_MINSIZE
            )
        sizer_grid.Add(
            self.int_grid_line_grouping, 
            0, 
            wx.LEFT|wx.RIGHT|wx.ALIGN_CENTER_VERTICAL|wx.FIXED_MINSIZE, 
            cfg.BORDER_WIDTH
            )
        sizer_grid.Add(
            self.label_grid_4, 
            0, 
            wx.ALIGN_CENTER_VERTICAL|wx.FIXED_MINSIZE
            )
        sizer_grid.Add(
            self.label_grid_5, 
            0, 
            cfg.LABEL_ALIGN|wx.ALIGN_CENTER_VERTICAL|wx.FIXED_MINSIZE
            )
        sizer_grid.Add(
            self.color_grid_minor, 
            0, 
            wx.LEFT|wx.ALIGN_CENTER_VERTICAL|wx.FIXED_MINSIZE,
            cfg.BORDER_WIDTH
            )
        sizer_grid.AddSpacer( (0, 0) )
        sizer_grid.Add(
            self.label_grid_6, 
            0, 
            cfg.LABEL_ALIGN|wx.ALIGN_CENTER_VERTICAL|wx.FIXED_MINSIZE
            )
        sizer_grid.Add(
            self.color_grid_major, 
            0, 
            wx.LEFT|wx.ALIGN_CENTER_VERTICAL|wx.FIXED_MINSIZE,
            cfg.BORDER_WIDTH
            )
        sizer_grid.AddSpacer( (0, 0) )
        sizer_grid_group.Add(
            sizer_grid, 
            1, 
            wx.LEFT|wx.RIGHT|wx.BOTTOM|wx.EXPAND, 
            cfg.BORDER_WIDTH
            )
        sizer_main.Add(
            sizer_grid_group, 
            0, 
            wx.ALL|wx.EXPAND
            , cfg.BORDER_WIDTH
            )
        sizer_angle.Add(
            self.label_angle_1, 
            0, 
            cfg.LABEL_ALIGN|wx.ALIGN_CENTER_VERTICAL|wx.FIXED_MINSIZE
            )
        sizer_angle.Add(
            self.float_angle, 
            0, 
            wx.LEFT|wx.RIGHT|wx.ALIGN_CENTER_VERTICAL|wx.FIXED_MINSIZE, 
            cfg.BORDER_WIDTH
            )
        sizer_angle.Add(
            self.label_angle_2, 
            0, 
            wx.ALIGN_CENTER_VERTICAL|wx.FIXED_MINSIZE
            )
        sizer_angle_group.Add(
            sizer_angle, 
            0, 
            wx.ALL|wx.EXPAND, 
            cfg.BORDER_WIDTH
            )
        sizer_main.Add(
            sizer_angle_group, 
            0, 
            wx.LEFT|wx.RIGHT|wx.BOTTOM|wx.EXPAND, 
            cfg.BORDER_WIDTH
            )
        sizer_size.Add(
            self.label_size_1, 
            0, 
            cfg.LABEL_ALIGN|wx.ALIGN_CENTER_VERTICAL|wx.FIXED_MINSIZE
            )
        sizer_size.Add(
            self.float_size, 
            0, 
            wx.LEFT|wx.RIGHT|wx.ALIGN_CENTER_VERTICAL|wx.FIXED_MINSIZE, 
            cfg.BORDER_WIDTH
            )
        sizer_size.Add(
            self.label_size_2, 
            0, 
            wx.ALIGN_CENTER_VERTICAL|wx.FIXED_MINSIZE
            )
        sizer_size_group.Add(
            sizer_size, 
            0, 
            wx.ALL|wx.EXPAND, 
            cfg.BORDER_WIDTH
            )
        sizer_main.Add(
            sizer_size_group, 
            0, 
            wx.LEFT|wx.RIGHT|wx.BOTTOM|wx.EXPAND, 
            cfg.BORDER_WIDTH
            )
        self._do_layout(sizer_main)
    
    def __bind_events(self):
        self.Bind(
            floatctrl.EVT_FLOAT, 
            self.on_line_spacing,
            self.float_grid_cell_size
            )
        self.Bind(
            wx.lib.intctrl.EVT_INT, 
            self.on_line_grouping,
            self.int_grid_line_grouping
            )
        self.Bind(
            wx.lib.colourselect.EVT_COLOURSELECT, 
            self.on_minor_color,
            self.color_grid_minor
            )
        self.Bind(
            wx.lib.colourselect.EVT_COLOURSELECT, 
            self.on_major_color,
            self.color_grid_major
            )
        self.Bind(
            floatctrl.EVT_FLOAT, 
            self.on_angle_snap,
            self.float_angle
            )
        self.Bind(
            floatctrl.EVT_FLOAT, 
            self.on_size_snap,
            self.float_size
            )

    def on_line_spacing(self, event):
        if self.float_grid_cell_size.get_value() > 0:
            spacing = self.float_grid_cell_size.get_value()
            app.get_grid().setspacing(spacing)
            get_repository_setting_value('grid')['line spacing'] = spacing

    def on_line_grouping(self, event):
        if self.int_grid_line_grouping.get_value() > 0:
            grouping = self.int_grid_line_grouping.get_value()
            app.get_grid().setnsubdivision(grouping)
            get_repository_setting_value('grid')['line grouping'] = grouping

    def on_minor_color(self, event):
        color255 = self.color_grid_minor.GetColour().Get()
        color = format.byte_rgb_2_unit_rgba(color255)
        app.get_grid().setlightcolor(color[0], color[1], color[2], color[3])
        get_repository_setting_value('grid')['minor color'] = color255

    def on_major_color(self, event):
        color255 = self.color_grid_major.GetColour().Get()
        color = format.byte_rgb_2_unit_rgba(color255)
        app.get_grid().setdarkcolor(color[0], color[1], color[2], color[3])
        get_repository_setting_value('grid')['major color'] = color255

    def on_angle_snap(self, event):
        angle_snap = get_repository_setting_value('angle snapping')
        angle_snap['angle'] = self.float_angle.get_value()
        restore_angle_snapping()

    def on_size_snap(self, event):
        sizer_snap = get_repository_setting_value('size snapping')
        size_snap = get_repository_setting_value('size snapping')
        size_snap['size'] = self.float_size.get_value()
        restore_size_snapping()

    
#------------------------------------------------------------------------------
# ThumbnailsPanel class
class ThumbnailsPanel(PrefsPanel):
    """Panel to adjust all configurable thumbnails"""
    
    def __init__(self, *args, **kwds):
        PrefsPanel.__init__(self, *args, **kwds)
        
        self.label_terrain_material = wx.StaticText(
                                                self, 
                                                -1, 
                                                "Terrain material resolution" 
                                                )
        self.int_terrain_material = intctrl.IntCtrl(
                                                self, 
                                                -1, 
                                                limited=True,
                                                value=32, 
                                                min=1, 
                                                max=1000, 
                                                size=wx.Size(40,-1), 
                                                style=wx.TE_RIGHT 
                                                )

        self.__set_properties()
        self.__do_layout()
        self.__bind_events()

    def __set_properties(self):
        size = get_repository_setting_value(
                    guisettings.ID_Thumbnail_TerrainMaterial
                    )
        self.int_terrain_material.set_value( size )

    def __do_layout(self):
        sizer_main = wx.BoxSizer(wx.VERTICAL)
        sizer_terrain_material = wx.FlexGridSizer(
                                            1, 
                                            2, 
                                            cfg.BORDER_WIDTH, 
                                            cfg.BORDER_WIDTH * 2
                                            )
        sizer_terrain_material.Add(
            self.label_terrain_material, 
            0, 
            cfg.LABEL_ALIGN|wx.ALIGN_CENTER_VERTICAL|wx.FIXED_MINSIZE
            )
        sizer_terrain_material.Add(
            self.int_terrain_material, 
            0, 
            wx.LEFT|wx.FIXED_MINSIZE,
            cfg.BORDER_WIDTH
            )
        sizer_main.Add(
            sizer_terrain_material, 
            0, 
            wx.ALL|wx.EXPAND, 
            cfg.BORDER_WIDTH
            )
        self._do_layout(sizer_main)

    def __bind_events(self):
        self.Bind(
            wx.lib.intctrl.EVT_INT, 
            self.on_terrain_material,
            self.int_terrain_material
            )

    def on_terrain_material(self, event):
        size = self.int_terrain_material.get_value()
        set_repository_setting_value(
            guisettings.ID_Thumbnail_TerrainMaterial, 
            size
            )


#------------------------------------------------------------------------------
# PrefsChoicebook class
class PrefsChoicebook(wx.Choicebook):
    """Choicebook to group similar preferences"""
    
    def __init__(self, *args, **kwds):
        wx.Choicebook.__init__(self, *args, **kwds)
        
        self.AddPage( GeneralPanel(self, -1), "General" )
        self.AddPage( FontsPanel(self, -1), "Fonts" )
        self.AddPage( SnapGridPanel(self, -1), "Snap/Grid" )
        self.AddPage( ThumbnailsPanel(self, -1), "Thumbnails" )


#------------------------------------------------------------------------------
# PreferencesDialog class
class PreferencesDialog(togwin.ChildToggableDialog):
    """Dialog to change conjurer preferences"""

    def __init__(self, parent):
        togwin.ChildToggableDialog.__init__(
            self, "Preferences", parent
            )
        
        self.choicebook = PrefsChoicebook(self, -1)
        
        self.__do_layout()

    def __do_layout(self):
        sizer = wx.BoxSizer(wx.VERTICAL)
        sizer.Add(self.choicebook, 0, wx.ALL|wx.EXPAND, cfg.BORDER_WIDTH)
        self.SetAutoLayout(True)
        self.SetSizer(sizer)
        sizer.Fit(self)
        sizer.SetSizeHints(self)
        self.Layout()
        self.SetSize( (400, 320) )

    def persist(self):
        data = {
            'selected panel': self.choicebook.GetSelection()
            }
        return [
            create_window,
            (), # no parameters for create function
            data
            ]

    def restore(self, data_list):
        data = data_list[0]
        
        # Select visible panel
        self.choicebook.SetSelection( data['selected panel'] )


#------------------------------------------------------------------------------
# create_window function
def create_window(parent):
    return PreferencesDialog(parent)
