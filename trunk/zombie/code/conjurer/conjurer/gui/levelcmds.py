##\file levelcmds.py
##\brief Level menu commands

import os
import wx

import pynebula

import app
import cmdmgrdlg
import dirdlg
import filedlg
import format
import menu
import nebulaguisettings as cfg
import preloaddlg
import servers
import trn
import waitdlg
import conjurerframework as cjr


# restore function
def restore():
    apply_level_preset()

# get_level_preset_name function
def get_level_preset_name():
    # Get the preset name from the current level's configuration file
    level_name = app.get_level().getname()
    level_path = format.append_to_path(
                        get_presets_path(), 
                        "levels/%s.txt" % level_name
                        )
    if os.path.exists(level_path):
        # The level has a configuration file: get the preset name from it
        level_file = open(level_path, 'r')
        preset_name = level_file.read()
        level_file.close()
    else:
        # The level doesn't have a configuration file: use the default preset
        preset_name = "default"
    return preset_name

# get_level_preset_path function
def get_level_preset_path():
    return format.append_to_path(
        get_presets_path(), 
        "lib/%s" % get_level_preset_name() 
        )

def  get_name_of_current_level():
    return get_current_level().getname()

def get_current_level():
    level_manager = servers.get_level_manager()
    return level_manager.getcurrentlevelobject()

def get_presets_path():
    return cfg.Repository.getsettingvalue(cfg.ID_PresetsPath)

# apply_preset function
def apply_preset(preset_name, preset_path):
    """Load and apply the specified preset"""
    # Validate preset path
    if not os.path.exists(preset_path):
        if preset_name != "default":
            level_name = get_name_of_current_level()
            msg = "Unable to find the '%s' preset attached to " \
                        "the '%s' level" % (preset_name, level_name) 
            cjr.show_error_message(msg)
        return

    # Load stdlight
    stdlight_path = format.append_to_path(preset_path, "stdlight.n2") 
    if not os.path.exists(stdlight_path):
        cjr.show_error_message(
            "Unable to find the Stdlight state for the '%s' preset"\
            % preset_name
            )
    else:
        entity_id = app.get_level().findentity('stdlight')
        if entity_id == 0:
            cjr.show_error_message("'stdlight' not found")
        else:
            entity = servers.get_entity_object_server().getentityobject(
                            entity_id 
                            )
            entity.loadstate( str(stdlight_path) )

    # Viewport UI
    viewport_ui_path = format.append_to_path( preset_path, "viewportui.n2" )
    if not os.path.exists( viewport_ui_path ):
        cjr.show_error_message(
            "Unable to find the Viewport UI state for the '%s' preset"\
            % preset_name
            )
    else:
        viewport_ui = app.get_viewports_dir()
        viewport_ui.loadstate( str(viewport_ui_path) )

    # Viewports
    for index in range(4):
        viewport_name = "viewport%s.n2" % index
        viewport_path = format.append_to_path( preset_path, viewport_name )
        if not os.path.exists( viewport_path ):
            cjr.show_error_message(
                "Unable to find the Viewport %s state for the '%s' preset" \
                % ( index, preset_name )
                )
        else:
            viewport = pynebula.lookup(
                                format.append_to_path(
                                    app.get_viewports_dir().getfullname(), 
                                    viewport_name[:-3]
                                    )
                                )
            viewport.loadstate( str(viewport_path) )
    # Special viewport rnsview
    viewport_path = format.append_to_path(preset_path, "rnsview.n2")
    if os.path.exists( viewport_path ):
        viewport = pynebula.lookup( '/usr/rnsview' )
        viewport.loadstate( str(viewport_path) )

    # Camera bookmarks
    bookmark_path = format.append_to_path(preset_path, "bookmarks.n2")
    if os.path.exists( bookmark_path ):
        servers.get_conjurer().loadbookmarks(bookmark_path)

    # Mouse settings (sensitivity, invert y-axis, etc)
    mouse_setting_path = format.append_to_path(preset_path, "mousesettings.n2")
    if os.path.exists( mouse_setting_path ):
        game_state = app.get_state("game")
        game_state.loadmousesettings( str(mouse_setting_path) )

# apply_level_preset function
def apply_level_preset():
    """
    Load and apply the preset attached with the current level
    
    If the level has no preset (or is invalid), the default one is used (if it exists)
    """
    preset_name = get_level_preset_name()
    preset_path = get_level_preset_path()
    apply_preset(preset_name, preset_path)

