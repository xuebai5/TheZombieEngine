import wx
import objdlg

import servers
import app

import conjurerconfig as cfg


class NOHTree(wx.TreeCtrl):
    """
    This wxTreeCtrl derivative displays a tree view of nebula's namespace.
    
    Set 'passive' to True if you don't want this control to access its parent
    or do anything else than allowing the user to select an object in the tree.
    """
    def __init__(
        self, 
        parent, 
        control_id, 
        root, 
        style=wx.TR_DEFAULT_STYLE, 
        modal=False, 
        passive=False 
        ):
        wx.TreeCtrl.__init__(self, parent, control_id, style=style)

        self.root = self.AddRoot(
                            root.getname(), 
                            -1, 
                            -1,
                            wx.TreeItemData(root)
                            )

        self.parent = parent
        self.modal = modal
        self.passive = passive

        # prepare images       
        self.image_list = None
        self.set_default_icons()
        
        # add image to root item
        self.SetItemImage(
            self.root, 
            self.fldridxnormal, 
            wx.TreeItemIcon_Normal
            )
        self.SetItemImage(
            self.root, 
            self.fldropenidxnormal, 
            wx.TreeItemIcon_Expanded
            )

        if root.gethead() != None:
            self.SetItemHasChildren(self.root, True)
            
        self.Bind(wx.EVT_TREE_ITEM_EXPANDING, self.on_item_expanding, self)
        self.Bind(wx.EVT_TREE_ITEM_COLLAPSED, self.on_item_collapsed, self)
        self.Bind(wx.EVT_TREE_SEL_CHANGED, self.on_sel_changed, self)
        self.Bind(wx.EVT_TREE_ITEM_ACTIVATED, self.on_activated, self)
        
        self.output = None
        self.selection_changed_callback = None

        self.Expand(self.GetRootItem())

    def set_default_icons (self):
        image_size = (16, 16)
        self.image_list = wx.ImageList(
                                image_size[0], 
                                image_size[1]
                                )
        self.fldridxnormal = self.image_list.Add(
                                    wx.ArtProvider_GetBitmap(
                                        wx.ART_FOLDER,
                                        wx.ART_OTHER, 
                                        image_size
                                        )
                                    )
        self.fldropenidxnormal = self.image_list.Add(
                                            wx.ArtProvider_GetBitmap(
                                                wx.ART_FILE_OPEN,
                                                wx.ART_OTHER, 
                                                image_size
                                                )
                                            )
        file_server = servers.get_file_server()
        bitmap_path = file_server.manglepath(
                                "outgui:images/tools/blue_folder_close.bmp"
                                )
        self.fldridxnative = self.image_list.Add( wx.Bitmap(bitmap_path) )
        bitmap_path = file_server.manglepath(
                                "outgui:images/tools/blue_folder_open.bmp"
                                )
        self.fldropenidxnative = self.image_list.Add( wx.Bitmap(bitmap_path) )
        bitmap_path = file_server.manglepath(
                                "outgui:images/tools/green_folder_close.bmp"
                                )
        self.fldridxuser = self.image_list.Add( wx.Bitmap(bitmap_path) )
        bitmap_path = file_server.manglepath(
                                "outgui:images/tools/green_folder_open.bmp"
                                )
        self.fldropenidxuser = self.image_list.Add( wx.Bitmap(bitmap_path) )
        self.SetImageList(self.image_list)

    def set_selection_changed_callback(self, callback):
        self.selection_changed_callback = callback

    def on_item_expanding (self, event):                       
        item = event.GetItem()
        if self.IsExpanded(item):
            # This event can happen twice in the self.Expand call
            return        

        obj = self.GetPyData(item)
        node = obj.gethead()
        while node != None:
            if node.hascommand("getsucc"):
                if not node.ishideineditor():
                    new_item = self.AppendItem(
                                        item, 
                                        node.getname(), 
                                        -1, 
                                        -1,
                                        wx.TreeItemData(node)
                                        )

                    self.SetItemImage(
                        new_item, 
                        self.fldridxnormal, 
                        wx.TreeItemIcon_Normal
                        )
                    self.SetItemImage(
                        new_item, 
                        self.fldropenidxnormal, 
                        wx.TreeItemIcon_Selected
                        )
                    self.SetItemImage(
                        new_item, 
                        self.fldropenidxnormal, 
                        wx.TreeItemIcon_Expanded)
        
                    if node.gethead() != None:
                        self.SetItemHasChildren(new_item, True)

                node = node.getsucc()
            else:
                # Some children aren't nroots...so skip checking
                break
        self.SortChildren(item)
    
    def on_item_collapsed(self, event):
        item = event.GetItem()
        self.DeleteChildren(item)        

    def on_sel_changed(self, event):
        if event.GetItem():
            obj = self.GetPyData(event.GetItem())
            if not self.passive:
                self.parent.parent.SetTitle(obj.getfullname())
                class_name = obj.getclass()
                self.parent.obj_class.SetLabel("Class: %s" % str(class_name))
                self.parent.current_path = obj.getfullname()
            if self.selection_changed_callback is not None:
                self.selection_changed_callback(obj)
        else:
            event.Skip()

    def on_activated(self, event):
        if not self.modal:
            obj = self.GetPyData(event.GetItem())
            if not self.passive:
                win = objdlg.create_window(
                            self.parent.parent.parent, 
                            obj.getfullname()
                            )
                win.display()

    def expand_tree (self, path):
        """
        Expand the NOH tree until last object in the provided path
        and select it

        \param Complete path to expand
        """
        node_name_list = path.rsplit('/') 
        node_name_list.pop(0)
        node = self.GetRootItem()
        self.CollapseAndReset(node)
        self.Expand(node)        

        # Autoexpand the tree to object

        end_main_loop = False
        for each in node_name_list:
            if end_main_loop:
                break
            if '' != each:
                node, cookie = self.GetFirstChild(node)
                should_exit = False
                while not should_exit:
                    # abandon if node is not valid
                    if not node.IsOk():
                        end_main_loop = True
                        break;
                    if self.GetItemText(node) == each:
                        self.Expand(node)
                        node = node
                        should_exit = True
                    else:                                
                        node, cookie = self.GetNextChild(node, cookie)

        # Select object and update title
        self.SelectItem(node)        

    def OnCompareItems(self, first_item_id, second_item_id):
        """Case INsenstive sort on item label"""
        first_item_label = self.GetItemText(first_item_id)
        second_item_label = self.GetItemText(second_item_id)
        return first_item_label.lower() >= second_item_label.lower()

