#-------------------------------------------------------------------------
# Conjurer Services, S.A. (2005)
#-------------------------------------------------------------------------

#-------------------------------------------------------------------------
# Imports
#-------------------------------------------------------------------------
"""
Posible table messages:

wxGRIDTABLE_REQUEST_VIEW_GET_VALUES,
wxGRIDTABLE_REQUEST_VIEW_SEND_VALUES,
wxGRIDTABLE_NOTIFY_ROWS_INSERTED,
wxGRIDTABLE_NOTIFY_ROWS_APPENDED,
wxGRIDTABLE_NOTIFY_ROWS_DELETED,
wxGRIDTABLE_NOTIFY_COLS_INSERTED,
wxGRIDTABLE_NOTIFY_COLS_APPENDED,
wxGRIDTABLE_NOTIFY_COLS_DELETED
"""

import wx
import wx.grid as gridlib

import pynebula
import togwin
import freedialoggui
import re
import servers
import objdlg
import app
import propertyview
import scriptingmodelmultiobject as scripting
import expentdlg
import importentity
import conjurerconfig as cfg
import conjurerframework as cjr
import format


class CustomDataTable(gridlib.PyGridTableBase):
    def __init__(self, grid, data):
        gridlib.PyGridTableBase.__init__(self)
        self.grid = grid
        self.col_labels = [
            'ID', 
            'HexID', 
            'Class name', 
            'Name', 
            'Layer', 
            'Dirty'
            ]
        self.data = data
        self.data_types = [
            gridlib.GRID_VALUE_NUMBER,
            gridlib.GRID_VALUE_STRING,
            gridlib.GRID_VALUE_STRING,
            gridlib.GRID_VALUE_STRING,
            gridlib.GRID_VALUE_STRING,
            gridlib.GRID_VALUE_STRING
            ]
        
    #--------------------------------------------------
    # required methods for the wxPyGridTableBase interface

    def GetNumberRows(self):
        return len(self.data)

    def GetNumberCols(self):
        return len(self.col_labels)

    def IsEmptyCell(self, row, col):
        try:
            return not self.data[row][col]
        except IndexError:
            return True
    
    def GetValue(self, row, col):
        try:
            return self.data[row][col]
        except IndexError:
            return ''

    def SetValue(self, row, col, value):
        try:
            self.data[row][col] = value
        except IndexError:
            # add a new row
            self.data.append([''] * self.GetNumberCols())
            self.SetValue(row, col, value)

            # tell the grid we've added a row
            #parameters are: the table, what we did to it, how many
            msg = gridlib.GridTableMessage(
                self,
                gridlib.GRIDTABLE_NOTIFY_ROWS_APPENDED,
                1
                )

    #--------------------------------------------------
    # Some optional methods

    def GetColLabelValue(self, col):
        return self.col_labels[col]

    def GetTypeName(self, row, col):
        return self.data_types[col]

    def CanGetValueAs(self, row, col, type_name):
        col_type = self.data_types[col].split(':')[0]
        if type_name == col_type:
            return True
        else:
            return False

    def CanSetValueAs(self, row, col, type_name):
        return self.CanGetValueAs(row, col, type_name)

    def AppendRows (self, num_rows = 1):
        return

    def InsertRows (self, pos, num_row = 1):
        grid = self.GetView()
        if grid:
            row = []
            self.data.insert(pos-1, row)
            # tell the grid we've inserted a row
            #parameters are: the table, what we did to it, how many    
            msg = gridlib.GridTableMessage(
                self,              
                gridlib.GRIDTABLE_NOTIFY_ROWS_INSERTED, 
                pos,
                1                                            
                )
            grid.ProcessTableMessage(msg)

    def SetView (self, data):
        self.data = data

    def DeleteRows (self, pos, num_rows = 1):
        grid = self.GetView()
        if grid:
            self.data.pop(pos)
            # tell the grid we've deleted a row
            #the parameters are: the table, what we did to it, how many
            msg = gridlib.GridTableMessage(
                self,                
                gridlib.GRIDTABLE_NOTIFY_ROWS_DELETED, 
                pos,
                1
                )
            grid.ProcessTableMessage(msg)            


class FastLayerNameGopher:
    """A small utility class for getting layer names fast.
    It holds a layer_manager instance for getting the 
    layer name and caches names against ids in a 
    dictionary to speed up access.
    Give it a layer id and it will return the appropriate string"""
    def __init__(self):
        self.layer_manager = servers.get_layer_manager()
        #initialize the dictionary with the default value
        self.layer_name_dictionary = {cfg.NO_LAYER_ID : "None"}

    def get_name_for_layer_with_id(self, layer_id):
        try:
            layer_name = self.layer_name_dictionary[layer_id]
        except:
            try:
                layer = self.layer_manager.searchlayerbyid(layer_id)
                layer_name = layer.getlayername()
            except:
                layer_name = "Invalid"
            self.layer_name_dictionary[layer_id] = layer_name
        return layer_name


