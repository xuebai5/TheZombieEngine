##\musictabledialog.py
##\brief Music Table Dialogs

import wx

import musictable
import conjurerconfig as cfg


class MusicTableSelectionDialog(wx.Dialog):
    def __init__(self, parent, title, label, list_of_strings, selected_string=None):
        wx.Dialog.__init__(
            self,
            parent,
            -1,
            title,
            style=cfg.DEFAULT_DIALOG_STYLE
            )       
        self.original_title = title
        self.label = wx.StaticText(self, -1, label)
        self.list = wx.ListBox(
                        self,
                        -1, 
                        choices = list_of_strings
                        )
        try:
            self.list.SetStringSelection(selected_string)
        except:
            pass
        self.button_ok = wx.Button(self, wx.ID_OK, "&OK")        
        self.button_cancel = wx.Button(self, wx.ID_CANCEL, "&Cancel")
        self.__set_properties()
        self.__do__layout()
        self.__bind_events()

    def __set_properties(self):
        self.__update_button_ok()

    def __do__layout(self):
        border_width = cfg.BORDER_WIDTH
        sizer_layout = wx.BoxSizer(wx.VERTICAL)
        sizer_buttons = wx.BoxSizer(wx.HORIZONTAL)
        sizer_layout.Add(self.label, 0, wx.TOP|wx.LEFT|wx.RIGHT, border_width)
        sizer_layout.Add(self.list, 1, wx.EXPAND|wx.ALL, border_width)
        horizontal_line = wx.StaticLine( self, -1, ( -1, -1 ), ( -1, -1 ), wx.LI_HORIZONTAL )
        sizer_layout.Add(horizontal_line, 0, wx.EXPAND|wx.LEFT|wx.RIGHT, border_width)
        sizer_buttons.Add(self.button_ok, 0, wx.ADJUST_MINSIZE, 0)
        sizer_buttons.Add(self.button_cancel, 0, wx.LEFT|wx.ADJUST_MINSIZE, border_width)
        sizer_layout.Add(sizer_buttons, 0, wx.ALL|wx.ALIGN_RIGHT, border_width)
        self.SetAutoLayout(True)
        self.SetSizer(sizer_layout)
        sizer_layout.Fit(self)
        sizer_layout.SetSizeHints(self)
        self.Layout()

    def __bind_events(self):
        self.Bind(
            wx.EVT_INIT_DIALOG,
            self.__on_init_dialog
            )
        self.Bind(
            wx.EVT_LISTBOX,
            self.__on_selection,
            self.list
            )
        self.Bind(
            wx.EVT_LISTBOX_DCLICK,
            self.__on_dclick_on_list,
            self.list
            )
        self.Bind(
            wx.EVT_BUTTON,
            self.__on_ok,
            self.button_ok
            )

    def __on_init_dialog(self, event):
        self.__set_title_from_original()
        event.Skip()

    def __update_button_ok(self):
        enable = self.__has_selected_item()
        self.button_ok.Enable( enable )

    def __on_dclick_on_list(self, event):
        self.__on_ok(event)

    def __on_ok(self, event):
        self.SetTitle( self.__get_selected_item() )
        self.EndModal(wx.ID_OK)

    def __on_selection(self, event):
        self.__update_button_ok()

    def __get_selected_item(self):
        return self.list.GetStringSelection()

    def __has_selected_item(self):
        return self.list.GetSelection() != wx.NOT_FOUND

    def __set_title_from_original(self):
        self.SetTitle(self.original_title)

def dialog_for_music_mood_selection(parent=None):
    mood_names = get_music_table().get_mood_names_sorted()
    dlg = MusicTableSelectionDialog( 
                parent, 
                "Music moods",
                "Choose a mood", 
                mood_names
                )
    return dlg
    
def dialog_for_music_sample_selection(parent=None):
    sample_names = get_music_table().get_sample_names_sorted()
    dlg = MusicTableSelectionDialog( 
                parent, 
                "Music samples",
                "Choose a sample", 
                sample_names
                )
    return dlg
    
def dialog_for_music_style_selection(parent=None):
    style_names = get_music_table().get_style_names_sorted()
    dlg = MusicTableSelectionDialog( 
                parent, 
                "Music styles",
                "Choose a style", 
                style_names
                )
    return dlg
    
def get_music_table():
    return musictable.MusicTable()