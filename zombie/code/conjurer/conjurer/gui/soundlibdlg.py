##\file soundlibdlg2.py
##\brief Dialog to manage the sound library

import wx
import wx.grid as gridlib
import wx.lib.scrolledpanel as scrolledpanel

import os.path

import pynebula
import conjurerconfig as cfg
import conjurerframework as cjr
import soundlib
import newsounddlg
import floatslider
import floatctrl
import conjurerspinctrl as spin

import app
import events
import servers
import togwin
import format


soundEVT_DIRECTORY_CHANGED = wx.NewEventType()
EVT_DIRECTORY_CHANGED = wx.PyEventBinder(
                                            soundEVT_DIRECTORY_CHANGED,
                                            1
                                            )

soundEVT_SOUND_LIBRARY_AMENDED = wx.NewEventType()
EVT_SOUND_LIBRARY_AMENDED = wx.PyEventBinder(
                                                soundEVT_SOUND_LIBRARY_AMENDED
                                                )


soundEVT_SELECTED_RECORDS_CHANGED = wx.NewEventType()
EVT_SELECTED_RECORDS_CHANGED = wx.PyEventBinder(
                                    soundEVT_SELECTED_RECORDS_CHANGED,
                                    1
                                    )

soundEVT_SOUND_RECORDS_AMENDED = wx.NewEventType()
EVT_SOUND_RECORDS_AMENDED = wx.PyEventBinder(
                                                soundEVT_SOUND_RECORDS_AMENDED
                                                )


# SoundDataTable class
class SoundDataTable(gridlib.PyGridTableBase):
    def __init__(self):
        gridlib.PyGridTableBase.__init__(self)
        
        # Column attributes
        self.Col_Mute = 0
        self.Col_Solo = 1
        self.Col_SoundId = 2
        self.Col_Volume = 3
        self.Col_Ambient = 4
        self.Col_VolumeVar = 5
        self.Col_PitchControl = 6
        self.Col_Delay = 7
        self.Col_MaxInstances = 8
        self.Col_Streamed = 9
        self.Col_Priority = 10
        
        self.ColsNumber = 11
        self.Label = 0
        self.GetCmd = 1
        self.Format = 2
        self.attributes = [
            ("Mute", self.__get_mute, gridlib.GRID_VALUE_BOOL),
            ("Solo", self.__get_solo, gridlib.GRID_VALUE_BOOL),
            ("Sound ID", self.__get_sound_id, gridlib.GRID_VALUE_STRING),
            ("Volume", self.__get_volume, gridlib.GRID_VALUE_FLOAT + ':6,2'),
            ("Ambient?", self.__get_ambient, gridlib.GRID_VALUE_BOOL),
            ("Vol. var.", self.__get_volume_var, gridlib.GRID_VALUE_FLOAT + ':6,2'),
            ("Pitch control", self.__get_pitch_flag, gridlib.GRID_VALUE_BOOL),
            ("Delay", self.__get_delay, gridlib.GRID_VALUE_FLOAT + ':6,2'),
            ("Max. instances", self.__get_max_instances, gridlib.GRID_VALUE_NUMBER + ':1,1000'),
            ("Streamed?", self.__get_streamed, gridlib.GRID_VALUE_BOOL),
            ("Priority", self.__get_priority, gridlib.GRID_VALUE_NUMBER + ':1,1000'),
            ]
            
        # Filter state
        self.filter_enabled = False
        self.filter_sound_indexes = []
        self.items = []

    def __get_mute(self, i):
        return self.items[i].get_mute()

    def __get_solo(self, i):
        return self.items[i].get_solo()

    def __get_sound_id(self, i):
        return self.items[i].get_sound_id()

    def __get_ambient(self, i):
        return self.items[i].get_ambient()

    def __get_volume(self, i):
        return self.items[i].get_volume()

    def __get_pitch_var(self, i):
        return self.items[i].get_pitch_var()

    def __get_pitch_flag(self, i):
        return self.items[i].get_pitch_control()

    def __get_delay(self, i):
        return self.items[i].get_delay()

    def __get_volume_var(self, i):
        return self.items[i].get_volume_var()
        
    def __get_max_instances(self, i):
        return self.items[i].get_max_instances()
        
    def __get_streamed(self, i):
        return self.items[i].get_streamed()

    def __get_priority(self, i):
        return self.items[i].get_priority()
   
    def GetNumberRows(self):
        return self.get_num_rows()
    
    def get_num_rows(self):
        return len(self.items)
    
    def GetNumberCols(self):
        return self.ColsNumber
    
    def IsEmptyCell(self, row, col):
        return not self.attributes[col][self.GetCmd](row)
    
    def GetValue(self, row, col):
        return self.attributes[col][self.GetCmd](row)
    
    def GetColLabelValue(self, col):
        return self.attributes[col][self.Label]
    
    def GetTypeName(self, row, col):
        return self.attributes[col][self.Format]
    
    def CanGetValueAs(self, row, col, type_name):
        col_type = self.attributes[col][self.Format].split(':')[0]
        if type_name == col_type:
            return True
        else:
            return False
    
    def CanSetValueAs(self, row, col, type_name):
        return self.CanGetValueAs(row, col, type_name)
    
class SoundLibraryTreeCtrl(wx.TreeCtrl):
    def __init__(self, parent, manager):
        wx.TreeCtrl.__init__(
            self,
            parent,
            -1,
            wx.DefaultPosition,
            wx.DefaultSize,
            wx.TR_HAS_BUTTONS
            )
        self.set_default_icons()
        self.manager = manager
        root_item = self.manager.root_tree_item
        self.root = self.AddRoot(
                        root_item.name(),
                        -1, 
                        -1,
                        wx.TreeItemData(root_item)
                        )
        self.SetItemImage(
            self.root, 
            self.fldridx, 
            wx.TreeItemIcon_Normal)
        self.SetItemImage(
            self.root, 
            self.fldropenidx, 
            wx.TreeItemIcon_Expanded)
        self.SetItemHasChildren(self.root, True)
        self.Bind(
            wx.EVT_TREE_ITEM_EXPANDING, 
            self.__on_item_expanding
            )
        self.Bind(
            wx.EVT_TREE_ITEM_COLLAPSED, 
            self.__on_item_collapsed
            )
        self.Expand(self.root)
        
    def set_default_icons (self):
        image_size = (16, 16)
        image_list = wx.ImageList(image_size[0], image_size[1])
        self.fldridx = image_list.Add(
                        wx.ArtProvider_GetBitmap(
                            wx.ART_FOLDER,      
                            wx.ART_OTHER, 
                            image_size
                            )
                        )
        self.fldropenidx = image_list.Add(
                            wx.ArtProvider_GetBitmap(
                                wx.ART_FILE_OPEN,   
                                wx.ART_OTHER, 
                                image_size
                                )
                            )
        self.SetImageList(image_list)
        self.image_list = image_list

    def __on_item_expanding (self, event):                       
        item = event.GetItem()
        if not self.IsExpanded(item):
            # This event can happen twice in the self.Expand call
            self.populate_item(item)

    def populate_item(self, item):
        sound_tree_item = self.GetPyData(item)
        for each_child in sound_tree_item.get_child_nodes_sorted():
            new_item = self.AppendItem(
                                item, 
                                each_child.name(),
                                -1,
                                -1,
                                wx.TreeItemData(each_child)
                                )  
            self.SetItemImage(
                new_item, 
                self.fldridx, 
                wx.TreeItemIcon_Normal)
            self.SetItemImage(
                new_item, 
                self.fldropenidx, 
                wx.TreeItemIcon_Expanded)
            self.SetItemHasChildren(
                new_item, 
                each_child.has_child_nodes()
                )
    
    def __on_item_collapsed(self, event):
        item = event.GetItem()
        self.DeleteChildren(item)        


