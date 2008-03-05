##\file reloaddlg.py
##\brief Dialog to reload individual python files related to OutGUI

import wx

import filedlg
import togwin

import conjurerconfig as cfg

# OutGUIReloaderDialog class
class OutGUIReloaderDialog(togwin.ChildToggableDialog):
    """Dialog to reload individual python files related to OutGUI"""

    def __init__(self, parent):
        togwin.ChildToggableDialog.__init__( self, "OutGUI reloader", parent )

        self.label = wx.StaticText(self, -1, "Python modules")
        self.list_modules = wx.ListBox(self, -1, style=wx.LB_MULTIPLE )
        self.button_reload = wx.Button(self, -1, "&Reload module")
        self.button_close = wx.Button(self, wx.ID_CANCEL, "&Close")

        self.__set_properties()
        self.__do_layout()
        self.__bind_events()

    def __set_properties(self):
        self.button_reload.Disable()

        # Python modules (except start up module)
        modules = filedlg.get_file_list( 'outgui:', ['pyc'] )
        try:
            modules.remove( 'outgui' )
        except:
            pass
        self.list_modules.AppendItems( modules )

    def __do_layout(self):
        sizer_border = wx.BoxSizer(wx.VERTICAL)
        sizer_layout = wx.BoxSizer(wx.VERTICAL)
        sizer_buttons = wx.BoxSizer(wx.HORIZONTAL)
        sizer_layout.Add(
            self.label, 
            0, 
            wx.ADJUST_MINSIZE
            )
        sizer_layout.Add(
            self.list_modules, 
            1, 
            wx.TOP|wx.BOTTOM|wx.EXPAND|wx.ADJUST_MINSIZE, 
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
            wx.EXPAND|wx.BOTTOM, 
            cfg.BORDER_WIDTH
            )
        sizer_buttons.Add(
            self.button_reload, 
            0,
            wx.ADJUST_MINSIZE
            )
        sizer_buttons.Add(
            self.button_close,
            0,
            wx.LEFT, 
            cfg.BORDER_WIDTH
            )
        sizer_layout.Add(
            sizer_buttons,
            0,
            wx.ALIGN_RIGHT
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
        self.Bind(
            wx.EVT_LISTBOX, 
            self.__on_select_module, 
            self.list_modules
            )
        self.Bind(
            wx.EVT_LISTBOX_DCLICK, 
            self.__on_reload_module, 
            self.list_modules
            )
        self.Bind(
            wx.EVT_BUTTON, 
            self.__on_reload_module, 
            self.button_reload
            )
        # use special ID so dialog closes when ESC pressed
        self.Bind(
            wx.EVT_BUTTON, 
            self.__on_close_button_clicked, 
            self.button_close
            )

    def __reload_module(self, module):
        exec( "import %s; reload(%s)" % (module, module) )

    def __on_select_module(self, event):
        self.button_reload.Enable()

    def __on_reload_module(self, event):
        modules_index = self.list_modules.GetSelections()
        for index in modules_index:
            module = self.list_modules.GetString(index)
            if module != "":
                self.__reload_module( module )

    def __on_close_button_clicked(self, event):
        self.Close()

    def persist(self):
        return [
            create_window,
            ()  #no parameters for create function
            ]


# create_window function
def create_window(parent):
    return OutGUIReloaderDialog(parent)