# save_preset function
def save_preset(preset_path):
    # Create preset path if it doesn't exist
    if not os.path.exists( preset_path ):
        os.makedirs( preset_path )
    # Save stdlight
    entity_id = app.get_level().findentity('stdlight')
    if entity_id == 0:
        cjr.show_error_message("'stdlight' not found")
    else:
        entity = servers.get_entity_object_server().getentityobject( entity_id )
        entity.savestateas(
            str( format.append_to_path(preset_path, "stdlight.n2") ) 
            )
    # Save viewport ui
    viewport_ui = app.get_viewports_dir()
    viewport_ui.savestateas(
        str( format.append_to_path(preset_path, "viewportui.n2") ) 
        )
    # Save viewports
    for index in range(4):
        viewport_name = "viewport%s.n2" % index
        viewport_path = format.append_to_path( preset_path, viewport_name )
        viewport = pynebula.lookup(
                            format.append_to_path(
                                app.get_viewports_dir().getfullname(), 
                                viewport_name[:-3]
                                )
                            )
        viewport.savestateas( str(viewport_path) )
    # Save special viewport rnsview
    viewport_path = format.append_to_path( preset_path, "rnsview.n2" )
    viewport = pynebula.lookup( '/usr/rnsview' )
    viewport.savestateas( str(viewport_path) )
    # Save camera bookmarks
    bookmark_path = format.append_to_path(
                                preset_path, 
                                "bookmarks.n2" 
                                )
    servers.get_conjurer().savebookmarks( bookmark_path )
    # Save mouse settings (sensitivity, invert y-axis, etc)
    mouse_setting_path = format.append_to_path(
                                    preset_path, 
                                    "mousesettings.n2" 
                                    )
    game_state = app.get_state( "game" )
    game_state.savemousesettings( str(mouse_setting_path) )

# unassign_preset function
def unassign_preset(level_name):
    # Build level configuration file path
    level_path = format.append_to_path(
                        get_presets_path(), 
                        "levels/%s.txt" % level_name
                        )
    # Delete the level configuration file, if exists
    if os.path.exists( level_path ):
        os.remove( level_path )

# prelevel_process function
def prelevel_process(is_new_level):
    # Forbid any level operation while in game mode
    if app.is_in_gameplay_mode():
        cjr.show_error_message(
            "Operation not allowed while in game mode"
            )
        return False

    # Exit from any test mode
    app.get_ai_tester().disablegameplayupdating()
    app.get_ai_tester().disablesoundsources()

    # Clear undo buffer
    servers.get_command_server().clean()

    if not is_new_level:
        return True
    # Discard deleted entities
    servers.get_entity_object_server().discarddeletedobjects()
    servers.get_entity_class_server().discarddeletedclasses()

    # What follows should be done only for new and open level commands

    # Clear selection
    app.get_object_state().resetselection()

    # Make terrain state release some buffers
    if servers.get_conjurer().getcurrentstate() == 'terrain':
        trn.get_terrain_module().setoutdoor(None)

    # Begin a persist-restore process to close those invalid windows for
    # the new level and refresh the valid ones (continue in postlevel_process)
    # Persist guiDialog windows
    frame = app.get_top_window()
    prelevel_process.child_data = []
    for child in frame.get_child_dialogs():
        data = persist_window(child)
        if data is not None:
            prelevel_process.child_data.insert(0, data)
    return True

# postlevel_process functiona
def postlevel_process(is_new_level):
    if not is_new_level:
        return
    # What follows should be done only for new and open level commands

    # Load the level preset
    apply_level_preset()

    # Set the new outdoor to the terrain state
    if servers.get_conjurer().getcurrentstate() == 'terrain':
        trn.get_terrain_module().setoutdoor(
            app.get_outdoor_obj() 
            )

    # End the persist-restore process (continue from prelevel_process)
    # Close guiDialog windows
    frame = app.get_top_window()
    for child in frame.get_child_dialogs():
        child.Close()
    # Refresh other GUI features
    frame.refresh_all()
    # Restore persisted guiDialog windows
    for data in prelevel_process.child_data:
        restore_window(data)
    prelevel_process.child_data = []

