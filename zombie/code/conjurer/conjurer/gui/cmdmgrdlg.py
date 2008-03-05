##\file cmdmgrdlg.py
##\brief Command manager dialog

import os
import shutil
import wx

import pynebula

import app
import childdialoggui
import events
import filedlg
import format
import fsm
import newcmddlg
import scripteditordlg
import servers
import usagelog

import conjurerframework as cjr
import conjurerconfig as cfg

# unmangled path where class commands are stored
ClassesPath = "wc:libs/classes"

# unmangled path where template scripts are stored
TemplatesPath = "wc:libs/templates"


# set_nclass_dirty_flag function
def set_nclass_dirty_flag(is_dirty):
    if is_dirty:
        if not get_nclass_dirty_flag():
            pynebula.new('nroot', '/editor/nclass_dirty')
    else:
        if get_nclass_dirty_flag():
            pynebula.delete('/editor/nclass_dirty')

# get_nclass_dirty_flag function
def get_nclass_dirty_flag():
    try:
        pynebula.lookup('/editor/nclass_dirty')
        return True
    except:
        return False

# remove_class_dir function
def remove_class_dir(path, report=None, class_name=None):
    # Remove all files from the directory
    children = os.listdir( path )
    for child in children:
        os.remove( format.append_to_path( path, child ) )
        if report is not None:
            report.append( (class_name, child[:-4]) )
    # Remove the directory
    os.rmdir( path )


# get_class_commands function
def get_class_commands(class_name, inherited_cmds = False):
    """Return the prototype of all the scripted commands for a class"""
    path = format.append_to_path( ClassesPath, class_name )
    path = format.mangle_path( path )
    cmds_list = filedlg.get_file_list( path, ['lua'] )
    if inherited_cmds:
        class_name = servers.get_kernel_server().getsuperclass( class_name )
        if class_name != ':null:':
            cmds_list.extend( get_class_commands(class_name, True) )
    cmds_list.sort()
    return cmds_list