class NOHSelectionDialog (wx.Dialog):
    def __init__(self, parent):
        wx.Dialog.__init__(
            self, 
            parent,
            -1,
            "Select a library location", 
            style=cfg.DEFAULT_DIALOG_STYLE
            )
        self.target_library = None
        self.tree_view = NOHTree(
                        self, 
                        -1, 
                        app.get_libraries(),
                        passive=True
                        )
        self.button_ok = wx.Button(self, -1, "&OK")
        #giving cancel button special ID so it will close when ESC is pressed
        self.button_cancel = wx.Button(self, wx.ID_CANCEL, "&Cancel")
        
        self.__set_properties()
        self.__do_layout()
        self.__bind_events()

    def __set_properties(self):
        self.SetDefaultItem(self.button_ok)

    def __do_layout(self):
        sizer_layout = wx.BoxSizer(wx.VERTICAL)
        sizer_buttons = wx.BoxSizer(wx.HORIZONTAL)
        sizer_layout.Add(
            self.tree_view,
            1, 
            wx.LEFT|wx.RIGHT|wx.TOP|wx.EXPAND,
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
            wx.EXPAND|wx.LEFT|wx.RIGHT|wx.TOP, 
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
            wx.ALL|wx.ALIGN_RIGHT, 
            cfg.BORDER_WIDTH
            )
        self.SetAutoLayout(True)
        # Set the sizer and make sure the dialog cannot be
        # made any smaller than its appropriate minimum size
        self.SetSizerAndFit(sizer_layout)
        # Set the initial size so that the title is visible
        self.SetSize( (250, 300) )
        self.Layout()

    def __bind_events(self):
        self.tree_view.set_selection_changed_callback(
            self.__on_select_target
            )
        self.Bind(
            wx.EVT_BUTTON, 
            self.__on_ok, 
            self.button_ok
            )
        self.Bind(
            wx.EVT_BUTTON, 
            self.__on_cancel, 
            self.button_cancel
            )

    def __on_select_target(self, selected_object):
        if selected_object.isa('nstringlist'):
            self.target_library = selected_object.getfullname()
        else:
            self.target_library = None
        self.__update_button_ok()

    def __update_button_ok(self):
        self.button_ok.Enable(
            self.target_library is not None
            )

    def __on_ok(self, event):
        self.EndModal(wx.ID_OK)

    def __on_cancel(self, event):
        self.EndModal(wx.ID_CANCEL)


def create_selection_dialog(frame):
    return NOHSelectionDialog(frame)
