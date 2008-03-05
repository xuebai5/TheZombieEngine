##\file fsmlibs.py
##\brief Dialogs to manage FSM related libraries

import wx

import pynebula

import app
import childdialoggui
import events
import format
import fsm
import fsmeditor
import fsmstates
import fsmtransitions
import scripteditordlg
import servers
import togwin

import conjurerframework as cjr

# LibraryDialog class
class LibraryDialog(togwin.ChildToggableDialog):
    """Dialog to manage some library"""
    
    def __init__(self, parent, lib_path, class_name, caption):
        title = caption[:1].upper() + caption[1:] + "s"
        togwin.ChildToggableDialog.__init__(
            self, title, parent
            )
        self.lib_path = lib_path
        self.class_name = class_name
        self.caption = caption
        
        self.label = wx.StaticText(self, -1, title + ":")
        self.list = wx.ListBox(self, -1, choices=[], style=wx.LB_SORT)
        self.button_new = wx.Button(self, -1, "New")
        self.button_dup = wx.Button(self, -1, "Duplicate")
        self.button_del = wx.Button(self, -1, "Delete")
        self.button_edit = wx.Button(self, -1, "Edit")
        
        self.button_dup.Disable()
        
        self.__set_properties()
        self.__do_layout()
        self.__bind_events()
    
    def __set_properties(self):
        lib = pynebula.lookup( self.lib_path )
        obj = lib.gethead()
        while obj is not None:
            self.list.Append( obj.getname() )
            obj = obj.getsucc()
    
    def __do_layout(self):
        sizer_border = wx.BoxSizer(wx.HORIZONTAL)
        sizer_layout = wx.BoxSizer(wx.VERTICAL)
        sizer_buttons = wx.BoxSizer(wx.HORIZONTAL)
        sizer_layout.Add(self.label, 0, wx.FIXED_MINSIZE, 0)
        sizer_layout.Add(self.list, 1, wx.TOP|wx.EXPAND, 5)
        sizer_buttons.Add(self.button_new, 1, wx.FIXED_MINSIZE, 0)
        sizer_buttons.Add(self.button_dup, 1, wx.LEFT|wx.FIXED_MINSIZE, 5)
        sizer_buttons.Add(self.button_del, 1, wx.LEFT|wx.FIXED_MINSIZE, 5)
        sizer_buttons.Add(self.button_edit, 1, wx.LEFT|wx.FIXED_MINSIZE, 5)
        sizer_layout.Add(sizer_buttons, 0, wx.TOP|wx.EXPAND, 5)
        sizer_border.Add(sizer_layout, 1, wx.ALL|wx.EXPAND, 10)
        self.SetAutoLayout(True)
        self.SetSizer(sizer_border)
        sizer_border.Fit(self)
        sizer_border.SetSizeHints(self)
        self.Layout()
    
    def __bind_events(self):
        self.Bind(wx.EVT_BUTTON, self.on_new, self.button_new)
        self.Bind(wx.EVT_BUTTON, self.on_duplicate, self.button_dup)
        self.Bind(wx.EVT_BUTTON, self.on_delete, self.button_del)
        self.Bind(wx.EVT_BUTTON, self.on_edit, self.button_edit)
        self.Bind(wx.EVT_LISTBOX_DCLICK, self.on_edit, self.list)
    
    def __refresh_editors(self):
        # Refresh opened FSM editors
        children = self.GetParent().GetChildren()
        for child in children:
            if isinstance( child, fsmeditor.FSMEditorDialog ):
                child.quiet_refresh()
    
    def _get_editor(self, obj_path):
        children = self.GetParent().GetChildren()
        for child in children:
            if self.is_editor( child, obj_path ):
                return child
        return None
    
    def on_new(self, event):
        """Ask for a name and create a new object it if it's a non existing name"""
        dlg = wx.TextEntryDialog( None, "Enter the " + self.caption + "'s name:", "New " + self.caption )
        if dlg.ShowModal() == wx.ID_OK:
            # Add the object only if there isn't another object with the same name yet
            obj_path = format.append_to_path( self.lib_path, dlg.GetValue() )
            try:
                # Lookup will fail if it's a new name (name not found)
                pynebula.lookup( obj_path )
                msg = "Another " + self.caption + " named '" + dlg.GetValue() + "' already exists."
                cjr.show_error_message(msg)
            except:
                # Create object, save it to disk and add it to list
                obj = pynebula.new( str(self.class_name), str(obj_path) )
                self.save_object( obj )
                self.list.Append( dlg.GetValue() )
        dlg.Destroy()
    
    def on_duplicate(self, event):
        """Ask for a name and clone the selected object it if it's a non existing name"""
        source_name = self.list.GetStringSelection()
        if source_name != "":
            dlg = wx.TextEntryDialog(
                None,
                "Enter the " + self.caption + "'s name:",
                "Duplicate " + self.caption,
                source_name
                )
            if dlg.ShowModal() == wx.ID_OK:
                # Clone the object only if there isn't another object with the same name yet
                target_name = dlg.GetValue()
                target_path = format.append_to_path( self.lib_path, target_name )
                try:
                    # Lookup will fail if it's a new name (name not found)
                    pynebula.lookup( target_path )
                    msg = "Another " + self.caption + " named '" + dlg.GetValue() + "' already exists."
                    cjr.show_error_message(msg)
                except:
                    # Clone object, save it to disk and add it to list
                    source_path = format.append_to_path( self.lib_path, source_name )
                    source_obj = pynebula.lookup( source_path )
                    target_obj = source_obj.clone( str(target_path) ) 
                    self.save_object( target_obj )
                    self.list.Append( target_name )
            dlg.Destroy()
    
    def save_object(self, object):
        """Override to save the object and any other stuff it needs to disk"""
        pass
    
    def on_delete(self, event):
        """Ask to confirm the object deletion and delete it if so"""
        name = self.list.GetStringSelection()
        if name != "":
            msg = "Deleting a %s cannot be undone.\n\n" \
                        "Are you sure you want to delete " \
                        "the '%s' %s?" % ( (self.caption, name, self.caption) )
            delete = cjr.warn_yes_no(self, msg)
            if delete == wx.ID_YES:
                obj_path = format.append_to_path( self.lib_path, name )
                if self.erase_object( pynebula.lookup(obj_path) ):
                    # Close the editor for this object if it's opened
                    editor = self._get_editor( obj_path )
                    if editor != None:
                        editor.Close()
                    # Delete the object
                    pynebula.delete( str(obj_path) )
                    self.list.Delete( self.list.GetSelection() )
                    self.__refresh_editors()
    
    def erase_object(self, object):
        """Override to erase the object and any other stuff it needs from disk"""
        return True
    
    def on_edit(self, event):
        """Open the object editor for the selected object"""
        name = self.list.GetStringSelection()
        if name != "":
            obj_path = format.append_to_path( self.lib_path, name )
            self.open_editor( obj_path )
    
    def is_editor(self, win, obj_name):
        """Override to tell if the a window is the editor for an object"""
        return False
    
    def open_editor(self, obj_path):
        """Override to open an editor to edit the object"""
        pass