# ClassesTree class
class ClassesTree(wx.TreeCtrl):
    """ Tree control to look for a particular class, 
        with some scope organization. """
    
    def __init__(self, parent):
        wx.TreeCtrl.__init__(
            self,
            parent, 
            -1,
            style=wx.TR_HAS_BUTTONS|wx.TR_DEFAULT_STYLE|wx.SUNKEN_BORDER
            )
        
        self.Bind(
            wx.EVT_TREE_ITEM_EXPANDING, 
            self.__on_item_expanding, 
            self
            )
        self.Bind(
            wx.EVT_TREE_ITEM_COLLAPSED, 
            self.__on_item_collapsed, 
            self
            )
        
        # Icons used to show if a node is a folder or an element
        image_size = (16, 16)
        image_list = wx.ImageList(image_size[0], image_size[1])
        self.close_img = image_list.Add(
                                wx.ArtProvider_GetBitmap(
                                    wx.ART_FOLDER, 
                                    wx.ART_OTHER, 
                                    image_size
                                    )
                                )
        self.open_img = image_list.Add(
                                wx.ArtProvider_GetBitmap(
                                    wx.ART_FILE_OPEN,
                                    wx.ART_OTHER, 
                                    image_size
                                    )
                                )
        self.native_img = image_list.Add(
                                    wx.ArtProvider_GetBitmap(
                                        wx.ART_NORMAL_FILE,
                                        wx.ART_OTHER, 
                                        image_size
                                        )
                                    )
        bmp = wx.Bitmap(
                    format.mangle_path(
                        "outgui:images/tools/red_doc.bmp"
                        )
                    )
        self.no_native_img = image_list.Add(bmp)
        self.global_image = image_list.Add(
                                    wx.ArtProvider_GetBitmap(
                                        wx.ART_EXECUTABLE_FILE,
                                        wx.ART_OTHER, 
                                        image_size
                                        )
                                    )
        self.SetImageList(image_list)
        self.il = image_list
        
        # Build root branch
        # To define a special expand/collapse behavior for a branch set an id
        # as data for that branch. Then implement the expand/collpase function
        # by postfixing the id to the function name (see current
        # implementations for examples)
        root_item = self.AddRoot(
                            "Classes", 
                            self.close_img, 
                            -1, 
                            wx.TreeItemData('root')
                            )
        self.SetItemImage(
            root_item, 
            self.open_img, 
            wx.TreeItemIcon_Expanded 
            )
        self.SetItemHasChildren(
            root_item, 
            True 
            )
        
        # Expand root branch by default
        self.Expand( root_item )

    def __on_item_expanding(self, event):
        # If the item has a postfix as data use its specific expand function
        # (do nothing if function not available), otherwise use the generic one
        item = event.GetItem()
        postfix = self.GetPyData(item)
        if postfix is not None:
            expand_func_name = '_ClassesTree__expand_' + postfix
            if ClassesTree.__dict__.has_key( expand_func_name ):
                ClassesTree.__dict__[expand_func_name]( self, item )
        else:
            self.__expand_class( item, self.GetItemText(item) )
        event.Skip()
    
    def __expand_root(self, item):
        child_item = self.AppendItem(
                            item, 
                            "AI",
                            self.close_img, 
                            -1, 
                            wx.TreeItemData('ai') 
                            )
        self.SetItemImage( child_item, self.open_img, wx.TreeItemIcon_Expanded )
        self.SetItemHasChildren( child_item, True )
        child_item = self.AppendItem( item, "nobject", self.native_img, -1 )
        self.SetItemHasChildren( child_item, True )
    
    def __expand_ai(self, item):
        folders = [
            ('Behaviour actions','action','gpactionscript'),
            ('FSM selectors','selector','nfsmselector'),
            ('Transition conditions','condition','nscriptcondition'),
            ('Trigger outputs','trigger','nscriptoperation')
            ]
        for each_folder in folders:
            child_item = self.AppendItem(
                                item, 
                                each_folder[0],
                                self.close_img, 
                                -1, 
                                wx.TreeItemData(each_folder[1]) 
                                )
            self.SetItemImage( 
                child_item, 
                self.open_img, 
                wx.TreeItemIcon_Expanded
                )
            self.__set_children_mark( child_item, each_folder[2] )
        child_item = self.AppendItem(
                            item, 
                            "Global",
                            self.global_image, 
                            -1, 
                            wx.TreeItemData('global') 
                            )
        self.SetItemHasChildren( child_item, False )

    def __expand_action(self, item):
        self.__expand_class( item, 'gpactionscript' )

    def __expand_condition(self, item):
        self.__expand_class( item, 'nscriptcondition' )

    def __expand_selector(self, item):
        self.__expand_class( item, 'nfsmselector' )

    def __expand_trigger(self, item):
        self.__expand_class( item, 'nscriptoperation', False )

    def __expand_class(self, item, class_name, show_natives=True):
        # Get children classes
        k_server = servers.get_kernel_server()
        classes_tuple = k_server.getsubclasslist( str(class_name) )
        # Sort classes
        classes = []
        for name in classes_tuple:
            if (not show_natives) and k_server.isclassnative( str(name) ):
                continue
            classes.append( name )
        classes.sort()
        # Append classes to parent's branch
        for name in classes:
            if k_server.isclassnative( str(name) ):
                img = self.native_img
            else:
                img = self.no_native_img
            child_item = self.AppendItem( item, name, img, -1 )
            self.__set_children_mark( child_item, name )

    def __set_children_mark(self, item, class_name):
        # Show the expansion mark if there's some class inheriting from the
        # given one, otherwise hide it
        k_server = servers.get_kernel_server()
        has_children = len(k_server.getsubclasslist(class_name) ) > 0
        self.SetItemHasChildren( item, has_children )

    def __on_item_collapsed(self, event):
        # All levels are dynamically created, so delete the children always
        item = event.GetItem()
        self.DeleteChildren( item )
        event.Skip()

    def can_add_class_to_selected_class(self):
        item = self.GetSelection()
        if not item.IsOk():
            return False
        postfix = self.GetPyData(item)
        return postfix != 'root' and postfix != 'ai' and postfix != 'global'

    def can_delete_selected_class(self):
        # A class can be deleted only if it isn't native
        item = self.GetSelection()
        if not item.IsOk():
            return False
        postfix = self.GetPyData(item)
        if postfix is not None:
            return False
        class_name = self.GetItemText(item)
        return not servers.get_kernel_server().isclassnative( str(class_name) )

    def can_add_cmd_to_selected_class(self):
        item = self.GetSelection()
        if not item.IsOk():
            return False
        postfix = self.GetPyData(item)
        return postfix is None

    def get_selected_class_name(self):
        # If the item has a postfix as data use its specific get name function
        # (do nothing if function not available), otherwise use the generic one
        item = self.GetSelection()
        if not item.IsOk():
            return None
        postfix = self.GetPyData(item)
        if postfix is not None:
            expand_func_name = '_ClassesTree__get_class_name_' + postfix
            if ClassesTree.__dict__.has_key( expand_func_name ):
                return ClassesTree.__dict__[expand_func_name]( self, item )
        else:
            return self.__get_class_name( item )

    def __get_class_name_action(self, item):
        return 'gpactionscript'

    def __get_class_name_condition(self, item):
        return 'nscriptcondition'

    def __get_class_name_selector(self, item):
        return 'nfsmselector'

    def __get_class_name_trigger(self, item):
        return 'nscriptoperation'

    def __get_class_name(self, item):
        return self.GetItemText( item )

    def on_new_class(self, class_name):
        # Refresh current branch and expand it
        item = self.GetSelection()
        if not item.IsOk():
            return
        self.Collapse(item)
        self.SetItemHasChildren( item, True )
        self.Expand(item)

        # Select new class
        item, cookie = self.GetFirstChild( item )
        while item.IsOk():
            if self.GetItemText(item) == class_name:
                self.SelectItem(item)
                self.EnsureVisible(item)
                break
            item = self.GetNextSibling(item)

    def on_delete_class(self):
        # Remove selected class form the tree
        item = self.GetSelection()
        if not item.IsOk():
            return
        self.Delete(item)
        
        # Throw selection event for new selected class so command list is refreshed
        item = self.GetSelection()
        if item.IsOk():
            self.SelectItem(item)

    def get_current_class_group(self):
        item = self.GetSelection()
        if not item.IsOk():
            return None
        return self.GetPyData(item)

    def get_current_top_class_group(self):
        item = self.GetSelection()
        while item.IsOk():
            class_group = self.GetPyData(item)
            if class_group is not None:
                return class_group
            item = self.GetItemParent(item)
        return None

    def get_selected_class_path(self):
        # Return full path of the selected class as a list of nodes
        nodes = []
        item = self.GetSelection()
        while item.IsOk():
            nodes.append( self.GetItemText(item) )
            item = self.GetItemParent(item)
        nodes.reverse()
        return nodes

    def expand_class_path(self, path):
        item = None
        for node in path:
            # Find next node in path
            if item is None:
                item = self.GetRootItem()
            else:
                item, cookie = self.GetFirstChild(item)
            node_found = False
            while item.IsOk() and not node_found:
                if self.GetItemText(item) == node:
                    node_found = True
                else:
                    item = self.GetNextSibling(item)
            
            # Expand next node in path, scroll to it and select it
            if not item.IsOk():
                break
            self.EnsureVisible(item)
            self.SelectItem(item)
            if not self.ItemHasChildren(item):
                break
            if not self.IsExpanded(item):
                self.Expand(item)


