##\file nodelibdlg.py
##\brief A dialog to browse all child nodes of an object that form a library

import wx
import pynebula

import conjurerconfig as cfg

# File dialog styles
NEW = 1
OPEN = 2
DELETE = 3
SELECT = 4

# get_node_list function
def get_node_list(path):
    """
    Lists all child nodes in the library path
    """
    gui_names = []
    if (pynebula.exists(str(path))):
        obj = pynebula.sel(path)
        children = obj.getchildren()
        for node in children:
            gui_names.append(node.getname())

    return gui_names

# NodeLibDialog class
class NodeLibDialog(wx.Dialog):
    """
    Browser for nodes in a given noh path:
    \param parent Parent window
    \param style  Style of the dialog (NEW, OPEN, DELETE)
    \param label1 Lowercase node type name
    \param label2 Capitalized node type name
    \param path   Hierarchy path to search for subnodes
    """

    def __init__(self, parent, style, label1, label2, path, default_name=""):
        if style == NEW:
            title = "Choose a %s name" % label1
        elif style == OPEN:
            title = "Choose a %s to open" % label1
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
        self.path = path
        if (pynebula.exists(str(path))):
            self.root = pynebula.lookup( str(path) )
        else:
            self.root = None

        self.list_nodes = wx.ListBox(
                                self, 
                                -1, 
                                choices = get_node_list(self.path)
                                )

        if self.style == NEW:
            self.label_file = wx.StaticText(
                                    self, 
                                    -1, 
                                    "%s name" % label2
                                    )
            self.text_file = wx.TextCtrl(self, -1, default_name)
        if self.style == NEW:
            ok_caption = "&New"
        elif self.style == OPEN:
            ok_caption = "&Open"
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
        self.__on_change_name(None)

    def __do_layout(self):
        sizer_border = wx.BoxSizer(wx.VERTICAL)
        sizer_layout = wx.BoxSizer(wx.VERTICAL)
        sizer_buttons = wx.BoxSizer(wx.HORIZONTAL)
        sizer_layout.Add(self.list_nodes, 1, wx.EXPAND)
        if self.style == NEW:
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
        self.Bind(wx.EVT_LISTBOX, self.__on_select_node, self.list_nodes)
        self.Bind(wx.EVT_LISTBOX_DCLICK, self.__on_ok, self.list_nodes)
        if self.style == NEW:
            self.Bind(wx.EVT_TEXT, self.__on_change_name, self.text_file)
        self.Bind(wx.EVT_BUTTON, self.__on_ok, self.button_ok)
        self.Bind(wx.EVT_BUTTON, self.__on_cancel, self.button_cancel)

    def __on_select_node(self, event):
        if self.style == NEW:
            self.text_file.SetValue( self.list_nodes.GetStringSelection() )
        else:
            self.__on_change_name(None)

    def __on_change_name(self, event):
        # enable the ok button only when there is some input text
        if self.style == NEW:
            self.button_ok.Enable( self.text_file.GetValue() != "" )
        else:
            self.button_ok.Enable(
                self.list_nodes.GetSelection() != wx.NOT_FOUND 
                )

        if self.style == SELECT:
            self.SetTitle( self.list_nodes.GetStringSelection() )

    def __on_ok(self, event):
        self.EndModal(wx.ID_OK)

    def __on_cancel(self, event):
        self.EndModal(wx.ID_CANCEL)

    def get_guiname(self):
        if self.style == NEW:
            return self.text_file.GetValue()
        else:
            return self.list_nodes.GetStringSelection()

    def node_exists(self):
        return self.get_node() is not None

    def get_node(self):
        if self.root is None:
            return None
        else:
            return self.root.find(
                str( self.get_guiname() ) 
                )
