##\file trnlayerprefsdlg.py
##\brief Terrain texture layer new and preferences dialog

import wx

import pynebula

import app
import colorsel
import filedlg
import format
import nebulaguisettings as guisettings
import servers

import conjurerframework as cjr
import conjurerconfig as cfg


# SettingsGroup class
class SettingsGroup:
    """Group of controls to set the terrain texture layer properties"""
    
    def __init__(self, win, layer_ctrl=None):
        self.window = win
        self.layer_ctrl = layer_ctrl
        self.label_name = wx.StaticText(win, -1, "Name")
        self.text_name = wx.TextCtrl(win, -1, "Unnamed")
        self.staticbox_texture = wx.StaticBox(win, -1, "Texture")
        self.radio_local_texture = wx.RadioButton(
                                                win, 
                                                -1, 
                                                "Local file", 
                                                style=wx.RB_SINGLE
                                                )
        self.combo_local_texture = wx.ComboBox(
                                                win, 
                                                -1, 
                                                style=wx.CB_DROPDOWN|wx.CB_READONLY|wx.CB_SORT
                                                )
        self.button_import = wx.Button(win, -1, "Import file...")
        self.radio_shared_texture = wx.RadioButton(
                                                win,
                                                -1, 
                                                "Shared file", 
                                                style=wx.RB_SINGLE
                                                )
        self.button_shared_texture = wx.Button(win, -1, "")
        self.label_u = wx.StaticText(win, -1, "U size", style=wx.ALIGN_RIGHT)
        self.choice_u = wx.ComboBox(win, -1, size=(50, -1))
        self.label_v = wx.StaticText(
                                win, 
                                -1, 
                                "V size", 
                                style=wx.ALIGN_RIGHT
                                )
        self.choice_v = wx.ComboBox(win, -1, size=(50, -1))
        self.label_projection = wx.StaticText(
                                        win, 
                                        -1, 
                                        "Projection", 
                                        style=wx.ALIGN_RIGHT
                                        )
        self.choice_projection = wx.Choice(win, -1, choices=["XZ", "XY", "ZY"])
        self.color_sel = colorsel.ColorSelector(win, -1, "Colour mask")
        self.staticbox_game_material = wx.StaticBox(win, -1, "Game material")
        self.label_game_material = wx.StaticText(win, -1, "Material")
        self.choice_game_material = wx.Choice(win, -1)
        
        self.__set_properties()
    
    def __set_properties(self):
        # default projection
        self.choice_projection.SetSelection(0)
        
        # allowed UV size values
        outdoor = app.get_outdoor()
        min_value = 0.25
        self.max_uvsize = outdoor.getheightmap().getgridscale() * \
                           (outdoor.getblocksize() - 1)
        while min_value <= self.max_uvsize:
            self.choice_u.Append( str(min_value) )
            self.choice_v.Append( str(min_value) )
            min_value = min_value * 2
        
        # valid texture files
        self.__update_combo_local_texture()
        
        # default texture files
        self.button_shared_texture.SetLabel(
            "dds/ground/cesped_test_terrain.dds" 
            )
        if self.combo_local_texture.GetCount() == 0:
            self.radio_local_texture.Enable(False)
        else:
            self.combo_local_texture.SetSelection(0)
        self.radio_local_texture.SetValue(False)
        self.radio_shared_texture.SetValue(True)
        
        # default UV size
        self.choice_u.SetValue("1.0")
        self.choice_v.SetValue("1.0")
        
        # align texture box fields by setting all labels to longest length
        format.align_labels([
            self.radio_local_texture,
            self.radio_shared_texture,
            self.label_u,
            self.label_projection
            ])
        
        # valid game materials
        self.__update_game_materials_choicer()
        
        # current settings
        if self.layer_ctrl is not None:
            self.text_name.SetValue( self.layer_ctrl.get_layer_name() )
            mangled_path = format.mangle_path(
                                    self.layer_ctrl.get_texture_filename() 
                                    )
            mangled_local_dir = format.mangle_path( self.get_textures_path() )
            mangled_shared_dir = format.mangle_path( self.get_shared_path() )
            if mangled_path.startswith( mangled_local_dir ):
                self.combo_local_texture.SetStringSelection(
                    format.get_relative_path( mangled_local_dir, mangled_path )
                    )
                self.radio_shared_texture.SetValue(False)
                self.radio_local_texture.SetValue(True)
            else:
                self.button_shared_texture.SetLabel(
                    format.get_relative_path( mangled_shared_dir, mangled_path )
                    )
                self.radio_local_texture.SetValue(False)
                self.radio_shared_texture.SetValue(True)
            uv_scale = self.layer_ctrl.get_uv_scale()
            self.choice_u.SetValue( str( self.uvscale2size(uv_scale[0]) ) )
            self.choice_v.SetValue( str( self.uvscale2size(uv_scale[1]) ) )
            self.choice_projection.SetSelection(
                self.layer_ctrl.get_projection()
                )
            color = self.layer_ctrl.get_color_mask().Get()
            self.color_sel.set_value(color)
    
    def __update_combo_local_texture(self):
        selection = self.combo_local_texture.GetStringSelection()
        self.combo_local_texture.Clear()
        self.combo_local_texture.AppendItems( filedlg.get_file_list(
            self.get_textures_path(), ['dds'], autoextension=False,
            recursive=True) )
        if selection != "":
            self.combo_local_texture.SetStringSelection( selection )
    
    def __update_game_materials_choicer(self):
        # Fill list
        self.choice_game_material.Clear()
        materials = []
        mat = pynebula.lookup('/usr/gamematerials').gethead()
        while mat is not None:
            materials.append( mat.getname() )
            mat = mat.getsucc()
        materials.sort()
        self.choice_game_material.AppendItems( materials )
        # Select material
        if self.layer_ctrl is not None:
            material = self.layer_ctrl.get_game_material_name()
            if material != "":
                self.choice_game_material.SetStringSelection( material )
        elif len( materials ) > 0:
            self.choice_game_material.SetSelection(0)

    def do_layout(self, parent_sizer):
        sizer_projection = wx.BoxSizer(wx.HORIZONTAL)
        sizer_scaling = wx.BoxSizer(wx.HORIZONTAL)
        sizer_shared_texture = wx.BoxSizer(wx.HORIZONTAL)
        sizer_local_texture = wx.BoxSizer(wx.HORIZONTAL)
        sizer_texture_box = wx.StaticBoxSizer(
                                        self.staticbox_texture,
                                        wx.VERTICAL
                                        )
        sizer_name = wx.BoxSizer(wx.HORIZONTAL)
        sizer_game_material = wx.StaticBoxSizer(
                                            self.staticbox_game_material, 
                                            wx.VERTICAL
                                            )
        sizer_game_material_name = wx.BoxSizer(wx.HORIZONTAL)
        sizer_name.Add(
            self.label_name, 
            0, 
            wx.ALIGN_CENTER_VERTICAL|wx.FIXED_MINSIZE 
            )
        sizer_name.Add(
            self.text_name, 
            1, 
            wx.LEFT|wx.ALIGN_CENTER_VERTICAL|wx.FIXED_MINSIZE,
            cfg.BORDER_WIDTH
            )
        parent_sizer.Add(
            sizer_name, 
            0,
            wx.EXPAND
            )
        sizer_local_texture.Add(
            self.radio_local_texture, 
            0, 
            wx.RIGHT|wx.ALIGN_CENTER_VERTICAL|wx.FIXED_MINSIZE, 
            cfg.BORDER_WIDTH
            )
        sizer_local_texture.Add(
            self.combo_local_texture, 
            1, 
            wx.RIGHT|wx.ALIGN_CENTER_VERTICAL|wx.FIXED_MINSIZE, 
            cfg.BORDER_WIDTH
            )
        sizer_local_texture.Add(
            self.button_import, 
            0,
            wx.ALIGN_CENTER_VERTICAL|wx.FIXED_MINSIZE
            )
        sizer_texture_box.Add(
            sizer_local_texture, 
            0, 
            wx.ALL|wx.EXPAND, 
            cfg.BORDER_WIDTH
            )
        sizer_shared_texture.Add(
            self.radio_shared_texture,
            0, 
            wx.RIGHT|wx.ALIGN_CENTER_VERTICAL|wx.FIXED_MINSIZE, 
            cfg.BORDER_WIDTH
            )
        sizer_shared_texture.Add(
            self.button_shared_texture,
            1,
            wx.ALIGN_CENTER_VERTICAL|wx.FIXED_MINSIZE
            )
        sizer_texture_box.Add(
            sizer_shared_texture,
            0, 
            wx.LEFT|wx.RIGHT|wx.BOTTOM|wx.EXPAND, 
            cfg.BORDER_WIDTH
            )
        sizer_scaling.Add(
            self.label_u,
            0,
            wx.RIGHT|wx.ALIGN_CENTER_VERTICAL|wx.FIXED_MINSIZE,
            cfg.BORDER_WIDTH
            )
        sizer_scaling.Add(
            self.choice_u,
            0, 
            wx.RIGHT|wx.ALIGN_CENTER_VERTICAL|wx.FIXED_MINSIZE, 
            cfg.BORDER_WIDTH
            )
        sizer_scaling.Add(
            self.label_v,
            0, 
            wx.LEFT|wx.RIGHT|wx.ALIGN_CENTER_VERTICAL|wx.FIXED_MINSIZE, 
            cfg.BORDER_WIDTH
            )
        sizer_scaling.Add(
            self.choice_v, 
            0,
            wx.ALIGN_CENTER_VERTICAL|wx.FIXED_MINSIZE
            )
        sizer_texture_box.Add(
            sizer_scaling,
            0,
            wx.LEFT|wx.RIGHT|wx.EXPAND,
            cfg.BORDER_WIDTH
            )
        sizer_projection.Add(
            self.label_projection, 
            0,
            wx.RIGHT|wx.ALIGN_CENTER_VERTICAL|wx.FIXED_MINSIZE, 
            cfg.BORDER_WIDTH
            )
        sizer_projection.Add(
            self.choice_projection,
            0, 
            wx.ALIGN_CENTER_VERTICAL|wx.FIXED_MINSIZE
            )
        sizer_texture_box.Add(
            sizer_projection,
            0, 
            wx.ALL|wx.EXPAND, 
            cfg.BORDER_WIDTH
            )
        parent_sizer.Add(
            sizer_texture_box, 
            0,
            wx.TOP|wx.EXPAND, 
            cfg.BORDER_WIDTH*2
            )
        parent_sizer.Add(
            self.color_sel, 
            0, 
            wx.TOP|wx.EXPAND, 
            cfg.BORDER_WIDTH*2
            )
        sizer_game_material_name.Add(
            self.label_game_material,
            0, 
            wx.ALIGN_CENTER_VERTICAL|wx.FIXED_MINSIZE
            )
        sizer_game_material_name.Add(
            self.choice_game_material, 
            1, 
            wx.LEFT|wx.ALIGN_CENTER_VERTICAL, 
            cfg.BORDER_WIDTH
            )
        sizer_game_material.Add(
            sizer_game_material_name,
            0, 
            wx.ALL|wx.EXPAND,
            cfg.BORDER_WIDTH
            )
        parent_sizer.Add(
            sizer_game_material, 
            0, 
            wx.TOP|wx.EXPAND, 
            cfg.BORDER_WIDTH
            )

    def bind_events(self):
        self.window.Bind(
            wx.EVT_RADIOBUTTON, 
            self.on_local_texture,
            self.radio_local_texture
            )
        self.window.Bind(
            wx.EVT_RADIOBUTTON, 
            self.on_shared_texture, 
            self.radio_shared_texture
            )
        self.window.Bind(
            wx.EVT_COMBOBOX, 
            self.on_choose_local_texture, 
            self.combo_local_texture
                )
        self.window.Bind(
            wx.EVT_BUTTON, 
            self.on_import_texture,
            self.button_import
            )
        self.window.Bind(
            wx.EVT_BUTTON, 
            self.on_choose_shared_texture, 
            self.button_shared_texture
            )
        self.window.Bind(
            wx.EVT_CHOICE, 
            self.on_choose_game_material,
            self.choice_game_material
            )

    def on_local_texture(self, event):
        self.radio_shared_texture.SetValue(False)

    def on_shared_texture(self, event):
        self.radio_local_texture.SetValue(False)

    def on_choose_local_texture(self, event):
        if self.combo_local_texture.GetStringSelection() != "":
            self.radio_local_texture.Enable(True)
            self.radio_shared_texture.SetValue(False)
            self.radio_local_texture.SetValue(True)

    def on_import_texture(self, event):
        target_dir = guisettings.Repository.getsettingvalue(
                                guisettings.ID_BrowserPath_LocalMaterial 
                                )
        if target_dir == "":
            target_dir = servers.get_file_server().manglepath(
                                "textures:"
                                )
        dlg = wx.FileDialog(
                    self.window, message="Choose an image file",
                    defaultDir = target_dir,
                    wildcard="Image files (*.dds)|*.dds",
                    style=wx.OPEN
                    )
        
        if dlg.ShowModal() == wx.ID_OK:
            if filedlg.copy_file( dlg.GetPath(), self.get_textures_path() ):
                self.__update_combo_local_texture()
            # Record last directory
            target_dir = format.get_directory( dlg.GetPath() )
            guisettings.Repository.setsettingvalue( 
                guisettings.ID_BrowserPath_LocalMaterial, 
                target_dir 
                )
        
        dlg.Destroy()

    def on_choose_shared_texture(self, event):
        # let the user choose a file among the shared textures
        mangled_shared_dir = format.mangle_path( self.get_shared_path() )
        target_dir = guisettings.Repository.getsettingvalue(
                                guisettings.ID_BrowserPath_SharedMaterial 
                                )
        if target_dir == "":
            target_dir = mangled_shared_dir
        dlg = wx.FileDialog(
            self.window, message="Choose an image file",
            defaultDir = target_dir,
            wildcard="Image files (*.dds)|*.dds",
            style=wx.OPEN
            )
        
        if dlg.ShowModal() == wx.ID_OK:
            mangled_path = format.mangle_path( dlg.GetPath() )
            if not mangled_path.startswith( mangled_shared_dir ):
                cjr.show_error_message(
                    "You should choose a texture file from the " \
                    "'%s' directory ' or below."  % self.get_shared_path()
                    )
            else:
                self.button_shared_texture.SetLabel(
                    format.get_relative_path( mangled_shared_dir, mangled_path )
                    )
                self.radio_shared_texture.Enable(True)
                self.radio_local_texture.SetValue(False)
                self.radio_shared_texture.SetValue(True)
                # Record last directory
                target_dir = format.get_directory( mangled_path )
                guisettings.Repository.setsettingvalue(
                    guisettings.ID_BrowserPath_SharedMaterial, 
                    target_dir 
                    )
        
        dlg.Destroy()

    def on_choose_game_material(self, event):
        pass

    def get_textures_path(self):
        return "wc:export/assets/%s/textures" % app.get_outdoor().getname() 

    def get_shared_path(self):
        return "wc:export/textures"
    
    def uvsize2scale(self, size):
        return 1 / size
    
    def uvscale2size(self, scale):
        return 1 / scale