# FSMLibraryDialog
class FSMLibraryDialog(LibraryDialog):
    """FSM library dialog"""
    
    def __init__(self, parent):
        LibraryDialog.__init__(self, parent, fsm.get_fsms_lib(), 'nfsm',
            "finite state machine")
    
    def on_app_event(self, event):
        if isinstance( event, events.FSMNameChanged ):
            i = self.list.FindString( event.get_old_name() )
            self.list.SetString( i, event.get_name() )
    
    def save_object(self, fsm):
        servers.get_fsm_server().savefsm( fsm )
    
    def erase_object(self, fsm):
        servers.get_fsm_server().erasefsm( fsm )
        return True
    
    def is_editor(self, win, obj_path):
        if isinstance (win, fsmeditor.FSMEditorDialog ):
            return win.get_fsm().getfullname() == obj_path
        return False

    def open_editor(self, fsm_path):
        win = fsmeditor.create_window(
                    app.get_top_window(), 
                    fsm_path
                    )
        win.display()

    def persist(self):
        return [
            create_fsm_lib_window,
            ()  # no parameters for create function
            ]


# ScriptsLibraryDialog
class ScriptsLibraryDialog(LibraryDialog):
    """Base class for all scripts libraries dialogs"""
    
    def __init__(self, parent, noh_lib_path, disk_lib_path, class_name, caption):
        LibraryDialog.__init__(self, parent, noh_lib_path, class_name, caption)
        self.disk_lib_path = disk_lib_path
        pass

    def __get_script_path(self, obj_path):
        script_name = format.get_name( obj_path ) + ".lua"
        script_path = format.append_to_path( self.disk_lib_path, script_name )
        return format.mangle_path( script_path )

    def _save_fsms(self):
        msg = "Deleting a %s will save any modified finite state machine.\n\n" % self.caption
        msg = msg + "Continue anyway?"
        result = cjr.confirm_yes_no(self, msg)
        delete = (result == wx.ID_YES)
        if delete:
            # Save modified FSMs
            fsm_paths = fsm.get_fsms()
            for fsm_path in fsm_paths:
                sm = pynebula.lookup( fsm_path )
                if sm.isdirty():
                    servers.get_fsm_server().savefsm( sm )
                    sm.setdirty( False )
                    app.get_top_window(self).emit_app_event(
                        events.FSMSaved( fsm_path )
                        )
        return delete

    def is_editor(self, win, obj_path):
        if isinstance( win, scripteditordlg.ScriptEditorDialog ):
            return win.get_filename() == self.__get_script_path( obj_path )
    
    def open_editor(self, obj_path):
        scripteditordlg.create_window( self.GetParent(),
            self.__get_script_path(obj_path) )
    
    def is_restorable(self, data):
        return False


