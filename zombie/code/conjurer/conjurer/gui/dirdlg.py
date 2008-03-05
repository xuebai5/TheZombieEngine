##\file dirdlg.py
##\brief Directory browser constraining the user within a directory

import os
import wx

import format
import servers

import conjurerconfig as cfg


# Directory dialog styles
NEW = 1
OPEN = 2
SAVE = 3
DELETE = 4
SELECT = 5


# get_directory_list function
def get_directory_list(directory):
    """
    Return a sequence of the directories within the given directory.
    
    Only the relative path to the given directory is returned for each file,
    not its full path.
    """
    gui_names = []
    mangled_dir = servers.get_file_server().manglepath( str(directory) )
    filenames = os.listdir( mangled_dir )
    for filename in filenames:
        if filename.startswith("."):
            # skip special directories/files
            continue
        file_path = format.append_to_path( mangled_dir, filename )
        if os.path.isdir( file_path ):
            gui_names.append( filename )
    return gui_names


# DirDialog class
class DirDialog(wx.Dialog):
    """
    Directory browser constraining the user within a directory.
    
    \param parent Parent window
    \param style Style of the dialog (NEW, OPEN, SAVE, DELETE, SELECT)
    \param label1 Lowercase version of the file type's name
    \param label2 Capitalized version of the file type's name
    \param directory Directory where to constraint the search
    """
    
    def __init__(self, parent, style, label1, label2, directory,
                 default_dir=""):
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
        self.directory = servers.get_file_server().manglepath( str(directory) )
        
        self.list_files = wx.ListBox(
                                self, 
                                -1,
                                choices = get_directory_list(self.directory)
                                )
        if self.style == NEW or self.style == SAVE:
            self.label_file = wx.StaticText(
                                    self, 
                                    -1, 
                                    "%s name" % label2
                                    )
            self.text_file = wx.TextCtrl(
                                    self, 
                                    -1, 
                                    default_dir
                                    )
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
        sizer_layout.Add(
            self.list_files,
            1, 
            wx.EXPAND
            )
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
        self.SetSizerAndFit(sizer_border)
        self.Layout()

    def __bind_events(self):
        self.Bind(wx.EVT_LISTBOX, self.__on_select_file, self.list_files)
        self.Bind(wx.EVT_LISTBOX_DCLICK, self.__on_ok, self.list_files)
        if self.style == NEW or self.style == SAVE:
            self.Bind(wx.EVT_TEXT, self.__on_change_filename, self.text_file)
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
                self.text_file.GetValue() != "" 
                )
        else:
            self.button_ok.Enable(
                self.list_files.GetSelection() != wx.NOT_FOUND 
                )

    def __on_ok(self, event):
        self.EndModal(wx.ID_OK)

    def __on_cancel(self, event):
        self.EndModal(wx.ID_CANCEL)

    def get_dirname(self):
        if self.style == NEW or self.style == SAVE:
            return self.text_file.GetValue()
        else:
            return self.list_files.GetStringSelection()

    def get_path(self):
        return format.append_to_path(
            self.directory, self.get_dirname() 
            )

    def dir_exists(self):
        return os.path.exists( self.get_path() )