# persist_window function
def persist_window(win):
    # Store window values and state
    try:
        win_list = win.persist()
    except:
        return None
    if len(win_list) > 0:
        data = {
            'parent': win.GetParent(),
            'data': win_list,
            'size': win.GetSize(),
            'position': win.GetPosition(),
            'iconized': win.IsIconized(),
            'maximized': win.IsMaximized()
            }
        return data
    else:
        return None

# restore_window function
def restore_window(data):
    if data is None:
        return
    # Create window
    frame = data['parent']
    list_persisted = data['data']
    create_function = list_persisted[0]
    create_parameters = list_persisted[1]
    child_window = create_function(
                frame,
                *create_parameters
                )
    # Restore window state
    if data['iconized']:
        child_window.Iconize(True)
    elif data['maximized']:
        child_window.Maximize(True)
    else:
        offset = wx.Point( 0, frame.get_toolbar().GetSize().y * 2 )
        child_window.SetPosition( data['position'] - offset )
        child_window.SetSize( data['size'] )
    # Restore window values if possible
    if len(list_persisted) > 2:
        data_list = list_persisted[2:]
        if child_window.is_restorable(data_list):
            child_window.restore(data_list)
        else:
            child_window.Destroy()
            child_window = None
    # Register open toggable windows
    if child_window is not None:
        frame.get_togwinmgr().set_window(
            create_function,
            child_window 
            )

# Level menu IDs
ID_New = wx.NewId()
ID_Open = wx.NewId()
ID_Save = wx.NewId()
ID_SaveAs = wx.NewId()
ID_Delete = wx.NewId()
ID_PreloadManager = wx.NewId()
ID_SaveClasses = wx.NewId()
ID_SaveDefaultPreset = wx.NewId()
ID_SavePreset = wx.NewId()
ID_DeletePreset = wx.NewId()
ID_AssignPreset = wx.NewId()
ID_ApplyPreset = wx.NewId()


