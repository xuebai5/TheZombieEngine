import wx

# Regular expression operations
import re

# Conjurer
import pynebula
import app
import nohtree
import servers
import os
import conjurerframework as cjr

class StringDropTarget (wx.PyDropTarget):
    # Drop target for script data
    def __init__ (self, window):
        wx.PyDropTarget.__init__(self)
        self.view = window

        # specify the type of data we will accept
        self.data = wx.TextDataObject()
        self.SetDataObject(self.data)
        self.mode = ""

    def __is_moving(self):
        return self.mode == "move"

    def __set_mode_to_move(self):
        self.mode = "move"

    def __set_mode_to_copy(self):
        self.mode = "copy"

    def OnDragOver (self, x, y , d):
        # We can copy or move objects
        if d == wx.DragCopy:
            self.__set_mode_to_copy()
        elif d == wx.DragMove:
            self.__set_mode_to_move()
        return d

    def OnData (self, x, y, d):
        # append the string copied with the drag & drop operation
        if self.GetData():
            text = self.data.GetText()
            element = self.view.HitTest( wx.Point(x, y) )
            if element[1] != wx.LIST_HITTEST_NOWHERE:     
                # get the root destination library
                root_library = self.view.get_root_library(element[0])

                lib_name = self.view.GetItemText(element[0])
                string_list = self.view.GetPyData(element[0])

                for string in text.rsplit():
                    if string_list.findstring(str(string)) == wx.NOT_FOUND:
                        string_list.appendstring(str(string))

                        # if this is a move operation the item must be deleted
                        if self.__is_moving():
                            source_obj = self.view.GetPyData(self.view.current)
                            source_obj.removestring( str(string) )
                        else:
                            d = wx.DragCopy

                        self.view.dirt_file(root_library)

                    # if the element is already in the target library
                    # the user is alerted
                    else:
                        if self.__is_moving():
                            action_name = "moved"
                        else:
                            action_name = "copied"
                        msg = "The element %s is already in the "\
                                    "%s library.\n\nIt will not be %s."\
                                    % (string, lib_name, action_name)
                        self.view.show_information_message(msg)
                        d = wx.DragError

            # user must drop in a tree element
            else:
                self.view.show_warning_message(
                    "Drop into a library, please", 
                            )
                d = wx.DragError
        return d


