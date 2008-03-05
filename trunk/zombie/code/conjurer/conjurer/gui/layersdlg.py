##\file layersdlg.py
##\brief Layers manager dialog

import wx
import wx.lib.buttons

import app
import servers
import pynebula
import togwin
import usagelog
import nebulaguisettings as guisettings
import expentdlg
import importentity
# import layereditdlg
# import layerobject

import conjurerconfig as cfg
import conjurerframework as cjr

CJR_NORMAL_ENTITY_TYPE = 0

# Event used to notify selection of a layer control
EVT_SELECT_TYPE = wx.NewEventType()
EVT_SELECT = wx.PyEventBinder(EVT_SELECT_TYPE, 1)

# LayerSelectedEvent class
class LayerSelectedEvent(wx.PyCommandEvent):
    def __init__(self, event_id, event_object):
        wx.PyCommandEvent.__init__(self, EVT_SELECT_TYPE, event_id)
        self.SetEventObject(event_object)


# LayerCtrl class
class LayerCtrl(wx.PyControl):
    """Control used to handle a single layer"""
    
    def __init__(self, parent, layer):
        wx.PyControl.__init__(
            self, 
            parent,
            -1, 
            style=wx.NO_BORDER
            )
        self.layer = layer
        self.is_selected = False
            
        # controls
        self.tog_lock = wx.lib.buttons.GenBitmapToggleButton(
                                self, 
                                -1, 
                                None, 
                                size=(22,22)
                                )
        self.tog_hide = wx.lib.buttons.GenBitmapToggleButton(
                                    self,
                                    -1, 
                                    None, 
                                    size=(22,22)
                                    )
        self.label_name = wx.StaticText(self, -1, "unnamed")

        self.set_properties()
        self.__do_layout()
        self.__bind_events()

    def set_properties(self):
        # images
        fileserver = servers.get_file_server()
        self.tog_lock.SetBitmapLabel(
            wx.Bitmap(
                fileserver.manglepath(
                    "outgui:images/tools/lock.bmp"
                    ), 
                wx.BITMAP_TYPE_ANY
                ) 
            )
        self.tog_lock.SetBitmapSelected(
            wx.Bitmap(
                fileserver.manglepath(
                    "outgui:images/tools/lockhighlight.bmp"
                    ), 
                wx.BITMAP_TYPE_ANY
                )
            )
        self.tog_lock.SetUseFocusIndicator(False)
        self.tog_hide.SetBitmapLabel(
            wx.Bitmap(
                fileserver.manglepath(
                    "outgui:images/tools/show.bmp"
                    ), 
                wx.BITMAP_TYPE_ANY
                )
            )
        self.tog_hide.SetBitmapSelected(
            wx.Bitmap(
                fileserver.manglepath(
                    "outgui:images/tools/hide.bmp"
                    ), 
                wx.BITMAP_TYPE_ANY
                )
            )
        self.tog_hide.SetUseFocusIndicator(False)
        
        # layer state
        self.tog_lock.SetToggle(
            self.is_layer_locked() 
            )
        self.tog_hide.SetToggle(
            not self.is_layer_active()
            )
        self.update_label_from_layer()

        self.Show()
        self.Refresh()

    def is_layer_locked(self):
        return self.layer.is_locked()

    def is_layer_active(self):
        return self.layer.is_active()

    def __do_layout(self):
        sizer = wx.BoxSizer(wx.HORIZONTAL)
        sizer.Add(  
            self.tog_lock,
            0, 
            wx.LEFT|wx.ALIGN_CENTER_VERTICAL|wx.FIXED_MINSIZE, 
            cfg.BORDER_WIDTH
            )
        sizer.Add(
            self.tog_hide, 
            0, 
            wx.ALIGN_CENTER_VERTICAL|wx.FIXED_MINSIZE 
            )
        sizer.Add(
            self.label_name,
            1,
            wx.LEFT|wx.RIGHT|wx.ALIGN_CENTER_VERTICAL|wx.FIXED_MINSIZE, 
            cfg.BORDER_WIDTH
            )
        self.SetSizerAndFit(sizer)

    def __bind_events(self):
        wx.EVT_SIZE(self, self.on_size)
        self.Bind(
            wx.EVT_LEFT_DOWN, 
            self.on_select_layer
            )
        self.label_name.Bind(
            wx.EVT_LEFT_DOWN, 
            self.on_select_layer
            )
        self.Bind(
            wx.EVT_LEFT_DCLICK, 
            self.on_edit_layer
            )
        self.label_name.Bind(
            wx.EVT_LEFT_DCLICK, 
            self.on_edit_layer
            )
        self.Bind(
            wx.EVT_BUTTON, 
            self.on_toggle_lock, 
            self.tog_lock
            )
        self.Bind(
            wx.EVT_BUTTON, 
            self.on_toggle_hide, 
            self.tog_hide
            )
        pynebula.pyBindSignal(
            self.layer.layer,
            'layerlocked',
            self, 
            'onlayerlocked', 
            0
            )

    def update_colors(self):
        if self.is_selected:
            bgcolor = wx.SystemSettings.GetColour(
                            wx.SYS_COLOUR_INACTIVECAPTION
                            )
        else:
            bgcolor = wx.SystemSettings.GetColour(
                            wx.SYS_COLOUR_BTNFACE
                            )
        self.SetBackgroundColour(bgcolor)
        self.label_name.SetBackgroundColour(bgcolor)
        self.Refresh()

    def on_size(self, event):
        self.Layout()
        event.Skip()

    def on_select_layer(self, event):
        self.select()
        self.GetEventHandler().ProcessEvent(
            LayerSelectedEvent(
                self.GetId(), 
                self 
                )
            )
        event.Skip()

    def on_edit_layer(self, event):
        self.edit_layer()
    
    def edit_layer(self):
        if self.is_layer_locked():
            cjr.show_error_message(
                "Unable to edit locked layer."
                )
        else:
            result = self.handle_edit_layer_request()
            # if result:
            #     dlg = layereditdlg.EditLayerDialog(
            #                 self,
            #                 self.layer
            #             )
            #     result = dlg.ShowModal()
            #     if result == wx.ID_OK:
            #         self.update_label_from_layer()
            #     dlg.Destroy()

    def on_toggle_lock(self, event):
        should_lock = event.GetIsDown()
        result = self.handle_lock_layer_request(should_lock)
        if not result:
            self.tog_lock.SetValue(not should_lock)
            return False
        self.update_colors()
        event.Skip()

    def handle_lock_layer_request(self, should_lock):
        # do nothing if already in the required state
        if self.layer.is_locked() == should_lock:
            return True
        if self.layer.requires_password():
            dlg = wx.PasswordEntryDialog(
                        self,
                        "Enter password for %s" % self.layer.get_name(),
                        "Password required" 
                        )
            if dlg.ShowModal() == wx.ID_OK:
                password = str( dlg.GetValue() )
                password_ok = self.layer.check_password(
                                        password
                                        )
                if not password_ok:
                    cjr.show_error_message(
                        "Password incorrect."
                        )
                    return False
            else:
                return False
        self.layer.set_locked(should_lock)
        return True

    def handle_edit_layer_request(self):
        if self.layer.requires_password():
            dlg = wx.PasswordEntryDialog(
                        self,
                        "Enter password for %s" % self.layer.get_name(),
                        "Password required" 
                        )
            if dlg.ShowModal() == wx.ID_OK:
                password = str( dlg.GetValue() )
                password_ok = self.layer.check_password(
                                        password
                                        )
                if not password_ok:
                    cjr.show_error_message(
                        "Password incorrect."
                        )
                    return False
            else:
                return False
        return True

    def show_or_hide_layer(self, flag):
        self.layer.set_active(flag)
        self.set_layer_visibility(flag)

    def set_layer_visibility(self, active):
        layer_id = self.get_layer_id()
        e_server = servers.get_entity_object_server()
        entity = e_server.getfirstentityobject()
        while entity is not None:
            if entity.hascomponent("ncSpatial") \
                and entity.hascomponent("ncEditor"):
                if entity.getlayerid() == layer_id and not entity.isdeleted():
                    if active:
                        entity.restorefromtemparray()
                    else:
                        entity.removetemporary()
            entity = e_server.getnextentityobject()

    def on_toggle_hide(self, event):
        active = not event.GetIsDown()
        self.show_or_hide_layer(active)
        event.Skip()

    def select(self):
        self.is_selected = True
        servers.get_layer_manager().selectlayer(
            self.get_layer_id()
            )
        self.update_colors()

    def on_deselect(self):
        # Don't deselect the layer in the nebula layer manager since
        # this method can be called when another layer got selected, so
        # another layer may be kept selected.
        self.is_selected = False
        self.update_colors()

    def get_layer(self):
        return servers.get_layer_manager().searchlayerbyid(
            self.get_layer_id()
            )

    def get_layer_id(self):
        return self.layer.get_id()

    def get_layer_name(self):
        return self.layer.get_name()

    def set_layer_name(self, name):
        self.layer.set_layer_name(
            str(name) 
            )
        self.update_label_from_layer()

    def update_label_from_layer(self):
        self.label_name.SetLabel(
            self.layer.get_name()
            )

    def onlayerlocked(self, flag):
        current_value = self.tog_lock.GetToggle()
        # only set the toggle if the value is different
        if current_value != flag:
            self.tog_lock.SetToggle(flag)

    def __del__(self):
        self.layer = None