# CommandManagerDialog class
class CommandManagerDialog(childdialoggui.childDialogGUI):
    """Command manager dialog"""
    
    def __init__(self, parent):
        childdialoggui.childDialogGUI.__init__(self, "Command manager", parent)
        
        self.sizer_cmds_staticbox = wx.StaticBox(self, -1, "Commands")
        self.sizer_classes_staticbox = wx.StaticBox(self, -1, "Classes")
        self.tree_classes = ClassesTree(self)
        self.button_new_class = wx.Button(self, -1, "New")
        self.button_del_class = wx.Button(self, -1, "Delete")
        self.button_refresh_class = wx.Button(self, -1, "Refresh")
        self.list_cmds = wx.ListBox(self, -1)
        self.button_new_cmd = wx.Button(self, -1, "New")
        self.button_del_cmd = wx.Button(self, -1, "Delete")
        self.button_edit_cmd = wx.Button(self, -1, "Edit")
        
        self.__set_properties()
        self.__do_layout()
        self.__bind_events()
    
    def __set_properties(self):
        self.__update_buttons()
    
    def __do_layout(self):
        sizer_layout = wx.BoxSizer(wx.VERTICAL)
        sizer_cmds = wx.StaticBoxSizer(
                            self.sizer_cmds_staticbox, 
                            wx.VERTICAL
                            )
        sizer_cmds_buttons = wx.BoxSizer(wx.HORIZONTAL)
        sizer_classes = wx.StaticBoxSizer(
                                self.sizer_classes_staticbox, 
                                wx.VERTICAL
                                )
        sizer_classes_buttons = wx.BoxSizer(wx.HORIZONTAL)
        sizer_classes.Add(
            self.tree_classes,
            1,
            wx.LEFT|wx.RIGHT|wx.BOTTOM|wx.EXPAND,
            cfg.BORDER_WIDTH
            )
        sizer_classes_buttons.Add(
            self.button_new_class,
            0,
            wx.ADJUST_MINSIZE
            )
        sizer_classes_buttons.Add(
            self.button_del_class,
            0,
            wx.LEFT|wx.ADJUST_MINSIZE,
            cfg.BORDER_WIDTH
            )
        sizer_classes_buttons.Add(
            self.button_refresh_class, 
            0,
            wx.LEFT|wx.ADJUST_MINSIZE,
            cfg.BORDER_WIDTH
            )
        sizer_classes.Add(
            sizer_classes_buttons, 
            0, 
            wx.ALL|wx.ALIGN_RIGHT, 
            cfg.BORDER_WIDTH
            )
        sizer_layout.Add(
            sizer_classes, 
            1, 
            wx.ALL|wx.EXPAND, 
            cfg.BORDER_WIDTH
            )
        sizer_cmds.Add(
            self.list_cmds,
            1,
            wx.LEFT|wx.RIGHT|wx.BOTTOM|wx.EXPAND|wx.ADJUST_MINSIZE,
            cfg.BORDER_WIDTH
            )
        sizer_cmds_buttons.Add(
            self.button_new_cmd, 
            0, 
            wx.ADJUST_MINSIZE
            )
        sizer_cmds_buttons.Add(
            self.button_del_cmd, 
            0,
            wx.LEFT|wx.ADJUST_MINSIZE,
            cfg.BORDER_WIDTH
            )
        sizer_cmds_buttons.Add(
            self.button_edit_cmd, 
            1,
            wx.LEFT|wx.ADJUST_MINSIZE,
            cfg.BORDER_WIDTH
            )
        sizer_cmds.Add(
            sizer_cmds_buttons,
            0, 
            wx.ALL|wx.ALIGN_RIGHT,
            cfg.BORDER_WIDTH
            )
        sizer_layout.Add(
            sizer_cmds, 
            0,
            wx.RIGHT|wx.LEFT|wx.BOTTOM|wx.EXPAND, 
            cfg.BORDER_WIDTH
            )
        self.SetSizerAndFit(sizer_layout)
        self.SetSize( (-1, 500) )

    def __bind_events(self):
        self.Bind(
            wx.EVT_TREE_SEL_CHANGED, 
            self.on_select_class, 
            self.tree_classes
            )
        self.Bind(
            wx.EVT_LISTBOX, 
            self.on_select_cmd, 
            self.list_cmds
            )
        self.Bind(
            wx.EVT_LISTBOX_DCLICK, 
            self.on_edit_cmd, 
            self.list_cmds
            )
        self.Bind(
            wx.EVT_BUTTON, 
            self.on_new_class,
            self.button_new_class
            )
        self.Bind(
            wx.EVT_BUTTON, 
            self.on_delete_class,
            self.button_del_class
            )
        self.Bind(
            wx.EVT_BUTTON, 
            self.on_refresh_class, 
            self.button_refresh_class
            )
        self.Bind(
            wx.EVT_BUTTON, 
            self.on_new_cmd, 
            self.button_new_cmd
            )
        self.Bind(
            wx.EVT_BUTTON, 
            self.on_delete_cmd, 
            self.button_del_cmd
            )
        self.Bind(
            wx.EVT_BUTTON, 
            self.on_edit_cmd, 
            self.button_edit_cmd
            )

    def __update_buttons(self):
        self.button_new_class.Enable(
            self.tree_classes.can_add_class_to_selected_class() 
            )
        self.button_refresh_class.Enable(
            self.tree_classes.can_add_class_to_selected_class() 
            )
        self.button_del_class.Enable(
            self.tree_classes.can_delete_selected_class() 
            )
        self.button_new_cmd.Enable(
            self.tree_classes.can_add_cmd_to_selected_class() 
            )
        self.button_del_cmd.Enable(
            self.list_cmds.GetSelection() != -1 
            )
        self.button_edit_cmd.Enable(
            self.list_cmds.GetSelection() != -1 
            )

    def __get_class_path(self, class_name, unmangled=False):
        path = format.append_to_path( ClassesPath, class_name )
        if unmangled:
            return path
        return format.mangle_path( path )

    def __get_cmd_path(self, class_name, cmd_name, unmangled=False):
        path = format.append_to_path(
                        self.__get_class_path(class_name),
                        cmd_name + ".lua" 
                        )
        if unmangled:
            return path
        return format.mangle_path( path )

    def __get_template_path(self, template_name, unmangled=False):
        path = format.append_to_path(
                    TemplatesPath, 
                    template_name + ".lua" 
                    )
        if unmangled:
            return path
        return format.mangle_path( path )

    def on_select_class(self, event):
        # If the item has a postfix as data use its specific get commands
        # function (do nothing if function not available), otherwise use the
        # generic one
        cmds = []
        item = event.GetItem()
        postfix = self.tree_classes.GetPyData(item)
        if postfix is not None:
            get_cmds_func_name = '_CommandManagerDialog__get_commands_of_' + postfix
            if CommandManagerDialog.__dict__.has_key( get_cmds_func_name ):
                cmds = CommandManagerDialog.__dict__[get_cmds_func_name](
                                self,
                                item 
                                )
        else:
            cmds = self.__get_commands_of_class(
                            item, 
                            self.tree_classes.GetItemText(item) 
                            )
        
        # Fill the commands list with the retrieved commands
        self.list_cmds.Clear()
        for cmd_name in cmds:
            self.list_cmds.Append( cmd_name )
        
        self.__update_buttons()

    def __get_commands_of_class(self, item, class_name):
        # Get all the file names in the directory wc:libs/classes/<class_name>
        # Each file name is the prototype of a command for that class
        return filedlg.get_file_list(
            self.__get_class_path(class_name), 
            ['lua']
            )

    def on_select_cmd(self, event):
        self.__update_buttons()

    def on_new_class(self, event):
        # Ask for a class name
        dlg = wx.TextEntryDialog(
                    None, 
                    "Enter the class name", 
                    "New class"
                    )
        result_ok = dlg.ShowModal() == wx.ID_OK
        if result_ok:
            class_name = dlg.GetValue().capitalize()
        dlg.Destroy()
        
        # Create the new class
        if result_ok:
            # Do some name validations
            ec_server = servers.get_entity_class_server()
            if not ec_server.checkclassname( str(class_name) ):
                cjr.show_error_message(
                    "Invalid class name"
                    )
                return
            k_server = servers.get_kernel_server()
            if k_server.findclass( str(class_name) ) != "":
                cjr.show_error_message(
                    "There is already another class called '%s'" % class_name
                    )
                return
            
            # Finally create the class
            parent_name = self.tree_classes.get_selected_class_name()
            if not ec_server.newclass( str(parent_name), str(class_name) ):
                msg = "Unknown error.\n\n" \
                            "Unable to create the class '%s'." % class_name
                cjr.show_error_message(msg)
                return
            set_nclass_dirty_flag(True)
            servers.get_script_server().addclass( str(class_name) )
            
            # For some classes, add commands and implementation by default
            class_group = self.tree_classes.get_current_class_group()
            if class_group is not None:
                add_cmds_func_name = '_CommandManagerDialog__add_commands_of_' + class_group
                if CommandManagerDialog.__dict__.has_key( add_cmds_func_name ):
                    CommandManagerDialog.__dict__[add_cmds_func_name](
                        self, 
                        class_name 
                        )
                    servers.get_script_server().refreshclass( str(class_name) )
            
            # Refresh the branch of the currently selected class
            self.tree_classes.on_new_class( class_name )

    def on_refresh_class(self, event):
        class_name = self.tree_classes.get_selected_class_name()
        servers.get_script_server().refreshclass( str(class_name) )

    def __add_command(self, class_name, template_name, cmd_proto):
        # Create an empty command for the selected class from a template
        class_path = self.__get_class_path( class_name )
        if not os.path.exists( class_path ):
            os.makedirs( class_path )
        template_path = self.__get_template_path( template_name )
        cmd_path = self.__get_cmd_path( class_name, cmd_proto )
        if os.path.exists( cmd_path ):
            # Give the opportunity to recover an old command (this may
            # happen when a command has been created but the user has forgot
            # to save the class before exiting Conjurer)
            msg = "A previously created command '%s' " \
                        "has been found.\n\n" \
                        "Do you want to recover it?" % cmd_proto
            recover = cjr.warn_yes_no(self, msg)    
            if recover == wx.ID_YES:
                return
        shutil.copy( template_path, cmd_path )

    def __add_commands_of_action(self, class_name):
        # Ask for the InitAction prototype
        dlg = newcmddlg.NewInitActionCmdDlg(self)
        result_ok = dlg.ShowModal() == wx.ID_OK
        if result_ok:
            init_proto = dlg.get_command_prototype()
        else:
            init_proto = 'b_init_o'
        dlg.Destroy()
        
        # Add action commands
        self.__add_command(class_name, 'action_init', init_proto)
        self.__add_command(class_name, 'action_run', 'b_run_v')
        self.__add_command(class_name, 'action_end', 'v_end_v')
        
        # Open InitAction script so the user doesn't 
        # forget to set the parameters
        if result_ok:
            cmd_unpath = self.__get_cmd_path(class_name, init_proto, True)
            win = scripteditordlg.create_window(
                        app.get_top_window(), 
                        cmd_unpath 
                        )
            win.display()

        app.get_top_window().emit_app_event(
            events.ActionAdded(class_name) 
            )

    def __add_commands_of_condition(self, class_name):
        self.__add_command(class_name, 'condition', 'b_evaluate_o')
        app.get_top_window().emit_app_event(
            events.ConditionAdded(class_name) 
            )

    def __add_commands_of_selector(self, class_name):
        self.__add_command(class_name, 'fsmselector', 's_SelectFSM_o')
        app.get_top_window().emit_app_event(
            events.FSMSelectorAdded(class_name) 
            )

    def __add_commands_of_trigger(self, class_name):
        self.__add_command(class_name, 'trigger_output', 'v_ExecuteOutput_oo')
        app.get_top_window().emit_app_event(
            events.TriggerScriptAdded(class_name) 
            )

    def on_delete_class(self, event):
        # Ask for delete confirmation
        class_name = self.tree_classes.get_selected_class_name()
        if class_name is None:
            return
        msg = "Deleting a class cannot be undone.\n\n" \
                    "Are you sure that you want to delete " \
                    "the class '%s'?" % class_name
        should_delete = cjr.warn_yes_no(
                                self, 
                                msg
                                )
        # Delete the class if the user has said they want to go ahead
        if should_delete == wx.ID_YES:
            class_group = self.tree_classes.get_current_top_class_group()
            # Forbid deleting some classes in some situations
            if class_group is not None:
                can_delete_func_name = '_CommandManagerDialog__can_delete_' + class_group
                if CommandManagerDialog.__dict__.has_key(can_delete_func_name):
                    if not CommandManagerDialog.__dict__[can_delete_func_name](self, class_name):
                        return

            # Delete the class
            servers.get_script_server().deleteclass( str(class_name) )
            if not servers.get_entity_class_server().removeclass( str(class_name) ):
                msg = "Couldn't delete the class '" + class_name + "'.\n" \
                      "Causes may be an instantiated object of that class, " \
                      "a class with children or a native class."
                cjr.show_error_message(msg)
                return
            set_nclass_dirty_flag( True )

            # Delete the class directory
            class_path = self.__get_class_path( class_name )
            if os.path.exists( class_path ):
                remove_class_dir( class_path )

            # Refresh the class tree (and indirectly the command list)
            self.tree_classes.on_delete_class()

    def __can_delete_action(self, class_name):
        usage_log = usagelog.find_behaviour_action( class_name )
        if len( usage_log[1] ) > 0:
            dlg = usagelog.DeleteErrorDialog(
                app.get_top_window(),
                "behaviour action '" + class_name + "'",
                usage_log
                )
            dlg.Show()
            return False

        # Save FSMs to avoid inconsistences between memory and persisted states
        # (and make the previous check valid)
        return self.__save_fsms('behaviour action')

    def __can_delete_condition(self, class_name):
        usage_log = usagelog.find_transition_condition( class_name )
        if len( usage_log[1] ) > 0:
            dlg = usagelog.DeleteErrorDialog(
                app.get_top_window(),
                "transition condition '" + class_name + "'",
                usage_log
                )
            dlg.Show()
            return False
        
        # Save FSMs to avoid inconsistences between memory and persisted states
        # (and make the previous check valid)
        delete = self.__save_fsms('transition condition')

        # Delete the singleton condition instance so the class can be deleted
        if delete:
            cond_path = format.append_to_path(
                                fsm.get_script_conditions_lib(), 
                                class_name 
                                )
            try:
                pynebula.delete( str(cond_path) )
            except:
                pass
        
        return delete

    def __can_delete_selector(self, class_name):
        usage_log = usagelog.find_fsm_selector( class_name )
        if len( usage_log[1] ) > 0:
            dlg = usagelog.DeleteErrorDialog(
                app.get_top_window(),
                "FSM selector '" + class_name + "'",
                usage_log
                )
            dlg.Show()
            return False

        # Save FSMs to avoid inconsistences between memory and persisted states
        # (and make the previous check valid)
        delete = self.__save_fsms('FSM selector')
        
        # Delete the singleton selector instance so the class can be deleted
        if delete:
            selector_path = format.append_to_path(
                                    fsm.get_fsm_selectors_lib(), 
                                    class_name 
                                    )
            try:
                pynebula.delete( str(selector_path) )
            except:
                pass
        return delete

    def __save_fsms(self, caption):
        msg = "Deleting a %s will save any modified " \
                    "finite state machine.\n\n" \
                    "Continue anyway?" % caption
        result = cjr.confirm_yes_no(self, msg)
        delete = (result == wx.ID_YES)
        if delete:
            # Save modified FSMs
            fsm_paths = fsm.get_fsms()
            for fsm_path in fsm_paths:
                state_machine = pynebula.lookup(fsm_path)
                if state_machine.isdirty():
                    servers.get_fsm_server().savefsm(state_machine)
                    state_machine.setdirty( False )
                    app.get_top_window().emit_app_event(
                        events.FSMSaved(fsm_path)
                        )
        return delete

    def on_new_cmd(self, event):
        # Ask for a command prototype
        dlg = newcmddlg.NewCommandDialog(self)
        result_ok = dlg.ShowModal() == wx.ID_OK
        if result_ok:
            cmd_proto = dlg.get_command_prototype()
        dlg.Destroy()

        # Create and add the new command to the selected class
        if result_ok:
            # Get the class name for the new command
            class_name = self.tree_classes.get_selected_class_name()
            if class_name is None:
                cjr.show_error_message(
                    "Cannot add commands to the selected class"
                    )
                return

            # Create an empty command for the selected class from a template
            self.__add_command( class_name, 'command', cmd_proto )

            # Add the new command to the command list
            self.list_cmds.Append( cmd_proto )
            servers.get_script_server().refreshclass( str(class_name) )

            # Open for editing the new command script
            cmd_unpath = self.__get_cmd_path(class_name, cmd_proto, True)
            win = scripteditordlg.create_window(
                        app.get_top_window(), 
                        cmd_unpath 
                        )
            win.display()

    def on_delete_cmd(self, event):
        # Ask for delete confirmation
        cmd_name = self.list_cmds.GetStringSelection()
        if cmd_name == "":
            return
        msg = "Deleting a command cannot be undone.\n\n" \
                    "Are you sure that you want to delete the " \
                    "command '%s'?" % cmd_name
        should_delete = cjr.warn_yes_no(self, msg)
        # Delete the command if the user has confirmed they want to
        if should_delete == wx.ID_YES:
            # Delete the script file
            class_name = self.tree_classes.get_selected_class_name()
            cmd_path = self.__get_cmd_path(class_name, cmd_name)
            os.remove( cmd_path )

            # Delete the class directory if there's no commmands left
            class_path = self.__get_class_path(class_name)
            if len( filedlg.get_file_list( class_path, ['lua'] ) ) == 0:
                try:
                    os.rmdir( class_path )
                except:
                    cjr.show_error_message(
                        "Unable to delete the class commands directory '" + class_path + "'.\n" \
                        "Maybe is it not empty or is another application using it?\n" \
                        "Please, remove it manually."
                        )

            # Delete the command from the nclass in memory
            servers.get_script_server().refreshclass( str(class_name) )

            # Remove the command from the commands list
            self.list_cmds.Delete( self.list_cmds.GetSelection() )
            self.__update_buttons()

    def on_edit_cmd(self, event):
        # If the item has a postfix as data use its specific edit command
        # function (do nothing if function not available), otherwise use the
        # generic one
        item = self.tree_classes.GetSelection()
        if not item.IsOk():
            return
        postfix = self.tree_classes.GetPyData(item)
        if postfix is not None:
            get_cmds_func_name = '_CommandManagerDialog__edit_command_of_' + postfix
            if CommandManagerDialog.__dict__.has_key(get_cmds_func_name):
                CommandManagerDialog.__dict__[get_cmds_func_name](
                    self,
                    item 
                    )
        else:
            self.__edit_command_of_class(
                self.tree_classes.GetItemText(item),
                self.list_cmds.GetStringSelection() 
                )

    def __edit_command_of_class(self, class_name, cmd_name):
        cmd_unpath = self.__get_cmd_path( class_name, cmd_name, True )
        win = scripteditordlg.create_window(
                    app.get_top_window(), 
                    cmd_unpath 
                    )
        win.display()

    def persist(self):
        data = {'class path': self.tree_classes.get_selected_class_path()}
        return [
            create_window, 
            (), # no parameters for create function
            data
            ]

    def restore(self, data_list):
        data = data_list[0]

        # Expand the class path
        if data.has_key('class path'):
            self.tree_classes.expand_class_path( data['class path'] )


# create_window function
def create_window(parent):
    return CommandManagerDialog(parent)
