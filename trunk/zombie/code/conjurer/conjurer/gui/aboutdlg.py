##\file aboutdlg.py
##\brief About dialog

import wx

import servers

import conjurerconfig as cfg


# get_version_number function
def get_version_number(version):
    version = str(version).zfill(6)
    string_1 = version[0:2]
    string_2 = version[2:4]
    string_3 = version[4:6]
    if int(string_1) < 10:
        string_1 = string_1[1]
    if int(string_2) < 10:
        string_2 = string_2[1]
    if int(string_3) < 10:
        string_3 = string_3[1]
    return "%s.%s.%s" % (string_1, string_2, string_3)


# AboutDialog class
class AboutDialog(wx.Dialog):
    """Dialog that shows name and release version info"""
    
    def __init__(self, *args, **kwds):
        kwds["style"] = wx.DEFAULT_DIALOG_STYLE
        wx.Dialog.__init__(self, *args, **kwds)
        self.bitmap_logo = wx.StaticBitmap(
                                    self, 
                                    -1,
                                    wx.Bitmap(
                                        servers.get_file_server().manglepath(
                                            "outgui:images/logo.jpg"
                                            ), 
                                        wx.BITMAP_TYPE_ANY
                                        ),
                                    style=wx.SUNKEN_BORDER
                                    )
        self.label_app = wx.StaticText(self, -1, "Application name")
        self.label_conjurer = wx.StaticText(self, -1, "Conjurer")
        self.label_version = wx.StaticText(self, -1, "Release version")
        self.label_number = wx.StaticText(self, -1, "")
        self.label_copyright = wx.StaticText(
                                        self, 
                                        -1, 
                                        "(c) 2007 Conjurer Services, S.A."
                                        )
        self.list_info = wx.ListCtrl(
                                self, 
                                -1, 
                                style=wx.LC_REPORT|wx.LC_NO_HEADER
                                )
        self.button_ok = wx.Button(self, wx.ID_CANCEL, "&OK")
        
        self.__set_properties()
        self.__do_layout()
        self.__bind_events()

    def __set_properties(self):
        self.SetTitle("About Conjurer")
        self.label_conjurer.SetFont(
            wx.Font(
                8, 
                wx.DEFAULT, 
                wx.NORMAL, 
                wx.BOLD, 
                0, 
                ""
                )
            )
        self.label_number.SetFont(
            wx.Font(
                8,
                wx.DEFAULT, 
                wx.NORMAL, 
                wx.BOLD, 
                0,
                ""
                )
            )
        self.list_info.InsertColumn(0, "Label")
        self.list_info.InsertColumn(1, "Value")

        # Relase build info        
        # get release info: int version, int major, int minor, int patch, 
        # char * versionstr, char * date, char * time, char * id 
        info = servers.get_conjurer().getreleasebuildinfo()
        #self.label_number.SetLabel( get_version_number(info[0]) )
        
        # Build info
        self.list_info.InsertStringItem( 0, "Release version" )
        self.list_info.SetStringItem( 0, 1, info[4] )
        self.list_info.InsertStringItem( 1, "Build date" )
        self.list_info.SetStringItem( 1, 1, info[5] )
        self.list_info.InsertStringItem( 2, "Build time" )
        self.list_info.SetStringItem( 2, 1, info[6] )
        self.list_info.InsertStringItem( 3, "Build id" )
        self.list_info.SetStringItem( 3, 1, info[7].strip('$BuildId: ') )
        
        # Subversion info
        # int revision, char * revstr, char* range, char* url, 
        # char* date, bool & mixed, bool & localmod
        info = servers.get_conjurer().getreleasesubversioninfo()
        self.list_info.InsertStringItem( 4, "SVN revision" )
        self.list_info.SetStringItem( 4, 1, str(info[0]) )
        self.label_number.SetLabel( info[1] )
        self.list_info.InsertStringItem( 5, "SVN revision range" )
        self.list_info.SetStringItem( 5, 1, info[2].strip('$Revision: ') )
        self.list_info.InsertStringItem( 6, "SVN head URL" )
        self.list_info.SetStringItem( 6, 1, info[3].strip('$HeadURL: ') )
        self.list_info.InsertStringItem( 7, "SVN date" )
        self.list_info.SetStringItem( 7, 1, info[4].strip('$Date: ') )
        self.list_info.InsertStringItem( 8, "SVN mixed WC" )
        self.list_info.SetStringItem( 8, 1, str(info[5]) )
        self.list_info.InsertStringItem( 9, "SVN locally modfified WC" )
        self.list_info.SetStringItem( 9, 1, str(info[6]) )
        
        self.list_info.SetColumnWidth(0, wx.LIST_AUTOSIZE)
        self.list_info.SetColumnWidth(1, wx.LIST_AUTOSIZE)

    def __do_layout(self):
        border_sizer = wx.BoxSizer(wx.VERTICAL)
        layout_sizer = wx.BoxSizer(wx.VERTICAL)
        grid_sizer_app = wx.FlexGridSizer(
                                    2, 
                                    2, 
                                    cfg.BORDER_WIDTH,
                                    cfg.BORDER_WIDTH
                                    )
        layout_sizer.Add(
            self.bitmap_logo, 
            0, 
            wx.FIXED_MINSIZE|wx.CENTRE
            )
        layout_sizer.Add(
            (cfg.BORDER_WIDTH, cfg.BORDER_WIDTH), 
            0, 
            wx.FIXED_MINSIZE
            )
        grid_sizer_app.Add(
            self.label_app, 
            0,
            wx.ALIGN_RIGHT|wx.FIXED_MINSIZE
            )
        grid_sizer_app.Add(
            self.label_conjurer,
            0, 
            wx.FIXED_MINSIZE
            )
        grid_sizer_app.Add(
            self.label_version,
            0,
            wx.ALIGN_RIGHT|wx.FIXED_MINSIZE
            )
        grid_sizer_app.Add(
            self.label_number, 
            0, 
            wx.FIXED_MINSIZE
            )
        layout_sizer.Add(
            grid_sizer_app,
            0,
            wx.ALIGN_CENTER_HORIZONTAL
            )
        layout_sizer.Add(
            (cfg.BORDER_WIDTH, cfg.BORDER_WIDTH), 
            0, 
            wx.FIXED_MINSIZE
            )
        layout_sizer.Add(
            self.label_copyright,
            0, 
            wx.ALIGN_CENTER_HORIZONTAL|wx.FIXED_MINSIZE
            )
        layout_sizer.Add(
            (cfg.BORDER_WIDTH, cfg.BORDER_WIDTH),
            0, 
            wx.FIXED_MINSIZE
            )
        layout_sizer.Add(
            self.list_info, 
            1,
            wx.EXPAND
            )
        layout_sizer.Add(
            (cfg.BORDER_WIDTH, cfg.BORDER_WIDTH), 
            0, 
            wx.FIXED_MINSIZE
            )
        layout_sizer.Add(
            self.button_ok, 
            0, 
            wx.ALIGN_CENTER_HORIZONTAL|wx.FIXED_MINSIZE
            )
        border_sizer.Add(
            layout_sizer,
            0, 
            wx.ALL|wx.ALIGN_CENTER_HORIZONTAL, 
            cfg.BORDER_WIDTH
            )
        self.SetSizerAndFit(border_sizer)

    def __bind_events(self):
        self.Bind(wx.EVT_BUTTON, self.on_ok, self.button_ok)

    def on_ok(self, event):
        self.EndModal(wx.ID_OK)


# create_window function
def create_window(parent):
    return AboutDialog(parent)