# LevelMenu class
class LevelMenu(menu.Menu):
    """Err... The level menu"""
    
    def __init__(self, frame):
        menu.Menu.__init__(self, frame)

    def create(self):
        # menu items
        self.Append( ID_New, "&New level..." )
        self.Append( ID_Open, "&Open level..." )
        self.Append( ID_Save, "&Save level" )
        self.Append( ID_SaveAs, "Sa&ve level as..." )
        self.Append( ID_Delete, "&Delete level..." )
        self.AppendSeparator()
        self.Append(
            ID_PreloadManager, 
            "Preload manager...",
            "Specify the resources to preload for the current level" 
            )
        self.Append(
            ID_SaveClasses, 
            "Save entity classes" 
            )
        self.AppendSeparator()
        self.Append(
            ID_SaveDefaultPreset, 
            "Save preset as default",
            "Save the current stdlight and viewports state as the default " \
            "one used by any level without an assigned preset" 
            )
        self.Append(
            ID_SavePreset, 
            "Save preset as...",
            "Save to disk the current stdlight and viewports state" 
            )
        self.Append(
            ID_DeletePreset, 
            "Delete preset...",
            "Delete one stored preset" 
            )
        self.Append( 
            ID_AssignPreset, 
            "Assign preset to level...",
            "Attach a stored preset to the current level (it won't be " \
            "applied until the next time the level is loaded)" 
            )
        self.Append(
            ID_ApplyPreset, 
            "Apply preset to level...",
            "Load and apply a stored preset (this doesn't change which " \
            "preset is assigned to the current level)" 
            )

        # bindings
        self.bind_function(ID_New, self.on_new_level)
        self.bind_function(ID_Open, self.on_open_level)
        self.bind_function(ID_Save, self.on_save_level)
        self.bind_function(ID_SaveAs, self.on_save_level_as)
        self.bind_function(ID_Delete, self.on_delete_level)
        self.bind_function(ID_PreloadManager, self.on_preload_manager)
        self.bind_function(ID_SaveClasses, self.on_save_classes)
        self.bind_function(ID_SaveDefaultPreset, self.on_save_default_preset)
        self.bind_function(ID_SavePreset, self.on_save_preset)
        self.bind_function(ID_DeletePreset, self.on_delete_preset)
        self.bind_function(ID_AssignPreset, self.on_assign_preset)
        self.bind_function(ID_ApplyPreset, self.on_apply_preset)

    def refresh(self):
        # Disable Save  and Save As options if the current level is 
        # the default, otherwise enable them
        level_name = get_name_of_current_level()
        should_enable = "default" != level_name
        self.Enable(ID_Save, should_enable)
        self.Enable(ID_SaveAs, should_enable)

        # Version number        
        info = servers.get_conjurer().getreleasesubversioninfo()
        version = info[1]

        # Update both InGUI and OutGUI titles to show current level
        title_name = " - Level %s.n2" % level_name 
        self.get_frame().SetTitle(
            "Conjurer%s" % title_name 
            )
        servers.get_conjurer().setwindowtitle(
            str(
                "Summoner%s - %s" % ( title_name, version )
                )
            )

    def on_new_level(self, event):
        """Create a new level replacing the old one, if user confirms"""
        # Ask for the level's name
        dlg = filedlg.FileDialog(
                    self.get_frame(), 
                    filedlg.NEW,
                    'level', 
                    'Level', 
                    "wc:levels", 
                    ["n2"]
                    )
        if dlg.ShowModal() == wx.ID_OK:
            # Check that's a new filename
            if dlg.file_exists():
                msg = "There is already a level called '%s'.\n\n" \
                            "Please enter a new level name." % dlg.get_guiname()
                cjr.show_error_message(msg)
            else:
                # Ask for final confirmation
                msg = "Any previous unsaved level data will be lost.\n\n" \
                        "Are you sure you want to create the new level '%s'?"\
                        % dlg.get_guiname()
                result = cjr.warn_yes_no(
                                self.get_frame(), 
                                msg
                                )
                if result == wx.ID_YES:
                    # Finally create the new level
                    if prelevel_process(True):
                        servers.get_conjurer().newlevel(
                            dlg.get_path_as_string()
                            )
                        postlevel_process(True)
        dlg.Destroy()

    def on_open_level(self, event):
        """Show a file browser and open the selected level"""
        # Ask for the level's name
        dlg = filedlg.FileDialog(
                    self.get_frame(), 
                    filedlg.OPEN,
                    'level', 
                    'Level', 
                    "wc:levels", 
                    ["n2"], 
                    import_button=False
                    )
        if dlg.ShowModal() == wx.ID_OK:
            # Check that the filename exists
            level_name = dlg.get_guiname()
            if not dlg.file_exists():
                msg = "So you want to open the '%s' level that doesn't exist?" \
                            "\n<sigh> How curious these humans are..." \
                            % level_name
                cjr.show_error_message(msg)
            else:
                # Ask for final confirmation
                msg = "Any previous unsaved level data will be lost.\n\n" \
                            "Are you sure that you want to load the "\
                            "'%s' level?" % level_name
                result = cjr.warn_yes_no( self.get_frame(), msg )
                if result == wx.ID_YES:
                    # Finally load the level
                    try:
                        try:
                            dlg3 = waitdlg.WaitDialog(
                                        self.get_frame(),
                                        "Loading level %s..." % level_name
                                        )
                            if prelevel_process(True):
                                servers.get_conjurer().loadlevel(
                                                    dlg.get_path_as_string()
                                                )
                                postlevel_process(True)
                        finally:
                            dlg3.Destroy()
                    except:
                        # make sure any errors are not hidden
                        raise
        dlg.Destroy()

    def on_save_level_as(self, event):
        """ Save the current level under a new name. """
        # Ask for a name for the new level
        dlg = filedlg.FileDialog(
                    self.get_frame(), 
                    filedlg.SAVE,
                    'level', 
                    'Level',
                    "wc:levels", 
                    ["n2"]
                    )
        if dlg.ShowModal() == wx.ID_OK:
            new_level_name = dlg.get_guiname()
            # Check the name hasn't already been taken
            if dlg.file_exists():
                msg = "There is already a level called '%s'.\n\n" \
                            "Please enter a new level name." % new_level_name
                cjr.show_error_message(msg)
            else:
                try:
                    try:
                        new_level_path = dlg.get_path_as_string()
                        dlg2 = waitdlg.WaitDialog(
                                    self.get_frame(),
                                    "Saving level as '%s'..." 
                                        % new_level_name
                                    )
                        save_ok = servers.get_conjurer().savecurrentlevelas(
                                            new_level_path
                                            )
                        if save_ok:
                            self.refresh()
                        else:
                            cjr.show_error_message(
                                "Unable to save level as '%s'." 
                                    % new_level_name
                                )
                    finally:
                        dlg2.Destroy()
                except:
                    raise # make sure any errors are not hidden
        dlg.Destroy()

    def on_save_level(self, event):
        """Save the current level, overwritting the old file"""
        level_name = get_name_of_current_level()
        if level_name == "default":
            cjr.show_error_message(
                "The default level cannot be saved."
                )
        else:
            try:
                try:
                    dlg = waitdlg.WaitDialog(
                                self.get_frame(),
                                "Saving '%s' level..." 
                                    % level_name
                                )
                    if prelevel_process(False):
                        servers.get_conjurer().savelevel()
                        postlevel_process(False)
                finally:
                    dlg.Destroy()
            except:
                raise # make sure any errors are not hidden

    def on_delete_level(self, event):
        """Show a file browser and delete the selected level"""
        # Ask for the level's name
        current_level_name = get_name_of_current_level()
        dlg = filedlg.FileDialog(
                    self.get_frame(), 
                    filedlg.DELETE,
                    'level', 
                    'Level', 
                    "wc:levels", 
                    ["n2"],
                    excluded_files=['default.n2', '%s.n2' % current_level_name]
                    )
        result_ok = dlg.ShowModal() == wx.ID_OK
        level_name = dlg.get_guiname()
        level_path = dlg.get_path_as_string()
        dlg.Destroy()
        if not result_ok:
            return

        # Ask for confirmation
        msg = "All your hard work is going to be removed, deleted, " \
            "cleared, lost forever (ok, you can still revert\n" \
            "your working copy, but your local level is going to be " \
            "erased for sure).\n\nSo, are you sure that you want to " \
            "delete the '%s' level?" % level_name
        result = cjr.warn_yes_no(
                    self.get_frame(), 
                    msg
                    )
        if result != wx.ID_YES:
            return

        # Ask for final confirmation
        msg = "If you have clicked OK because of a tick in your finger, this " \
            "is your last chance to avoid throwing away the whole level.\n\n" \
            "Again, and for last time, are you ABSOLUTELY sure that you " \
            "want to delete the '%s' level?" % level_name
        result = cjr.warn_yes_no(
                        self.get_frame(), 
                        msg
                        )
        if result != wx.ID_YES:
            return

        # Finally delete the level (and the presets configuration)
        try:
            try:
                dlg = waitdlg.WaitDialog(
                            self.get_frame(),
                            "Deleting level %s..." % level_name
                            )
                unassign_preset( level_name )
                servers.get_conjurer().deletelevel(level_path)
            finally:
                dlg.Destroy()
        except:
            # make sure any errors are not hidden
            raise

    def on_preload_manager(self, event):
        """Show the preload manager"""
        dlg = preloaddlg.PreloadDialog( self.get_frame() )
        dlg.ShowModal()
        dlg.Destroy()

    def on_save_classes(self, event):
        """Save the entity classes"""
        servers.get_entity_class_server().saveentityclasses()
        cmdmgrdlg.set_nclass_dirty_flag(False)

    def __prepresets_process(self):
        # Ask for presets directory creation if it doesn't exist yet
        path = get_presets_path()
        if not os.path.exists( path ):
            msg = "The presets directory '%s' doesn't exist.\n" \
                        "A presets directory is needed to continue.\n\n" \
                        "Do you want to create it?" % path
            result = cjr.confirm_yes_no(
                            self.get_frame(),
                            msg
                            )
            if result == wx.ID_YES:
                os.makedirs( path )
        # Create subpaths if doesn't exist yet
        lib_path = format.append_to_path( path, "lib" )
        levels_path = format.append_to_path( path, "levels" )
        if os.path.exists( path ):
            # Create subpaths only if root path has been created
            if not os.path.exists( lib_path ):
                os.mkdir( lib_path )
            if not os.path.exists( levels_path ):
                os.mkdir( levels_path )
        # Return true only if all the presets subpaths finally exists
        return os.path.exists( lib_path ) and os.path.exists( levels_path )

    def __delete_preset(self, preset_path):
        # Remove stdlight state
        stdlight_path = format.append_to_path( preset_path, "stdlight.n2" )
        if os.path.exists( stdlight_path ):
            os.remove( stdlight_path )
        # Remove viewport ui state
        viewport_ui_path = format.append_to_path( preset_path, "viewportui.n2" )
        if os.path.exists( viewport_ui_path ):
            os.remove( viewport_ui_path )
        # Remove viewports
        for index in range(4):
            viewport_name = "viewport%s.n2" % index
            viewport_path = format.append_to_path( preset_path, viewport_name )
            if os.path.exists( viewport_path ):
                os.remove( viewport_path )
        # Remove preset directory
        try:
            os.rmdir( preset_path )
        except:
            cjr.show_error_message(
                "Unable to delete the preset directory '%s'.\n" \
                "It may not be empty or another application may be using it." \
                "\n\nPlease remove it manually to delete " \
                "the preset." % preset_path
                )
        # Remove bookmarks, if present
        bookmark_path = format.append_to_path(
                                    preset_path, 
                                    "bookmarks.n2" 
                                    )
        if os.path.exists( bookmark_path ):
            os.remove( bookmark_path )
        # Remove mouse settings, if present
        mouse_setting_path = format.append_to_path(
                                            preset_path, 
                                            "mousesettings.n2" 
                                            )
        if os.path.exists( mouse_setting_path ):
            os.remove( mouse_setting_path )

    def __assign_preset(self, preset_name):
        # Build level configuration file path
        level_path = format.append_to_path(
                            get_presets_path(), 
                            "levels/%s.txt" % app.get_level().getname()
                            )
        # Write the preset name to the level configuration file
        level_file = open( level_path, 'w' )
        level_file.write( preset_name )
        level_file.close()

    def on_save_default_preset(self, event):
        """Save the current preset as the default one"""
        # Ask for the preset's name
        if self.__prepresets_process():
            path = format.append_to_path(
                        get_presets_path(),
                        "lib/default" )
            save_preset( path )

    def on_save_preset(self, event):
        """Save the current preset with the name chosen by the user"""
        # Ask for the preset's name
        if self.__prepresets_process():
            path = format.append_to_path(
                        get_presets_path(), 
                        "lib" 
                        )
            dlg = dirdlg.DirDialog(
                        self.get_frame(), 
                        dirdlg.SAVE,
                        'preset', 
                        'Preset', 
                        path
                        )
            if dlg.ShowModal() == wx.ID_OK:
                if dlg.dir_exists():
                    msg = "Overwrite the '%s' preset?" % dlg.get_dirname()
                    result = cjr.confirm_yes_no(
                                    self.get_frame(), 
                                    msg
                                    )
                    if result == wx.ID_YES:
                        save_preset( dlg.get_path() )
                else:
                    save_preset( dlg.get_path() )
            dlg.Destroy()

    def on_delete_preset(self, event):
        """Delete a preset"""
        # Ask for the preset to delete
        if self.__prepresets_process():
            path = format.append_to_path(
                        get_presets_path(), 
                        "lib" 
                        )
            dlg = dirdlg.DirDialog( 
                        self.get_frame(), 
                        dirdlg.DELETE,
                        'preset', 
                        'Preset', 
                        path 
                        )
            if dlg.ShowModal() == wx.ID_OK:
                self.__delete_preset( dlg.get_path() )
            dlg.Destroy()

    def on_assign_preset(self, event):
        """Assign the choosen preset to the current level"""
        # Ask for the preset to assign
        if self.__prepresets_process():
            path = format.append_to_path(
                        get_presets_path(), 
                        "lib" 
                        )
            dlg = dirdlg.DirDialog(
                        self.get_frame(), 
                        dirdlg.SELECT,
                        'preset', 
                        'Preset',
                        path 
                        )
            if dlg.ShowModal() == wx.ID_OK:
                self.__assign_preset( dlg.get_dirname() )
            dlg.Destroy()

    def on_apply_preset(self, event):
        """Apply the choosen preset onto the current level"""
        # Ask for the preset to apply
        if self.__prepresets_process():
            path = format.append_to_path(
                        get_presets_path(), 
                        "lib" 
                        )
            dlg = dirdlg.DirDialog(
                        self.get_frame(), 
                        dirdlg.OPEN,
                        'preset', 
                        'Preset', 
                        path 
                        )
            if dlg.ShowModal() == wx.ID_OK:
                apply_preset( dlg.get_dirname(), dlg.get_path() )
            dlg.Destroy()