class EntityBrowserGrid(gridlib.Grid):
    def __init__(self, parent, class_list):
        gridlib.Grid.__init__(
            self, 
            parent, 
            -1, 
            style = wx.BORDER_SUNKEN
            )
        self.data = self.create_data(class_list)
        self.table = CustomDataTable(self, self.data)        
        self.SetTable(self.table, False)
        self.SetSelectionMode(self.wxGridSelectRows)
        self.DisableDragRowSize()
        self.SetRowLabelSize(0)
        self.SetMargins(0, 0)
        self.AutoSizeColumns(False)
        self.update_col_attrs()        
        self.parent = parent
        gridlib.EVT_GRID_CELL_LEFT_DCLICK(self, self.__on_left_dlcick)
        gridlib.EVT_GRID_SELECT_CELL(self, self.__on_select_cell)
        gridlib.EVT_GRID_RANGE_SELECT(self, self.__on_multiple_select)

    def get_number_rows (self):
        return self.table.GetNumberRows()

    def get_number_cols (self):
        return self.table.GetNumberCols()

    def set_data (self, data):
        self.table.data = data

    def create_data (self, class_type_list, pattern=""):
        data = []
        # prevent bad expression error
        if "*" == pattern:
            pattern = "."
        #set up a temporary dictionary to make lookup quicker
        if class_type_list is not None:
            class_type_dict = {}
            for each in class_type_list:
                class_type_dict[each] = True
        entity_server = servers.get_entity_object_server()
        layer_name_gopher = FastLayerNameGopher()
        entity = entity_server.getfirstentityobject()
        while entity is not None:
            if not entity.isinlimbo() and not entity.ishideineditor():
                if class_type_list is None:
                    include_class = True
                else:
                    native_class = entity.getclasses()[-3]
                    include_class = class_type_dict.has_key(
                                            native_class
                                            )
                if include_class:
                    class_name = entity.getclass()
                    if pattern == "":
                        # don't do the search unless really necessary
                        match = True
                    else:
                        match = re.compile(
                                        pattern, 
                                        re.IGNORECASE
                                        ).search(class_name)
                    if match:
                        row = []
                        # get entity id and cool name, if defined
                        entity_id = entity.getid()
                        if entity.hascommand("getname"):
                            entity_cool_name = entity.getname()
                        else:
                            entity_cool_name = '-'
                        # get layername
                        if entity.hascommand("geteditorkeyint"):
                            layer_id = entity.geteditorkeyint("layerId")
                        else:
                            layer_id = -1
                        layer_name = layer_name_gopher.\
                                                get_name_for_layer_with_id(
                                                    layer_id
                                                    )
                        # get dirty state
                        dirty = entity_server.getentityobjectdirty(
                                    entity_id
                                    )                
                        # fill row
                        row.append(entity_id)
                        row.append( hex(entity_id) )
                        row.append(class_name)
                        row.append(entity_cool_name)
                        row.append(layer_name)
                        if dirty:
                            row.append("Yes")
                        else:
                            row.append("No")
                        data.append(row)
            entity = entity_server.getnextentityobject()
        return data

    def update_col_attrs(self):
        for col_num, col_name in enumerate(self.table.col_labels):
            attr = gridlib.GridCellAttr()
            if 'ID' == col_name:
                attr.SetTextColour(wx.BLUE)
            attr.SetReadOnly(True)
            self.SetColAttr(col_num, attr)

    def __on_select_cell(self, event):
        """ Select the whole row. """
        row_index = event.GetRow()
        self.__select_rows( [row_index], False )
        event.Skip()
    
    def __select_rows(self, row_index_list, add_to_selection=False):
        if not add_to_selection:
            self.ClearSelection()
            app.get_object_state().resetselection()
        for row_index in row_index_list:
            self.select_entity_in_summoner(row_index)
            # select manually in the grid too
            self.SelectRow(row_index, True)
        self.GetParent().parent.update_toolbar()

    def select_entity_in_summoner(self, idx):
        """ Select entity in Summoner. Return true if
        selected OK, otherwise false."""
        entity_id = self.__get_entity_id_for_row(idx)
        entity = self.__get_entity_with_id(entity_id)
        if self.can_entity_be_selected_in_summoner(entity):
            if app.is_terrain_cell_mode_active():
                cjr.show_error_message(
                    "Entity %s cannot be selected in Summoner "\
                    "when in terrain cell mode." % entity_id
                    )
                return False
            else:
                app.get_object_state().addentitytoselection(entity_id)
                return True
        else:
            return False

    def can_entity_be_selected_in_summoner(self, an_entity):
        return an_entity.hascomponent('ncSpatial') \
            or an_entity.hascomponent('ncPhysicsObj')

    def deselect_entity_in_summoner (self, idx):
        """ Deselect entity in summoner """
        entity_id = self.__get_entity_id_for_row(idx)
        entity = self.__get_entity_with_id(entity_id)
        if self.can_entity_be_selected_in_summoner(entity):
            app.get_object_state().removeentityfromselection(entity_id)
            return True
        else:
            return False

    def __get_entity_with_id(self, entity_id):
        entity_object_server = servers.get_entity_object_server()
        return entity_object_server.getentityobject(entity_id)

    def select_entities_with_ids(self, id_list):
        if len(id_list) == 0:
            self.ClearSelection()
        for each_id in id_list:
            idx = self.__get_data_index_for_entity_id(each_id)
            if idx != wx.NOT_FOUND:
                self.SelectRow( idx, True)

    def __get_data_index_for_entity_id(self, entity_id):
        for index, data in enumerate(self.data):
            if data[0] == entity_id:
                return index
        return wx.NOT_FOUND

    def __deselect_row(self, row_index):
        if not self.deselect_entity_in_summoner(row_index):
            self.DeselectRow(row_index)

    def __on_multiple_select (self, event):        
        """Select multiple entities from browser, 
        you can use ctrl to add an individual entity or 
        shift to select a range. You can also use mouse 
        drag to select a block
        """
        if event.ControlDown():
            current_row = event.GetTopRow()
            if event.Selecting():
                self.__select_rows([current_row], True)
            elif not event.Selecting():
                self.__deselect_row(current_row)
            return
        else:
            top_left = self.GetSelectionBlockTopLeft()
            bottom_right = self.GetSelectionBlockBottomRight()
            if top_left != [] and bottom_right != []:
                if event.Selecting():
                    row_index_list = []
                    for each in range( len(top_left )):
                        top_row = top_left[each][0]
                        bottom_row = bottom_right[each][0] + 1
                        for row_index in xrange( top_row, bottom_row):
                            row_index_list.append(row_index)
                    self.__select_rows(row_index_list, False)
                else:
                    self.__deselect_row( event.GetTopRow() )

    def __on_left_dlcick (self, event):
        # get entity id
        row_index = event.GetRow()
        row_data = self.__get_data_for_row(row_index)
        an_id = row_data[0]
        if self.parent.modal:
            entity_name = row_data[2]
            string_to_show = "%s (id = %s)" % ( entity_name, str(an_id) )
            grand_parent = self.parent.GetParent()
            grand_parent.SetTitle(string_to_show)
            grand_parent.Hide()
            grand_parent.EndModal(wx.ID_OK)
        else:
            # show an inspector window for the entity
            win = objdlg.create_window(
                        app.get_top_window(), 
                        an_id
                        )
            win.display()

    def has_items_selected(self):
        return len( self.get_selected_items() ) > 0

    def get_selected_items(self):
        # complicated, because we need to take into account blocks too...
        selected_rows = self.GetSelectedRows()
        top_left_blocks = self.GetSelectionBlockTopLeft()
        bottom_right_blocks = self.GetSelectionBlockBottomRight()
        for count in range( len(top_left_blocks) ):
            start_index = top_left_blocks[count][0]
            end_index = bottom_right_blocks[count][0] 
            for index in range(start_index, end_index + 1):
                selected_rows.append(index)
        # get rid of any duplicates
        temp_dict = {}
        for each_row in selected_rows:
            temp_dict[each_row] = True
        return temp_dict.keys()

    def __has_selected_items(self):
        return len( self.get_selected_items() )

    def get_selected_entities(self):
        """"Return a list of all entities currently selected in the 
        list, regardless of whether they are selected in Summoner"""
        entity_server = servers.get_entity_object_server()
        entity_list = []
        for each_id in self.__get_ids_of_selected_entities():
            entity_list.append(
                entity_server.getentityobject(each_id)
                )
        return entity_list

    def __get_ids_of_selected_entities(self):
        id_list = []
        for row_index in self.get_selected_items():
            id_list.append(
                self.__get_entity_id_for_row(row_index)
                )
        return id_list

    def __get_entity_id_for_row(self, row_index):
        row_data = self.__get_data_for_row(row_index)
        if row_data is None:
            return None
        else:
            return row_data[0]

    def __get_data_for_row(self, row_index):
        return self.data[row_index]