class SoundLibrarySelectionChangedEvent(wx.PyCommandEvent):
    def __init__(self, event_type, widget_id):
        wx.PyCommandEvent.__init__(self, event_type, widget_id)
        self.value = None

    def set_value(self, value):
        self.value = value

    def get_value(self):
        return self.value


class SoundLibraryTreeViewPanel(wx.Panel):
    """Panel for the tree view of the sound library."""
    def __init__(self, parent, library):
        wx.Panel.__init__(self, parent)
        self.library = library
        self.tree = SoundLibraryTreeCtrl(
                        self, 
                        self.library.tree
                        )
        self.__do_layout()
        self.__bind_events()

    def __do_layout(self):
        sizer = wx.BoxSizer(wx.VERTICAL)
        sizer.Add(
            self.tree, 
            1, 
            wx.EXPAND | wx.ALL, 
            cfg.BORDER_WIDTH
            )
        
        self.SetSizer(sizer)
        sizer.Fit(self)
        sizer.SetSizeHints(self)
        self.Layout()

    def __bind_events(self):
        self.Bind(
            wx.EVT_TREE_SEL_CHANGED,
            self.__on_selected_tree_item_changed,
            self.tree)

    def __on_selected_tree_item_changed(self, event):
        item = self.tree.GetPyData( event.GetItem() )
        custom_event = SoundLibrarySelectionChangedEvent(
                                soundEVT_DIRECTORY_CHANGED, 
                                self.GetId()
                                )
        custom_event.set_value(item)
        self.GetEventHandler().ProcessEvent(
            custom_event
            )
        event.Skip()


class SoundLibraryGrid(gridlib.Grid):
    def __init__(self, parent):
        gridlib.Grid.__init__(self, parent, -1)
        self.__set_properties()
        self.__bind_events()

    def __set_properties(self):
        # show the number column
        self.SetRowLabelSize(40)
        self.DisableDragRowSize()
        self.EnableEditing(False)

    def __bind_events(self):
        self.Bind(
            gridlib.EVT_GRID_SELECT_CELL,
            self.__on_select_cell
            )
        self.Bind(
            wx.EVT_KEY_DOWN,
            self.__on_key_down
            )

    def __on_select_cell(self, event):
        row_index = event.GetRow()
        self.SelectRow(row_index)
        event.Skip()

    def __on_key_down(self, event):
      # override default behaviour to tab to next control
        if event.KeyCode() == wx.WXK_TAB:
            self.Navigate()
        else:
            event.Skip()


