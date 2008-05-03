import wx

# Regular expression operations
import re
import os

# Conjurer
import pynebula
import childdialoggui
import freedialoggui
import nebulahelpevent as nh
import app
import stringtree
import servers
import stringlister
import nebulaguisettings as guisettings
import objdlg
import objbrowserwindow
import levelcmds
import waitdlg
import inspectorstatetab
import conjurerconfig as cfg
import conjurerframework as cjr

import wx.propgrid as pg
import propgrid

class GrimoirePanel(wx.Panel):
    ID_WINDOW_BROWSER = 5101
    ID_WINDOW_INSPECTOR = 5102

    last_selected_path = None

    def __init__(self, parent, modal):
        wx.Panel.__init__(self, parent, -1)

        # Settings
        self.parent = parent 
        self.recursive = False
        self.preview = False
        self.entities = {}        
        self.entity_server = servers.get_entity_class_server()
        self.current_library = None  
        self.state = app.get_object_state()
        self.string_list = pynebula.lookup('/editor/object/classList')
        self.view = 'normal'
        self.modal = modal

        # variables for finder control
        self.match_count = 0
        self.match_list = []

        self.__mouse_pos = None

        # entities list window
        # will occupy the space not used by the Layout Algorithm
        self.lister = stringlister.StringLister(self, 0)

        # browser window
        win = wx.SashLayoutWindow(
                    self, 
                    self.ID_WINDOW_BROWSER, 
                    wx.DefaultPosition, 
                    (200, 0), 
                    wx.NO_BORDER|wx.SW_3D
                    )

        win.SetDefaultSize( (150, 1000) )
        win.SetOrientation(wx.LAYOUT_VERTICAL)
        win.SetAlignment(wx.LAYOUT_LEFT)
        win.SetSashVisible(wx.SASH_RIGHT, True)
        self.browser = stringtree.StringTree(
                                win, 
                                0, 
                                app.get_libraries(), 
                                "Libraries", 
                                False
                                )
        self.browser_window = win
        self.current_library = self.browser.GetRootItem()

        # inspector window
        win = wx.SashLayoutWindow(
                    self, 
                    self.ID_WINDOW_INSPECTOR, 
                    wx.DefaultPosition, 
                    (200, 0),
                    wx.NO_BORDER|wx.SW_3D
                    )

        win.SetDefaultSize( (1000, 300) )
        win.SetOrientation(wx.LAYOUT_HORIZONTAL)
        win.SetAlignment(wx.LAYOUT_BOTTOM)
        win.SetSashVisible(wx.SASH_TOP, True)
        self.inspector = inspectorstatetab.StateTab(win, None, True)
        self.inspector_window = win

        # In modal mode state is not shown
        if self.modal:
            self.inspector_window.Hide()

        # Bind events
        self.Bind(
            wx.EVT_SASH_DRAGGED_RANGE, 
            self.on_sash_drag, 
            id=self.ID_WINDOW_BROWSER, 
            id2= self.ID_WINDOW_INSPECTOR
            )
        self.Bind(
            wx.EVT_SIZE, 
            self.on_size
            )
        self.Bind(
            wx.EVT_TREE_SEL_CHANGED, 
            self.on_library_sel_changed, 
            self.browser
            )
        self.Bind(
            wx.EVT_LIST_ITEM_SELECTED, 
            self.on_entity_selected, 
            self.lister.list
            )
        self.Bind(
            wx.EVT_LIST_ITEM_DESELECTED, 
            self.on_entity_deselected, 
            self.lister.list
            )
        self.Bind(
            wx.EVT_LIST_KEY_DOWN, 
            self.on_key_down, 
            self.lister.list
            )
        self.browser.Bind(
            wx.EVT_RIGHT_DOWN, 
            self.on_tree_ms_right_down
            )
        self.Bind(
            wx.EVT_LIST_ITEM_RIGHT_CLICK, 
            self.on_right_click, 
            self.lister.list
            )
        self.lister.list.Bind(
            wx.EVT_LEFT_DCLICK, 
            self.on_double_click
            )
        self.lister.list.Bind(
            wx.EVT_LIST_BEGIN_DRAG, 
            self.on_begin_drag
            )
        # side store library path, since it will be overwritten
        # during load_config method
        library_path = self.__class__.last_selected_path
        self.load_config()
        self.create_natives()
        if library_path is not None:
            self.expand_tree_to_library_path(library_path)
        return self

    def expand_tree_to_library_path(self, library_path):
        adjusted_path = library_path.replace(
                                    '/editor/',
                                    ''
                                    )
        self.browser.expand_tree(adjusted_path)

    def refresh(self):
        entity_name = self.browser.GetItemText(self.browser.current) 
        entity = self.get_entity_class( str(entity_name) )
        if entity is not None:
            self.entity_server.loadentitysubclasses(entity)
            self.create_natives(entity_name)

    def on_tree_ms_right_down (self, event):
        self.browser.__mouse_position = event.GetPosition()
        item = self.browser.HitTest( event.GetPosition() )
        if item[1] != wx.LIST_HITTEST_NOWHERE:
            item_data = self.browser.GetPyData(item[0])
            if item[0] == self.browser.GetRootItem():
                event.Skip()
                return
            elif item_data.getparent() == self.browser.GetPyData( self.browser.GetRootItem() ) and not self.browser.is_native(item[0]):
                event.Skip()
                return
            elif self.browser.is_native(item[0]):
                if self.browser.GetItemText(item[0]) != "natives":
                    if not hasattr(self, "subclassnative_id"):
                        subclassnative_id = wx.NewId()
                        loadsubclasses_id = wx.NewId()
                    self.Bind(
                        wx.EVT_MENU, 
                        self.on_subclass_from_tree, 
                        id=subclassnative_id
                        )
                    self.Bind(
                        wx.EVT_MENU, 
                        self.on_load_subclasses, 
                        id=loadsubclasses_id
                        )
                    menu = wx.Menu()
                    item = wx.MenuItem(
                                menu, 
                                subclassnative_id,
                                "Create new entity subclass"
                                )
                    menu.AppendItem(item)
                    item = wx.MenuItem(
                                menu, 
                                loadsubclasses_id,
                                "Load subclasses"
                                )
                    menu.AppendItem(item)

                    event.Skip()

                    self.browser.PopupMenu( menu, event.GetPosition() )
                    menu.Destroy()
                else:
                    event.Skip()
            else:
                event.Skip()

    def on_load_subclasses ( self, event ):
        try:
            wx.BeginBusyCursor()
            entity_name = self.browser.GetItemText(self.browser.current)
            entity = self.get_entity_class( str(entity_name) )
            self.entity_server.loadentitysubclasses(entity)
            self.create_natives(entity_name)
        finally:
            wx.EndBusyCursor()

    def create_natives (self, path_to_expand=None):
        root = app.get_libraries()
        new_library_path = root.getfullname() + "/natives/"
        native_lib = pynebula.new("nstringlist", new_library_path)
        self.browser.files["natives"] = ["", False]
        native_object = pynebula.lookup("/sys/nobject/nentityobject")
        node = native_object.gethead()
        while node is not None:
            if node.hascommand("getsucc") and self.__is_class_native( node.getname() ):
                name = node.getname()
                native_lib.appendstring(name)
                pynebula.new(
                    "nstringlist", 
                    new_library_path + name
                    )
                self.fill_native(node, new_library_path)
                node = node.getsucc()
            else:
                # Prevent no nroot found
                node = None
        if path_to_expand is not None:
            path_to_expand = path_to_expand.replace(
                                        '/editor/libraries/natives/', 
                                        ''
                                        )
            new_library_path += path_to_expand
            self.expand_tree_to_library_path(new_library_path)
        else:
            self.browser.expand_tree('')

    def __is_class_native(self, class_name):
        return servers.get_kernel_server().isclassnative(class_name)

    def get_entity_class(self, class_name):
        """ Return the entity class with the given name """
        return self.entity_server.getentityclass(class_name)

    def fill_native (self, node, lib_path):
        name = node.getname()
        # To improve interface entities with no children are not displayed
        if node.gethead() is not None:
            new_native = pynebula.new("nstringlist", lib_path + name)
        new_lib_path = lib_path + name + "/"
        child = node.gethead()

        # Add all children recursively
        while child is not None:
            if child.hascommand("getsucc"):
                new_native.appendstring( child.getname() )
                self.fill_native(child, new_lib_path)
                child = child.getsucc()
            else:
                child = None

    def on_begin_drag (self, event):
        item = -1
        strings = ""
        while True:
            item = self.lister.list.GetNextItem(
                        item,
                        wx.LIST_NEXT_ALL,
                        wx.LIST_STATE_SELECTED
                        )
            if ( item == -1 ):
                break;
            else:
                strings = strings + self.lister.list.GetItemText(item) + " "
        data = wx.TextDataObject(strings[:-1])
        drop_source = wx.DropSource(self)
        drop_source.SetData(data)
        result = drop_source.DoDragDrop(wx.Drag_AllowMove)
        # if move refresh the lister with the list updated
        if result == wx.DragMove:
            self.lister.list.DeleteItem(self.lister.current_item)
            self.browser.dirt_file(self.current_library)
            self.lister.Refresh()
            self.inspector.draw_state_commands(None)

    def load_config (self):
        files = guisettings.Repository.getsettingvalue(guisettings.ID_Grimoire)
        if files is not None:
            self.browser.load_files(files)
            # Backward compatibility for the "general" case
            if self.browser.files.has_key("general"):
                general_lib_path = self.__mangle_path(
                                            "wc:libs/grimoire/general.n2"
                                            )
                self.browser.files = { 'general': [general_lib_path, False] }
        else:
            print "Problem loading libraries"

    def save_config (self):
        self.browser.save_files()
        # "general" library must not be saved in configuration file
        if self.browser.files.has_key("general"):
            self.browser.files.pop("general")
        result = guisettings.Repository.setsettingvalue(
                        guisettings.ID_Grimoire, 
                        self.browser.files
                        )
        if not result:
            print "Cannot save libraries"    

    def refresh_list (self, new_class=None):
        if new_class is not None:
            obj = self.browser.GetPyData(self.current_library)
            obj.appendstring( str(new_class) )
        self.browser.dirt_file(self.current_library)
        self.get_entity_list(obj)
        self.lister.populate_list( self.entities.copy() )

    def on_subclass (self, event):
        entity_name = self.lister.list.GetItemText(self.lister.current_item)
        if self.new_subclass(entity_name) is not None:
            self.lister.populate_list( self.entities.copy() )

    def on_subclass_from_tree (self, event):
        entity_name = self.browser.GetItemText(self.browser.current)
        if self.new_subclass(entity_name) is not None:
            self.lister.populate_list( self.entities.copy() )

    def new_subclass (self, entity_name):
        if self.browser.is_valid(self.current_library):
            dlg = wx.TextEntryDialog(
                        self, 
                        'Enter name for the class',
                        'Create subclass', 
                        ''
                        )

            if dlg.ShowModal() == wx.ID_OK:
                subclass_name = dlg.GetValue()
                dlg.Destroy()

                if not self.entity_server.checkclassname( str(subclass_name) ):
                    subclass_name = subclass_name.lower()
                    subclass_name = subclass_name.capitalize()

                new_entity = self.create_subclass( entity_name, subclass_name )
                if new_entity is not None:
                    obj = self.browser.GetPyData(self.current_library)
                    self.browser.dirt_file(self.current_library)
                    obj.appendstring( str(subclass_name) )
                    self.create_natives()
                    self.expand_tree_to_library_path(
                        obj.getfullname() 
                        )
                else:
                    cjr.show_error_message(
                        "Error creating subclass, please report this..."
                        )
                return new_entity
            dlg.Destroy() 
                
        else:
            self.read_only_message("Library is read-only")
            return None

    def create_subclass (self, parent_class, subclass_name):
        if self.entity_server.checkclassname( str(subclass_name) ):
            if self.lister.list.FindItem(-1, subclass_name) != wx.NOT_FOUND:
                cjr.show_information_message(
                    "There is already a class with this name.\n\n"\
                    "Please try another."
                    )
                return
            entity = self.get_entity_class( str(parent_class) )
            new_entity = self.entity_server.newentityclass(
                                    entity, 
                                    str(subclass_name)
                                    )
            return new_entity
        else:
            cjr.show_information_message(
                'Bad class name: name must begin with uppercase character'\
                ' and be followed by a lowercase character, number or "_"'
            )
        return None

    def on_newclass (self, event):
        if self.browser.is_valid(self.current_library):
            dlg = objbrowserwindow.create_dialog(
                        self, 
                        True, 
                        "/sys/nobject/nentityobject"
                        )
            if dlg.ShowModal() == wx.ID_OK:
                class_name = dlg.GetTitle()
                not_used, class_name = os.path.split(class_name)
                obj = self.browser.GetPyData(self.current_library)
                obj.appendstring( str(class_name) )
                self.browser.dirt_file(self.current_library)
                self.get_entity_list(obj)
                self.lister.populate_list( self.entities.copy() )
        else:
            self.read_only_message("Library is read-only")

    def read_only_message(self, message=""):
        cjr.show_warning_message(message)

    def delete (self):
        if self.browser.is_valid(self.current_library):
            obj = self.browser.GetPyData(self.current_library)
            entity_name = self.lister.list.GetItemText(self.lister.current_item)
            if self.browser.get_current_root_library_name() != 'natives':
                if self.browser.get_current_root_library_name() == 'natives':
                    reference_list = []
                    self.find_entity(
                        entity_name, 
                        self.browser.get_current_root_library_nobject(), 
                        reference_list, 
                        True
                        )
                    if len(reference_list) == 1:
                        cjr.show_error_message(
                            "This is the last reference to this entity: "\
                            "you cannot delete it.\n\n"\
                             "If you want delete it from the system use"\
                             "the 'delete class from system' option"
                             )
                        return
                msg = "This will delete the '%s' entity class from "\
                            "the grimoire\n\n"\
                            "Are you sure you want to continue?" % entity_name
                modal = cjr.warn_yes_no( app.get_top_window(), msg )
                if ( modal == wx.ID_YES):
                    obj.removestring( str(entity_name) )
                    self.lister.list.DeleteItem(self.lister.current_item)
                    self.browser.dirt_file(self.current_library)
                    self.lister.Refresh()
                    self.get_entity_list(obj)
                    self.lister.populate_list( self.entities.copy() )
                    self.inspector.draw_state_commands(None)
        else:
            self.read_only_message("Element is read-only")

    def on_delete (self, event):
        self.delete()

    def on_open (self, event):
        entity_name = self.lister.list.GetItemText(self.lister.current_item)
        entity = self.get_entity_class( str(entity_name) )
        win = objdlg.create_window(
                    self.GetGrandParent(), 
                    entity.getfullname()
                    )
        win.display()

    def on_openinviewer (self, event):
        entity_name = self.lister.list.GetItemText(self.lister.current_item)
        default_level = str(
                                self.__mangle_path(
                                    "wc:levels/default.n2"
                                    )
                                )
        # Ask for final confirmation
        msg = "Any unsaved level data will be lost.\n\n"\
                    "Are you sure you want to load the '%s' "\
                    " class in the viewer?" % entity_name
        result = cjr.warn_yes_no( app.get_top_window(), msg )
        if result == wx.ID_YES:
            # Finally load the level
            try:
                try:
                    dlg = waitdlg.WaitDialog(
                                app.get_top_window(),
                                "Loading default level..." 
                                )
                    if levelcmds.prelevel_process(True):
                        servers.get_conjurer().loadlevel( default_level )
                        levelcmds.postlevel_process(True)
                        dlg.set_message(
                            "Placing selected entity..." 
                            )
                        level = app.get_level()
                        entity_id = level.findentity("outdoor")
                        outdoor = servers.get_entity_object_server().getentityobject(
                                        entity_id
                                        )
                        entity_class = outdoor.getentityclass()
                        if entity_class.hascommand("getheightmap"):
                            height_map = entity_class.getheightmap()
                            terrain_size = height_map.getextent()
                            world_interface = servers.get_world_interface()
                            entity = world_interface.newentity(
                                            str(entity_name), 
                                            terrain_size / 2, 
                                            0, 
                                            terrain_size / 2 
                                            )
                            selector = app.get_object_state()
                            selector.addentitytoselection( entity.getid() )
                            servers.get_conjurer().movecameratoselection()
                finally:
                    dlg.Destroy()
            except:
                # make sure any errors are not hidden
                raise

    def create_entity_popup (self, position):
        if not hasattr(self, "subclass_id"):
            subclass_id = wx.NewId()
            deleteclass_id = wx.NewId()
            delete_id = wx.NewId()
            open_id = wx.NewId()
            newclass_id = wx.NewId()
            openinviewer_id = wx.NewId()
            asset_id = wx.NewId()
            merge_id = wx.NewId()
            impostor_id = wx.NewId()

            self.Bind(wx.EVT_MENU, self.on_subclass, id=subclass_id)
            self.Bind(wx.EVT_MENU, self.on_deleteclass, id=deleteclass_id)
            self.Bind(wx.EVT_MENU, self.on_delete, id=delete_id)
            self.Bind(wx.EVT_MENU, self.on_open, id=open_id)
            self.Bind(wx.EVT_MENU, self.on_newclass, id=newclass_id)
            self.Bind(
                wx.EVT_MENU, 
                self.on_openinviewer, 
                id=openinviewer_id
                )
            self.Bind(wx.EVT_MENU, self.on_asset, id=asset_id)
            self.Bind(wx.EVT_MENU, self.on_merge, id=merge_id)
            self.Bind(wx.EVT_MENU, self.on_impostor, id=impostor_id)

        # make a menu

        menu = wx.Menu()

        item = wx.MenuItem(menu, subclass_id, "Create new entity subclass")
        menu.AppendItem(item)
        menu.Append(deleteclass_id, "Delete entity class from system")
        menu.Append(asset_id, "Create class with another asset")
        menu.Append(merge_id, "Merge with selected object")
        menu.AppendSeparator()
        if self.browser.get_current_root_library_name() != 'natives':
            # This entries is not valid for natives
            menu.Append(delete_id, "Delete entity class from library")
            menu.Append(newclass_id, "Add entity class to Library")
            menu.AppendSeparator()
        menu.Append(impostor_id, "Build impostor level")
        menu.AppendSeparator()
        menu.Append(openinviewer_id, "Open in viewer")
        menu.Append(open_id, "Open inspector")

        # Popup the menu.  If an item is selected then its handler
        # will be called before PopupMenu returns.

        self.PopupMenu(
            menu, 
            position + wx.Point(40, 10)
            )
        menu.Destroy()

    def on_impostor(self, event):
        entity_name = self.lister.list.GetItemText(self.lister.current_item)
        state = app.get_state('class')
        state.buildimpostorlevel( str(entity_name) )

    def on_merge (self, event):
        num_entities = app.get_object_state().getselectioncount()
        if num_entities > 0:
            entity = app.get_object_state().getselectedentity(0)
            position = entity.getposition()
            entity_name = self.lister.list.GetItemText(self.lister.current_item)
            entity = servers.get_world_interface().newentity(
                            str(entity_name), 
                            position[0], 
                            position[1], 
                            position[2]
                            )
            entity.setposition(position[0], position[1], position[2])

        else:
            cjr.show_error_message(
                "You must select target object in summoner"
                )

    def on_asset (self, event):
        entity_name = self.lister.list.GetItemText(self.lister.current_item)
        asset_dlg = AssetDialog(self, entity_name)
        asset_dlg.Show()

    def on_deleteclass (self, event):
        entity_name = self.lister.list.GetItemText(
                                self.lister.current_item
                                )

        if entity_name.startswith('ne'):
            cjr.show_error_message(
                "You can only delete system entities"
                )
            return

        if self.__is_class_native( str(entity_name) ):
            cjr.show_error_message(
                "You cannot delete native entities"
                )
            return

        msg = "This will delete the class from the system.\n\n Are you sure?"
        result = cjr.warn_yes_no(
                        app.get_top_window(), 
                        msg 
                        )
        if result == wx.ID_YES:
            deleted_ok = self.deleteclass(entity_name)
            if deleted_ok:
                # Remove the item and update the state panel
                self.lister.list.DeleteItem(self.lister.current_item)
                self.inspector.draw_state_commands(None)
            else:
                cjr.show_error_message(
                    "Unable to delete entity class. Posible causes:\n"\
                    "    * You are trying to delete a native class.\n"\
                    "    * There are instances of this class in your scene.\n"\
                    "    * This class has subclasses"
                    )

    def on_deleteclassfromtree (self, event):
        pass

    def deleteclass ( self, entity_name ):
        """ Search all objects that contain the entity class, 
            remove entity class and delete it from system """
        # Build list of all the libraries where entity class is
        class_list = []
        library_obj = self.browser.GetPyData(
                            self.browser.GetRootItem() 
                            )
        library_obj = library_obj.gethead()            
        # Search in all libraries
        self.find_entity(
            entity_name, 
            library_obj, 
            class_list, 
            True  
            )
        # Remove the class from all the libraries
        for entity in class_list:
            obj = pynebula.lookup(entity["library"])
            obj.removestring( str(entity_name) )
            self.browser.dirt_file( str(obj.getname()) )
        # Remove from system
        return self.entity_server.removeentityclass(
            str(entity_name) 
            )

    def on_preview (self, event):
        # Turn on/off the preview mode
        if event.IsChecked():
            self.preview = True
            self.show_preview()
        else:
            self.preview = False
            self.hide_preview()

    def show_preview(self):
        # Show a preview for current selected entity
        entity_name = self.lister.list.GetItemText(self.lister.current_item)
        servers.get_conjurer().openpreview( str(entity_name) )

    def hide_preview(self):
        # Close the previewer (nothing happens if is not open)
        servers.get_conjurer().closepreview()
    
    def on_right_click(self, event):
        if self.browser.GetItemText(self.browser.current) != "natives":
            self.create_entity_popup( event.GetPosition() )
        event.Skip()
    
    def mouse_left_down (self, event):
        self.__mouse_pos = event.GetPosition()
        event.Skip()

    def on_icon_view (self, event):
        if not hasattr(self, "report_id"):
            icon_list_id = wx.NewId()
            icon_normal_id = wx.NewId()
            icon_large_id = wx.NewId()

            self.Bind(wx.EVT_MENU, self.on_list_list, id=icon_list_id)
            self.Bind(wx.EVT_MENU, self.on_list_normal, id=icon_normal_id)
            self.Bind(wx.EVT_MENU, self.on_list_large, id=icon_large_id)

        # make a menu
        menu = wx.Menu()
        menu.Append(icon_list_id, "List")
        menu.Append(icon_normal_id, "Normal icons")
        menu.Append(icon_large_id, "Large icons")

        # Popup the menu.  
        self.PopupMenu(menu, self.__mouse_pos)
        menu.Destroy()

    def on_icon_save (self, event):
        self.browser.save()        

    def on_icon_saveall (self, event):
        self.browser.save_files()

    def on_icon_new (self, event):
        self.browser.create_new_lib( self.GetPosition() )

    def on_key_down (self, event):
        event_key_code = event.GetKeyCode()
        if  event_key_code == wx.WXK_DELETE:  
            self.delete()

        elif event_key_code == nh.KEY_HELP:
            value = "grimoire|wiki"
            self.GetParent().GetEventHandler().ProcessEvent(
                nh.HelpEvent(
                    self.GetId(),
                    value, 
                    self
                    )
                )
        elif event_key_code == wx.WXK_F2:
            self.match_count = self.match_count - 1
            if self.match_count < 0:
                self.match_count = len(self.match_list) - 1
            self.select_next_match()
        elif event_key_code == wx.WXK_F3:
            self.match_count = self.match_count + 1             
            if self.match_count > len(self.match_list) - 1:
                self.match_count = 0            
            self.select_next_match()

        event.Skip()

    def on_recursive (self, event):
        if event.IsChecked():
            self.recursive = True
        else:
            self.recursive = False

        obj = self.browser.GetPyData(self.current_library)
        self.get_entity_list(obj)
        self.lister.populate_list( self.entities.copy() )

    def on_list_normal (self, event):
        self.view = 'normal'
        self.lister.display_normal()
        self.lister.populate_list(self.entities)

    def on_list_large (self, event):
        self.view = 'large'
        self.lister.display_large()
        self.lister.populate_list(self.entities)

    def on_list_list (self, event):
        self.view = 'list'
        self.lister.display_list()
        self.lister.populate_list(self.entities)

    def on_find (self, event):
        dlg = wx.TextEntryDialog(
                    self, 
                    'Enter entity to search',
                    'Find entity', 
                    ''
                    )
        
        if dlg.ShowModal() == wx.ID_OK:
            find_query = dlg.GetValue()
            dlg.Destroy()
        else:
            dlg.Destroy()
            return

        root_item = self.browser.GetRootItem()
        if not self.recursive and self.current_library != root_item:
            library_obj = self.browser.GetPyData(self.current_library)
        else:
            library_obj = self.browser.GetPyData(root_item)
            library_obj = library_obj.gethead()

        del self.match_list
        self.match_list = []

        self.find_entity(find_query, library_obj, self.match_list)

        if self.match_list == []:
            cjr.show_information_message("Entity not found")
        else:
            self.match_count = 0
            self.select_next_match()

    def find_entity(self, find_query, library_obj, ref_list, restrictive=False):
        if library_obj is not None:       
            entity_list = library_obj.getlist()
            for index in xrange( len(entity_list) ):
                if restrictive == False:
                    search_object = re.compile(find_query, re.IGNORECASE)
                    if search_object.search(entity_list[index]):
                        match = {
                                    'name': entity_list[index], 
                                    'library': library_obj.getfullname() 
                                    }
                        ref_list.append(match)
                else:
                    if find_query == entity_list[index]:
                        match = {
                                    'name': entity_list[index], 
                                    'library': library_obj.getfullname() 
                                    }
                        ref_list.append(match)
            self.find_entity(
                find_query, 
                library_obj.getsucc(), 
                ref_list, 
                restrictive
                )
            self.find_entity(
                find_query, 
                library_obj.gethead(), 
                ref_list, 
                restrictive
                )

    def select_next_match (self):
        if len(self.match_list) > 0:
            match = self.match_list[self.match_count]
            library_name = match['library']
            name = match['name']
            self.expand_tree_to_library_path(library_name)
            item = self.lister.list.FindItem(-1, name)
            self.lister.list.SetItemState(
                                item, 
                                wx.LIST_STATE_SELECTED, 
                                wx.LIST_STATE_DROPHILITED
                                    |wx.LIST_STATE_FOCUSED
                                    |wx.LIST_STATE_SELECTED
                                )
            item_pos = self.lister.list.GetItemPosition(item)
            self.lister.list.ScrollList(item_pos[0], item_pos[1])

    def on_sash_drag(self, event):
        if event.GetDragStatus() == wx.SASH_STATUS_OUT_OF_RANGE:
            return

        event_id = event.GetId()

        if event_id == self.ID_WINDOW_BROWSER:
            self.browser_window.SetDefaultSize(
                (event.GetDragRect().width, 1000)
                )
        elif event_id == self.ID_WINDOW_INSPECTOR:
            self.inspector_window.SetDefaultSize(
                (1000, event.GetDragRect().height)
                )

        wx.LayoutAlgorithm().LayoutWindow(self, self.lister)
        self.lister.Refresh()

    def on_size(self, event):
        wx.LayoutAlgorithm().LayoutWindow(self, self.lister)

    def on_library_sel_changed (self, event):
        item = event.GetItem()
        if item:
            try:
                wx.BeginBusyCursor()
                self.inspector.draw_state_commands(None)
                self.current_library = item
                obj = self.browser.GetPyData(item)
                self.get_entity_list(obj)
                self.__class__.last_selected_path = obj.getfullname()
                self.lister.populate_list( self.entities.copy() )
            finally:
                wx.EndBusyCursor()
        else:
            event.Skip()

    def get_entity_list (self, obj):
        self.entities.clear()
        if obj is not None:
            i = 0            
            if obj.getname() != "libraries": 
                for entity in obj.getlist():
                    self.set_entity_image( entity, i)
                    i = i + 1
            if self.recursive:
                self.get_entity_list_recursive(obj.gethead(), i)
        else:
            print """Ueeeps! Cannot get object entities, object not found
                    Execution will continue, but report this error please"""

    def get_entity_list_recursive (self, obj, i=0):
        if obj is not None:
            for entity in obj.getlist():
                if not self.exist_entity(entity):
                    self.set_entity_image( entity, i)
                    i = i +1

            i = self.get_entity_list_recursive(obj.getsucc(), i)
            self.get_entity_list_recursive(obj.gethead(), i)
        return i

    def set_entity_image (self, entity, i):
        if servers.get_kernel_server().isclassloaded( str(entity) ):
            entity_obj = self.get_entity_class( str(entity) )
        else:
            entity_obj = None

        if (entity_obj is not None) and (entity_obj.getparent().getname() == "nentityobject"):
            self.entities[i] = (
                                    self.__mangle_path(
                                       "outgui:images/native_entity.bmp"
                                       ), 
                                    entity
                                    )
        else: 
            try:
                if entity_obj is not None:
                    rsc_dir = entity_obj.getresourcefile()
                else:
                    rsc_dir = "wc:export/assets/" + entity

                image_file = rsc_dir + "/debug/thumb.jpg"
                
                # admit bmp or jpg formats
                mangle_path = self.__mangle_path(image_file)
    
                if os.path.exists(mangle_path):
                    self.entities[i] = (mangle_path, entity)
                else:
                    self.entities[i] = (
                                            self.__mangle_path(
                                                "outgui:images/no_entity.bmp"
                                                ),
                                            entity
                                            )
            except:
                self.entities[i] = (
                                        self.__mangle_path(
                                            "outgui:images/game_entity.bmp"
                                            ), 
                                        entity
                                        )

    def exist_entity (self, entity):
        items = self.entities.items()
        for index, data in items:
            if data[1] == entity:
                return True
        return False

    def __mangle_path(self, path_string):
        return servers.get_file_server().manglepath(path_string)

    def on_double_click (self, event):
        element = self.lister.list.HitTest( event.GetPosition() ) 
        if element[0] != -1:
            entity_name = self.lister.list.GetItemText(element[0])

            if self.modal:
                self.parent.SetTitle(entity_name)
                self.parent.Hide()
                self.parent.EndModal(wx.ID_OK)
            else:
                entity = self.get_entity_class( str(entity_name) )
                win = objdlg.create_window(
                            self.GetGrandParent(), 
                            entity.getfullname()
                            )
                win.display()

    def append_all_selected (self):
        item = -1
        self.string_list.clear()
        while True:
            item = self.lister.list.GetNextItem(
                        item, 
                        wx.LIST_NEXT_ALL,
                        wx.LIST_STATE_SELECTED
                        )
            if ( item == -1 ):
                break;
            else: 
                entity_name = self.lister.list.GetItemText(item)
                self.string_list.appendstring( str(entity_name) )

    def on_entity_selected (self, event):
        entity_name = self.lister.list.GetItemText(self.lister.current_item)
        if self.modal:
            self.parent.SetTitle(entity_name)
        else:
            entity_name_as_string = str(entity_name)
            # show the state editor
            if servers.get_kernel_server().isclassloaded(entity_name_as_string):
                entity = self.get_entity_class(entity_name_as_string)
            else:
                entity = None
            self.inspector.draw_state_commands(entity)
            # add to the placer tool the selected items
            self.append_all_selected()
            # Show preview if activated
            if self.preview:
                self.show_preview()

    def on_entity_deselected (self, event):
        # add to the placer tool the selected items
        self.append_all_selected()