# LayerDialog class
class LayerDialog(wx.Dialog):
    """Base class for new and settings layer dialogs"""
    
    def __init__(self, parent, title, layer_ctrl=None):
        wx.Dialog.__init__(
            self, 
            parent, 
            -1, 
            title,
            style = wx.DEFAULT_DIALOG_STYLE|wx.RESIZE_BORDER|wx.TAB_TRAVERSAL
            )
        
        self.settings = SettingsGroup(self, layer_ctrl)
        self.button_ok = wx.Button(self, -1, "&OK")
        self.button_cancel = wx.Button(self, wx.ID_CANCEL, "&Cancel")

        self.__do_layout()
        self.__bind_events()

    def __do_layout(self):
        border_sizer = wx.BoxSizer(wx.VERTICAL)
        layout_sizer = wx.BoxSizer(wx.VERTICAL)
        self.settings.do_layout(layout_sizer)
        # add sizer to peg buttons to bottom
        layout_sizer.Add(
            (-1, -1),
            1,
            wx.EXPAND
            )
        sizer_buttons = wx.BoxSizer(wx.HORIZONTAL)
        sizer_buttons.Add(
            self.button_ok, 
            0,
            wx.FIXED_MINSIZE
            )
        sizer_buttons.Add(
            self.button_cancel, 
            0, 
            wx.LEFT|wx.FIXED_MINSIZE,
            cfg.BORDER_WIDTH
            )
        layout_sizer.Add(
            sizer_buttons, 
            0, 
            wx.TOP|wx.ALIGN_RIGHT, 
            cfg.BORDER_WIDTH
            )
        border_sizer.Add(
            layout_sizer,
            1, 
            wx.ALL|wx.EXPAND, 
            cfg.BORDER_WIDTH
            )
        self.SetSizerAndFit(border_sizer)

    def __bind_events(self):
        self.settings.bind_events()
        self.Bind(wx.EVT_BUTTON, self.on_ok, self.button_ok)
        self.Bind(wx.EVT_BUTTON, self.on_cancel, self.button_cancel)

    def on_ok(self, event):
        try:
            float( self.settings.choice_u.GetValue() )
            float( self.settings.choice_v.GetValue() )
        except:
            cjr.show_error_message(
                "Bad u and v values"
                )
            return
        self.EndModal(wx.ID_OK)

    def on_cancel(self, event):
        self.EndModal(wx.ID_CANCEL)

    def apply_layer_settings(self, layer_ctrl):
        u_value = float( self.settings.choice_u.GetValue() )
        v_value = float( self.settings.choice_v.GetValue() )
        
        layer_ctrl.set_layer_name(
            str( self.settings.text_name.GetValue() )
            )
        if self.settings.radio_local_texture.GetValue():
            layer_ctrl.set_texture_filename(
                str(
                    format.append_to_path(
                        self.settings.get_textures_path(),
                        self.settings.combo_local_texture.GetStringSelection()
                        )
                    )
                )
        else:
            layer_ctrl.set_texture_filename(
                str(
                    format.append_to_path(
                        self.settings.get_shared_path(),
                        self.settings.button_shared_texture.GetLabel()
                        )
                    )
                )
        layer_ctrl.set_uv_scale((
            self.settings.uvsize2scale(u_value),
            self.settings.uvsize2scale(v_value)
            ))
        layer_ctrl.set_projection(
            self.settings.choice_projection.GetSelection() 
            )
        layer_ctrl.set_color_mask(
            self.settings.color_sel.get_value() 
            )
        layer_ctrl.set_game_material(
            self.settings.choice_game_material.GetStringSelection() 
            )
        
        # update material info
        try:
            app.get_outdoor().updatematerialall()
        except:
            pass


# NewLayerDialog class
class NewLayerDialog(LayerDialog):
    """Dialog to set the initial settings for a new terrain texture layer"""
    def __init__(self, parent):
        LayerDialog.__init__(self, parent, "New terrain material")


# LayerSettingsDialog class
class LayerSettingsDialog(LayerDialog):
    """Dialog to change the settings of a terrain texture layer"""
    def __init__(self, parent, layer_ctrl):
        LayerDialog.__init__(
            self, 
            parent,
            "Terrain material settings", 
            layer_ctrl
            )