class SoundLibraryListViewPanel(wx.Panel):
    def __init__(self, parent, sound_library):
        wx.Panel.__init__(
            self, 
            parent, 
            style = wx.BORDER_SUNKEN | wx.TAB_TRAVERSAL
            )
        self.table = SoundDataTable()
        self.node_path = None
        self.sound_library = sound_library
        self.grid = SoundLibraryGrid(self)
        self.grid.SetTable(self.table)
        self.grid.SetSelectionMode(gridlib.Grid.wxGridSelectRows)
        self.button_play = wx.Button(self, -1, "&Play")
        self.button_add = wx.Button(self, -1, "&Add")
        self.button_remove = wx.Button(self, -1, "Re&move")
        self.button_refresh = wx.Button(self, -1, "Scan for &New Files")
        self.__set_properties()
        self.__do_layout()
        self.__bind_events()
    
    def __set_properties(self):
        self.show_sounds_for_current_node()

    def __do_layout(self):
        sizer_layout = wx.BoxSizer(wx.VERTICAL)
        sizer_buttons = wx.BoxSizer(wx.HORIZONTAL)
        sizer_layout.Add(
            self.grid, 
            1, 
            wx.EXPAND | wx.ALL, 
            cfg.BORDER_WIDTH
            )
        sizer_buttons.Add(self.button_play, 0)
        sizer_buttons.Add(self.button_add, 0, wx.LEFT, cfg.BORDER_WIDTH)
        sizer_buttons.Add(self.button_remove, 0, wx.LEFT, cfg.BORDER_WIDTH)
        sizer_buttons.Add(self.button_refresh, 0, wx.LEFT, cfg.BORDER_WIDTH)
        sizer_layout.Add(
            sizer_buttons, 
            0, 
            wx.ALIGN_RIGHT | wx.ALL, 
            cfg.BORDER_WIDTH
            )
        self.SetSizer(sizer_layout)
        sizer_layout.Fit(self)
        sizer_layout.SetSizeHints(self)
        self.Layout()
    
    def __bind_events(self):
        self.Bind(
            gridlib.EVT_GRID_RANGE_SELECT, 
            self.__on_change_selection, 
            self.grid
            )
        self.Bind(
            wx.EVT_BUTTON, 
            self.__on_play, 
            self.button_play
            )
        self.Bind(
            wx.EVT_BUTTON, 
            self.__on_add, 
            self.button_add
            )
        self.Bind(
            wx.EVT_BUTTON, 
            self.__on_remove, 
            self.button_remove
            )
        self.Bind(
            wx.EVT_BUTTON, 
            self.__on_refresh, 
            self.button_refresh
            )

    def update_data(self, new_data):
        self.grid.BeginBatch()
        current_num_rows = self.grid.GetNumberRows()
        new_num_rows = len(new_data)
        self.table.items = new_data
        if new_num_rows < current_num_rows:
            msg = gridlib.GridTableMessage(
                        self.table,
                        gridlib.GRIDTABLE_NOTIFY_ROWS_DELETED,
                        current_num_rows - new_num_rows,    # position
                        current_num_rows - new_num_rows)    # how many
            self.grid.ProcessTableMessage(msg)
        if new_num_rows > current_num_rows:
            msg = gridlib.GridTableMessage(
                        self.table,
                        gridlib.GRIDTABLE_NOTIFY_ROWS_APPENDED,
                        new_num_rows - current_num_rows)    # how many
            self.grid.ProcessTableMessage(msg)
        self.grid.EndBatch()
        msg = gridlib.GridTableMessage(
                    self.table, 
                    gridlib.GRIDTABLE_REQUEST_VIEW_GET_VALUES
                    )
        self.grid.ProcessTableMessage(msg)

    def show_sounds_for_node(self, node_path):
        # make sure any pending changes in the current cell are saved
        self.grid.SaveEditControlValue()
        self.node_path = node_path
        self.show_sounds_for_current_node()
        # automatically size all columns to fit contents
        # boolean means will NOT also make this min width
        # ...a bit slow if there are lots of rows
        self.grid.AutoSizeColumns(False)

    def __get_child_records_for_current_node(self):
        return self.sound_library.get_child_records_for_path(
            self.node_path
            )

    def show_sounds_for_current_node(self):
        if self.__has_node_path():
            self.grid.ClearSelection()
            self.update_data(
                self.__get_child_records_for_current_node()
                )
        self.__update_buttons()

    def refresh_all_rows(self):
        msg = gridlib.GridTableMessage(
                    self.table, 
                    gridlib.GRIDTABLE_REQUEST_VIEW_GET_VALUES
                    )
        self.grid.ProcessTableMessage(msg)

    def __update_buttons(self):
        enable = self.__has_selected_items()
        has_path = self.__has_node_path() 
        self.button_play.Enable(enable)
        self.button_add.Enable(has_path)
        self.button_remove.Enable(enable)
        self.button_refresh.Enable(has_path)
    
    def __has_node_path(self):
        return self.node_path != None

    def __signal_sound_library_amended(self):
        custom_event = wx.PyCommandEvent(
                                soundEVT_SOUND_LIBRARY_AMENDED, 
                                self.GetId()
                                )
        self.GetEventHandler().ProcessEvent(
            custom_event
            )

    def __on_change_selection(self, event):
        self.__signal_selected_records_changed()
        self.__update_buttons()
        event.Skip()

    def __signal_selected_records_changed(self):
        items = self.get_selected_items() 
        custom_event = SoundLibrarySelectionChangedEvent(
                                soundEVT_SELECTED_RECORDS_CHANGED, 
                                self.GetId()
                                )
        custom_event.set_value(items)
        self.GetEventHandler().ProcessEvent(
            custom_event
            )

    def __create_sound(self):
        dlg = newsounddlg.create_window(
            self,
            self.sound_library,
            self.node_path
            )
        dlg.ShowModal()
        dlg.Destroy()
        self.show_sounds_for_current_node()
        self.__signal_sound_library_amended()
        self.__update_buttons()

    def __on_add(self, event):
        self.__create_sound()
    
    def __on_remove(self, event):
        items = self.get_selected_items()
        for each_sound in items:
            msg = "Deleting a sound cannot be undone and will not " \
                        "update any open class event lists.\n" \
                        "Are you sure that you want to delete the sound '%s'?" % each_sound.get_sound_id()
            answer = cjr.warn_yes_no(self, msg)
            if answer == wx.ID_YES:
                self.sound_library.remove_sound(each_sound)
            elif answer == wx.ID_CANCEL:
                break
        self.show_sounds_for_current_node()    
        self.__signal_sound_library_amended()
        self.__update_buttons()
    
    def __on_play(self, event):
        self.sound_library.stop_all_sounds()
        for each_item in self.get_selected_items():
            each_item.play()

    def __on_refresh(self, event):
        self.__local_refresh()

    def __local_refresh(self):
        if self.__has_node_path():
            try:
                wx.BeginBusyCursor()
                self.sound_library.do_file_scan(self.node_path, False)
                self.show_sounds_for_current_node()
                self.__signal_sound_library_amended()
            finally:
                wx.EndBusyCursor()

    def get_selected_items(self):
        # complicated, because we need to take into account blocks too...
        selected_rows = self.grid.GetSelectedRows()
        top_left_blocks = self.grid.GetSelectionBlockTopLeft()
        bottom_right_blocks = self.grid.GetSelectionBlockBottomRight()
        for count in range( len(top_left_blocks) ):
            start_index = top_left_blocks[count][0]
            end_index = bottom_right_blocks[count][0] 
            for index in range(start_index, end_index + 1):
                selected_rows.append(index)
        item_list = []
        for index in selected_rows:
            item_list.append( self.table.items[index] )
        return item_list

    def __has_selected_items(self):
        return len( self.get_selected_items() )