# LayerManagerDialog class
class LayerManagerDialog(togwin.ChildToggableDialog):
    """Dialog to manage layers"""

    def __init__(self, parent):
        togwin.ChildToggableDialog.__init__(self, "Layers", parent)
        self.layer_selected = None

        # Make layer manager load layers state from file
        self.__load_layers_state()

        # all controls but layers
        self.scroll_layers = wx.ScrolledWindow(
                                    self, 
                                    -1, 
                                    style=wx.NO_BORDER|wx.TAB_TRAVERSAL
                                    )

        self.button_new = wx.Button(self, -1, "&New")
        self.button_delete = wx.Button(self, -1, "&Delete")
        self.button_edit = wx.Button(self, -1, "&Edit")
        self.button_hideall = wx.Button(self, -1, "&Hide all")
        self.button_showall = wx.Button(self, -1, "&Show all")
        self.button_lockall = wx.Button(self, -1, "&Lock all")
        self.button_unlockall = wx.Button(self, -1, "&Unlock all")
        self.label_filter = wx.StaticText(self, -1, "Filter")
        self.text_filter = wx.TextCtrl(self)
        self.button_select_all = wx.Button(
                                            self,
                                            -1, 
                                            "Add all entities in " \
                                                "this layer to selection"
                                            )
        self.button_move = wx.Button(
                                    self, 
                                    -1, 
                                    "Move selected entities to selected layer"
                                    )
        self.button_export = wx.Button(
                                        self, 
                                        -1, 
                                        "Export all entities in selected layer"
                                        )
        self.button_import = wx.Button(
                                        self, 
                                        -1, 
                                        "Import entities to selected layer"
                                        )
        self.lines = []
        
        # layer controls
        self.layers = []
        self.__add_all_layer_ctrls()
        
        self.__set_properties()
        self.__do_layout()
        self.__bind_events()

    def __set_properties(self):
        self.SetSize( (500, 300) )
        self.scroll_layers.SetScrollRate(10, 10)

    def __do_layout(self):
        main_sizer = wx.BoxSizer(wx.VERTICAL)
        sizer_list_and_buttons = wx.BoxSizer(wx.HORIZONTAL)
        sizer_filter = wx.BoxSizer(wx.HORIZONTAL)
        sizer_buttons = wx.BoxSizer(wx.VERTICAL)
        sizer_list_and_buttons.Add(
            self.scroll_layers, 
            1, 
            wx.EXPAND,
            cfg.BORDER_WIDTH
            )
        vertical_line = wx.StaticLine(
                                self,
                                style=wx.LI_VERTICAL
                                )    
        sizer_list_and_buttons.Add(
            vertical_line, 
            0,
            wx.EXPAND|wx.LEFT,
            cfg.BORDER_WIDTH
            )
        sizer_buttons.Add(
            self.button_new, 
            0,
            wx.EXPAND
            )
        sizer_buttons.Add(
            self.button_delete,
            0, 
            wx.EXPAND|wx.TOP,
            cfg.BORDER_WIDTH
            )
        sizer_buttons.Add(
            self.button_edit,
            0, 
            wx.EXPAND|wx.TOP,
            cfg.BORDER_WIDTH
            )
        sizer_buttons.Add(
            self.button_hideall,
            0, 
            wx.EXPAND|wx.TOP,
            cfg.BORDER_WIDTH
            )
        sizer_buttons.Add(
            self.button_showall,
            0, 
            wx.EXPAND|wx.TOP,
            cfg.BORDER_WIDTH
            )
        sizer_buttons.Add(
            self.button_lockall,
            0, 
            wx.EXPAND|wx.TOP,
            cfg.BORDER_WIDTH
            )
        sizer_buttons.Add(
            self.button_unlockall,
            0, 
            wx.EXPAND|wx.TOP,
            cfg.BORDER_WIDTH
            )
        sizer_list_and_buttons.Add(
            sizer_buttons,
            0,
            wx.LEFT|wx.FIXED_MINSIZE,
            cfg.BORDER_WIDTH
            )
        main_sizer.Add(
            sizer_list_and_buttons,
            1,
            wx.EXPAND|wx.ALL,
            cfg.BORDER_WIDTH
            )
        horizontal_line = wx.StaticLine(
                                self,
                                style=wx.LI_HORIZONTAL
                                )
        main_sizer.Add(
            horizontal_line, 
            0, 
            wx.EXPAND|wx.LEFT|wx.RIGHT,
            cfg.BORDER_WIDTH
            )
        sizer_filter.Add(
            self.label_filter,
            0, 
            wx.LEFT|wx.ALIGN_CENTER_VERTICAL|wx.FIXED_MINSIZE, 
            cfg.BORDER_WIDTH
            )
        sizer_filter.Add(
            self.text_filter, 
            1,
            wx.ALL|wx.FIXED_MINSIZE|wx.ALIGN_CENTER_VERTICAL,
            cfg.BORDER_WIDTH
            )
        main_sizer.Add(
            sizer_filter, 
            0, 
            wx.EXPAND
            )
        horizontal_line2 = wx.StaticLine(
                                    self,
                                    style=wx.LI_HORIZONTAL
                                    )
        main_sizer.Add(
            horizontal_line2, 
            0, 
            wx.EXPAND|wx.LEFT|wx.RIGHT|wx.BOTTOM,
            cfg.BORDER_WIDTH
            )
        main_sizer.Add(
            self.button_select_all, 
            0,
            wx.EXPAND|wx.LEFT|wx.RIGHT|wx.BOTTOM,
            cfg.BORDER_WIDTH
            )
        main_sizer.Add(
            self.button_move, 
            0, 
            wx.EXPAND|wx.LEFT|wx.RIGHT|wx.BOTTOM,
            cfg.BORDER_WIDTH
            )
        main_sizer.Add(
            self.button_export,
            0, 
            wx.EXPAND|wx.LEFT|wx.RIGHT|wx.BOTTOM,
            cfg.BORDER_WIDTH
            )
        main_sizer.Add(
            self.button_import,
            0,
            wx.EXPAND|wx.LEFT|wx.RIGHT|wx.BOTTOM,
            cfg.BORDER_WIDTH
            )
        self.SetSizerAndFit(main_sizer)
        self.__refresh_layers()

    def __bind_events(self):
        self.Bind(
            wx.EVT_BUTTON, 
            self.on_new_layer, 
            self.button_new
            )
        self.Bind(
            wx.EVT_BUTTON, 
            self.on_delete_layer, 
            self.button_delete
            )
        self.Bind(
            wx.EVT_BUTTON, 
            self.on_edit_layer, 
            self.button_edit
            )
        self.Bind(
            wx.EVT_BUTTON, 
            self.on_select_all, 
            self.button_select_all
            )
        self.Bind(
            wx.EVT_BUTTON,
            self.on_move_entities, 
            self.button_move
            )
        self.Bind(
            wx.EVT_BUTTON, 
            self.on_import_entities,
            self.button_import
            )
        self.Bind(
            wx.EVT_BUTTON, 
            self.on_export_entities,
            self.button_export
            )
        self.Bind(
            wx.EVT_BUTTON, 
            self.on_hideall, 
            self.button_hideall
            )
        self.Bind(
            wx.EVT_BUTTON, 
            self.on_showall,
            self.button_showall
            )
        self.Bind(
            wx.EVT_BUTTON, 
            self.on_lockall,
            self.button_lockall
            )
        self.Bind(
            wx.EVT_BUTTON, 
            self.on_unlockall, 
            self.button_unlockall
            )
        self.Bind(
            wx.EVT_TEXT, 
            self.on_filter,
            self.text_filter
            )

    def __add_all_layer_ctrls(self):
        selected_layer_id = servers.get_layer_manager().getselectedlayerid()
        layer = servers.get_layer_manager().gethead()
        while layer is not None:
            layer_id = layer.getid()
            # layer_ctrl = self.__add_layer_ctrl(
            #                     layerobject.LayerObject(layer)
            #                     )
            # if layer_id == selected_layer_id:
            #     self.layer_selected = layer_ctrl
            #     self.layer_selected.select()
            # layer = layer.getsucc()

    def __del_all_layer_ctrls(self):
        self.layer_selected = None
        while len(self.layers) > 0:
            self.__delete_layer_ctrl( self.layers[0] )

    def __get_layers_path(self):
        levelmanager = servers.get_level_manager()
        level = levelmanager.getcurrentlevelobject()
        layers_path = guisettings.Repository.getsettingvalue(
                                guisettings.ID_PresetsPath
                                ) + '/layermanager_' + level.getname() + '.n2'
        return servers.get_file_server().manglepath(
            str(layers_path) 
            )

    def get_id_for_selected_layer(self):
        return self.layer_selected.get_layer_id()

    def get_name_for_selected_layer(self):
        return self.layer_selected.get_layer_name()

    def __load_layers_state(self):
        """ Get the layers state persisted to the 
        layermanager file in the presets directory.
        The selected layer and the active flag value 
        for each layer is stored, but NOT the value 
        of the locked flag, since this is part of the 
        data persisted with its owning level. """
        servers.get_layer_manager().loadlayers(
            self.__get_layers_path() 
            )

    def save_layers_state(self):
        """ Save the layers state to file - see comment
        for the __load_layers_state method. """
        servers.get_layer_manager().savelayers(
            self.__get_layers_path() 
            )

    def on_new_layer(self, event):
        dlg = layereditdlg.NewLayerDialog(
                    self
                    )
        result = dlg.ShowModal()
        if result == wx.ID_OK:
            layer_created = dlg.layer
            # create new layer control
            self.Freeze()
            self.__add_layer_ctrl(
                layerobject.LayerObject(
                    layer_created
                    )
                )
            self.__refresh_layers()
            self.save_layers_state()
            self.Thaw()
        dlg.Destroy()

    def on_delete_layer(self, event):
        if self.layer_selected is None:
            cjr.show_error_message(
                "Please select a layer to delete."
                )
        else:
            msg = "Are you sure that you want to delete " \
                        "the '%s' layer?" % self.get_name_for_selected_layer()
            result = cjr.confirm_yes_no(self, msg)
            if result == wx.ID_YES:
                self.delete_selected_layer()

    def delete_selected_layer(self):
        if layer_can_be_deleted( self.layer_selected.get_layer() ):
            self.__delete_selected_layer()
        else:
            pass

    def __delete_selected_layer(self):
        layer_id = self.get_id_for_selected_layer()
        layer_name = self.get_name_for_selected_layer()
        cmd_server = servers.get_command_server()
        redo = "noreport = pynebula.lookup" \
                    "('/sys/servers/layermanager')." \
                    "removelayerbyname('%s');" \
                    "del noreport" % layer_name
        undo = "noreport = pynebula.lookup" \
                    "('/sys/servers/layermanager')." \
                    "createnewlayer2('%s',%s);" \
                    "del noreport" % (layer_name, layer_id)
        cmd_server.newcommand(
            str(redo), 
            str(undo) 
            )

        # delete layer control
        self.__delete_layer_ctrl(self.layer_selected)
        self.__refresh_layers()
        self.save_layers_state()

        # select first layer by default
        self.layer_selected = self.layers[0]
        self.layer_selected.select()

    def on_edit_layer(self, event):
        if self.layer_selected is None:
            cjr.show_error_message(
                "Please select a layer to edit."
                )
        else:
            self.layer_selected.edit_layer()

    def on_select_layer(self, event):
        if self.layer_selected != event.GetEventObject():
            if self.layer_selected is not None:
                self.layer_selected.on_deselect()
            self.layer_selected = event.GetEventObject()
            self.update_buttons()
            self.save_layers_state()

    def get_all_entities_for_selected_layer(self):
        """Return a list of entities"""
        layer_id = self.get_id_for_selected_layer()
        entity_list = []
        entity_server = servers.get_entity_object_server()
        entity = entity_server.getfirstentityobject()
        while entity is not None:
            if entity.hascomponent('ncEditor') \
                and entity.getlayerid() == layer_id:
                entity_list.append(entity)
            entity = entity_server.getnextentityobject()
        return entity_list

    def get_all_non_deleted_entities_for_selected_layer(self):
        def is_not_deleted(entity): return not entity.isdeleted()
        return filter (
            is_not_deleted, 
            self.get_all_entities_for_selected_layer()
            )

    def get_all_exportable_entities_for_selected_layer(self):
        """ Return a list of entities belonging to the layer that can be 
        exported.
        They must NOT be deleted, be of type NORMAL and NOT be 
        an outdoor. """
        result_list = []
        entity_list = self.get_all_non_deleted_entities_for_selected_layer()
        for each_entity in entity_list:
            if self.is_normal_entity(each_entity) \
                and not self.is_outdoor_entity(each_entity):
                result_list.append(each_entity)
        return result_list

    def is_normal_entity(self, entity):
        return self.get_entity_type_for_entity(entity) == CJR_NORMAL_ENTITY_TYPE

    def is_outdoor_entity(self, entity):
        return entity.isa('neoutdoor')

    def get_entity_type_for_entity(self, entity):        
        entity_server = servers.get_entity_object_server()
        return entity_server.getentityobjecttype(
            entity.getid() 
            )

    def on_select_all(self, event):
        if self.layer_selected is None:
            cjr.show_error_message("Please select a layer")
        else:
            entity_list = self.get_all_non_deleted_entities_for_selected_layer()
            if entity_list:
                obj_state = app.get_object_state()
                for each_entity in entity_list:
                    obj_state.addentitytoselection( each_entity.getid() )
            else:
                cjr.show_error_message(
                    "There are no selectable entities in this layer"
                    )

    def on_move_entities(self, event):
        if self.layer_selected is None:
            cjr.show_error_message("Please select a layer")
        else:
            layer_id = self.get_id_for_selected_layer()
            obj_state = app.get_object_state()
            items_were_moved = False
            for i in range( obj_state.getselectioncount() ):
                entity = obj_state.getselectedentity(i)
                if entity.hascomponent('ncEditor'):
                    entity.setlayerid(layer_id)
                    e_server = servers.get_entity_object_server()
                    e_server.setentityobjectdirty(entity, True)
                    items_were_moved = True
            if not items_were_moved:
                cjr.show_error_message(
                    "There are no entities currently selected "\
                        "that can be moved to this layer"
                    )

    def on_import_entities(self, event):
        if self.layer_selected is None:
            cjr.show_error_message("Please select a layer")
        else:
            importentity.importEntitiesFromFile(
                self, 
                self.get_id_for_selected_layer() 
                )

    def on_export_entities(self, event):
        """Open an export dialog"""
        if self.layer_selected is None:
            cjr.show_error_message("Please select a layer")
            return
        entity_list = self.get_all_exportable_entities_for_selected_layer()
        if entity_list:
            dialog = expentdlg.create_window(
                            app.get_top_window(), 
                            entity_list 
                            )
            dialog.ShowModal()
            dialog.Destroy()
        else:
            cjr.show_error_message(
                "There are no entities available for export in this layer"
                )

    def on_showall(self, event):
        try:
            try:
                wx.BeginBusyCursor()
                for layer_ctrl in self.layers:
                    layer_ctrl.tog_hide.SetToggle(False)
                    layer_ctrl.show_or_hide_layer(True)
                self.save_layers_state()
            finally:
                wx.EndBusyCursor()
        except:
            # make sure any errors are not hidden
            raise

    def on_hideall(self, event):
        try:
            try:
                wx.BeginBusyCursor()
                for layer_ctrl in self.layers:
                    layer_ctrl.tog_hide.SetToggle(True)
                    layer_ctrl.show_or_hide_layer(False)
                self.save_layers_state()
            finally:
                wx.EndBusyCursor()
        except:
            # make sure any errors are not hidden
            raise

    def on_lockall(self, event):
        for layer_ctrl in self.layers:
            layer_ctrl.handle_lock_layer_request(True)
            layer_ctrl.update_colors()
        self.update_buttons()

    def on_unlockall(self, event):
        for layer_ctrl in self.layers:
            layer_ctrl.handle_lock_layer_request(False)
            layer_ctrl.update_colors()
        self.update_buttons()

    def on_filter(self, event):
        self.__refresh_layers()

    def __add_layer_ctrl(self, layer):
        layer_ctrl = LayerCtrl(
                            self.scroll_layers, 
                            layer
                            )
        # insert layer sorted by name
        pos = 0
        layer_name = layer_ctrl.get_layer_name().lower()
        for each_ctrl in self.layers:
            if each_ctrl.get_layer_name().lower() > layer_name:
                break
            pos += 1
        self.layers.insert(
            pos, 
            layer_ctrl 
            )
        self.Bind(
            EVT_SELECT, 
            self.on_select_layer, 
            layer_ctrl
            )
        self.Bind(
            wx.EVT_BUTTON, 
            self.on_toggle_lock_layer, 
            layer_ctrl.tog_lock
            )
        self.Bind(
            wx.EVT_BUTTON, 
            self.on_toggle_hide_layer, 
            layer_ctrl.tog_hide
            )
        return layer_ctrl

    def __delete_layer_ctrl(self, layer_ctrl):
        self.Bind(EVT_SELECT, None, layer_ctrl)
        self.layers.remove(layer_ctrl)
        if self.layer_selected == layer_ctrl:
            self.layer_selected = None
        self.Freeze()
        layer_ctrl.Destroy()
        self.Thaw()

    def __refresh_layers(self):
        self.Freeze()
        # clean up layers subwindow
        for line in self.lines:
            line.Destroy()
        self.lines = []
        self.scroll_layers.SetSizer(None)
        
        # rebuild layers subwindow
        sizer_layers = wx.BoxSizer(wx.VERTICAL)
        filter_string = self.text_filter.GetValue().lower()
        for layer_ctrl in self.layers:
            if layer_ctrl.get_layer_name().lower().startswith(filter_string):
                sizer_layers.Add(
                    layer_ctrl, 
                    0,
                    wx.EXPAND
                    )
                line = wx.StaticLine(self.scroll_layers, -1)
                self.lines.append(line)
                sizer_layers.Add(
                    line, 
                    0, 
                    wx.EXPAND
                    )
                layer_ctrl.Show()
            else:
                layer_ctrl.Hide()
        
        # refresh layers subwindow
        self.scroll_layers.SetSizerAndFit(sizer_layers)
        sizer_layers.SetVirtualSizeHints(self.scroll_layers)
        self.Layout()
        self.update_buttons()
        self.Thaw()

    def update_buttons(self):
        more_than_one_layer = len(self.layers) > 1
        if self.layer_selected is None:
            is_selected_layer_locked = True
        else:
            is_selected_layer_locked = self.layer_selected.is_layer_locked()
        # enable or disable buttons based on selection
        self.button_delete.Enable(
            more_than_one_layer and not is_selected_layer_locked
            )
        self.button_edit.Enable(
            not is_selected_layer_locked
            )
        self.button_select_all.Enable(
            not is_selected_layer_locked
            )
        self.button_move.Enable(
            not is_selected_layer_locked
            )
        self.button_export.Enable(
            not is_selected_layer_locked
            )
        self.button_import.Enable(
            not is_selected_layer_locked
            )

    def on_toggle_lock_layer(self, event):
        self.update_buttons()

    def on_toggle_hide_layer(self, event):
        self.save_layers_state()
    
    def refresh(self):
        self.__del_all_layer_ctrls()
        self.__refresh_layers()
        self.__add_all_layer_ctrls()
        self.__refresh_layers()

    def persist(self):
        data = { 'filter':self.text_filter.GetValue() }
        return [
            create_window,
            (), # no parameters for create function
            data
            ]

    def restore(self, data_list):
        data = data_list[0]
        self.text_filter.SetValue( data['filter'] )