class SelectNativesHeadingPanel (wx.Panel):
    def __init__ (self, parent):
        wx.Panel.__init__(
            self, 
            parent, 
            -1 
            )
        self.SetBackgroundColour(
            cfg.HEADER_BACKGROUND_COLOUR
            )
        self.label = wx.StaticText(
                            self, 
                            -1, 
                            'Filter by natives'
                            )
        font_to_use = self.label.GetFont()
        font_to_use.SetWeight(
            wx.FONTWEIGHT_BOLD
            )
        self.label.SetFont(font_to_use)
        bmap = wx.Bitmap(
                    format.mangle_path(
                        "outgui:images/tools/tiny_close.bmp"
                        )
                    )
        self.button_close = wx.BitmapButton(
                                        self,
                                        -1, 
                                        bmap,
                                        style = wx.NO_BORDER
                                        )
        self.__do_layout()

    def __do_layout(self):
        sizer = wx.BoxSizer(wx.HORIZONTAL)
        sizer.Add(
            self.label,
            1, 
            wx.ALL|wx.ALIGN_CENTER_VERTICAL,
            cfg.BORDER_WIDTH
            )
        sizer.Add(
            self.button_close,
            0, 
            wx.ALL|wx.ALIGN_CENTER_VERTICAL,
            cfg.BORDER_WIDTH
            )
        self.SetSizerAndFit(sizer)


nativesEVT_SELECTED_NATIVES = wx.NewEventType()
EVT_SELECTED_NATIVES = wx.PyEventBinder(
                                        nativesEVT_SELECTED_NATIVES,
                                        )


