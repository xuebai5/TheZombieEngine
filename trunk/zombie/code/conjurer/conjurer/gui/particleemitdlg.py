##\file particleemitdlg.py
##\brief Particle emitter list dialog

import wx

import servers
import app
import format

import childdialoggui
import objdlg
import particle2
import nebulaguisettings as guisettings

import conjurerconfig as cfg
import conjurerframework as cjr


class NewParticleEmitterDialog(wx.Dialog):
    """ Dialog to create a new particle emitter """
    def __init__(self, parent, particle_system):
        wx.Dialog.__init__(
            self, 
            parent, 
            -1, 
            "New particle emitter",
            style = cfg.DEFAULT_DIALOG_STYLE
            )
        self.particle_system = particle_system
        self.label_new_name = wx.StaticText(
                                            self, 
                                            -1, 
                                            "Emitter name", 
                                            style=wx.ALIGN_RIGHT
                                            )
        self.text_new_name = wx.TextCtrl(
                                            self, 
                                            -1, 
                                            ""
                                            )
        self.staticbox_texture_grp = wx.StaticBox(
                                                        self, 
                                                        -1, 
                                                        "Texture"
                                                        )
        self.check_default_texture = wx.CheckBox(
                                                    self, 
                                                    -1, 
                                                    "Use default"
                                                    )
        self.label_custom_texture = wx.StaticText(
                                                self, 
                                                -1, 
                                                "Custom texture"
                                                )
        self.label_texture_name = wx.StaticText(
                                                self, 
                                                -1, 
                                                style=wx.SUNKEN_BORDER
                                                        |wx.ST_NO_AUTORESIZE,
                                                size=( (130, -1) )
                                                )
        self.button_custom_texture = wx.Button(
                                                    self, 
                                                    -1, 
                                                    "&Browse...", 
                                                    style=wx.BU_EXACTFIT
                                                    )
        self.button_ok = wx.Button(self, -1, "&OK")
        # magic ID means it will close when ESC is pressed
        self.button_cancel = wx.Button(self, wx.ID_CANCEL, "&Cancel")
        self.SetDefaultItem(self.button_ok)

        self.__set_properties()
        self.__do_layout()
        self.__bind_events()

    def __set_properties(self):
        #set the max length of the new name text field
        self.text_new_name.SetMaxLength(cfg.MAX_LENGTH_CLASS_NAME)
        #Set the initial selection to use the default texture
        self.check_default_texture.SetValue(True)
        #Disable custom texture controls
        self.update_custom_texture_controls()
        # Disable ok button since there's no name entered on start up
        self.update_button_ok()

    def __do_layout(self):
        sizer_layout = wx.BoxSizer(wx.VERTICAL)
        sizer_buttons = wx.BoxSizer(wx.HORIZONTAL)
        grid_sizer = wx.FlexGridSizer(
                            1, 
                            2, 
                            cfg.BORDER_WIDTH, 
                            cfg.BORDER_WIDTH * 2
                            )
        grid_sizer.AddGrowableCol(1)
        grid_sizer.Add(
            self.label_new_name, 
            0, 
            cfg.LABEL_ALIGN|wx.ALIGN_CENTER_VERTICAL|wx.ADJUST_MINSIZE
            )
        grid_sizer.Add(
            self.text_new_name, 
            0, 
            wx.ADJUST_MINSIZE|wx.EXPAND
            )
        sizer_layout.Add(
            grid_sizer, 
            0, 
            wx.ALL|wx.EXPAND,  
            cfg.BORDER_WIDTH
            )
        sizer_textures_grp = wx.StaticBoxSizer(
                                            self.staticbox_texture_grp,
                                            wx.VERTICAL
                                            )
        grid_sizer_texture = wx.FlexGridSizer(
                                    2, 
                                    3, 
                                    cfg.BORDER_WIDTH, 
                                    cfg.BORDER_WIDTH * 2
                                    )
        grid_sizer_texture.AddGrowableCol(1)
        grid_sizer_texture.Add(
            self.check_default_texture,
            0, 
            wx.EXPAND|wx.ALIGN_CENTER_VERTICAL
            )
        grid_sizer_texture.AddSpacer( (0, 0) )
        grid_sizer_texture.AddSpacer( (0, 0) )
        grid_sizer_texture.Add(
            self.label_custom_texture, 
            0, 
            cfg.LABEL_ALIGN|wx.ALIGN_CENTER_VERTICAL|wx.ADJUST_MINSIZE
            )
        grid_sizer_texture.Add(
            self.label_texture_name, 
            0, 
            wx.EXPAND
            )
        grid_sizer_texture.Add(
            self.button_custom_texture,
            0
            )
        sizer_textures_grp.Add(
            grid_sizer_texture,
            0,
            wx.ALL|wx.EXPAND,
            cfg.BORDER_WIDTH
            )
        sizer_layout.Add(
            sizer_textures_grp, 
            1, 
            wx.ALL|wx.EXPAND,  
            cfg.BORDER_WIDTH
            )
        sizer_buttons.Add(
            self.button_ok, 
            0, 
            wx.ADJUST_MINSIZE, 
            )
        sizer_buttons.Add(
            self.button_cancel, 
            0, 
            wx.LEFT|wx.ADJUST_MINSIZE,  
            cfg.BORDER_WIDTH
            )
        sizer_layout.Add(
            sizer_buttons, 
            0, 
            wx.ALL|wx.ALIGN_RIGHT, 
            cfg.BORDER_WIDTH
            )
        self.SetAutoLayout(True)
        self.SetSizerAndFit(sizer_layout)
        self.Layout()

    def __bind_events(self):
        self.Bind(
            wx.EVT_TEXT, 
            self.on_change_name, 
            self.text_new_name
            )
        self.Bind(
            wx.EVT_BUTTON, 
            self.on_custom_texture, 
            self.button_custom_texture
            )
        self.Bind(
            wx.EVT_BUTTON, 
            self.on_ok, 
            self.button_ok
            )
        self.Bind(
            wx.EVT_BUTTON, 
            self.on_cancel, 
            self.button_cancel
            )
        self.Bind(
            wx.EVT_CHECKBOX, 
            self.on_change_use_default_texture, 
            self.check_default_texture
            )

    def has_valid_class_name(self):
        name = self.get_new_class_name()
        if name == "":
            return False
        else:
            ecs = servers.get_entity_class_server()
            return ecs.checkclassname( str( name.capitalize() ) )

    def has_valid_texture_setting(self):
        if self.is_use_default_texture_selected():
            return True
        else:
            return self.has_custom_texture()
            
    def has_custom_texture(self):
        return self.get_custom_texture() != ""

    def get_custom_texture(self):
        return self.label_texture_name.GetLabel()

    def update_button_ok(self):
        enable = self.has_valid_texture_setting() and \
                        self.has_valid_class_name() 
        self.button_ok.Enable( enable )

    def update_custom_texture_controls(self):
        if self.is_use_default_texture_selected():
            self.disable_and_clear_custom_texture_controls()
        else:
            self.enable_custom_texture_controls()

    def enable_disable_custom_texture_ctrls(self, boolean):
        self.label_custom_texture.Enable(boolean)
        self.label_texture_name.Enable(boolean)
        self.button_custom_texture.Enable(boolean)

    def enable_custom_texture_controls(self):
        self.enable_disable_custom_texture_ctrls(True)

    def disable_and_clear_custom_texture_controls(self):
        self.enable_disable_custom_texture_ctrls(False)
        self.label_texture_name.SetLabel("")

    def on_change_name(self, event):
        self.update_button_ok()

    def on_change_use_default_texture(self, event):
        self.update_custom_texture_controls()
        self.update_button_ok()

    def set_custom_texture(self, label_string):
        self.label_texture_name.SetLabel(label_string)
        self.update_button_ok()

    def on_custom_texture(self, event):
        # Let the user choose a file from the shared textures
        mangled_shared_dir = format.mangle_path( "wc:export/textures" )
        directory = guisettings.Repository.getsettingvalue(
                            guisettings.ID_BrowserPath_SharedMaterial
                            )
        if directory == "":
            directory = mangled_shared_dir
        dlg = wx.FileDialog(
            self, message="Choose an image file",
            defaultDir = directory,
            wildcard="Image files (*.dds)|*.dds",
            style=wx.OPEN
            )

        if dlg.ShowModal() == wx.ID_OK:
            mangled_path = format.mangle_path( dlg.GetPath() )
            if not mangled_path.startswith( mangled_shared_dir ):
                msg = "You should choose a texture file from the" \
                  " directory 'wc:export/textures' or below."
                cjr.show_error_message(msg)
            else:
                self.set_custom_texture(
                    format.get_relative_path(
                        mangled_shared_dir, 
                        mangled_path 
                        )
                    )
                # Record last directory
                directory = format.get_directory(mangled_path)
                guisettings.Repository.setsettingvalue(
                    guisettings.ID_BrowserPath_SharedMaterial, 
                    directory
                    )
        dlg.Destroy()

    def is_use_default_texture_selected(self):
        return self.check_default_texture.GetValue()

    def get_new_class_name(self):
        return self.text_new_name.GetValue()

    def get_custom_texture_name(self):
        return self.label_texture_name.GetLabel()

    def on_ok(self, event):
        try:
            wx.BeginBusyCursor()
            emitter_name = str( self.get_new_class_name() )
            if self.is_use_default_texture_selected():
                emitter = particle2.add_emitter(
                                self.particle_system, 
                                emitter_name
                                )
            else:
                texture = format.append_to_path(
                                "wc:export/textures", 
                                self.get_custom_texture_name() 
                                )
                emitter = particle2.add_emitter(
                                self.particle_system, 
                                emitter_name,
                                str(texture) 
                                )
            if emitter is None:
                msg = "Unable to create '%s' particle emitter"  % emitter_name
                cjr.show_error_message(msg)
            else:
                self.EndModal(wx.ID_OK)
        finally:
            wx.EndBusyCursor()

    def on_cancel(self, event):
        self.EndModal(wx.ID_CANCEL)


