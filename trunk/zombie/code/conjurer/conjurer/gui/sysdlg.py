##\file sysdlg.py
##\brief System information dialog

import wx

import pynebula

import format
import togwin

import conjurerconfig as cfg


# SystemInfoDialog
class SystemInfoDialog(togwin.ChildToggableDialog):
    """Dialog that shows some system information"""
    
    def __init__(self, parent):
        togwin.ChildToggableDialog.__init__(
            self, "System information", parent
            )
        
        # controls
        self.resources_sizer_staticbox = wx.StaticBox(self, -1, "Resources")
        self.dirs_sizer_staticbox = wx.StaticBox(self, -1, "Main directories")
        self.list_ctrl_dirs = wx.ListCtrl(
                                    self,
                                    -1, 
                                    style=wx.LC_REPORT|
                                                wx.LC_HRULES|
                                                wx.LC_VRULES|
                                                wx.SUNKEN_BORDER
                                    )
        self.list_ctrl_resources = wx.ListCtrl(
                                            self,
                                            -1, 
                                            style=wx.LC_REPORT|
                                                        wx.STATIC_BORDER
                                            )
        
        self.__set_properties()
        self.__do_layout()

    def __set_properties(self):
        # main directories
        self.list_ctrl_dirs.InsertColumn(0, 'Directory')
        self.list_ctrl_dirs.InsertColumn(1, 'Assign')
        self.list_ctrl_dirs.InsertColumn(2, 'Path')
        fileserver = pynebula.lookup('/sys/servers/file2')
        assigns = [
            # [directory, assign, path]
            ['Home directory', 'home:', ''],
            ['User directory', 'user:', ''],
            ['Project directory', 'proj:', ''],
            ['Texture directory', 'textures:', ''],
            ['Mesh directory', 'meshes:', ''],
            ['Shader directory', 'shaders:', ''],
            ['Working copy directory', 'wc:', ''],
            ['GUI directory', 'outgui:', '']
            ]
        # fill rows
        for assign in assigns:
            # get path from assign
            assign[2] = fileserver.manglepath(assign[1])
            # fill row
            i = self.list_ctrl_dirs.InsertStringItem(1000, assign[0])
            self.list_ctrl_dirs.SetStringItem(i, 1, assign[1])
            self.list_ctrl_dirs.SetStringItem(i, 2, assign[2])
        # adjust column sizes to contents
        for i in range(3):
            self.list_ctrl_dirs.SetColumnWidth(i, wx.LIST_AUTOSIZE)
        
        # resources
        # resource type codes are taken from 
        # nResource::Type (nresource.h, line 51)
        self.list_ctrl_resources.InsertColumn(0, 'Resource')
        self.list_ctrl_resources.InsertColumn(1, 'Number', wx.LIST_FORMAT_RIGHT)
        self.list_ctrl_resources.InsertColumn(2, 'Memory', wx.LIST_FORMAT_RIGHT)
        resserver = pynebula.lookup('/sys/servers/resource')
        resources = [
            # [type id, name, number, memory]
            [1<<0, 'Bundles', '', ''],
            [1<<1, 'Meshes', '', ''],
            [1<<2, 'Textures', '', ''],
            [1<<3, 'Shaders', '', ''],
            [1<<4, 'Animations', '', ''],
            [1<<5, 'Sound resources', '', ''],
            [1<<6, 'Sound instances', '', ''],
            [1<<7, 'Fonts', '', ''],
            [1<<8, 'Others', '', ''],
            [0, 'Total', '', '']
            ]
        # fill rows
        total_nb = 0
        total_bytes = 0
        for res in resources:
            if res[1] != 'Total':
                # get number and memory from resource type and accumulate them
                num_resources = resserver.getnumresources(res[0])
                total_nb = total_nb + num_resources
                res[2] = str(num_resources)
                bytes = resserver.getresourcebytesize(res[0])
                total_bytes = total_bytes + bytes
                res[3] = format.bytes2best_str(bytes)
            else:
                # get number and memory from accumulated totals
                res[2] = str(total_nb)
                res[3] = format.bytes2best_str(total_bytes)
            # fill row
            i = self.list_ctrl_resources.InsertStringItem(1000, res[1])
            self.list_ctrl_resources.SetStringItem(i, 1, res[2])
            self.list_ctrl_resources.SetStringItem(i, 2, res[3])
        # adjust columns to largest contents or header
        for index in range(3):
            self.list_ctrl_resources.SetColumnWidth(
                index,
                wx.LIST_AUTOSIZE
                )
            size1 = self.list_ctrl_resources.GetColumnWidth(index)
            self.list_ctrl_resources.SetColumnWidth(
                    index,
                    wx.LIST_AUTOSIZE_USEHEADER
                    )
            size2 = self.list_ctrl_resources.GetColumnWidth(index)
            self.list_ctrl_resources.SetColumnWidth(
                index, 
                max(size1, size2)
                )

    def __do_layout(self):
        border_sizer = wx.BoxSizer(wx.VERTICAL)
        resources_sizer = wx.StaticBoxSizer(
                                    self.resources_sizer_staticbox, 
                                    wx.VERTICAL
                                    )
        dirs_sizer = wx.StaticBoxSizer(
                            self.dirs_sizer_staticbox, 
                            wx.VERTICAL
                            )
        dirs_sizer.Add(
            self.list_ctrl_dirs, 
            1, 
            wx.ALL|wx.EXPAND, 
            cfg.BORDER_WIDTH
            )
        border_sizer.Add(
            dirs_sizer, 
            5,
            wx.ALL|wx.EXPAND, 
            cfg.BORDER_WIDTH
            )
        resources_sizer.Add(
            self.list_ctrl_resources, 
            1,
            wx.ALL|wx.EXPAND, 
            cfg.BORDER_WIDTH
            )
        border_sizer.Add(
            resources_sizer,
            4, 
            wx.LEFT|wx.RIGHT|wx.BOTTOM|wx.EXPAND, 
            cfg.BORDER_WIDTH
            )
        self.SetSizerAndFit(border_sizer)
        self.SetSize( (450, 350) )

    def persist(self):
        return [
            create_window,
            ()  # no parameters for create function
            ]


# create_window function
def create_window(parent):
    return SystemInfoDialog(parent)