def layer_can_be_deleted(layer):
    """ Check that the layer can be deleted.
    Warn the user if it isn't and return false.
    Otherwise just return true. """
    # check the layer has not been locked
    layer_name = layer.getlayername()
    if layer.islocked():
        cjr.show_error_message(
            "You cannot delete the '%s' layer.\n\n" \
                "It is not possible to delete a locked layer." % layer_name
            )
        return False

    # check that there're at least two layers
    manager = servers.get_layer_manager()
    if manager.getnumlayers() <= 1:
        msg = "You cannot delete the '%s' layer.\n\n" \
                    "A level must always have at least one layer." % layer_name
        cjr.show_error_message(msg)
        return False

    # check if there's some entity not marked as deleted within the layer
    try:
        try:
            wx.BeginBusyCursor()
            e_server = servers.get_entity_object_server()
            entity = e_server.getfirstentityobject()
            report = []
            while entity is not None:
                if entity.hascomponent('ncEditor'):
                    if not entity.isdeleted():
                        entity_layer_id = entity.getlayerid()
                        if entity_layer_id >= 0:
                            entity_layer = manager.searchlayerbyid(
                                                    entity_layer_id 
                                                    )
                            if entity_layer.getlayername() == layer_name:
                                entity_class = entity.getentityclass().getname()
                                entity_name = ""
                                if entity.hascomponent('ncGameplay'):
                                    entity_name = entity.getname()
                                report.append(
                                    (entity.getid(), entity_class, entity_name)
                                    )
                entity = e_server.getnextentityobject()
        finally:
            wx.EndBusyCursor()
    except:
        # make sure any errors are not hidden
        raise
    
    # show a report about all the entities found in the layer, if any
    if len( report ) > 0:
        caption = "Error deleting the '%s' layer" % layer_name
        msg = "Unable to delete the '%s' layer because it " \
                    "still contains the following entities" % layer_name
        log = ( ['Id', 'Class', 'Name'], report )
        usagelog.UsageLogDialog(
            app.get_top_window(), 
            caption, 
            msg, 
            log 
            )
        return False
    
    return True


# create_window function
def create_window(parent):
    return LayerManagerDialog(parent)