class AssetDialog (wx.Dialog):
    """ Opens a dialog that provides an interface to connect
        the asset of a class with another class """
    def __init__ (self, parent, asset_class):
        wx.Dialog.__init__(
            self, parent, -1, title = "Create class with another asset"
            )
        self.parent = parent

        self.grid = propgrid.PropertyGrid(
                        self, 
                        style=pg.wxPG_BOLD_MODIFIED|
                                    pg.wxPG_DEFAULT_STYLE|
                                    pg.wxPG_SPLITTER_AUTO_CENTER
                                )
        # Use the a basic format with an entityclass selector
        format = { 
                    "type": 's',
                    "editor": None,
                    "max": None,
                    "min": None,
                    "step": None,
                    "subtype": "entityclass",
                    "flags": None,
                    "enums": None
                    }

        self.pid_asset = self.grid.append_property(
                                None, 
                                "Set asset from", 
                                format
                                )
        self.grid.set_property_value(self.pid_asset, asset_class)
        self.pid_superclass = self.grid.append_property(
                                        None, 
                                        "Superclass", 
                                        format
                                        )

        text = wx.StaticText(self, -1, "New class name:")
        self.new_class_name = wx.TextCtrl(self, -1, "")
        self.create_button = wx.Button(self, -1, "Create")
        self.cancel_button = wx.Button(self, -1, "Cancel")

        # add filterby label and boxlist
        sizer = wx.BoxSizer (wx.VERTICAL)

        hori_sizer = wx.BoxSizer(wx.HORIZONTAL)
        hori_sizer.Add(self.grid, 1, wx.EXPAND|wx.ALL, cfg.BORDER_WIDTH)
        sizer.Add(hori_sizer, 0, wx.EXPAND)
        hori_sizer = wx.BoxSizer(wx.HORIZONTAL)
        hori_sizer.Add(text, 0, wx.EXPAND|wx.ALL, cfg.BORDER_WIDTH)
        hori_sizer.Add(
            self.new_class_name, 
            1, 
            wx.EXPAND|wx.ALL, 
            cfg.BORDER_WIDTH
            )
        sizer.Add(hori_sizer, 0, wx.EXPAND)
        hori_sizer = wx.BoxSizer(wx.HORIZONTAL)
        hori_sizer.Add(
            self.create_button, 
            1, 
            wx.ALL|wx.EXPAND, 
            cfg.BORDER_WIDTH
            )
        hori_sizer.Add(
            self.cancel_button, 
            1, 
            wx.ALL|wx.EXPAND, 
            cfg.BORDER_WIDTH
            )
        sizer.Add(hori_sizer, 0, wx.ALIGN_CENTER)
        self.SetSizer(sizer)

        self.SetSize( wx.Size(300, 170) )

        self.Bind(wx.EVT_BUTTON, self.on_create, self.create_button)
        self.Bind(wx.EVT_BUTTON, self.on_cancel, self.cancel_button)

    def on_create (self, event):
        assetclass = self.grid.get_property_value(self.pid_asset)
        superclass = self.grid.get_property_value(self.pid_superclass)
        if servers.get_kernel_server().findclass( str(superclass) ) and\
                servers.get_kernel_server().findclass( str(assetclass) ):
            ec_server = servers.get_entity_class_server()
            entity_asset = ec_server.getentityclass(
                                    str(assetclass)
                                    )
            class_name = self.new_class_name.GetValue()
            if not ec_server.checkclassname( str(class_name) ):
                class_name = class_name.lower()
                class_name = class_name.capitalize()

            new_class = self.parent.create_subclass( superclass, class_name )
            if new_class is not None:
                # Set to new class the asset resource file
                new_class.setresourcefile(
                    str( entity_asset.getresourcefile() )
                    )
                new_class.copycommoncomponentsfrom(entity_asset)
                parent_browser = self.parent.browser
                if parent_browser.get_current_root_library_name() != 'natives':
                    path = parent_browser.get_current_library_nobject().getfullname()
                    self.parent.refresh_list(class_name)
                    parent_browser.expand_tree_to_library_path(path)
                else:
                    path, tail = os.path.split( new_class.getfullname() )
                    self.parent.create_natives(
                        path.replace('/sys/nobject/nentityobject/', '')
                        )
                    # Select item in the new lister
                    item = self.parent.lister.list.FindItem(-1, class_name)
                    self.parent.lister.list.SetItemState(
                        item, 
                        wx.LIST_STATE_SELECTED,
                        wx.LIST_STATE_DROPHILITED
                            |wx.LIST_STATE_FOCUSED
                            |wx.LIST_STATE_SELECTED
                        )

                self.Close()
        else:
            cjr.show_error_message(
                "Class not found. Please insert a valid "\
                    "class for the asset and a valid superclass."
                )

    def on_cancel (self, event):
        self.Close()