class SelectNativesPanel (wx.Panel):
    def __init__ (self, parent, native_list=[]):
        wx.Panel.__init__(
            self, 
            parent, 
            -1,
            style = wx.BORDER_SUNKEN
            )
        self.heading_panel = SelectNativesHeadingPanel(self)
        self.checkbox_apply_filter = wx.CheckBox(
                                                self, 
                                                -1, 
                                                "Apply when selecting" 
                                                )
        self.checkbox_all_natives = wx.CheckBox(
                                                self, 
                                                -1, 
                                                "All natives" 
                                                )
        self.checkbox_all_natives.SetValue(True)
        self.filter_list = wx.CheckListBox(
                                self, 
                                -1, 
                                (80, 50), 
                                wx.DefaultSize, 
                                native_list,
                                style = wx.LB_SORT
                                )
        self.filter_list.Disable()

        self.__do_layout()
        self.__bind_events()

    def __do_layout(self):
        sizer = wx.BoxSizer(wx.VERTICAL)
        horizontal_sizer = wx.BoxSizer(wx.HORIZONTAL)
        horizontal_sizer.Add(
            self.heading_panel, 
            1, 
            wx.ALIGN_CENTER_VERTICAL
            )
        sizer.Add(
            horizontal_sizer, 
            0, 
            wx.EXPAND|wx.BOTTOM, 
            cfg.BORDER_WIDTH
            )
        sizer.Add(
            self.checkbox_apply_filter,
            0,
            wx.EXPAND|wx.ALL,
            cfg.BORDER_WIDTH
            )
        sizer.Add(
            self.checkbox_all_natives,
            0,
            wx.EXPAND|wx.ALL,
            cfg.BORDER_WIDTH
            )
        sizer.Add(
            self.filter_list, 
            1, 
            wx.EXPAND|wx.LEFT|wx.RIGHT|wx.BOTTOM,
            cfg.BORDER_WIDTH            
            )
        self.SetSizer(sizer)

    def __bind_events(self):
        self.Bind(
            wx.EVT_CHECKBOX,
            self.__on_checkbox_all_natives,
            self.checkbox_all_natives
            )
        self.Bind(
            wx.EVT_CHECKLISTBOX,
            self.__on_check_list_box,
            self.filter_list
            )

    def __deselect_all_in_natives_list(self):
        for index in range( self.filter_list.GetCount() ):
            self.filter_list.Check(index, False)

    def __clear_and_disable_native_list(self):
        self.filter_list.Disable()
        self.__deselect_all_in_natives_list()

    def signal_native_selection_changed(self):
        custom_event = wx.PyCommandEvent(
                                    nativesEVT_SELECTED_NATIVES
                                    )
        self.GetEventHandler().ProcessEvent(
            custom_event
            )

    def is_show_all_natives_selected(self):
        return self.checkbox_all_natives.GetValue()

    def __on_checkbox_all_natives(self, event):
        if event.GetSelection():
            self.__clear_and_disable_native_list()
        else:
            self.filter_list.Enable()
        self.signal_native_selection_changed()

    def __on_check_list_box(self, event):
        self.signal_native_selection_changed()

    def get_selected_classes(self):
        class_list = []
        for index in range( self.filter_list.GetCount() ):
            if self.filter_list.IsChecked(index):
                class_list.append( self.filter_list.GetString(index) )
        return class_list

    def get_min_width_for_heading(self):
        return self.heading_panel.GetMinSize()[0] + cfg.BORDER_WIDTH * 2