# ConditionScriptsLibraryDialog
class ConditionScriptsLibraryDialog(ScriptsLibraryDialog):
    """Condition scripts library dialog"""
    
    def __init__(self, parent):
        ScriptsLibraryDialog.__init__(self, parent,
            fsm.get_script_conditions_lib(),
            fsm.get_script_conditions_lib(False),
            'nscriptcondition', "condition script")
        pass
    
    def __find_condition(self, sm, condition, log):
        # Look for the condition in all the script transitions of the FSM (and event filters)
        state_paths = fsm.get_states( sm.getfullname() )
        for state_path in state_paths:
            trans_paths = fsm.get_transitions( state_path )
            for trans_path in trans_paths:
                trans = pynebula.lookup( trans_path )
                trans_cond = trans.getcondition()
                if condition == trans_cond:
                    log.append( [
                        sm.getname(),
                        fsmstates.get_state_gui_name( state_path ),
                        fsmtransitions.get_transition_gui_name( trans_path )
                        ] )
                elif trans_cond.isa('neventcondition'):
                    if condition == trans_cond.getfiltercondition():
                        log.append( [
                            sm.getname(),
                            fsmstates.get_state_gui_name( state_path ),
                            fsmtransitions.get_transition_gui_name( trans_path )
                            ] )
    
    def save_object(self, condition):
        servers.get_fsm_server().createconditionscript( condition.getname() )
        servers.get_fsm_server().savescriptcondition( condition )
    
    def erase_object(self, condition):
        # Forbid deleting the script if some FSM references it
        log = []
        fsm_paths = fsm.get_fsms()
        for fsm_path in fsm_paths:
            self.__find_condition( pynebula.lookup(fsm_path), condition, log )
        if len(log) > 0:
            dlg = DeleteErrorDialog(
                self.GetParent(),
                "condition script '" + condition.getname() + "'",
                ['FSM', 'State', 'Transition'],
                log
                )
            dlg.Show()
            return False
        # Save FSMs to avoid inconsistences between memory and persisted states
        # (and make the previous check valid)
        if not self._save_fsms():
            return
        # Delete the script
        servers.get_fsm_server().erasescriptcondition( condition )
        return True
    
    def persist(self):
        return [create_condition_scripts_lib_window]


# FSMSelectorScriptsLibraryDialog
class FSMSelectorScriptsLibraryDialog(ScriptsLibraryDialog):
    """FSM selector scripts library dialog"""
    
    def __init__(self, parent):
        ScriptsLibraryDialog.__init__(self, parent,
            fsm.get_fsm_selectors_lib(), fsm.get_fsm_selectors_lib(False),
            'nfsmselector', "FSM selection script")
        pass
    
    def __find_selector(self, sm, selector, log):
        # Look for the FSM selector in all the state nodes of the FSM
        state_paths = fsm.get_states( sm.getfullname() )
        for state_path in state_paths:
            state = pynebula.lookup( state_path )
            if state.isa('nnodestate'):
                if state.getfsmselector() == selector:
                    log.append( [
                        sm.getname(),
                        fsmstates.get_state_gui_name( state_path )
                        ] )
    
    def save_object(self, selector):
        servers.get_fsm_server().createfsmselectorscript( selector.getname() )
        servers.get_fsm_server().savefsmselector( selector )
    
    def erase_object(self, selector):
        # Forbid deleting the script if some FSM references it
        log = []
        fsm_paths = fsm.get_fsms()
        for fsm_path in fsm_paths:
            self.__find_selector( pynebula.lookup(fsm_path), selector, log )
        if len(log) > 0:
            dlg = DeleteErrorDialog(
                self.GetParent(),
                "FSM selection script '" + selector.getname() + "'",
                ['FSM', 'State'],
                log
                )
            dlg.Show()
            return False
        # Save FSMs to avoid inconsistences between memory and persisted states
        # (and make the previous check valid)
        if not self._save_fsms():
            return
        # Delete the script
        servers.get_fsm_server().erasefsmselector( selector )
        return True
    
    def persist(self):
        return [create_fsm_selector_scripts_lib_window]