class SoundRecordDetailsPanel(scrolledpanel.ScrolledPanel):
    def __init__(self, parent, sound_library):
        scrolledpanel.ScrolledPanel.__init__(
            self,
            parent
            )
        self.sound_library = sound_library
        self.sound_records = []
        text_box_size = (65, -1)
        self.label_value_sound_id = wx.StaticText(
                                                self, 
                                                -1, 
                                                "",
                                                style=wx.ST_NO_AUTORESIZE
                                                )
        self.label_value_sound_id.SetMinSize( (20, 20) )
        self.label_value_sound_id.SetBackgroundColour(
            cfg.HEADER_BACKGROUND_COLOUR
            )
        self.staticbox_general_group = wx.StaticBox(self, -1, "")
        self.label_volume =  wx.StaticText(
                                        self, 
                                        -1, 
                                        "Volume"
                                        )
        self.fslider_volume = floatslider.FloatSlider(
                                        self,
                                        -1,
                                        "", 
                                        100.0, 
                                        0.0, 
                                        200.0, 
                                        precision = 1.0
                                        )
        self.label_volume_var = wx.StaticText(self, -1, "Volume variation")
        self.text_volume_var = floatctrl.FloatCtrl(
                                        self, 
                                        -1, 
                                        size=text_box_size,
                                        style=wx.TE_RIGHT
                                        )
        self.label_pitch_var = wx.StaticText(self, -1, "Pitch variation")
        self.text_pitch_var = floatctrl.FloatCtrl(
                                        self, 
                                        -1, 
                                        size=text_box_size, 
                                        style=wx.TE_RIGHT
                                        )
        self.check_box_mute = wx.CheckBox(self, -1, "Mute")
        self.check_box_solo = wx.CheckBox(self, -1, "Solo")
        self.label_min_dist = wx.StaticText(self, -1, "Minimum distance")
        self.text_min_dist = floatctrl.FloatCtrl(
                                        self, 
                                        -1, 
                                        size=text_box_size, 
                                        style=wx.TE_RIGHT
                                        )
        self.label_max_dist = wx.StaticText(self, -1, "Maximum distance")
        self.text_max_dist = floatctrl.FloatCtrl(
                                        self, 
                                        -1, 
                                        size=text_box_size, 
                                        style=wx.TE_RIGHT
                                        )
        self.check_box_streamed = wx.CheckBox(self, -1, "Streamed")
        self.check_box_ambient = wx.CheckBox(self, -1, "Ambient")
        self.label_delay = wx.StaticText(self, -1, "Delay")
        self.text_delay = floatctrl.FloatCtrl(
                                        self, 
                                        -1, 
                                        size=text_box_size, 
                                        style=wx.TE_RIGHT
                                        )
        self.label_max_instances = wx.StaticText(self, -1, "Max instances")
        self.spin_max_instances = spin.ConjurerSpinCtrl(
                                                self, 
                                                -1, 
                                                "", 
                                                min=1, 
                                                max=1000,
                                                size=text_box_size
                                                )
        self.check_box_pitch_control = wx.CheckBox(self, -1, "Pitch control")
        self.check_box_mobile = wx.CheckBox(self, -1, "Mobile")
        self.label_priority = wx.StaticText(self, -1, "Priority")
        self.spin_priority = spin.ConjurerSpinCtrl(
                                    self, 
                                    -1, 
                                    "", 
                                    min=1, 
                                    max=1000,
                                    size=text_box_size
                                    )
        self.staticbox_resource_group = wx.StaticBox(self, -1, "Sound file")
        self.label_value_resource = wx.StaticText(
                                                self, 
                                                -1,
                                                style=wx.SUNKEN_BORDER | 
                                                            wx.ST_NO_AUTORESIZE
                                                )
        self.button_resource = wx.Button(
                                                    self, 
                                                    -1, 
                                                    "&Browse...", 
                                                    style=wx.BU_EXACTFIT
                                                    )
        self.__set_properties()
        self.__do_layout()
        self.__bind_events()

    def __set_properties(self):
        self.SetScrollRate(10, 10)
        if len(self.sound_records) == 0:
            self.Enable(False)
        else:
            self.Enable(True)
        self.__update_label_value_sound_id()
        self.__update_fslider_volume()
        self.__update_check_box_mute()
        self.__update_check_box_solo()
        self.__update_check_box_streamed()
        self.__update_check_box_ambient()
        self.__update_check_box_pitch_control()
        self.__update_check_box_mobile()
        self.__update_volume_var()
        self.__update_pitch_var()
        self.__update_min_dist()
        self.__update_max_dist()
        self.__update_delay()
        self.__update_max_instances()
        self.__update_priority()
        self.__update_label_value_resource()

    def __update_label_value_sound_id(self):
        number_of_sounds = len(self.sound_records)
        if number_of_sounds == 1:
            string_to_show = self.sound_records[0].get_sound_id()
        elif number_of_sounds == 0:
            string_to_show = ''
        else:
            string_to_show = '*Multi*'
        self.label_value_sound_id.SetLabel( ''.join(['  ', string_to_show]) )

    def __update_fslider_volume(self):
        number_of_sounds = len(self.sound_records)
        if number_of_sounds == 1:
            value_to_use = self.sound_records[0].get_volume()
        else:
            value_to_use = 100.0
        self.fslider_volume.set_value(value_to_use)

    def __update_label_value_resource(self):
        number_of_sounds = len(self.sound_records)
        if number_of_sounds == 1:
            string_to_show = self.sound_records[0].get_resource()
            self.staticbox_resource_group.Enable(True)
            self.button_resource.Enable(True)
        else:
            string_to_show = ''
            self.staticbox_resource_group.Enable(False)
            self.button_resource.Enable(False)
        self.label_value_resource.SetLabel(string_to_show)

    def __update_check_box_solo(self):
        #should set to true if ALL true, otherwise false
        number_of_sounds = len(self.sound_records)
        if number_of_sounds == 1:
            value_to_use = self.sound_records[0].get_solo()
        else:
            value_to_use = False
        self.check_box_solo.SetValue(value_to_use)

    def __update_check_box_mute(self):
        #should set to true if ALL true, otherwise false
        number_of_sounds = len(self.sound_records)
        if number_of_sounds == 1:
            value_to_use = self.sound_records[0].get_mute()
        else:
            value_to_use = False
        self.check_box_mute.SetValue(value_to_use)

    def __update_check_box_streamed(self):
        #should set to true if ALL true, otherwise false
        number_of_sounds = len(self.sound_records)
        if number_of_sounds == 1:
            value_to_use = self.sound_records[0].get_streamed()
        else:
            value_to_use = False
        self.check_box_streamed.SetValue(value_to_use)

    def __update_check_box_ambient(self):
        #should set to true if ALL true, otherwise false
        number_of_sounds = len(self.sound_records)
        if number_of_sounds == 1:
            value_to_use = self.sound_records[0].get_ambient()
        else:
            value_to_use = False
        self.check_box_ambient.SetValue(value_to_use) 

    def __update_check_box_pitch_control(self):
        #should set to true if ALL true, otherwise false
        number_of_sounds = len(self.sound_records)
        if number_of_sounds == 1:
            value_to_use = self.sound_records[0].get_pitch_control()
        else:
            value_to_use = False
        self.check_box_pitch_control.SetValue(value_to_use)

    def __update_check_box_mobile(self):
        #not a multi one
        number_of_sounds = len(self.sound_records)
        if number_of_sounds == 1:
            value_to_use = self.sound_records[0].get_mobile()
            self.check_box_mobile.Enable(True)
        else:
            value_to_use = False
            self.check_box_mobile.Enable(False)            
        self.check_box_mobile.SetValue(value_to_use)

    def __update_volume_var(self):
        number_of_sounds = len(self.sound_records)
        if number_of_sounds == 1:
            value_to_use = self.sound_records[0].get_volume_var()
        else:
            value_to_use = 0.0
        self.text_volume_var.set_value(value_to_use, False)

    def __update_pitch_var(self):
        number_of_sounds = len(self.sound_records)
        if number_of_sounds == 1:
            value_to_use = self.sound_records[0].get_pitch_var()
            self.text_pitch_var.Enable(True)
        else:
            value_to_use = 0.0
            self.text_pitch_var.Enable(False)
        self.text_pitch_var.set_value(value_to_use, False)

    def __update_min_dist(self):
        number_of_sounds = len(self.sound_records)
        if number_of_sounds == 1:
            value_to_use = self.sound_records[0].get_min_dist()
            self.text_min_dist.Enable(True)
        else:
            value_to_use = 0.0
            self.text_min_dist.Enable(False)
        self.text_min_dist.set_value(value_to_use, False)

    def __update_max_dist(self):
        number_of_sounds = len(self.sound_records)
        if number_of_sounds == 1:
            value_to_use = self.sound_records[0].get_max_dist()
            self.text_max_dist.Enable(True)
        else:
            value_to_use = 0.0
            self.text_max_dist.Enable(False)
        self.text_max_dist.set_value(value_to_use, False)

    def __update_delay(self):
        number_of_sounds = len(self.sound_records)
        if number_of_sounds == 1:
            value_to_use = self.sound_records[0].get_delay()
        else:
            value_to_use = 0.0
        self.text_delay.set_value(value_to_use, False)

    def __update_max_instances(self):
        number_of_sounds = len(self.sound_records)
        if number_of_sounds == 1:
            value_to_use = self.sound_records[0].get_max_instances()
        else:
            value_to_use = 1
        self.spin_max_instances.silent_set_value(value_to_use)

    def __update_priority(self):
        number_of_sounds = len(self.sound_records)
        if number_of_sounds == 1:
            value_to_use = self.sound_records[0].get_priority()
        else:
            value_to_use = 1
        self.spin_priority.silent_set_value(value_to_use)

    def set_sound_records(self, sound_records):
        self.sound_records = sound_records
        self.__set_properties()

    def __do_layout(self):
        border_width = cfg.BORDER_WIDTH
        sizer = wx.BoxSizer(wx.VERTICAL)
        sizer_sound_id = wx.BoxSizer(wx.HORIZONTAL)
        sizer_sound_id.Add(
            self.label_value_sound_id, 
            1, 
            wx.EXPAND | wx.ALIGN_CENTER_VERTICAL
            )
        sizer_general_group = wx.StaticBoxSizer(
                                            self.staticbox_general_group, 
                                            wx.VERTICAL
                                            )
        grid_sizer = wx.FlexGridSizer(
                            4, 
                            6, 
                            border_width, 
                            border_width * 2
                            )
        grid_sizer.AddGrowableCol(1)
        grid_sizer.Add(
            self.label_volume, 
            0, 
            cfg.LABEL_ALIGN | wx.ALIGN_CENTER_VERTICAL | wx.ADJUST_MINSIZE, 
            0
            )
        grid_sizer.Add(
            self.fslider_volume, 
            0, 
            cfg.LABEL_ALIGN | wx.ALIGN_CENTER_VERTICAL | wx.EXPAND,
            0
            )
        grid_sizer.Add(
            self.label_volume_var, 
            0, cfg.LABEL_ALIGN | wx.ALIGN_CENTER_VERTICAL | wx.ADJUST_MINSIZE | wx.LEFT, 
            border_width
            )
        grid_sizer.Add(
            self.text_volume_var, 
            0, 
            wx.ADJUST_MINSIZE
            )
        grid_sizer.Add(
            self.label_pitch_var, 
            0, 
            cfg.LABEL_ALIGN | wx.ALIGN_CENTER_VERTICAL | wx.ADJUST_MINSIZE | wx.LEFT,
            border_width
            )
        grid_sizer.Add(
            self.text_pitch_var, 
            0, 
            wx.ADJUST_MINSIZE
            )
        # next line...
        grid_sizer.Add(
            self.check_box_mute, 
            0
            )
        grid_sizer.Add(
            self.check_box_solo, 
            0
            )
        grid_sizer.Add(
            self.label_min_dist, 
            0, 
            cfg.LABEL_ALIGN | wx.ALIGN_CENTER_VERTICAL | wx.ADJUST_MINSIZE | wx.LEFT,
            border_width
            )
        grid_sizer.Add(
            self.text_min_dist, 
            0, 
            wx.ADJUST_MINSIZE
            )
        grid_sizer.Add(
            self.label_max_dist, 
            0, 
            cfg.LABEL_ALIGN | wx.ALIGN_CENTER_VERTICAL | wx.ADJUST_MINSIZE | wx.LEFT,
            border_width
            )
        grid_sizer.Add(
            self.text_max_dist, 
            0, 
            wx.ADJUST_MINSIZE
            )
        # next line...
        grid_sizer.Add(
            self.check_box_streamed, 
            0
            )
        grid_sizer.Add(
            self.check_box_ambient, 
            0
            )
        grid_sizer.Add(
            self.label_delay, 
            0, 
            cfg.LABEL_ALIGN | 
                wx.ALIGN_CENTER_VERTICAL | 
                wx.ADJUST_MINSIZE | 
                wx.LEFT,
            border_width
            )
        grid_sizer.Add(
            self.text_delay, 
            0, 
            wx.ADJUST_MINSIZE
            )
        grid_sizer.Add(
            self.label_max_instances, 
            0, 
            cfg.LABEL_ALIGN | wx.ALIGN_CENTER_VERTICAL | wx.ADJUST_MINSIZE | wx.LEFT,
            border_width
            )
        grid_sizer.Add(
            self.spin_max_instances, 
            0, 
            wx.ADJUST_MINSIZE
            )
        # next line...
        grid_sizer.Add(
            self.check_box_pitch_control, 
            0
            )
        grid_sizer.Add(
            self.check_box_mobile,
            0
            )
        grid_sizer.Add(
            self.label_priority, 
            0, 
            cfg.LABEL_ALIGN | wx.ALIGN_CENTER_VERTICAL | wx.ADJUST_MINSIZE | wx.LEFT,
            border_width
            )
        grid_sizer.Add(
            self.spin_priority, 
            0, 
            wx.ADJUST_MINSIZE
            )
        grid_sizer.AddSpacer( (0, 0) )
        grid_sizer.AddSpacer( (0, 0) )
        sizer_general_group.Add(
            grid_sizer,
            0, 
            wx.EXPAND | wx.ALL,
            border_width
            )
        sizer_resource_group = wx.StaticBoxSizer(
                                            self.staticbox_resource_group, 
                                            wx.VERTICAL
                                            )
        sizer_resource = wx.BoxSizer(wx.HORIZONTAL)
        sizer_resource.Add(
            self.label_value_resource, 
            1, 
            wx.EXPAND | wx.ALIGN_CENTER_VERTICAL | wx.LEFT,
            border_width
            )
        sizer_resource.Add(
            self.button_resource, 
            0, 
            wx.ADJUST_MINSIZE | wx.LEFT,
            border_width
            )
        sizer_resource_group.Add(
            sizer_resource,
            0, 
            wx.EXPAND | wx.ALL,
            border_width
            )
        sizer.Add(
            sizer_sound_id, 
            0, 
            wx.EXPAND | wx.ALL, 
            border_width
            )
        sizer.Add(
            sizer_general_group, 
            0, 
            wx.LEFT | wx.RIGHT | wx.EXPAND, 
            border_width
            )
        sizer.Add(
            sizer_resource_group,
            0, 
            wx.EXPAND | wx.LEFT | wx.RIGHT, 
            border_width
            )
        self.SetSizer(sizer)
        sizer.Fit(self)
        sizer.SetSizeHints(self)
        self.SetSize(self.GetVirtualSize() )
        self.Layout()

    def __bind_events(self):
        self.Bind(
            wx.EVT_CHECKBOX,
            self.__on_check_box_solo,
            self.check_box_solo
            )
        self.Bind(
            wx.EVT_CHECKBOX,
            self.__on_check_box_mute,
            self.check_box_mute
            )
        self.Bind(
            wx.EVT_CHECKBOX,
            self.__on_check_box_streamed,
            self.check_box_streamed
            )
        self.Bind(
            wx.EVT_CHECKBOX,
            self.__on_check_box_ambient,
            self.check_box_ambient
            )
        self.Bind(
            wx.EVT_CHECKBOX,
            self.__on_check_box_pitch_control,
            self.check_box_pitch_control
            )
        self.Bind(
            wx.EVT_CHECKBOX,
            self.__on_check_box_mobile,
            self.check_box_mobile
            )
        self.Bind(
            events.EVT_CHANGING, 
            self.__on_changing_volume, 
            self.fslider_volume
            )
        self.Bind(
            events.EVT_CHANGED, 
            self.__on_changed_volume,
            self.fslider_volume
            )
        self.Bind(
            floatctrl.EVT_FLOAT,
            self.__on_changed_volume_var,
            self.text_volume_var
            )
        self.Bind(
            floatctrl.EVT_FLOAT,
            self.__on_changed_pitch_var,
            self.text_pitch_var
            )        
        self.Bind(
            floatctrl.EVT_FLOAT,
            self.__on_changed_min_dist,
            self.text_min_dist
            )
        self.Bind(
            floatctrl.EVT_FLOAT,
            self.__on_changed_max_dist,
            self.text_min_dist
            )
        self.Bind(
            floatctrl.EVT_FLOAT,
            self.__on_changed_delay,
            self.text_delay
            )        
        self.Bind(
            wx.EVT_SPINCTRL,
            self.__on_changed_max_instances,
            self.spin_max_instances
            )
        self.Bind(
            wx.EVT_TEXT,
            self.__on_changed_max_instances,
            self.spin_max_instances
            )
        self.Bind(
            wx.EVT_SPINCTRL,
            self.__on_changed_priority,
            self.spin_priority
            )
        self.Bind(
            wx.EVT_TEXT,
            self.__on_changed_priority,
            self.spin_priority
            )
        self.Bind(
            wx.EVT_BUTTON,
            self.__on_button_resource,
            self.button_resource
            )

    def __on_button_resource(self, event):
        mangled_file = format.mangle_path( self.sound_records[0].get_resource() )
        dlg = wx.FileDialog(
            self, 
            message="Choose a sound file",
            defaultFile = os.path.realpath(mangled_file),
            wildcard="WAV files (*.wav)|*.wav|OGG files (*.ogg)|*.ogg",
            style=wx.OPEN
            )
        if dlg.ShowModal() == wx.ID_OK:
            path_string =  dlg.GetPath() 
            if not self.__is_valid_resource_name(path_string):
                msg = "You must choose a sound file from %s" % self.__get_mangled_sound_root_dir()
                cjr.show_error_message(msg)
            else:
                new_string = format.get_relative_path(
                                    self.__get_mangled_sound_root_dir(), 
                                    path_string
                                    )
                self.__set_resource(new_string)
        dlg.Destroy()
        event.Skip()

    def __set_resource(self, path_string):
        string_to_use = ''.join( [
                                    self.sound_library.root_directory, 
                                    "/", 
                                    path_string
                                    ]
                                    )
        self.label_value_resource.SetLabel(string_to_use)
        self.__on_changed_resource(string_to_use)

    def __on_changed_resource(self, path_string):
        for each_sound in self.sound_records:
            each_sound.set_resource(path_string)
        self.__signal_records_amended()

    def __get_mangled_sound_root_dir(self):
        return self.sound_library.get_mangled_root_directory()

    def __is_valid_resource_name(self, resource_path):
        mangled_path = format.mangle_path( resource_path )
        mangled_sound_directory = self.__get_mangled_sound_root_dir()
        return mangled_path.startswith(mangled_sound_directory)

    def __on_changing_volume(self, event):
        for each_record in self.sound_records:
            each_record.set_volume( event.get_value() )
        self.__signal_records_amended()
        event.Skip()
        
    def __on_changed_volume(self, event):
        #new_value = event.get_value()
        #old_value = event.get_old_value()
        #for each_record in self.sound_records:
        #    each_record.set_volume(
        #        each_record.get_volume() + (new_value - old_value) )
        pass

    def __on_check_box_solo(self, event):
        for each_sound in self.sound_records:
            each_sound.set_solo( event.IsChecked() )
        self.__signal_records_amended()
        event.Skip()

    def __on_check_box_mute(self, event):
        for each_sound in self.sound_records:
            each_sound.set_mute( event.IsChecked() )
        self.__signal_records_amended()
        event.Skip()

    def __on_check_box_streamed(self, event):
        for each_sound in self.sound_records:
            each_sound.set_streamed( event.IsChecked() )
        self.__signal_records_amended()
        event.Skip()

    def __on_check_box_ambient(self, event):
        for each_sound in self.sound_records:
            each_sound.set_ambient( event.IsChecked() )
        self.__signal_records_amended()
        event.Skip()

    def __on_check_box_pitch_control(self, event):
        for each_sound in self.sound_records:
            each_sound.set_pitch_control( event.IsChecked() )
        self.__signal_records_amended()
        event.Skip()

    def __on_check_box_mobile(self, event):
        for each_sound in self.sound_records:
            each_sound.set_mobile( event.IsChecked() )
        self.__signal_records_amended()
        event.Skip()

    def __on_changed_volume_var(self, event):
        for each_sound in self.sound_records:
            each_sound.set_volume_var( event.get_value() )
        self.__signal_records_amended()
        event.Skip()

    def __on_changed_pitch_var(self, event):
        for each_sound in self.sound_records:
            each_sound.set_pitch_var( event.get_value() )
        self.__signal_records_amended()
        event.Skip()

    def __on_changed_min_dist(self, event):
        for each_sound in self.sound_records:
            each_sound.set_min_dist( event.get_value() )
        self.__signal_records_amended()
        event.Skip()

    def __on_changed_max_dist(self, event):
        for each_sound in self.sound_records:
            each_sound.set_max_dist( event.get_value() )
        self.__signal_records_amended()
        event.Skip()

    def __on_changed_delay(self, event):
        for each_sound in self.sound_records:
            each_sound.set_delay( event.get_value() )
        self.__signal_records_amended()
        event.Skip()

    def __on_changed_max_instances(self, event):
        for each_sound in self.sound_records:
            each_sound.set_max_instances( self.spin_max_instances.GetValue() )
        self.__signal_records_amended()
        event.Skip()

    def __on_changed_priority(self, event):
        for each_sound in self.sound_records:
            each_sound.set_priority( self.spin_priority.GetValue() )
        self.__signal_records_amended()
        event.Skip()

    def __signal_records_amended(self):
        custom_event = wx.PyCommandEvent(
                                soundEVT_SOUND_RECORDS_AMENDED, 
                                self.GetId()
                                )
        self.GetEventHandler().ProcessEvent(
            custom_event
            )