class StringTree (nohtree.NOHTree):

    def __init__(self, parent, id, root, name, show_strings=False):
        nohtree.NOHTree.__init__(
            self, 
            parent, 
            id, 
            root, 
            style=wx.TR_HAS_BUTTONS|wx.TR_EDIT_LABELS
            )
        self.parent = parent
        self.name = name
        self.show_strings = show_strings
        self.drag_type = "move"
        self.current = None

        fileserver = servers.get_file_server()

        root_img = self.image_list.Add(
                            wx.Bitmap(
                                fileserver.manglepath(
                                    "outgui:images/tools/browser.bmp"
                                    ),
                                wx.BITMAP_TYPE_ANY
                                )
                            )

        self.SetImageList(self.image_list)

        # add image to root item
        self.SetItemImage(self.root, root_img, wx.TreeItemIcon_Normal)

        self.Expand(self.GetRootItem())
        self.SelectItem(self.GetRootItem())

        general_lib_path = fileserver.manglepath(
                                    "wc:libs/grimoire/general.n2"
                                    )
        # dictionary for storage library file paths
        self.files = { 'general': [general_lib_path, False] } 

        # tree events
        self.Bind(wx.EVT_TREE_ITEM_EXPANDING, self.on_item_expanding, self)
        self.Bind(wx.EVT_TREE_SEL_CHANGED, self.on_sel_changed, self)
        self.Bind(wx.EVT_TREE_BEGIN_LABEL_EDIT, self.on_begin_edit, self)
        self.Bind(wx.EVT_TREE_END_LABEL_EDIT, self.on_end_edit, self)
        self.Bind(wx.EVT_TREE_BEGIN_DRAG, self.on_begin_drag, self)
        self.Bind(wx.EVT_TREE_END_DRAG, self.on_end_drag, self)

        # global events
        self.Bind(wx.EVT_RIGHT_DOWN, self.on_ms_right_down, self)
        self.Bind(wx.EVT_KEY_DOWN, self.on_key_down, self)
        self.Bind(wx.EVT_KEY_UP, self.on_key_up, self)
        self.Bind(wx.EVT_LEFT_DCLICK, self.on_left_dclick, self)
        
        self.selection_changed_callback = None
        
        # this control is a target for drop strings

        drop_target = StringDropTarget(self)
        self.SetDropTarget(drop_target)

    def on_activated (self, event):
        return

    def on_left_dclick(self, event):
        point = event.GetPosition()
        item, flags = self.HitTest(point)

        # Avoid root item collapse
        if item == self.GetRootItem():
            return
        else:
            event.Skip()

    def on_key_down (self, event):
        if event.GetKeyCode() == wx.WXK_CONTROL:
            self.drag_type = "copy"
            self.SetCursor(wx.StockCursor(wx.CURSOR_CROSS))
        elif event.KeyCode() == wx.WXK_DELETE:
            self.__mouse_position = event.GetPosition()
            if self.is_library(self.current):
                self.delete(self.current, 'lib')
            else:
                self.delete(self.current, 'dir')

        event.Skip()

    def on_key_up (self, event):
        if event.GetKeyCode() == wx.WXK_CONTROL:
            self.drag_type = "move"
            self.SetCursor(wx.STANDARD_CURSOR)

        event.Skip()

    def is_root(self, item):
        return item == self.root

    def __can_be_dragged(self, item):
        if self.is_root(item) or self.is_library(item) or self.is_native(item):
            return False
        else:
            return True

    def on_begin_drag (self, event):
        item = event.GetItem()
        if self.__can_be_dragged(item):
            event.Allow()        
            self.drag_object = item
        else:    
            wx.Bell()
            event.Veto()

    def on_end_drag (self, event):
        self.SetCursor(wx.STANDARD_CURSOR)  # restore cursor
        test_object = self.HitTest( event.GetPoint() )

        if test_object[1] != wx.TREE_HITTEST_TORIGHT:
            # get nebula objects
            dest_object = self.GetPyData(test_object[0])
            orig_object = self.GetPyData(self.drag_object)
            # check if we are trying to move a parent to a child
            dest = test_object[0]
            is_parent = False
            while dest != self.GetRootItem() and not is_parent:
                if dest == self.drag_object:
                    is_parent = True
                dest = self.GetItemParent(dest)
            # destination must be valid
            if self.is_valid(test_object[0]) and dest_object != orig_object \
                and not is_parent and dest_object != orig_object.getparent():
                
                new_obj = '/'.join(
                                ( dest_object.getfullname(), orig_object.getname() )
                                )
                orig_object.clone( str(new_obj) )

                # delete item if is a move operation
                if self.drag_type == 'move':
                    if not self.is_library(self.drag_object):
                        self.dirt_file(self.drag_object)
                    else:
                        self.files.pop(self.GetItemText(self.drag_object))

                    pynebula.delete(orig_object.getfullname())

                self.dirt_file(test_object[0])

                # expand tree to the target library
                self.expand_tree( new_obj.replace('/editor/','') )

            else:
                wx.Bell()
                cjr.show_warning_message(
                    "Operation not allowed"
                    )
        else:
            wx.Bell()
    
    def is_native (self, item):
        # Check if a item is a "natives" children
        object = self.GetPyData(item)
        if object.getname() == "natives":
            is_native = True
        else:
            node = object.getparent()
            is_native = False
    
            while node != None:
                if node.getname() == "natives":
                    is_native = True
                node = node.getparent()
        return is_native

    def on_item_expanding (self, event):
        self.expand_sublists( event.GetItem() )

    def expand_sublists (self, item):
        if self.IsExpanded(item):
            # This event can happen twice in the self.Expand call
            return
        obj = self.GetPyData(item)
        node = obj.gethead()
        fileserver = servers.get_file_server()
        self.libidx = self.image_list.Add(
                            wx.Bitmap(
                                fileserver.manglepath(
                                    "outgui:images/tools/new.bmp"
                                    ),
                            wx.BITMAP_TYPE_ANY
                            )
                        )
        while node != None:
            obj = pynebula.lookup(node.getfullname())
            new_item = self.AppendItem(
                                item, 
                                obj.getname(), 
                                -1, 
                                -1,
                                wx.TreeItemData(obj)
                                )
            if self.is_library(new_item):
                self.SetItemImage(
                    new_item, self.libidx, wx.TreeItemIcon_Normal
                    )
                self.SetItemImage(
                    new_item, self.libidx, wx.TreeItemIcon_Selected
                    )
                self.SetItemImage(
                    new_item, self.libidx, wx.TreeItemIcon_Expanded
                    )
            elif self.GetItemText(self.get_root_library(new_item)) == 'natives' and servers.get_kernel_server().isclassnative(str(self.GetItemText(new_item))):
                self.SetItemImage(
                    new_item, self.fldridxnative, wx.TreeItemIcon_Normal
                    )
                self.SetItemImage(
                    new_item, self.fldropenidxnative, wx.TreeItemIcon_Selected
                    )
                self.SetItemImage(
                    new_item, self.fldropenidxnative, wx.TreeItemIcon_Expanded
                    )
            elif self.GetItemText(self.get_root_library(new_item)) == 'natives' and not servers.get_kernel_server().isclassnative(str(self.GetItemText(new_item))):
                self.SetItemImage(
                    new_item, self.fldridxuser, wx.TreeItemIcon_Normal
                    )
                self.SetItemImage(
                    new_item, self.fldropenidxuser, wx.TreeItemIcon_Selected
                    )
                self.SetItemImage(
                    new_item, self.fldropenidxuser, wx.TreeItemIcon_Expanded
                    )
            else:
                self.SetItemImage(
                    new_item, self.fldridxnormal, wx.TreeItemIcon_Normal
                    )
                self.SetItemImage(
                    new_item, self.fldropenidxnormal, wx.TreeItemIcon_Selected
                )
                self.SetItemImage(
                    new_item, self.fldropenidxnormal, wx.TreeItemIcon_Expanded
                    )
            if obj.gethead() != None:
                self.SetItemHasChildren(new_item, True)
            node = obj.getsucc()
            self.SortChildren(item)

    def expand_strings (self, item):
        image_size = (16, 16)
        fileidx = self.image_list.Add(
                        wx.ArtProvider_GetBitmap(
                            wx.ART_REPORT_VIEW,
                            wx.ART_OTHER, image_size
                            )
                        )

        obj = self.GetPyData(item)
        obj = pynebula.sel(obj.getfullname)
        string_list = obj.getlist()
        for name in string_list:
            new_item = self.AppendItem(
                                item, 
                                name, 
                                -1, 
                                -1,
                                wx.TreeItemData(name)
                                )
            self.SetItemImage(new_item, fileidx, wx.TreeItemIcon_Normal)
            self.SetItemImage(new_item, fileidx, wx.TreeItemIcon_Expanded)

    def on_begin_edit (self, event):
        # show how to prevent edit...
        #root_lib_name = self.GetItemText(
        #                          self.get_root_library(event.GetItem())
        #                          )
        #if not self.files.has_key(root_lib_name):
        wx.Bell()
        event.Veto()

    def on_end_edit (self, event):
        new_name = event.GetLabel()
        if not event.IsEditCancelled():
            if re.compile('^[a-zA-Z0-9_\.]+$').match(new_name):
                obj = self.GetPyData(self.current)
                obj = pynebula.sel(obj.getfullname())
                obj.setname( str(new_name) )
                old_name = self.GetItemText(event.GetItem())
                file_tmp = self.files[old_name]
                self.files.pop(old_name)
                self.files[new_name] = file_tmp
            else:
                msg = "Bad name for library: \n\n" \
                            "Only alphanumerics," \
                            " '.' and '_' can be used"
                cjr.show_information_message(msg)
                event.Veto()

    def on_sel_changed (self, event):
        item = event.GetItem()
        self.current = item

        event.Skip()

    def on_ms_right_down (self, event):
        self.__mouse_position = event.GetPosition()
        item = self.HitTest(event.GetPosition())
        if item[1] != wx.LIST_HITTEST_NOWHERE:
            item_data = self.GetPyData(item[0])
    
            # get the type of the menu
            menu_type = ''

            if item[0] == self.GetRootItem():
                menu_type = "main"
            elif item_data.getparent() == self.GetPyData(self.GetRootItem()) and not self.is_native(item[0]):
                menu_type = "library"
            elif self.is_native(item[0]):
                if self.GetItemText(item[0]) != "natives":
                    menu_type = "natives"
            else:
                menu_type = "directory"
    
            if not hasattr(self, "newlib_id"):
                self.newlib_id = wx.NewId()
                self.newdir_id = wx.NewId()
                self.load_id = wx.NewId()
                self.unload_id = wx.NewId()
                self.saveas_id = wx.NewId()
                self.save_id = wx.NewId()
                self.delete_id = wx.NewId()
                self.delete_from_disk_id = wx.NewId()
                self.find_id = wx.NewId()
                self.clone_id = wx.NewId()
    
                self.Bind(wx.EVT_MENU, self.on_new_lib, id=self.newlib_id)
                self.Bind(wx.EVT_MENU, self.on_new_dir, id=self.newdir_id)
                self.Bind(wx.EVT_MENU, self.on_load, id=self.load_id)
                self.Bind(wx.EVT_MENU, self.on_unload, id=self.unload_id)
                self.Bind(wx.EVT_MENU, self.on_delete, id=self.delete_id)
                self.Bind(wx.EVT_MENU, self.on_saveas, id=self.saveas_id)
                self.Bind(wx.EVT_MENU, self.on_save, id=self.save_id)
                self.Bind(
                    wx.EVT_MENU, 
                    self.on_delete_from_disk, 
                    id=self.delete_from_disk_id
                    )
                self.Bind(wx.EVT_MENU, self.on_clone, id=self.clone_id)
    
            # make a menu
            menu = wx.Menu()

            if menu_type == "main":
                item = wx.MenuItem(menu, self.newlib_id,"New Library")
                menu.AppendItem(item)
                menu.Append(self.load_id, "Load")
            elif menu_type == "library":
                menu.Append(self.newdir_id, "New Directory")
                menu.Append(self.unload_id, "Unload library")
                menu.Append(self.saveas_id, "Save As...")
                menu.Append(self.save_id, "Save")
                menu.Append(self.delete_id, "Delete from disk")
                menu.Append(self.clone_id, "Clone")
            elif menu_type == "directory":
                menu.Append(self.newdir_id, "New Directory")
                menu.Append(self.delete_id, "Delete")
                menu.Append(self.clone_id, "Clone")

            # Popup the menu.  If an item is selected then its handler
            # will be called before PopupMenu returns.
            event.Skip()
    
            self.PopupMenu(menu, event.GetPosition())
            menu.Destroy()

        event.Skip()    

    def load_files (self, files):
        self.files = files
        general_lib_path = servers.get_file_server().manglepath(
                                    "wc:libs/grimoire/general.n2"
                                    )
        self.files['general'] = [general_lib_path, False]

        trash = []
        for key, file in files.iteritems():
            file_name = file[0].replace('\\\\', '/')
            self.load(file_name)

        # Delete libraries that failed on load (maybe It was deleted from disk)
        #for element in trash:
        #    del element

        self.expand_tree(self.GetItemText(self.GetRootItem()))

    def save_files (self):
        for file in self.files.itervalues():
            self.save(file)

    def must_save (self):
        for file in self.files.itervalues():
            if file[1]:
                return True

    def dirt_file (self, library_name):
        """ Dirt the library, can be do it with a 
            library name or with a tree item """
        if not isinstance(library_name, str):
            library = self.get_root_library(library_name)
            library_name = self.GetItemText(library)
    
        if self.files.has_key(library_name):
            self.files[library_name][1] = True
            return True
        else:
            return False

    def is_file_dirty (self, file):
        return file[1]

    def delete (self, library, type='dir'):
        element_name = self.GetItemText(library)

        if self.is_valid(library):
            if 'lib' == type:
                msg1 = "This will unload the %s library from memory.\n\n"\
                                "Are you sure?" % (element_name)
            elif 'dir' == type:
                msg1 = "This will delete the %s directory.\n\n"\
                                "Are you sure?" % (element_name)
        
            position = self.__get_cursor_position()
            dlg_result = cjr.confirm_yes_no_at_position(
                                self,
                                msg1,
                                position
                                )
            if dlg_result == wx.ID_YES:
                # get full nebula path
                library_data = self.GetPyData(library)
                full_path = library_data.getfullname()

                # the same with the parent for expand the tree after delete
                parent = library_data.getparent()
                parent_name = parent.getfullname()

                pynebula.delete(full_path)  # delete nebula object
                self.Delete(self.current)   # delete object from tree
    
                # update state
                if 'lib' == type:
                    self.files.pop(element_name)
                elif 'dir' == type:
                    self.dirt_file(self.get_root_library(self.current))
        
                # expand the parent
                self.expand_tree(parent_name.replace('/editor/', ''))

        else:
            cjr.show_warning_message("Element is read-only")

    def on_unload (self, event):
        self.delete(self.current, "lib")

    def on_delete (self, event):
        self.delete(self.current)

    def load (self, file_path):
        if os.path.exists(file_path):
            root = app.get_libraries()
            kernel = servers.get_kernel_server()
    
            # get file name without extension
            foo, file_name = os.path.split(file_path)
            file_name, ext = os.path.splitext(file_name)
    
            # load new library into memory
            new_library_path = root.getfullname() + "/" + file_name
            if not pynebula.exists(new_library_path):
                kernel.loadas(str(file_path), str(new_library_path))
                        
            self.files[file_name] = [file_path, False]
            return new_library_path

        return ''

    def save (self, file):
        root = app.get_libraries()

        # get file name without extension
        foo, file_name = os.path.split(file[0])
        file_name, ext = os.path.splitext(file_name)

        # save file if needed
        if self.is_file_dirty(file) and file[0] != '':
            obj_name = root.getfullname() + "/" + file_name
            obj = pynebula.lookup(str(obj_name))
            obj.saveas(str(file[0]))
            self.files[file_name][1] = False
    
    def on_load (self, event):
        # open a file dialog and load the selected file
        wildcard = "Library files (*.n2)|*.n2"
        
        dlg = wx.FileDialog(
            self.GetGrandParent(), 
            message="Choose a library", 
            wildcard=wildcard,
            style=wx.OPEN|wx.CHANGE_DIR
            )
        
        if dlg.ShowModal() == wx.ID_OK:
            new_library_path = self.load(dlg.GetPath()) 
            if new_library_path != '':
                self.expand_tree(new_library_path.replace('/editor/',''))
        
        dlg.Destroy()

    def on_saveas (self, event):
        # open a file dialog and save the selected library
        library = self.GetPyData(self.get_root_library(self.current))
        
        wildcard = "Library files (*.n2)|*.n2"
        
        dlg = wx.FileDialog(
            self.GetGrandParent(), 
            message="Save library", 
            wildcard=wildcard,
            style=wx.SAVE|wx.CHANGE_DIR|wx.OVERWRITE_PROMPT
            )

        if dlg.ShowModal() == wx.ID_OK:
            library.saveas(str(dlg.GetPath()))
        dlg.Destroy()

    def get_root_library (self, item):
        """ Return the root library of the selected item """
        if item != self.GetRootItem():
            while self.GetItemParent(item) != self.GetRootItem():
                item = self.GetItemParent(item)
        return item

    def get_current_root_library_name (self):
        """ Return the name of the current item """
        return self.GetItemText(self.get_root_library(self.current))

    def get_current_root_library_nobject (self):
        """ Return the name of the current item """
        return self.GetPyData(self.get_root_library(self.current))

    def get_current_library_nobject (self):
        """ Return the nebula object of the current item """
        return self.GetPyData(self.current)

    def on_save (self, event):
        library = self.GetPyData(self.get_root_library(self.current))
        lib_name = library.getname()
        if self.files.has_key(lib_name):
            self.save(self.files[lib_name])
        else:
            cjr.show_warning_message(
                "Element is read-only"
                )

    def __get_cursor_position(self):
        return (
            self.GetGrandParent().parent.GetPosition()
                + self.GetParent().GetPosition() 
                + self.__mouse_position
            )

    def on_new_lib (self, event):
        position = self.__get_cursor_position()
        self.create_new_lib(position)

    def on_new_dir (self, event):
        position = self.__get_cursor_position()
        self.create_new_dir(self.current, position)

    def create_new_dir (self, parent, position):
        if parent != self.GetRootItem():
            # get library name
            library = self.get_root_library(parent)
            library_name = self.GetItemText(library)
            if self.files.has_key(library_name):    
                # create new sublibrary
                parent = self.GetPyData(parent)
                dlg = wx.TextEntryDialog(
                    self, 
                    'Enter new name for sublibrary',
                    'New library', 
                    ''
                    )
                dlg.SetPosition(position)
    
                if dlg.ShowModal() == wx.ID_OK:
                    new_lib_name = dlg.GetValue()
                    # create object
                    new_library_path = parent.getfullname() + '/' + new_lib_name
                    pynebula.new( 'nstringlist', new_library_path )
                    self.files[library_name][1] = True
                    self.expand_tree( new_library_path.replace('/editor/','') )

                dlg.Destroy()

            else:
                cjr.show_warning_message(
                    "This library is read-only"
                    )

    def create_new_lib (self, position):
        wildcard = "Library files (*.n2)|*.n2"

        # Show file dialog
        dlg = wx.FileDialog(
            self.GetGrandParent(), 
            message="Create library", 
            wildcard=wildcard,
            style=wx.SAVE|wx.CHANGE_DIR|wx.OVERWRITE_PROMPT, 
            pos=position
            )

        # get the selected file
        if dlg.ShowModal() == wx.ID_OK:
            root = app.get_libraries()
            new_library_path = root.getfullname() + "/" + dlg.GetFilename()
            new_library_path, ext = os.path.splitext(new_library_path)

            # test extension
            if ext != ".n2":
                cjr.show_error_message(
                    "Bad extension: must be .n2. Try again."
                )
            else:
                # create object and save to disk
                save = True

                if pynebula.exists(str(new_library_path)):
                    dlg2_result = cjr.warn_yes_no(
                                "You are using a library with the same name."\
                                "\n\n Are you sure you want to delete it?"
                                )

                    if dlg2_result == wx.ID_NO:
                        save = False
                    else:
                        pynebula.delete( str(new_library_path) )

                if save:
                    library = pynebula.new("nstringlist", new_library_path)
                    path_string = str( dlg.GetPath() )
                    library.saveas(path_string)
                    foo, lib_name = os.path.split(new_library_path)
                    self.files[lib_name] = [path_string, False]
                    self.expand_tree(new_library_path.replace('/editor/',''))

        dlg.Destroy()

    def on_delete_from_disk (self, event):
        obj = self.GetPyData(self.current)
        lib_name = obj.getname()

        # delete only files avilable in the file list, files that are not
        # in this list are supossed read-only
        root_lib_name = self.GetItemText(self.get_root_library(self.current))
        if self.files.has_key(root_lib_name):
            position = self.__get_cursor_position()
            dlg_result = cjr.confirm_yes_no_at_position(
                                self,
                                "Are you sure?", 
                                position
                                )
            if dlg_result == wx.ID_YES:
                parent = obj.getparent() 
                dir_to_expand = parent.getfullname()
                os.remove(self.files[lib_name][0]) # delete file from disk!
                self.files.pop(lib_name)
                pynebula.delete(obj.getfullname())
                self.expand_tree(dir_to_expand.replace('/editor/', '') )
        else:
            cjr.show_warning_message("Element is read-only")

    def on_clone (self, event):
        obj = self.GetPyData(self.current)
        parent = pynebula.sel(obj.getfullname)
        new_lib_name = ''
        dlg = wx.TextEntryDialog(
                self, 
                'Enter name for clone',
                'New library', 
                obj.getname() + '_copy'
                )
        dlg.Destroy()

    def is_library (self, element):
        return self.GetItemParent(element) == self.GetRootItem()

    def is_valid (self, library):
        # return true if the element belongs to a valid library

        #root_library = self.get_root_library(library)
        #return self.files.has_key(self.GetItemText(root_library))
        return True