# ActionScriptsLibraryDialog
class ActionScriptsLibraryDialog(ScriptsLibraryDialog):
    """Action scripts library dialog"""
    
    def __init__(self, parent):
        ScriptsLibraryDialog.__init__(self, parent,
            fsm.get_action_scripts_lib(), fsm.get_action_scripts_lib(False),
            'nactionscript', "action script")
        pass
    
    def __find_action(self, sm, action_script, log):
        # Look for the action script in all the leaf nodes of the FSM
        state_paths = fsm.get_states( sm.getfullname() )
        for state_path in state_paths:
            state = pynebula.lookup( state_path )
            if state.isa('nleafstate'):
                action = state.getbehaviouralaction()
                if action.getactiontype() == 999:
                    action.rewindargs()
                    if action.getstringarg() == action_script.getname():
                        log.append( [
                            sm.getname(),
                            fsmstates.get_state_gui_name( state_path )
                            ] )
    
    def save_object(self, action_script):
        servers.get_fsm_server().createactionscript( action_script.getname() )
        servers.get_fsm_server().saveactionscript( action_script )
    
    def erase_object(self, action_script):
        # Forbid deleting the script if some FSM references it
        log = []
        fsm_paths = fsm.get_fsms()
        for fsm_path in fsm_paths:
            self.__find_action( pynebula.lookup(fsm_path), action_script, log )
        if len(log) > 0:
            dlg = DeleteErrorDialog(
                self.GetParent(),
                "action script '%s'" % action_script.getname(),
                ['FSM', 'State'],
                log
                )
            dlg.Show()
            return False
        # Save FSMs to avoid inconsistences between memory and persisted states
        # (and make the previous check valid)
        if not self._save_fsms():
            return
        # Delete the script
        servers.get_fsm_server().eraseactionscript( action_script )
        return True
    
    def persist(self):
        return [create_action_scripts_lib_window]


# DeleteErrorDialog
class DeleteErrorDialog(childdialoggui.childDialogGUI):
    """Show the objects that has caused a deletion error"""
    
    def __init__(self, parent, name, titles, log):
        childdialoggui.childDialogGUI.__init__(
            self, "Error deleting the " + name, parent
            )
        
        self.label = wx.StaticText(
                            self, 
                            -1, 
                            "Unable to delete the %s because it's still being used by:" % name
                            )
        self.list = wx.ListCtrl(self, -1, style=wx.LC_REPORT)

        for i in range( len(titles) ):
            self.list.InsertColumn(i, titles[i])
        for line in log:
            self.list.Append( line )
        
        self.__do_layout()
        
        wx.Bell()
    
    def __do_layout(self):
        sizer = wx.BoxSizer(wx.VERTICAL)
        sizer_list = wx.BoxSizer(wx.VERTICAL)
        sizer_list.Add(self.label, 0, wx.FIXED_MINSIZE, 0)
        sizer_list.Add(self.list, 1, wx.TOP|wx.EXPAND, 5)
        sizer.Add(sizer_list, 1, wx.ALL|wx.EXPAND, 10)
        self.SetAutoLayout(True)
        self.SetSizer(sizer)
        sizer.Fit(self)
        sizer.SetSizeHints(self)
        self.Layout()


# create_fsm_lib_window function
def create_fsm_lib_window(parent):
    return FSMLibraryDialog(parent)

# create_condition_scripts_lib_window function
def create_condition_scripts_lib_window(parent):
    return ConditionScriptsLibraryDialog(parent)

# create_fsm_selector_scripts_lib_window function
def create_fsm_selector_scripts_lib_window(parent):
    return FSMSelectorScriptsLibraryDialog(parent)

# create_action_scripts_lib_window function
def create_action_scripts_lib_window(parent):
    return ActionScriptsLibraryDialog(parent)