class SoundLibraryPanel(wx.Panel):
    """Panel to manage the sound library"""
    ID_SASH_WINDOW_TREE = 5101
    ID_SASH_WINDOW_DETAILS = 5102
    def __init__(self, parent, sound_library):
        wx.Panel.__init__(
            self, 
            parent, 
            -1
            )
        self.sound_library = sound_library
        self.tree_view_sash = wx.SashLayoutWindow(
                                        self, 
                                        self.ID_SASH_WINDOW_TREE
                                        )
        self.tree_view_panel = SoundLibraryTreeViewPanel(
                                            self.tree_view_sash, 
                                            self.sound_library
                                            )
        self.list_panel = SoundLibraryListViewPanel(
                                self, 
                                self.sound_library
                                )
        self.details_sash = wx.SashLayoutWindow(
                                    self, 
                                    self.ID_SASH_WINDOW_DETAILS, 
                                    style = wx.BORDER_SUNKEN
                                        )
        self.details_panel = SoundRecordDetailsPanel(
                                self.details_sash,
                                self.sound_library
                                )
        self.__set_properties()
        self.__layout_window()
        self.__bind_events()

    def __set_properties(self):
        self.tree_view_sash.SetOrientation(wx.LAYOUT_VERTICAL)
        self.tree_view_sash.SetAlignment(wx.LAYOUT_LEFT)
        self.tree_view_sash.SetSashVisible(wx.SASH_RIGHT, True)
        ideal_width = self.tree_view_sash.GetBestSize()[0]
        self.tree_view_sash.SetDefaultSize(
            (ideal_width + (cfg.BORDER_WIDTH * 2),
            -1)
            )
        self.details_sash.SetOrientation(wx.LAYOUT_HORIZONTAL)
        self.details_sash.SetAlignment(wx.LAYOUT_BOTTOM)
        self.details_sash.SetSashVisible(wx.SASH_TOP, True)
        ideal_height = self.details_sash.GetBestSize()[1]
        self.details_sash.SetDefaultSize( 
            ( -1, ideal_height )
            )
        self.details_sash.SetMaxSize( 
            ( -1, ideal_height )
            )

    def __bind_events(self):
        self.Bind(
            wx.EVT_SASH_DRAGGED_RANGE, 
            self.__on_sash_drag, 
            id =self.ID_SASH_WINDOW_TREE,
            id2 = self.ID_SASH_WINDOW_DETAILS
            )
        self.Bind(
            wx.EVT_SIZE, 
            self.__on_size
            )
        self.Bind(
            EVT_DIRECTORY_CHANGED,
            self.__on_change_directory,
            self.tree_view_panel
            )
        self.Bind(
            EVT_SELECTED_RECORDS_CHANGED,
            self.__on_change_selected_records,
            self.list_panel
            )
        self.Bind(
            EVT_SOUND_RECORDS_AMENDED,
            self.__on_amend_sound_records,
            self.details_panel
            )

    def __on_change_directory(self, event):
        node_item = event.get_value()
        self.list_panel.show_sounds_for_node(node_item.directory)

    def __on_change_selected_records(self, event):
        selected_records = event.get_value()
        self.details_panel.set_sound_records(selected_records)

    def __on_amend_sound_records(self, event):
        self.list_panel.refresh_all_rows()
        event.Skip()

    def __on_sash_drag(self, event):
        if event.GetDragStatus() == wx.SASH_STATUS_OUT_OF_RANGE:
            return
        event_id = event.GetId()
        if event_id == self.ID_SASH_WINDOW_TREE:
            self.tree_view_sash.SetDefaultSize(
                (event.GetDragRect().width,
                -1)
                )
        elif event_id == self.ID_SASH_WINDOW_DETAILS:
            max_height = self.details_sash.GetMaxSize()[1]
            new_height = event.GetDragRect().height
            self.details_sash.SetDefaultSize(
                (-1, 
                min(new_height, max_height) )
                )
        self.__layout_window()
        event.Skip()

    def __on_size(self, event):
        self.__layout_window()
        event.Skip()

    def show_or_hide_tree_view_sash(self):
        """If the sash is currently visible, then hide it, 
        otherwise show it."""
        if self.tree_view_sash.IsShown():
            self.hide_tree_view_sash()
        else:
            self.show_tree_view_sash()

    def __layout_window(self):
        wx.LayoutAlgorithm().LayoutWindow(self, self.list_panel)    
    
    def show_tree_view_sash(self):
        self.tree_view_sash.Show()
        self.__layout_window()
   
    def hide_tree_view_sash(self):
        self.tree_view_sash.Hide()
        self.__layout_window()

    def update_list_view(self):
        self.list_panel.show_sounds_for_current_node()