class ParticleEmitterListDialog(childdialoggui.InstanceTrackingChildDialogGUI):
    hide_text = "Hide"
    show_text = "Show"
    
    def init(self, parent, particle_system_name):
        childdialoggui.InstanceTrackingChildDialogGUI.init(
            self,
            "",
            parent
            )
        ec_server = servers.get_entity_class_server()
        self.particle_system = ec_server.getentityclass(
                                            particle_system_name
                                            )
        self.list_emitters = wx.ListBox(
                        self, 
                        -1, 
                        choices=[], 
                        style=wx.LB_SORT
                        )
        self.button_new = wx.Button(self, -1, "&New")
        self.button_edit = wx.Button(self, -1, "&Edit")
        self.button_delete = wx.Button(self, -1, "&Delete")
        self.button_visibility = wx.Button(self, -1, "Hide")
        self.button_close = wx.Button(self, wx.ID_CANCEL, "&Close")

        self.__set_properties()
        self.__do_layout()
        self.__bind_events()
    
    def __set_properties(self):
        self.__build_emitter_list()
        self.__set_title()

    def __set_title(self):
        self.SetTitle(
            "Particle Emitters for '%s' System" 
                % self. __get_particle_system_name()
            )

    def __get_particle_system_name(self):
        return self.particle_system.getname()

    def __update_buttons(self):
        should_enable = self.has_item_selected()
        self.button_edit.Enable(should_enable)
        self.button_delete.Enable(should_enable)
        self.button_visibility.Enable(should_enable)
        self.__update_label_on_visibility_button()

    def __get_show_text(self):
        return self.__class__.show_text

    def __get_hide_text(self):
        return self.__class__.hide_text

    def __update_label_on_visibility_button(self):
        if self.has_item_selected():
            selected_item = self.get_selected_emitter()
            if selected_item.getinvisible():
                label_string = self.__get_show_text()
            else:
                label_string = self.__get_hide_text()
        else:
            label_string = self.__get_hide_text()
        self.button_visibility.SetLabel(label_string)

    def __build_emitter_list(self):
        self.particle_system.loadresources()
        self.list_emitters.Clear()
        root_node = self.particle_system.getrootnode()
        if root_node is not None:
            for each_child in root_node.getchildren():
                self.list_emitters.Append( each_child.getname(), each_child )
        self.__update_buttons()

    def __do_layout(self):
        sizer_main = wx.BoxSizer(wx.VERTICAL)
        sizer_list_and_buttons = wx.BoxSizer(wx.HORIZONTAL)
        sizer_list_and_buttons.Add(
            self.list_emitters,
            1, 
            wx.EXPAND|wx.ALL, 
            cfg.BORDER_WIDTH
            )
        vertical_line = wx.StaticLine(
                                self, 
                                -1, 
                                ( -1, -1 ), 
                                ( -1, -1 ), 
                                wx.LI_VERTICAL
                                )
        sizer_list_and_buttons.Add(
            vertical_line, 
            0, 
            wx.EXPAND|wx.TOP|wx.BOTTOM, 
            cfg.BORDER_WIDTH
            )        
        sizer_buttons = wx.BoxSizer(wx.VERTICAL)        
        sizer_buttons.Add(
            self.button_new,
            0,
            wx.FIXED_MINSIZE
            )
        sizer_buttons.Add(
            self.button_edit, 
            0, 
            wx.TOP|wx.FIXED_MINSIZE, 
            cfg.BORDER_WIDTH
            )
        sizer_buttons.Add(
            self.button_delete, 
            0, 
            wx.TOP|wx.FIXED_MINSIZE, 
            cfg.BORDER_WIDTH
            )
        sizer_buttons.Add(
            self.button_visibility, 
            0, 
            wx.TOP|wx.FIXED_MINSIZE, 
            cfg.BORDER_WIDTH
            )
        sizer_list_and_buttons.Add(
            sizer_buttons, 
            0, 
            wx.ALIGN_RIGHT|wx.ALL, 
            cfg.BORDER_WIDTH
            )
        sizer_main.Add(
            sizer_list_and_buttons,
            1,
            wx.EXPAND
            )
        horizontal_line = wx.StaticLine(
                                self, 
                                -1, 
                                ( -1, -1 ), 
                                ( -1, -1 ), 
                                wx.LI_HORIZONTAL
                                )
        sizer_main.Add(
            horizontal_line, 
            0, 
            wx.EXPAND|wx.LEFT|wx.RIGHT, 
            cfg.BORDER_WIDTH
            )
        sizer_main.Add(
            self.button_close, 
            0, 
            wx.FIXED_MINSIZE|wx.ALL|wx.ALIGN_RIGHT, 
            cfg.BORDER_WIDTH
            )
        self.SetAutoLayout(True)
        self.SetSizerAndFit(sizer_main)
        self.SetSize( (270, 280) )
        self.Layout()

    def __bind_events(self):
        self.Bind(
            wx.EVT_BUTTON, 
            self.__on_new, 
            self.button_new
            )
        self.Bind(
            wx.EVT_BUTTON, 
            self.__on_edit, 
            self.button_edit
            )
        self.Bind(
            wx.EVT_LISTBOX_DCLICK, 
            self.__on_edit, 
            self.list_emitters
            )
        self.Bind(
            wx.EVT_BUTTON, 
            self.__on_delete, 
            self.button_delete
            )
        self.Bind(
            wx.EVT_BUTTON, 
            self.__on_visibility, 
            self.button_visibility
            )
        self.Bind(
            wx.EVT_BUTTON, 
            self.__on_close_button_clicked,
            self.button_close
            )
        self.Bind(
            wx.EVT_LISTBOX, 
            self.__on_select_emitter, 
            self.list_emitters
            )

    def get_selected_emitter_name(self):
        return self.list_emitters.GetStringSelection()

    def get_selected_emitter(self):
        """ Return the actual emitter, if one is selected. """
        return self.list_emitters.GetClientData(
            self.list_emitters.GetSelection()
            )

    def has_item_selected(self):
        return self.list_emitters.GetSelection() != wx.NOT_FOUND

    def refresh(self):
        self.__build_emitter_list()

    def __on_new(self, event):
        dlg = NewParticleEmitterDialog(
                    self, 
                    self.particle_system
                    )
        result = dlg.ShowModal()
        dlg.Destroy()
        if result == wx.ID_OK:
            self.refresh()

    def __on_edit(self, event):
        if self.has_item_selected():
            particle_emitter = self.get_selected_emitter()
            try:
                wx.BeginBusyCursor()
                win = objdlg.create_window(
                            app.get_top_window() , 
                            particle_emitter.getfullname()
                        )
            finally:
                wx.EndBusyCursor()
            win.display()
        else:
            cjr.show_error_message(
                "Please select a Particle Emitter to edit."
                )
        event.Skip()

    def __on_delete(self, event):
        if self.has_item_selected():
            emitter_name = self.get_selected_emitter_name()
            result = cjr.warn_yes_no(
                            self,
                            "Are you sure you want to delete the "\
                            "'%s' Particle Emitter?" % emitter_name
                            )
            if result == wx.ID_YES:
                self. __delete_emitter(
                    self.get_selected_emitter() 
                    )
        else:
            cjr.show_error_message(
                "Please select a Particle Emitter to delete."
                )

    def __delete_emitter(self, emitter):
        result = particle2.remove_emitter(
                        self.particle_system,
                        self.get_selected_emitter()
                        )
        if result:
            self.refresh()
        else:
            cjr.show_error_message(
                "Unable to delete Particle Emitter."
                )

    def __on_visibility(self, event):
        """ Toggle visible. """
        if self.has_item_selected():
            emitter = self.get_selected_emitter()
            is_invisible = emitter.getinvisible()
            emitter.setinvisible(not is_invisible)
            self.__update_label_on_visibility_button()
        else:
            cjr.show_error_message(
                "Please select a Particle Emitter to show / hide."
                )

    def __on_close_button_clicked(self, event):
        self.Close()

    def __on_select_emitter(self, event):
        self.__update_buttons()

    def persist(self):
        return [
            create_window, 
            (self.particle_system.getname(),), # trailing comma makes it a tuple
            None
            ]


def create_window(parent, particle_system_name):
    return ParticleEmitterListDialog(parent, particle_system_name)