class GrimoireWindow(childdialoggui.childDialogGUI):
    
    ID_ICON_NEW = wx.NewId()
    ID_ICON_SAVE = wx.NewId()
    ID_ICON_SAVEALL = wx.NewId()
    ID_ICON_VIEW = wx.NewId()
    ID_RECURSIVE = wx.NewId()
    ID_PREVIEW = wx.NewId()
    ID_FIND = wx.NewId()


    # If the object is intanced from this class, will be a child dialog

    def __init__(self, parent, modal=False):
        childdialoggui.childDialogGUI.__init__(self, "Grimoire", parent)

        self.grimoire = GrimoirePanel(self, modal)

        self.SetSize( wx.Size(600, 700) )

        self.toolbar = None
        self.create_toolbar()

        return self.grimoire

    def create_toolbar (self):
        fileserver = servers.get_file_server()
        self.toolbar = self.CreateToolBar(
                            wx.TB_HORIZONTAL
                            | wx.NO_BORDER
                            | wx.TB_FLAT
                            | wx.TB_TEXT
                            )
        self.toolbar.AddLabelTool(
            self.ID_ICON_NEW, 
            "", 
            wx.Bitmap(
              fileserver.manglepath("outgui:images/tools/new.bmp"),
               wx.BITMAP_TYPE_ANY
               ), 
            wx.NullBitmap, 
            wx.ITEM_NORMAL,
            "Create library", 
            "New"
            )

        self.toolbar.AddLabelTool(
            self.ID_ICON_SAVE, 
            "", 
            wx.Bitmap(
              fileserver.manglepath("outgui:images/tools/save.bmp"), 
              wx.BITMAP_TYPE_ANY
              ), 
            wx.NullBitmap, 
            wx.ITEM_NORMAL,
            "Save current library", 
            "Save"
            )

        self.toolbar.AddLabelTool(
            self.ID_ICON_SAVEALL, 
            "", 
            wx.Bitmap(
              fileserver.manglepath("outgui:images/tools/saveall.bmp"), 
              wx.BITMAP_TYPE_ANY
              ), 
            wx.NullBitmap, 
            wx.ITEM_NORMAL,
            "Save all libraries", 
            "Save All"
            )

        self.toolbar.AddSeparator()

        self.toolbar.AddLabelTool(
            self.ID_ICON_VIEW, 
            "", 
            wx.Bitmap(
              fileserver.manglepath("outgui:images/tools/icons_view.bmp"), 
              wx.BITMAP_TYPE_ANY
              ), 
            wx.NullBitmap, 
            wx.ITEM_NORMAL,
            "Change icon view", 
            "IconView"
            )

        self.toolbar.AddLabelTool(
            self.ID_RECURSIVE, 
            "", 
            wx.Bitmap(
              fileserver.manglepath("outgui:images/tools/undo.bmp")
              ), 
            wx.NullBitmap, 
            wx.ITEM_CHECK,
            "List libary recursively",
            "Recursive" 
            )

        self.toolbar.AddLabelTool(
            self.ID_FIND, 
            "", 
            wx.Bitmap(
              fileserver.manglepath("outgui:images/tools/find.bmp")
              ), 
            wx.NullBitmap, 
            wx.ITEM_NORMAL,
            "Search an entity in libraries", 
            "Find" 
            )

        self.toolbar.AddLabelTool(
            self.ID_PREVIEW, 
            "", 
            wx.Bitmap(
              fileserver.manglepath("outgui:images/tools/show.bmp")
              ), 
            wx.NullBitmap, 
            wx.ITEM_CHECK,
            "Show a preview of the entity in the summoner", 
            "Recursive" 
            )

        self.toolbar.Realize()

        # bif for capture mouse position when click in the toolbar
        self.toolbar.Bind(wx.EVT_LEFT_DOWN, self.grimoire.mouse_left_down)

        # Bind toolbar actions
        self.Bind(
            wx.EVT_TOOL, 
                self.grimoire.browser.on_save, 
                id=self.ID_ICON_SAVE
                )
        self.Bind(
            wx.EVT_TOOL, 
            self.grimoire.on_icon_saveall, 
            id=self.ID_ICON_SAVEALL
            )
        self.Bind(wx.EVT_TOOL, self.grimoire.on_icon_view, id=self.ID_ICON_VIEW)
        self.Bind(wx.EVT_TOOL, self.grimoire.on_icon_new, id=self.ID_ICON_NEW)
        self.Bind(wx.EVT_TOOL, self.grimoire.on_recursive, id=self.ID_RECURSIVE)
        self.Bind(wx.EVT_TOOL, self.grimoire.on_find, id=self.ID_FIND)
        self.Bind(wx.EVT_TOOL, self.grimoire.on_preview, id=self.ID_PREVIEW)
        self.Bind(wx.EVT_CLOSE, self.on_close)

        pynebula.pyBindSignal(
            app.get_libraries(), 
            'objectchanges',
            self, 
            'refresh', 
            0 
            )

    def __del__ (self):
        pynebula.pyUnbindTargetObject(
            app.get_libraries(), 
            'objectchanges', 
            self
            )

    def persist(self):
        obj = self.grimoire.browser.GetPyData(self.grimoire.current_library)
        if obj is not None:
            obj_path = obj.getfullname()
        else:
            obj_path = "/editor/libraries/"

        data = {
                    'lib_path': obj_path,
                    'view': self.grimoire.view,
                    } 
        self.grimoire.save_config()

        return [
            create_window, 
            (), # no parameters for create function
            data
            ]

    def restore(self, data_list):
        data = data_list[0]        
        library = data['lib_path']
        view = data['view']

        # expand the tree
        self.grimoire.load_config()
        self.grimoire.create_natives()

        self.grimoire.get_entity_list( pynebula.lookup(library) )

        # show the icons in the last size used
        if view == 'large':
            self.grimoire.lister.display_large()
            self.grimoire.lister.populate_list(self.grimoire.entities)
            self.grimoire.view = 'large'
        elif view == 'list':
            self.grimoire.lister.display_list()
            self.grimoire.lister.populate_list(self.grimoire.entities)
            self.grimoire.view = 'list'
        else:
            self.grimoire.view = 'normal'

        self.grimoire.expand_tree_to_library_path(
            library
            )

    def refresh(self):
        self.grimoire.refresh()

    def is_dirty(self):
        return self.grimoire.browser.must_save()

    def get_saver(self):
        return self

    def get_brief(self):        
        return "(grimoire) all libraries"

    def save (self):
        self.grimoire.save_config()

    def on_close(self, event):
        if self.grimoire.browser.must_save():
            # Save only if user wants
            result = cjr.confirm_yes_no(self, 'Save changes?')
            if result == wx.ID_YES:
                self.grimoire.save_config()    
        self.Destroy()

class GrimoireDialog (freedialoggui.freeDialogGUI):
    def __init__(self, parent, modal):
        # If the object is intanced from this class, will be a free dialog

        freedialoggui.freeDialogGUI.__init__(self, "Grimoire", parent)

        self.grimoire = GrimoirePanel(self, modal)

        # Settings
        self.SetSize( (600, 500) )

        return self.grimoire

def create_window(frame, modal=False):
    return GrimoireWindow( frame, modal)

def create_dialog(frame, modal=True):
    return GrimoireDialog(frame, modal)
