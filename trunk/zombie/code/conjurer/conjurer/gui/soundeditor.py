##\file soundeditor.py
##\brief Specific sound table editor panel

import wx

import app
import editorpanel
import format
import servers

import conjurerframework as cjr
import conjurerconfig as cfg


# AddSoundDialog class
class AddSoundDialog(wx.Dialog):
    """Dialog to add a sound"""
    
    def __init__(self, parent, sound):
        wx.Dialog.__init__(
            self, 
            parent, 
            -1, 
            "Add a sound event",
            style = wx.DEFAULT_DIALOG_STYLE|
                        wx.RESIZE_BORDER|
                        wx.TAB_TRAVERSAL
                        )
        self.sound = sound
        self.label_event = wx.StaticText(
                                    self, 
                                    -1, 
                                    "Event", 
                                    style=wx.TE_RIGHT
                                    )
        self.text_event = wx.TextCtrl(self, -1, "")
        self.label_material = wx.StaticText(
                                        self,
                                        -1, 
                                        "Material", 
                                        style=wx.TE_RIGHT
                                        )
        self.text_material = wx.TextCtrl(self, -1, "")
        self.label_id = wx.StaticText(
                                self,
                                -1, 
                                "Sound ID", 
                                style=wx.TE_RIGHT
                                )
        self.list_ids = wx.ListBox(self, -1, choices=[])
        self.button_add = wx.Button(self, -1, "&Add sound")
        self.button_cancel = wx.Button(self, wx.ID_CANCEL, "&Cancel")
        
        self.__set_properties()
        self.__do_layout()
        self.__bind_events()
    
    def __set_properties(self):
        # Align labels
        format.align_labels([
            self.label_event,
            self.label_material,
            self.label_id
            ])
        
        # Fill sound id list
        lib = servers.get_sound_library()
        for i in range( lib.getnumberofsounds() ):
            if self.list_ids.FindString(lib.getsoundid(i)) == wx.NOT_FOUND :
                self.list_ids.Append( lib.getsoundid(i) )
        
        self.__update_button_add()
    
    def __do_layout(self):
        sizer_border = wx.BoxSizer(wx.VERTICAL)
        sizer_layout = wx.BoxSizer(wx.VERTICAL)
        sizer_buttons = wx.BoxSizer(wx.HORIZONTAL)
        sizer_id = wx.BoxSizer(wx.HORIZONTAL)
        sizer_material = wx.BoxSizer(wx.HORIZONTAL)
        sizer_event = wx.BoxSizer(wx.HORIZONTAL)
        sizer_event.Add(
            self.label_event, 
            0, 
            wx.ALIGN_CENTER_VERTICAL|wx.FIXED_MINSIZE
            )
        sizer_event.Add(
            self.text_event, 
            1, 
            wx.LEFT|wx.FIXED_MINSIZE,
            cfg.BORDER_WIDTH
            )
        sizer_layout.Add(
            sizer_event, 
            0, 
            wx.EXPAND
            )
        sizer_material.Add(
            self.label_material,
            0,
            wx.ALIGN_CENTER_VERTICAL|wx.FIXED_MINSIZE, 
            )
        sizer_material.Add(
            self.text_material,
            1, 
            wx.LEFT|wx.FIXED_MINSIZE, 
            cfg.BORDER_WIDTH
            )
        sizer_layout.Add(
            sizer_material,
            0, 
            wx.TOP|wx.EXPAND,
            cfg.BORDER_WIDTH
            )
        sizer_id.Add(
            self.label_id,
            0, 
            wx.FIXED_MINSIZE
            )
        sizer_id.Add(
            self.list_ids, 
            1, 
            wx.LEFT|wx.EXPAND|wx.FIXED_MINSIZE, 
            cfg.BORDER_WIDTH
            )
        sizer_layout.Add(
            sizer_id,
            1, 
            wx.TOP|wx.EXPAND, 
            cfg.BORDER_WIDTH
            )
        horizontal_line = wx.StaticLine(self, -1)
        sizer_layout.Add(
            horizontal_line,
            0, 
            wx.EXPAND|wx.TOP, 
            cfg.BORDER_WIDTH
            )
        sizer_buttons.Add(
            self.button_add, 
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
        self.SetSizerAndFit(sizer_border)
    
    def __bind_events(self):
        self.Bind(wx.EVT_TEXT, self.on_change_sound, self.text_event)
        self.Bind(wx.EVT_TEXT, self.on_change_sound, self.text_material)
        self.Bind(wx.EVT_LISTBOX, self.on_change_sound, self.list_ids)
        self.Bind(wx.EVT_BUTTON, self.on_add, self.button_add)
        self.Bind(wx.EVT_BUTTON, self.on_cancel, self.button_cancel)
    
    def __update_button_add(self):
        self.button_add.Enable( self.text_event.GetValue() != "" and
            self.list_ids.GetSelection() != -1 )
    
    def on_change_sound(self, event):
        self.__update_button_add()
    
    def on_add(self, event):
        event = self.text_event.GetValue()
        material = self.text_material.GetValue()
        sound_id = self.list_ids.GetStringSelection()
        parent = self.GetParent()
        for index in range( parent.list.GetItemCount() ):
            if parent.list.GetItem(index, parent.Col_Event).GetText() == event and \
                parent.list.GetItem(index, parent.Col_Material).GetText() == material:
                cjr.show_error_message(
                    "There's already another sound event with the" \
                    " name '" + event + "' and material '" + material + "'."
                    )
                return
        self.sound.safegetsoundtable().addsoundevent(
            str(event), 
            str(material),
            str(sound_id) 
            )
        servers.get_entity_class_server().setentityclassdirty(
            self.sound, 
            True 
            )
        index = parent.list.InsertStringItem( 0, event )
        parent.list.SetStringItem( index, parent.Col_Material, material )
        parent.list.SetStringItem( index, parent.Col_SoundId, sound_id )
        self.EndModal(wx.ID_OK)
    
    def on_cancel(self, event):
        self.EndModal(wx.ID_CANCEL)
    
    def get_sound_id(self):
        return self.list_ids.GetStringSelection()


# SoundTablePanel class
class SoundTablePanel(editorpanel.EditorPanel):
    """Specific sound table editor panel"""
    
    # Animations list columns indices
    Col_Event = 0
    Col_Material = 1
    Col_SoundId = 2
    
    def __init__(self, *args, **kwds):
        editorpanel.EditorPanel.__init__(self, *args, **kwds)
        self.sound = None
        
        self.label = wx.StaticText(self, -1, "Sound events")
        self.list = wx.ListCtrl(
                        self, 
                        -1,
                        style=wx.LC_REPORT|
                                    wx.LC_SINGLE_SEL|
                                    wx.LC_SORT_ASCENDING
                        )
        self.label_id = wx.StaticText(self, -1, "Sound ID")
        self.button_id = wx.Button(self, -1, "")
        self.button_play = wx.ToggleButton(self, -1, "Play")
        self.spin_repeat = wx.SpinCtrl(
                                    self, 
                                    -1, 
                                    "", 
                                    min=0,
                                    max=999, 
                                    size=(50,-1)
                                    )
        self.label_repeat = wx.StaticText(self, -1, "times (0 = infinite)")
        self.button_add = wx.Button(self, -1, "Add")
        self.button_delete = wx.Button(self, -1, "Delete")
        self.button_import = wx.Button(self, -1, "Import group")
        self.button_duplicate_events = wx.Button(self, -1, "Duplicate events")
        
        self.__set_properties()
        self.__do_layout()
        self.__bind_events()
    
    def __set_properties(self):
        self.SetScrollRate(10, 10)
        
        # Sound table columns
        self.list.InsertColumn(self.Col_Event, "Event")
        self.list.InsertColumn(self.Col_Material, "Material")
        self.list.InsertColumn(self.Col_SoundId, "Sound ID")
        
        # Default repeat
        self.spin_repeat.SetValue(1)
    
    def __do_layout(self):
        sizer = wx.BoxSizer(wx.VERTICAL)
        sizer_list = wx.BoxSizer(wx.VERTICAL)
        sizer_id = wx.BoxSizer(wx.HORIZONTAL)
        sizer_play = wx.BoxSizer(wx.HORIZONTAL)
        sizer_buttons = wx.BoxSizer(wx.HORIZONTAL)
        sizer_buttons2 = wx.BoxSizer(wx.HORIZONTAL)
        sizer_list.Add(
            self.label,
            0, 
            wx.FIXED_MINSIZE
            )
        sizer_list.Add(
            self.list,
            1,
            wx.TOP|wx.EXPAND,
            cfg.BORDER_WIDTH
            )
        sizer.Add(
            sizer_list,
            1, 
            wx.LEFT|wx.RIGHT|wx.TOP|wx.EXPAND, 
            cfg.BORDER_WIDTH
            )
        sizer.Add(
            (cfg.BORDER_WIDTH, cfg.BORDER_WIDTH) 
            )
        sizer_id.Add(
            self.label_id,
            0, 
            wx.ALIGN_CENTER_VERTICAL|wx.FIXED_MINSIZE
            )
        sizer_id.Add(
            self.button_id,
            1, 
            wx.LEFT|wx.FIXED_MINSIZE, 
            cfg.BORDER_WIDTH
            )
        sizer.Add(
            sizer_id, 
            0, 
            wx.LEFT|wx.RIGHT|wx.EXPAND, 
            cfg.BORDER_WIDTH
            )
        sizer.Add(
            (cfg.BORDER_WIDTH, cfg.BORDER_WIDTH)
            )
        sizer_play.Add(
            self.button_play, 
            1, 
            wx.ALIGN_CENTER_VERTICAL|wx.FIXED_MINSIZE
            )
        sizer_play.Add(
            self.spin_repeat,
            0,
            wx.LEFT|wx.ALIGN_CENTER_VERTICAL|wx.FIXED_MINSIZE, 
            cfg.BORDER_WIDTH
            )
        sizer_play.Add(
            self.label_repeat, 
            0, 
            wx.LEFT|wx.ALIGN_CENTER_VERTICAL|wx.FIXED_MINSIZE, 
            cfg.BORDER_WIDTH
            )
        sizer.Add(
            sizer_play,
            0, 
            wx.LEFT|wx.RIGHT|wx.EXPAND,
            cfg.BORDER_WIDTH
            )
        sizer.Add( 
            (cfg.BORDER_WIDTH, cfg.BORDER_WIDTH) 
            )
        sizer_buttons.Add(
            self.button_add,
            1,
            wx.FIXED_MINSIZE
            )
        sizer_buttons.Add(
            self.button_delete,
            1,
            wx.LEFT|wx.FIXED_MINSIZE, 
            cfg.BORDER_WIDTH
            )
        sizer.Add(
            sizer_buttons,
            0, 
            wx.LEFT|wx.RIGHT|wx.BOTTOM|wx.EXPAND, 
            cfg.BORDER_WIDTH
            )
        sizer_buttons2.Add(
            self.button_import, 
            1,
            wx.FIXED_MINSIZE
            )
        sizer_buttons2.Add(
            self.button_duplicate_events,
            1, 
            wx.LEFT|wx.FIXED_MINSIZE,
            cfg.BORDER_WIDTH
            )
        sizer.Add(
            sizer_buttons2, 
            0,
            wx.LEFT|wx.RIGHT|wx.BOTTOM|wx.EXPAND, 
            cfg.BORDER_WIDTH
            )
        self.SetSizerAndFit(sizer)

    def __bind_events(self):
        self.Bind(
            wx.EVT_LIST_ITEM_SELECTED, 
            self.on_select_sound, 
            self.list
            )
        self.Bind(
            wx.EVT_LIST_ITEM_ACTIVATED, 
            self.on_change_sound_id, 
            self.list
            )
        self.Bind(
            wx.EVT_BUTTON, 
            self.on_change_sound_id, 
            self.button_id
            )
        self.Bind(
            wx.EVT_TOGGLEBUTTON, 
            self.on_toggle_play, 
            self.button_play
            )
        self.Bind(
            wx.EVT_BUTTON, 
            self.on_add_sound, 
            self.button_add
            )
        self.Bind(
            wx.EVT_BUTTON, 
            self.on_delete_sound, 
            self.button_delete
            )
        self.Bind(
            wx.EVT_BUTTON, 
            self.on_import_sound,
            self.button_import
            )
        self.Bind(
            wx.EVT_BUTTON, 
            self.on_duplicate_events, 
            self.button_duplicate_events
            )

    def __get_selected_list_index(self):
        return self.list.GetNextItem(
            -1,
            wx.LIST_NEXT_ALL, 
            wx.LIST_STATE_SELECTED
            )

    def __get_selected_list_item(self, column):
        index = self.__get_selected_list_index()
        if index == -1:
            return ""
        else:
            return self.list.GetItem(index, column).GetText()

    def __get_sound_table(self):
        return self.sound.safegetsoundtable()

    def __update_ctrls(self):
        index = self.__get_selected_list_index()
        enable1 = self.sound.safegetsoundtable() is not None
        enable2 = enable1 and index != -1
        self.list.Enable( enable1 )
        self.button_play.Enable( enable2 )
        self.button_add.Enable( enable1 )
        self.button_id.Enable( enable2 )
        self.button_delete.Enable( enable2 )
        self.button_import.Enable( enable1 )
        self.button_duplicate_events.Enable( enable1 )
        if enable2:
            event = self.__get_selected_list_item( self.Col_Event )
            material = self.__get_selected_list_item( self.Col_Material )
            table = self.__get_sound_table()
            sound_id = table.getsoundid( str(event), str(material) )
            self.button_id.SetLabel( sound_id )

    def __update_sound_triggers(self):
        es = servers.get_entity_object_server()

    def on_select_sound(self, event):
        self.__update_ctrls()

    def on_change_sound_id(self, event):
        index = self.__get_selected_list_index()
        if index != -1:
            sound_ids = []
            lib = servers.get_sound_library()
            for i in range( lib.getnumberofsounds() ):
                if not lib.getsoundid(i) in sound_ids:
                    sound_ids.append( lib.getsoundid(i) )
            dlg = wx.SingleChoiceDialog(
                        None, 
                        "Sound IDs",
                        "Choose a Sound ID", 
                        sound_ids 
                        )
            if dlg.ShowModal() == wx.ID_OK and dlg.GetStringSelection() != "":
                sound_id = dlg.GetStringSelection()
                event = self.__get_selected_list_item( self.Col_Event )
                material = self.__get_selected_list_item( self.Col_Material )
                table = self.__get_sound_table()
                table.setsoundid(
                    str(event),
                    str(material),
                    str(sound_id) 
                    )
                servers.get_entity_class_server().setentityclassdirty(
                    self.sound, 
                    True 
                    )
                self.list.SetStringItem(
                    index, 
                    self.Col_SoundId,
                    sound_id 
                    )
                self.button_id.SetLabel( sound_id )
                self.__update_sound_triggers()
            dlg.Destroy()

    def on_toggle_play(self, event):
        # Play/stop all selected entities with the ncSound component
        state = app.get_object_state()
        play = self.button_play.GetValue()
        for i in range( state.getselectioncount() ):
            entity = state.getselectedentity(i)
            if entity.hascommand('playmaterial'):
                if play:
                    event = self.__get_selected_list_item(
                                    self.Col_Event 
                                    )
                    material = self.__get_selected_list_item(
                                    self.Col_Material 
                                    )
                    repeat = self.spin_repeat.GetValue()
                    entity.playmaterial(
                        str(event), 
                        str(material),
                        repeat 
                        )
                else:
                    entity.stopsound()

    def on_add_sound(self, event):
        dlg = AddSoundDialog(self, self.sound)
        if dlg.ShowModal() == wx.ID_OK:
            self.__update_sound_triggers()
        dlg.Destroy()

    def on_delete_sound(self, event):
        index = self.__get_selected_list_index()
        if index != -1:
            event = self.__get_selected_list_item( self.Col_Event )
            material = self.__get_selected_list_item( self.Col_Material )
            table = self.sound.safegetsoundtable()
            table.removesoundevent( str(event), str(material) )
            servers.get_entity_class_server().setentityclassdirty(
                self.sound, 
                True 
                )
            self.list.DeleteItem( index )
            self.__update_ctrls()
            self.__update_sound_triggers()

    def on_import_sound(self, event):
        lib = servers.get_sound_library()
        groups = []
        for index in range( lib.getnumberofsoundgroups() ):
            groups.append( lib.getsoundgroupname(index) )
        dialog = wx.SingleChoiceDialog(
                        self, 
                        "Select a group", 
                        "Select sound group", 
                        groups
                        )
        val = dialog.ShowModal()
        if val == wx.ID_OK:
            table = self.sound.safegetsoundtable()
            groupname = dialog.GetStringSelection()
            suc = table.importsoundgroup( str(groupname) )
            if suc:
                servers.get_entity_class_server().setentityclassdirty(
                    self.sound, 
                    True 
                    )
                self.refresh()
            else:
                cjr.show_error_message(
                    "%s group not found" % groupname
                    )
        dialog.Destroy()

    def on_duplicate_events(self, event):
        table = self.sound.safegetsoundtable()
        events = []
        for i in range( table.getnumberofsounds() ):
            event, material, sound_id = table.getsoundevent(i)
            if not event in events:
                events.append( event )

        dialog = wx.SingleChoiceDialog(
                        self, 
                        "Select an event to copy",
                        "Select event", 
                        events
                        )
        val = dialog.ShowModal()
        if val == wx.ID_OK:
            event_name = dialog.GetStringSelection()
            
            dialog2 = wx.TextEntryDialog(
                            self, 
                            'Enter new event name',
                            'Duplicate events', 
                            ''
                            )
            dest_event = ""
            if dialog2.ShowModal() == wx.ID_OK:
                dest_event = dialog2.GetValue()
            dialog2.Destroy()
            
            if dest_event == "":
                return

            num_added = table.duplicatesoundevent(
                                    str(event_name),
                                    str(dest_event) 
                                    )
            if num_added != 0:
                servers.get_entity_class_server().setentityclassdirty(
                    self.sound, 
                    True 
                    )
                self.refresh()
        dialog.Destroy()
        

    def set_sound(self, sound):
        self.sound = sound
        table = self.__get_sound_table()
        
        # Fill sound table
        if table is not None:
            self.list.DeleteAllItems()
            for i in range( table.getnumberofsounds() ):
                event, material, sound_id = table.getsoundevent(i)
                index = self.list.InsertStringItem( 0, event )
                self.list.SetStringItem( index, self.Col_Material, material )
                self.list.SetStringItem( index, self.Col_SoundId, sound_id )
        
        # Update controls that depends on selected sound
        self.__update_ctrls()
    
    def refresh(self):
        try:
            self.set_sound( self.sound )
        except:
            self.Close()


# create_all_editors function
def create_all_editors(sound, parent):
    editor = SoundTablePanel(parent, -1)
    editor.set_sound(sound)
    return [ ('Sounds', editor) ]
