##\file newsoundsdlg.py
##\brief Dialog to add new sound

import wx

import os.path

import conjurerconfig as cfg
import conjurerframework as cjr
import format


class NewSoundDialog(wx.Dialog):
    """Dialog to create a new sound system"""
    def __init__(self, parent, sound_library, location):
        wx.Dialog.__init__(
            self, 
            parent, 
            -1, 
            "Add sound",
            style = cfg.DEFAULT_DIALOG_STYLE
            )
        self.sound_library = sound_library
        self.location = location
        self.label_resource = wx.StaticText(
                                        self, 
                                        -1, 
                                        "Sound file"
                                        )
        self.label_show_resource = wx.StaticText(
                                                self, 
                                                -1, 
                                                style=wx.SUNKEN_BORDER | wx.ST_NO_AUTORESIZE
                                                )
        self.button_resource = wx.Button(
                                                    self, 
                                                    -1, 
                                                    "&Browse...", 
                                                    style=wx.BU_EXACTFIT
                                                    )
        self.label_sound_id = wx.StaticText(
            self, 
            -1, 
            "Sound ID", 
            style=wx.ALIGN_RIGHT
            )
        self.text_sound_id = wx.TextCtrl(self, -1, "")
        self.button_ok = wx.Button(self, -1, "&OK")
        #giving cancel button ID_CANCEL so it will close when ESC is pressed
        self.button_cancel = wx.Button(self, wx.ID_CANCEL, "&Cancel")
        self.SetDefaultItem(self.button_ok)

        self.__set_properties()
        self.__do_layout()
        self.__bind_events()
    
    def __set_properties(self):
        # Disable ok button since there's no sound id entered on start up
        self.update_button_ok()

    def __do_layout(self):
        border_width = cfg.BORDER_WIDTH
        sizer_layout = wx.BoxSizer(wx.VERTICAL)
        grid_sizer = wx.FlexGridSizer(
                            2, 
                            3, 
                            border_width, 
                            border_width * 2
                            )
        grid_sizer.AddGrowableCol(1)
        grid_sizer.Add(
            self.label_resource, 
            0, 
            cfg.LABEL_ALIGN | wx.ALIGN_CENTER_VERTICAL | wx.ADJUST_MINSIZE, 
            0
            )
        grid_sizer.Add(
            self.label_show_resource, 
            0, 
            wx.ADJUST_MINSIZE | wx.EXPAND
            )
        grid_sizer.Add(
            self.button_resource
            )
        grid_sizer.Add(
            self.label_sound_id, 
            0, 
            cfg.LABEL_ALIGN | wx.ALIGN_CENTER_VERTICAL | wx.ADJUST_MINSIZE, 
            0
            )
        grid_sizer.Add(
            self.text_sound_id, 
            0, 
            wx.EXPAND,
            0
            )
        grid_sizer.AddSpacer( (0, 0) )
        sizer_layout.Add(
            grid_sizer, 
            1,
            wx.ALL | wx.EXPAND, 
            border_width
            )
        horizontal_line = wx.StaticLine(
                                    self, 
                                    -1, 
                                    (-1, -1), 
                                    (-1, -1), 
                                    wx.LI_HORIZONTAL 
                                    )
        sizer_layout.Add(
            horizontal_line, 
            0, 
            wx.EXPAND
            )
        sizer_buttons = wx.BoxSizer(wx.HORIZONTAL)
        sizer_buttons.Add(
            self.button_ok, 
            wx.ADJUST_MINSIZE,
            0)
        sizer_buttons.Add(
            self.button_cancel, 
            0, 
            wx.LEFT | wx.ADJUST_MINSIZE, 
            border_width
            )
        sizer_layout.Add(
            sizer_buttons, 
            0, 
            wx.ALL | wx.ALIGN_RIGHT, 
            border_width
            )
        self.SetAutoLayout(True)
        self.SetSizer(sizer_layout)
        sizer_layout.Fit(self)
        sizer_layout.SetSizeHints(self)
        self.Layout()
        self.SetSize( (380, -1) )
    
    def __bind_events(self):
        self.Bind(wx.EVT_BUTTON, self.on_resource, self.button_resource)
        self.Bind(wx.EVT_TEXT, self.on_change_name, self.text_sound_id)
        self.Bind(wx.EVT_BUTTON, self.on_ok, self.button_ok)
        self.Bind(wx.EVT_BUTTON, self.on_cancel, self.button_cancel)
    
    def __has_valid_name(self):
        name = self.get_sound_id()
        if name == "":
            return False
        else:
            return True
    
    def update_button_ok(self):
        enable = self.__has_resource() and \
                        self.__has_valid_name() 
        self.button_ok.Enable( enable )

    def on_change_name(self, event):
        self.update_button_ok()

    def set_resource(self, path_string):
        self.label_show_resource.SetLabel(path_string)
        if path_string:
            self.update_sound_id()

    def __has_sound_id(self):
        return self.get_sound_id()

    def update_sound_id(self):
        if not self.__has_sound_id():
            self.set_sound_id(
                self.suggest_sound_id_from(
                    self.get_resource_file_name_no_suffix()
                    )
                )

    def set_sound_id(self, id_string):
        self.text_sound_id.SetValue(id_string)

    def __get_mangled_sound_root_dir(self):
        return self.sound_library.get_mangled_root_directory()

    def suggest_sound_id_from(self, base_id, unique_id = False):
        if unique_id:
            i = 0
            while True:
                i = i + 1
                suggested_id = base_id + str(i).zfill(3)
                if not self.__does_sound_exist_with_id(suggested_id):
                    break
        else:
            suggested_id = base_id
        return suggested_id

    def __does_sound_exist_with_id(self, some_id):
        return self.sound_library.get_sound_with_id(some_id)

    def on_resource(self, event):
        # Let the user choose a file 
        mangled_dir = format.mangle_path(self.location)
        dlg = wx.FileDialog(
            self, 
            message="Choose a sound file",
            defaultDir = mangled_dir,
            wildcard="WAV files (*.wav)|*.wav|OGG files (*.ogg)|*.ogg",
            style=wx.OPEN
            )

        if dlg.ShowModal() == wx.ID_OK:
            path_string =  dlg.GetPath() 
            if not self.__is_valid_resource_name(path_string):
                msg = "You must choose a sound file from %s" % self.location
                cjr.show_error_message(msg)
            else:
                self.set_resource(
                    format.get_relative_path(
                        self.__get_mangled_sound_root_dir(), 
                        format.mangle_path(path_string)
                        )
                    )
        dlg.Destroy()
    
    def __is_valid_resource_name(self, resource_path):
        mangled_path = format.mangle_path( resource_path )
        mangled_sound_directory = self.__get_mangled_sound_root_dir()
        return mangled_path.startswith(mangled_sound_directory)
    
    def get_sound_id(self):
        return self.text_sound_id.GetValue()
        
    def get_resource_name(self):
        return self.label_show_resource.GetLabel()

    def get_resource_file_name(self):
        return os.path.basename(
            self.get_resource_name()
            )
    
    def get_resource_file_name_no_suffix(self):
        file_name = self.get_resource_file_name()
        return ( os.path.splitext(file_name ) ) [0]

    def __has_resource(self):
        return self.get_resource_name() != ''

    def on_ok(self, event):
        try:
            wx.BeginBusyCursor()
            sound_id = str( self.get_sound_id() )
            all_ok = self.sound_library.create_new_sound_for_id_and_resource(
                            sound_id, 
                            self.get_resource_name()
                            )
            if all_ok:
                self.EndModal(wx.ID_OK)
            else:
                cjr.show_error_message("Unable to create the new sound.")
        finally:
            wx.EndBusyCursor()

    def on_cancel(self, event):
        self.EndModal(wx.ID_CANCEL)


def create_window(parent_view, sound_library, location):
    return NewSoundDialog(
        parent_view, 
        sound_library,
        location)
