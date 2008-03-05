##\file filedlg.py
##\brief File browser constraining the user in a directory

import os
import shutil
import wx

import format
import servers

import conjurerconfig as cfg
import conjurerframework as cjr


# File dialog styles
NEW = 1
OPEN = 2
SAVE = 3
DELETE = 4
SELECT = 5


# get_file_list function
def get_file_list(directory, extensions, autoextension=True, recursive=False,
    excluded_files=[]):
    """
    Return a sequence of the files of the given directory.
    
    The file extension is removed if autoextension is set to True and only one
    file extension is given.
    
    Only the relative path to the given directory is returned for each file,
    not its full path.
    """
    gui_names = []
    mangled_dir = servers.get_file_server().manglepath( str(directory) )
    try:
        filenames = os.listdir( mangled_dir )
    except:
        return []
    for filename in filenames:
        if filename.startswith("."):
            # skip special directories/files
            continue
        file_path = format.append_to_path( mangled_dir, filename )
        if os.path.isdir( file_path ):
            if recursive:
                # recurse directories
                subnames = get_file_list(
                                    file_path, 
                                    extensions, 
                                    autoextension, 
                                    recursive
                                    )
                for name in subnames:
                    gui_names.append( filename + "/" + name )
        else:
            # add the file if its extension matches one of the desired options
            for extension in extensions:
                try:
                    excluded_files.index( filename )
                except:
                    if filename.endswith( "." + extension ):
                        if autoextension:
                            gui_names.append( filename[:-len("."+extension)] )
                        else:
                            gui_names.append( filename )
    return gui_names


# copy_file function
def copy_file(source_file, target_directory):
    """
    Copy a file into a directory.
    
    If there's already another file with the same name in the target directory
    the user is asked to enter a new file name, overwrite the existing one or
    just cancel the operation.
    
    Return true if the file has been copied, false otherwise.
    """
    src = servers.get_file_server().manglepath( str(source_file) )
    dir_name = servers.get_file_server().manglepath( str(target_directory) )
    target = format.append_to_path(
                    dir_name, 
                    format.get_name(src) 
                    )
    while os.path.exists( target ):
        result = cjr.confirm_yes_no(
                        None,
                        "A file called '%s' already exists.\n\n" \
                            "Do you want to rename the new file?\n" \
                            "(answering 'no' will overwrite the old one)"\
                            % format.get_name(src) 
                        )
        if result == wx.ID_YES:
            dlg = wx.TextEntryDialog(
                        None, 
                        "New file name:", 
                        "Conjurer",
                        format.get_name(target)
                        )
            if dlg.ShowModal() == wx.ID_OK and dlg.GetValue() != "":
                target = format.append_to_path( dir, dlg.GetValue() )
        elif result == wx.ID_NO:
            break
        else:
            return False
    shutil.copy(src, target)
    return True