class EntityBrowser(wx.Panel):
    def __init__(self, parent, modal):
        wx.Panel.__init__(self, parent, -1)
        self.modal = modal
        self.use_active_filter = False
        self.apply_filter_when_selecting = False
        self.parent = parent
        #initialize the list of native entities
        self.master_native_list = self.get_all_native_entities()        
        #initialize the list of natives we're interested in
        #to be a copy of the list of native entities
        self.natives = None
        self.pattern = ''
        self.grid = EntityBrowserGrid(self, self.natives)
        win = wx.SashLayoutWindow(
            self, 
            -1, 
            wx.DefaultPosition,
            style = wx.SW_3D
            )
        self.native_sash_panel = SelectNativesPanel(
                                            win, 
                                            self.master_native_list
                                            )
        ideal_width = self.native_sash_panel.GetBestSize()[0]
        win.SetDefaultSize( (ideal_width, -1) )
        win.SetMaxSize( (ideal_width, -1) )
        min_width = self.native_sash_panel.get_min_width_for_heading()
        win.SetMinimumSizeX(min_width)
        win.SetOrientation(wx.LAYOUT_VERTICAL)
        win.SetAlignment(wx.LAYOUT_RIGHT)
        win.SetSashVisible(wx.SASH_LEFT, True)
        self.native_sash = win
        self.native_sash.Hide()
        self.select_entities_from_summoner()
        # bind events
        self.Bind(
            wx.EVT_SASH_DRAGGED_RANGE, 
            self.__on_sash_drag, 
            self.native_sash
            )
        self.Bind(
            wx.EVT_SIZE, 
            self.__on_size
            )
        self.native_sash.Bind(
            wx.EVT_BUTTON, 
            self.__on_click_close_button_in_sash
            )
        self.Bind(
            wx.EVT_CHECKBOX, 
            self.__on_change_apply_filter,
            self.native_sash_panel.checkbox_apply_filter
            )
        self.native_sash.Bind(
            EVT_SELECTED_NATIVES, 
            self.__on_change_selected_natives
            )

    def should_show_all_natives(self):
        return self.native_sash_panel.is_show_all_natives_selected()

    def __on_change_apply_filter(self, event):
        self.apply_filter_when_selecting = event.GetSelection()
        self.update_selection_exceptions()

    def __update_natives(self):
        if self.should_show_all_natives():
            self.natives = None
        else:
            selected_list = self.native_sash_panel.get_selected_classes()
            self.natives = selected_list

    def __on_change_selected_natives(self, event):
        self.__update_natives()
        if self.apply_filter_when_selecting:
            self.update_selection_exceptions()            
        if self.use_active_filter:
            self.update_grid()

    def __on_click_close_button_in_sash(self, event):
        self.show_or_hide_natives_sash()

    def __on_sash_drag(self, event):
        if event.GetDragStatus() == wx.SASH_STATUS_OUT_OF_RANGE:
            return
        new_width = event.GetDragRect().width
        max_width = self.native_sash.GetMaxSize()[0]
        self.native_sash.SetDefaultSize(
            (
                min(new_width, max_width),
                -1
                )
            )
        self.layout_window()

    def __on_size(self, event):
        self.layout_window()
        event.Skip()

    def selectionchanged (self):
        """The items selected in Summoner have changed,
        so update the grid to reflect this."""
        self.select_entities_from_summoner()
        self.parent.update_toolbar()

    def select_entities_from_summoner (self):
        """ Select all entities that are selected in Summoner """
        num_entities = self.__get_num_entities_selected_in_summoner()
        entity_id_list = []
        for index in xrange(num_entities):
            entity = app.get_object_state().getselectedentity(index)
            entity_id_list.append( entity.getid() )
        self.grid.select_entities_with_ids(entity_id_list)

    def get_selected_entities(self):
        return self.grid.get_selected_entities()

    def get_entities_selected_in_summoner(self):
        state = app.get_object_state()
        select_count = state.getselectioncount()
        entity_list = []
        for i in range(select_count):
            entity = state.getselectedentity(i)
            entity_list.append(entity)        
        return entity_list

    def has_entities_selected(self):
        """Return true if there are entities selected in the list,
        regardless of whether they are selected in Summoner."""
        return self.grid.has_items_selected()

    def has_entities_selected_in_summoner(self):
        """Only return true if there are entities selected in Summoner"""
        return self.__get_num_entities_selected_in_summoner() > 0

    def __get_num_entities_selected_in_summoner(self):
        state = app.get_object_state()
        return state.getselectioncount()

    def has_exportable_entities_selected(self):
        return len( self.get_selected_entities_ok_for_export() )

    def is_entity_suitable_for_export(self, an_entity):
        return self.is_normal_entity(an_entity) \
            and not self.is_outdoor_entity(an_entity)

    def is_normal_entity(self, an_entity):
        a_type = self.get_entity_type_for_entity(an_entity)
        return a_type == cfg.NORMAL_ENTITY_TYPE 

    def is_outdoor_entity(self, an_entity):
        return an_entity.isa('neoutdoor')

    def get_entity_type_for_entity(self, an_entity):        
        entity_server = servers.get_entity_object_server()
        return entity_server.getentityobjecttype( an_entity.getid() )

    def get_selected_entities_ok_for_export(self):
        """Return a list of those currently selected entities 
        which are suitable for export."""
        return filter(
            self.is_entity_suitable_for_export, 
            self.get_selected_entities () 
            )

    def refresh(self):
        """ refresh the entity browser """
        self.update_grid()
        self.select_entities_from_summoner()

    def update_selection_exceptions(self):
        editor_state = app.get_object_state()
        editor_state.resetselectionexceptions()
        if self.apply_filter_when_selecting:
            # if self.natives is none it means that no 
            # natives have been selected for exclusion, 
            # so don't do the next bit
            if self.natives is not None:
                for each_native in self.master_native_list:
                    if each_native not in self.natives:
                        editor_state.addselectionexception(
                            each_native
                            )

    def update_grid(self):
        try:
            try:
                wx.BeginBusyCursor()
                old_rows_count = self.grid.get_number_rows()
                new_data = self.grid.create_data(
                                    self.natives, 
                                    self.pattern
                                    )
                self.grid.set_data(new_data)
                self.grid.data = new_data
                # Get new values
                msg = gridlib.GridTableMessage(
                            self.grid.table,
                            gridlib.GRIDTABLE_REQUEST_VIEW_GET_VALUES
                            )
                self.grid.ProcessTableMessage(msg)
                new_rows_count = self.grid.get_number_rows()
                if new_rows_count > 0:
                    rows_diff = old_rows_count - new_rows_count       
                    # Add or delete rows dynamically
                    if rows_diff > 0:                
                        msg = gridlib.GridTableMessage(
                                    self.grid.table,
                                    gridlib.GRIDTABLE_NOTIFY_ROWS_DELETED, 
                                    new_rows_count - rows_diff, 
                                    rows_diff
                                    )
                        self.grid.ProcessTableMessage(msg)
                    elif rows_diff < 0:
                        msg = gridlib.GridTableMessage(
                                    self.grid.table,
                                    gridlib.GRIDTABLE_NOTIFY_ROWS_INSERTED, 
                                    old_rows_count, 
                                    -rows_diff
                                    )
                        self.grid.ProcessTableMessage(msg)
                else:
                    msg = gridlib.GridTableMessage(
                                self.grid.table,
                                gridlib.GRIDTABLE_NOTIFY_ROWS_DELETED, 
                                old_rows_count, 
                                old_rows_count
                                )
                    self.grid.ProcessTableMessage(msg)
            finally:
                wx.EndBusyCursor()
        except:
            # make sure any errors are not hidden
            raise

    def move_camera (self):
        conjurer = servers.get_conjurer()
        conjurer.movecameratoselection()

    def snap_to_terrain(self):
        if app.is_terrain_cell_mode_active():
            cjr.show_error_message(
                "Function not available when terrain cell mode is active"
                )
        else:
            if self.has_entities_selected_in_summoner():
                snapped_ok = app.get_object_state().snapselectiontoterrain()
                if not snapped_ok:
                    cjr.show_error_message(
                        "Failed to snap selected items to terrain"
                        )
            else:
                cjr.show_error_message(
                    "There are no entities currently selected in Summoner"
                    )

    def delete_selected_entities (self):
        """ Delete all entities that are selected in summoner """
        entity_list = self.get_selected_entities()
        if entity_list:
            self.grid.ClearSelection()
            app.get_object_state().delentities()

    def reset_selection (self):
        self.reset_selection_in_summoner()
        self.clear_selection_in_grid()

    def reset_selection_in_summoner(self):
        selector = app.get_object_state()
        selector.resetselection()

    def clear_selection_in_grid(self):
        self.grid.ClearSelection()

    def rename_entity ( self ):
        """ Renames the name of an entity """
        rows = self.grid.GetSelectedRows()
        entity_server = servers.get_entity_object_server()
        for idx in rows:
            row = self.grid.data[idx]
            entity_id = row[0]
            entity = entity_server.getentityobject(entity_id)
            if not entity.hascomponent("ncGameplay"):
                cjr.show_information_message(
                    "Only gameplay entities may have a name"
                    )
            else:
                dlg = wx.TextEntryDialog(
                            None, 
                            "Enter entity name:",
                            "Rename entity", 
                            entity.getname() 
                            )
                if dlg.ShowModal() == wx.ID_OK:
                    name = dlg.GetValue()
                    entity.setname( str(name) )
                    entity_server.setentityobjectdirty(entity, True)
                    self.refresh()
                dlg.Destroy()

    def open_inspector (self):
        """Open a property editor for selected entities"""
        entity_list = self.get_selected_entities()
        select_count = len(entity_list)
        if select_count > 1:
            model = scripting.ScriptingModelMultiObject(entity_list)
            propertyview.create_window(
                app.get_top_window(), 
                model 
                )
        elif select_count == 1:
            entity = entity_list[0]
            entity_id = entity.getid()
            win = objdlg.create_window(
                        app.get_top_window(), 
                        entity_id 
                        )
            win.display()

    def open_export_dialog(self):
        """Open an export dialog for the selected entities"""
        entity_list = self.get_selected_entities_ok_for_export()
        select_count = len(entity_list)
        if select_count > 0:
            dialog = expentdlg.createWindow( 
                            app.get_top_window(),   
                            entity_list 
                            )
            dialog.ShowModal()
            dialog.Destroy()
        else:
            cjr.show_error_message(
                "There are no entities currently selected"
                )

    def get_all_native_entities (self):
        entity_list = []
        root_entity = app.get_main_entityobject()
        entity = root_entity.gethead()        
        # visit all siblings
        while entity is not None:
            #two entities are not suitable, so exclude them
            entity_name = entity.getname()
            if entity_name != 'neoutdoorcell' \
            and entity_name != 'neragdolllimb':
                entity_list.append(entity_name)
            entity = entity.getsucc()
        return entity_list

    def show_or_hide_natives_sash(self):
        """If the sash is currently visible, then hide it, 
        sash, otherwise show it."""
        if self.native_sash.IsShown():
            self.hide_natives_sash()
        else:
            self.show_natives_sash()

    def layout_window(self):
        wx.LayoutAlgorithm().LayoutWindow(self, self.grid)    

    def show_natives_sash(self):
        self.native_sash.Show()
        self.layout_window()
   
    def hide_natives_sash(self):
        self.native_sash.Hide()
        self.layout_window()