# SoundLibraryDialog class
class SoundLibraryDialog(togwin.ChildToggableDialog):
    """Dialog to manage the sound library"""
    def __init__(self, parent):
        togwin.ChildToggableDialog.__init__(
            self, 
            "Sound library", 
            parent 
            )
        self.sound_library = soundlib.SoundLibrary()
        self.main_panel = SoundLibraryPanel(
                                    self, 
                                    self.sound_library
                                    )
        self.button_save_library = wx.Button(
                                                self, 
                                                -1, 
                                                "&Save library"
                                                )
        self.button_stop_all = wx.Button(
                                        self, 
                                        -1, 
                                        "S&top All"
                                        )
        self.button_refresh_files = wx.Button(
                                                self, 
                                                -1, 
                                                "Scan A&ll for New Files"
                                                )
        self.button_clear_mutesolo = wx.Button(
                                                    self, 
                                                    -1, 
                                                    "Clear &Mute/Solo"
                                                    )
        self.button_filter_by_selection = wx.ToggleButton(
                                                        self, 
                                                        -1, 
                                                        "&Filter by Selection"
                                                        )
        self.button_close = wx.Button(
                                        self, 
                                        wx.ID_CANCEL, 
                                        "&Close"
                                        )
        self.__set_properties()
        self.__do_layout()
        self.__bind_events()

    def __set_properties(self):
        self.__update_buttons_and_title()

    def __do_layout(self):
        sizer = wx.BoxSizer(wx.VERTICAL)
        button_sizer = wx.BoxSizer(wx.HORIZONTAL)
        button_sizer.Add(self.button_save_library, 0)
        button_sizer.Add(
            self.button_stop_all, 
            0, 
            wx.LEFT, 
            cfg.BORDER_WIDTH
            )
        button_sizer.Add(
            self.button_refresh_files, 
            0, 
            wx.LEFT, 
            cfg.BORDER_WIDTH
            )
        button_sizer.Add(
            self.button_clear_mutesolo, 
            0, 
            wx.LEFT, 
            cfg.BORDER_WIDTH)
        button_sizer.Add(
            self.button_filter_by_selection, 
            0, 
            wx.LEFT, 
            cfg.BORDER_WIDTH
            )
        button_sizer.Add(
            self.button_close, 
            0, 
            wx.LEFT, 
            cfg.BORDER_WIDTH
            )
        sizer.Add(self.main_panel, 1, wx.EXPAND)
        horizontal_line = wx.StaticLine(
                                    self, 
                                    -1, 
                                    (-1, -1), 
                                    (-1, -1), 
                                    wx.LI_HORIZONTAL 
                                    )
        sizer.Add(horizontal_line, 0, wx.EXPAND)
        sizer.Add(
            button_sizer, 
            0, 
            wx.ALIGN_RIGHT | wx.ALL, 
            cfg.BORDER_WIDTH
            )
        self.SetSizer(sizer)

    def __bind_events(self):
        pynebula.pyBindSignal(
            app.get_object_state(), 
            'selectionchanged',
            self, 
            'nebulaselectionchanged', 
            0 
            )
        self.Bind(
            wx.EVT_BUTTON, 
            self.__on_save_library, 
            self.button_save_library
            )
        self.Bind(
            wx.EVT_BUTTON, 
            self.__on_stop_all, 
            self.button_stop_all
            )
        self.Bind(
            wx.EVT_BUTTON, 
            self.__on_refresh_files, 
            self.button_refresh_files
            )
        self.Bind(
            wx.EVT_BUTTON, 
            self.__on_clear_mutesolo, 
            self.button_clear_mutesolo
            )
        self.Bind(
            wx.EVT_TOGGLEBUTTON, 
            self.__on_filter_by_selection, 
            self.button_filter_by_selection
            )
        self.Bind(
            wx.EVT_BUTTON, 
            self.__on_click_close_button,
            self.button_close
            )
        self.Bind(
            EVT_SOUND_LIBRARY_AMENDED,
            self.__on_sound_library_amended
            )
        self.Bind(
            EVT_SOUND_RECORDS_AMENDED,
            self.__on_sound_records_amended
            )

    def __on_sound_records_amended(self, event):
        self.__update_buttons_and_title()

    def __on_sound_library_amended(self, event):
        self.__update_buttons_and_title()
    
    def __update_buttons_and_title(self):
        self.__update_title()
        self.__update_buttons()

    def __on_clear_mutesolo(self, event):
        try:
            wx.BeginBusyCursor()
            self.sound_library.clear_mute_and_solo_on_all()
        finally:
            wx.EndBusyCursor()
        self.Refresh()

    def __on_filter_by_selection(self, event):
        enable = event.GetSelection() == 1
        self.sound_library.set_filter_enabled(enable)
        if enable:
            self.__update_filter()
        self.__update_list_view()
        self.__update_buttons_and_title()
        event.Skip()
        
    def nebulaselectionchanged(self):
        if self.sound_library.filter_enabled:
            self.__update_filter()
            self.__update_list_view()
            self.__update_buttons_and_title()
        
    def __update_filter(self):
        sound_seq_no_list = []
        class_dictionary = dict()
        lib = servers.get_sound_library()
        object_state = app.get_object_state()
        object_server = servers.get_entity_object_server()
        class_server = servers.get_entity_class_server()
        for index in range(object_state.getselectioncount()):
            each_entity = object_state.getselectedentity(index)
            entity_class = each_entity.getentityclass()
            class_dictionary[entity_class] = True
        for each_class in class_dictionary.keys():
            if each_class.hascomponent("ncSoundClass"):
                sound_table = each_class.getsoundtable()
                if sound_table is not None:
                    for row_index in range( sound_table.getnumberofsounds() ):
                        event, material, sound_id = sound_table.getsoundevent(
                                                                row_index
                                                                )
                        sound_index = lib.getsoundindex( sound_id )
                        num_var = lib.getsoundnumberofvariations(sound_index)
                        if num_var > 0:
                            for each_var in range(num_var):
                                sound_seq_no_list.append(
                                    sound_index + each_var
                                    )
        self.sound_library.set_filter_from_list(sound_seq_no_list)

    def __on_save_library(self, event):
        try:
            wx.BeginBusyCursor()
            self.sound_library.save_library()
            app.get_top_window().emit_app_event(
                events.SoundLibSaved()
                )
        finally:
            wx.EndBusyCursor()

    def __on_stop_all(self, event):
        try:
            wx.BeginBusyCursor()
            self.sound_library.stop_all_sounds()
        finally:
            wx.EndBusyCursor()
    
    def __on_refresh_files(self, event):
        msg = (
            "Scanning all directories for new files can take some time. " \
            "\nAre you sure you want to continue?"
            )
        answer = cjr.confirm_yes_no(self, msg)
        if answer == wx.ID_YES:
            self.refresh_files()

    def refresh_files(self):
        try:
            wx.BeginBusyCursor()
            self.sound_library.refresh_all_files()
            app.get_libraries().emit('objectchanges')
            self.__update_list_view()
            self.__update_buttons_and_title()
        finally:
            wx.EndBusyCursor()

    def __on_click_close_button(self, event):
        self.Close()

    def on_app_event(self, event):
        if isinstance(event, events.SoundLibSaved):
            self.__update_buttons_and_title()

    def __update_buttons(self):
        should_enable = self.sound_library.has_changes_pending()
        self.button_save_library.Enable(should_enable)

    def __update_title(self):
        title = "Sound library"
        if self.sound_library.has_changes_pending():
            title = title + "*"
        self.SetTitle( title )

    def __update_list_view(self):
        self.main_panel.update_list_view()

    def __del__(self):
        pynebula.pyUnbindTargetObject(
            app.get_object_state(), 
            'selectionchanged', 
            self
            )

    def persist(self):
        return [
            create_window,
            ()  # no parameters for create function
            ]

# create_window function
def create_window(parent):
    return SoundLibraryDialog(parent)