# FileDialog class
class FileDialog(wx.Dialog):
    """
    File browser constraining the user in a directory.
    
    \param parent Parent window
    \param style Style of the dialog (NEW, OPEN, SAVE, DELETE, SELECT)
    \param label1 Lowercase version of the file type's name
    \param label2 Capitalized version of the file type's name
    \param directory Directory where to constraint the file operations (except for import option)
    \param extensions Sequence of the extensions of the files to show (without the dot)
    \param autoextension Hide extension (and autocomplete it) when true (valid only when just one extension is given)
    \param import_button Show an import button (valid only for OPEN and SELECT styles)
    \param excluded_files List of files to not show (with file extension)
    """

    def __init__(self, parent, style, label1, label2, directory, extensions,
                 default_file="", autoextension=True, import_button=True,
                 excluded_files=[]):
        if style == NEW:
            title = "Choose a %s name" % label1
        elif style == OPEN:
            title = "Choose a %s to open" % label1
        elif style == SAVE:
            title = "Save %s as..." % label1
        elif style == DELETE:
            title = "Choose a %s to delete" % label1
        else:
            title = "Choose a %s" % label1
        wx.Dialog.__init__(
            self, 
            parent, 
            title=title,
            style=wx.DEFAULT_DIALOG_STYLE|wx.RESIZE_BORDER
            )
        self.style = style
        self.undirectory = directory
        self.directory = servers.get_file_server().manglepath( str(directory) )
        self.extensions = extensions
        self.autoextension = autoextension and len(extensions)==1
        self.import_button = import_button
        
        self.list_files = wx.ListBox(
                                self, 
                                -1, 
                                choices = get_file_list(
                                                self.directory, 
                                                self.extensions, 
                                                self.autoextension,
                                                excluded_files=excluded_files
                                                )
                                )
        if self.style == NEW or self.style == SAVE:
            self.label_file = wx.StaticText(
                                    self, 
                                    -1, 
                                    "%s name" % label2
                                    )
            self.text_file = wx.TextCtrl(self, -1, default_file)
        elif self.style != DELETE and self.import_button:
            self.button_import = wx.Button(
                                            self, 
                                            -1, 
                                            "&Import a %s" % label1)
        if self.style == NEW:
            ok_caption = "&New"
        elif self.style == OPEN:
            ok_caption = "&Open"
        elif self.style == SAVE:
            ok_caption = "&Save"
        elif self.style == DELETE:
            ok_caption = "&Delete"
        else:
            ok_caption = "&OK"
        self.button_ok = wx.Button(self, -1, ok_caption)
        self.button_cancel = wx.Button(self, wx.ID_CANCEL, "&Cancel")
        
        self.__set_properties()
        self.__do_layout()
        self.__bind_events()
    
    def __set_properties(self):
        # Make the ok button the default one when pressing 'return'
        self.button_ok.SetDefault()
        # Enable/disable ok button
        self.__on_change_filename(None)
    
    def __do_layout(self):
        sizer_border = wx.BoxSizer(wx.VERTICAL)
        sizer_layout = wx.BoxSizer(wx.VERTICAL)
        sizer_buttons = wx.BoxSizer(wx.HORIZONTAL)
        sizer_layout.Add(self.list_files, 1, wx.EXPAND)
        if self.style == NEW or self.style == SAVE:
            sizer_file = wx.BoxSizer(wx.HORIZONTAL)
            sizer_file.Add(
                self.label_file, 
                0, 
                wx.ALIGN_CENTER_VERTICAL|wx.FIXED_MINSIZE
                )
            sizer_file.Add(
                self.text_file, 
                1, 
                wx.LEFT|wx.FIXED_MINSIZE, 
                cfg.BORDER_WIDTH
                )
            sizer_layout.Add(
                sizer_file, 
                0, 
                wx.TOP|wx.EXPAND,
                cfg.BORDER_WIDTH
                )
        elif self.style != DELETE and self.import_button:
            sizer_layout.Add(
                self.button_import,
                0, 
                wx.TOP|wx.EXPAND, 
                cfg.BORDER_WIDTH
                )
        sizer_buttons.Add(
            self.button_ok, 
            1, 
            wx.FIXED_MINSIZE
            )
        sizer_buttons.Add(
            self.button_cancel, 
            1,
            wx.LEFT|wx.FIXED_MINSIZE, 
            cfg.BORDER_WIDTH
            )
        horizontal_line = wx.StaticLine(
                                    self, 
                                    -1, 
                                    ( -1, -1 ), 
                                    ( -1, -1 ), 
                                    wx.LI_HORIZONTAL 
                                    )
        sizer_layout.Add(
            horizontal_line, 
            0, 
            wx.EXPAND|wx.TOP, 
            cfg.BORDER_WIDTH
            )
        sizer_layout.Add(
            sizer_buttons, 
            0, 
            wx.TOP|wx.ALIGN_RIGHT, 
            cfg.BORDER_WIDTH
            )
        sizer_border.Add(
            sizer_layout, 
            1, 
            wx.ALL|wx.EXPAND, 
            cfg.BORDER_WIDTH
            )
        self.SetAutoLayout(True)
        self.SetSizer(sizer_border)
        sizer_border.Fit(self)
        sizer_border.SetSizeHints(self)
        self.Layout()
    
    def __bind_events(self):
        self.Bind(wx.EVT_LISTBOX, self.__on_select_file, self.list_files)
        self.Bind(wx.EVT_LISTBOX_DCLICK, self.__on_ok, self.list_files)
        if self.style == NEW or self.style == SAVE:
            self.Bind(wx.EVT_TEXT, self.__on_change_filename, self.text_file)
        elif self.style != DELETE and self.import_button:
            self.Bind(wx.EVT_BUTTON, self.__on_import, self.button_import)
        self.Bind(wx.EVT_BUTTON, self.__on_ok, self.button_ok)
        self.Bind(wx.EVT_BUTTON, self.__on_cancel, self.button_cancel)

    def __on_select_file(self, event):
        if self.style == NEW or self.style == SAVE:
            self.text_file.SetValue( self.list_files.GetStringSelection() )
        else:
            self.__on_change_filename(None)

    def __on_change_filename(self, event):
        # Enable the ok button only when there's some text for the file name
        if self.style == NEW or self.style == SAVE:
            self.button_ok.Enable(
                self.__has_value_in_file_name_field() 
                )
        else:
            self.button_ok.Enable(
                self.list_files.GetSelection() != wx.NOT_FOUND
                )

    def __has_value_in_file_name_field(self):
        name = self.__get_value_in_file_name_field()
        return not name.isspace()

    def __get_value_in_file_name_field(self):
        return self.text_file.GetValue()

    def __on_import(self, event):
        # TODO
        pass

    def __on_ok(self, event):
        self.EndModal(wx.ID_OK)

    def __on_cancel(self, event):
        self.EndModal(wx.ID_CANCEL)

    def get_guiname(self):
        if self.style == NEW or self.style == SAVE:
            return self.__get_value_in_file_name_field()
        else:
            return self.list_files.GetStringSelection()

    def get_filename(self):
        if self.autoextension:
            return self.get_guiname() + "." + self.extensions[0]
        else:
            return self.get_guiname()

    def get_path(self):
        return format.append_to_path( self.directory, self.get_filename() )

    def get_path_as_string(self):
        return str( self.get_path() )

    def get_unpath(self):
        return format.append_to_path( self.undirectory, self.get_filename() )

    def file_exists(self):
        return os.path.exists( self.get_path() )