class EntityBrowserWindow(togwin.ChildToggableDialog):
    ID_ICON_MOVE = wx.NewId()
    ID_ICON_RENAME = wx.NewId()
    ID_ICON_RESET = wx.NewId()
    ID_ICON_INSPECTOR = wx.NewId()
    ID_ICON_DELETE = wx.NewId()
    ID_ICON_NATIVES = wx.NewId()
    ID_ICON_FILTER = wx.NewId()
    ID_ICON_REFRESH = wx.NewId()
    ID_ICON_IMPORT = wx.NewId()
    ID_ICON_EXPORT = wx.NewId()
    ID_ICON_SNAP = wx.NewId()

    # If the object is instantiated from this class, it will be a child dialog
    def __init__(self, parent, modal=False):
        togwin.ChildToggableDialog.__init__(
            self, 
            "Entity Browser", 
            parent
            )                   
        self.entity_browser = EntityBrowser(self, modal)   
        self.__create_toolbar()
        sizer = wx.BoxSizer(wx.VERTICAL)
        horizontal_line = wx.StaticLine(
                                    self, 
                                    -1, 
                                    (-1, -1), 
                                    (-1, -1), 
                                    wx.LI_HORIZONTAL 
                                    )
        sizer.Add(horizontal_line, 0, wx.EXPAND)
        sizer.Add(self.entity_browser, 1, wx.EXPAND|wx.ALL, cfg.BORDER_WIDTH)
        self.SetSizer(sizer)
        self.SetSize(wx.Size(430, 510))

        # Binding this signals will be warned about object changes
        pynebula.pyBindSignal(
            app.get_object_state(), 
            'singleentityplaced',
            self.entity_browser, 
            'refresh', 
            0
            )
        pynebula.pyBindSignal(
            app.get_object_state(), 
            'entitymodified',
            self.entity_browser, 
            'refresh', 
            0
            )
        pynebula.pyBindSignal(
            app.get_object_state(), 
            'selectionchanged',
            self.entity_browser, 
            'selectionchanged', 
            0
            )

    def __del__ (self):
        """ Unbind used signals """
        pynebula.pyUnbindTargetObject(
            app.get_object_state(), 
            'singleentityplaced', 
            self.entity_browser
            )
        pynebula.pyUnbindTargetObject(
            servers.get_command_server(), 
            'entitymodified', 
            self.entity_browser
            )
        pynebula.pyUnbindTargetObject(
            app.get_object_state(), 
            'selectionchanged', 
            self.entity_browser
            )

    def __create_toolbar (self):
        fileserver = servers.get_file_server()
        self.tbar = self.CreateToolBar(
                            wx.TB_HORIZONTAL |
                            wx.NO_BORDER |
                            wx.TB_FLAT |
                            wx.TB_TEXT 
                            )
        self.tbar.AddTool(
            self.ID_ICON_MOVE, 
            wx.Bitmap(
                fileserver.manglepath(
                    "outgui:images/tools/camera.bmp"
                    ),
                wx.BITMAP_TYPE_ANY
                ), 
            wx.NullBitmap, 
            wx.ITEM_NORMAL,
            "Move camera to selection", 
            "Move"
            )

        self.tbar.AddTool(
            self.ID_ICON_SNAP, 
            wx.Bitmap(
                fileserver.manglepath(
                    "outgui:images/tools/snap_to_terrain.bmp"
                    ),
                wx.BITMAP_TYPE_ANY
                ), 
            wx.NullBitmap, 
            wx.ITEM_NORMAL,
            "Snap selected items to terrain", 
            "Snap to terrain"
            )

        self.tbar.AddTool(
            self.ID_ICON_RENAME, 
            wx.Bitmap(
                fileserver.manglepath(
                    "outgui:images/tools/red_doc.bmp"
                    )
                ), 
            wx.NullBitmap, 
            wx.ITEM_NORMAL,
            "Rename selected entity", 
            "Rename" 
            )

        self.tbar.AddTool(
            self.ID_ICON_RESET, 
            wx.Bitmap(
                fileserver.manglepath(
                    "outgui:images/tools/select_none.bmp"
                    )
                ), 
            wx.NullBitmap, 
            wx.ITEM_NORMAL,
            "Reset selection",
            "Reset" 
            )

        self.tbar.AddTool(
            self.ID_ICON_INSPECTOR, 
            wx.Bitmap(
                fileserver.manglepath(
                    "outgui:images/tools/show.bmp"
                    )
                ), 
            wx.NullBitmap, 
            wx.ITEM_NORMAL,
            "Open an inspector for selected entities", 
            "Inspector" 
            )

        self.tbar.AddTool(
            self.ID_ICON_IMPORT, 
            wx.Bitmap(
                fileserver.manglepath(
                    "outgui:images/tools/import.bmp"
                    )
                ), 
            wx.NullBitmap, 
            wx.ITEM_NORMAL,
            "Import selected entities", 
            "Import from file" 
            )

        self.tbar.AddTool(
            self.ID_ICON_EXPORT, 
            wx.Bitmap(
                fileserver.manglepath(
                    "outgui:images/tools/export.bmp"
                    )
                ), 
            wx.NullBitmap, 
            wx.ITEM_NORMAL,
            "Export selected entities", 
            "Export to file" 
            )

        self.tbar.AddTool(
            self.ID_ICON_DELETE, 
            wx.Bitmap(
                fileserver.manglepath(
                    "outgui:images/tools/erase.bmp"
                    )
                ), 
            wx.NullBitmap, 
            wx.ITEM_NORMAL,
            "Delete selected object", 
            "Delete" 
            )

        self.tbar.AddSeparator()
        
        self.active_filter_checkbox = wx.CheckBox(
                                                    self.tbar, 
                                                    -1, 
                                                    "active filter " 
                                                    )
        self.tbar.AddControl(self.active_filter_checkbox)

        self.finder_combo = wx.TextCtrl(self.tbar, -1, "")        
        self.tbar.AddControl( self.finder_combo )

        self.tbar.AddTool(
            self.ID_ICON_NATIVES, 
            wx.Bitmap(
                fileserver.manglepath(
                    "outgui:images/tools/hyrearchy.bmp"
                    )
                ), 
            wx.NullBitmap,
            wx.ITEM_NORMAL,
            "Filter by natives...", 
            "Natives" 
            )

        self.tbar.AddSeparator()

        self.tbar.AddTool(
            self.ID_ICON_REFRESH, 
            wx.Bitmap(
                fileserver.manglepath(
                    "outgui:images/tools/refresh.bmp"
                    )
                ), 
            wx.NullBitmap, 
            wx.ITEM_NORMAL,
            "Refresh browser", 
            "Refresh" 
            )
        self.tbar.Realize()

        # Bind toolbar actions
        self.Bind(wx.EVT_TOOL, self.__on_move, id=self.ID_ICON_MOVE)
        self.Bind(wx.EVT_TOOL, self.__on_snap_to_terrain, id=self.ID_ICON_SNAP)
        self.Bind(wx.EVT_TOOL, self.__on_rename, id=self.ID_ICON_RENAME)
        self.Bind(wx.EVT_TOOL, self.__on_reset, id=self.ID_ICON_RESET)
        self.Bind(wx.EVT_TOOL, self.__on_inspector, id=self.ID_ICON_INSPECTOR)
        self.Bind(wx.EVT_TOOL, self.__on_delete, id=self.ID_ICON_DELETE)
        self.Bind(wx.EVT_TOOL, self.__on_natives, id=self.ID_ICON_NATIVES)
        self.Bind(wx.EVT_TOOL, self.__on_refresh, id=self.ID_ICON_REFRESH)
        self.Bind(wx.EVT_TOOL, self.__on_import, id = self.ID_ICON_IMPORT)
        self.Bind(wx.EVT_TOOL, self.__on_export, id = self.ID_ICON_EXPORT)
        self.Bind(wx.EVT_TEXT, self.__on_filter)
        self.Bind(wx.EVT_TEXT_ENTER, self.__on_enter_in_finder_ctrl)
        self.Bind(
            wx.EVT_CHECKBOX,
            self.__on_active_filter_checkbox,
            self.active_filter_checkbox
            )

        self.update_toolbar()

    def update_toolbar (self):
        """Update the toolbar, enabling or disabling 
        items based on what is currently selected."""
        self.tbar.EnableTool( 
            self.ID_ICON_MOVE, 
            self.should_move_tool_be_enabled() 
            )
        self.tbar.EnableTool( 
            self.ID_ICON_SNAP, 
            self.should_snap_to_terrain_tool_be_enabled() 
            )
        self.tbar.EnableTool( 
            self.ID_ICON_RENAME, 
            self.should_rename_tool_be_enabled() 
            )
        self.tbar.EnableTool( 
            self.ID_ICON_RESET, 
            self.should_reset_tool_be_enabled() 
            )
        self.tbar.EnableTool( 
            self.ID_ICON_DELETE, 
            self.should_delete_tool_be_enabled() 
            )
        self.tbar.EnableTool( 
            self.ID_ICON_INSPECTOR, 
            self.should_inspector_tool_be_enabled() 
            )
        self.tbar.EnableTool( 
            self.ID_ICON_IMPORT, 
            self.should_import_tool_be_enabled() 
            )
        self.tbar.EnableTool( 
            self.ID_ICON_EXPORT, 
            self.should_export_tool_be_enabled()
            )

    def should_move_tool_be_enabled(self):
        """return true always"""
        #return self.has_entities_currently_selected()
        return True

    def should_snap_to_terrain_tool_be_enabled(self):
        return self.has_entities_currently_selected_in_summoner()

    def should_rename_tool_be_enabled(self):
        """return true always"""
        #return self.has_entities_currently_selected()
        return True

    def should_reset_tool_be_enabled(self):
        """return true if there are items selected, otherwse false"""
        return self.has_entities_currently_selected()

    def should_delete_tool_be_enabled(self):
        return self.has_entities_currently_selected()

    def should_inspector_tool_be_enabled(self):
        """return true if there are items selected, otherwise false"""
        return self.has_entities_currently_selected()

    def should_import_tool_be_enabled(self):
        """return true always"""
        return True

    def should_export_tool_be_enabled(self):
        """return true if there are items selected, otherwse false"""
        return self.has_exportable_entities_selected()

    def has_entities_currently_selected(self):
        return self.entity_browser.has_entities_selected()

    def has_entities_currently_selected_in_summoner(self):
        return self.entity_browser.has_entities_selected_in_summoner()

    def has_exportable_entities_selected(self):
        return self.entity_browser.has_exportable_entities_selected()

    def __on_active_filter_checkbox(self, event):
        self.entity_browser.use_active_filter = event.GetSelection()

    def __on_filter (self, event):
        pattern = self.finder_combo.GetValue()
        self.entity_browser.pattern = pattern
        if self.entity_browser.use_active_filter:
            self.refresh_entity_browser()

    def __on_enter_in_finder_ctrl(self, event):
        self.refresh_entity_browser()

    def __on_refresh (self, event):
        self.refresh_entity_browser()

    def refresh_entity_browser(self):
        self.entity_browser.refresh()

    def __on_move (self, event):
        self.entity_browser.move_camera()

    def __on_snap_to_terrain (self, event):
        self.entity_browser.snap_to_terrain()

    def __on_rename (self, event):
        self.entity_browser.rename_entity()

    def __on_reset (self, event):
        self.entity_browser.reset_selection()

    def __on_inspector (self, event):
        self.entity_browser.open_inspector()

    def __on_delete (self, event):
        self.entity_browser.delete_selected_entities()

    def __on_natives (self, event):
        self.entity_browser.show_or_hide_natives_sash()

    def __on_import (self, event):
        imported_ok = importentity.importEntitiesFromFile(
                                self, 
                                cfg.DEFAULT_LAYER_ID
                                )
        if imported_ok:
            self.refresh_entity_browser()

    def __on_export (self, event):
        self.entity_browser.open_export_dialog()

    def __on_close_button_clicked(self, event):
        self.Close()

    def persist(self):
        return [
            create_window,
            () # no parameters for create function
            ]

    def restore(self, data_list):
        pass
    
    def refresh(self):
        pass


class EntityBrowserDialog (freedialoggui.freeDialogGUI):
    def __init__(self, parent, modal):        
        # If the object is intanced from this class, will be a free dialog
        freedialoggui.freeDialogGUI.__init__(self, "Entity Browser", parent)
        self.entity_browser = EntityBrowser(self, modal)
        # Settings
        self.SetSize( wx.Size(387, 400) )
   
def create_window(frame, modal=False):
    try:
        try:
            wx.BeginBusyCursor()
            win = EntityBrowserWindow(frame, modal)
        finally:
            wx.EndBusyCursor()
    except:
        # make sure any errors are not hidden
        raise
    return win

def create_dialog(frame, modal=True):
    return EntityBrowserDialog(frame, modal)
