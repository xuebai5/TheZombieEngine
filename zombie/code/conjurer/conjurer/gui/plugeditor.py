##\file plugeditor.py
##\brief Specific plugs editor panel

import wx

import app
import editorpanel
import grimoire
import servers

import conjurerconfig as cfg
import conjurerframework as cjr

# PlugsPanel class
class PlugsPanel(editorpanel.EditorPanel):
    """Specific plugs editor panel"""
    
    # Plugs list columns indices
    ID_Slot = 0
    ID_Entity = 1
    
    def __init__(self, *args, **kwds):
        editorpanel.EditorPanel.__init__(self, *args, **kwds)
        self.entity = None
        self.plug_indices = []
        
        self.label = wx.StaticText(self, -1, "Plugged entities")
        self.list = wx.ListCtrl(self, -1,
            style=wx.LC_REPORT|wx.LC_SINGLE_SEL|wx.LC_SORT_ASCENDING)
        self.button_create = wx.Button(self, -1, "Create and plug entity")
        self.button_plug = wx.Button(self, -1, "Plug selected entity")
        self.button_unplug = wx.Button(self, -1, "Unplug entity")
        self.button_select = wx.Button(self, -1, "Select plugged entity")
        
        self.__set_properties()
        self.__do_layout()
        self.__bind_events()
    
    def __set_properties(self):
        self.SetScrollRate(10, 10)
        
        # Plugs list columns
        self.list.InsertColumn(self.ID_Slot, "Slot")
        self.list.InsertColumn(self.ID_Entity, "Entity", wx.LIST_FORMAT_RIGHT)
    
    def __do_layout(self):
        border_width = cfg.BORDER_WIDTH
        sizer = wx.BoxSizer(wx.VERTICAL)
        sizer_list = wx.BoxSizer(wx.VERTICAL)
        sizer_buttons = wx.BoxSizer(wx.VERTICAL)
        sizer_list.Add(self.label, 0, wx.FIXED_MINSIZE, 0)
        sizer_list.Add(self.list, 1, wx.TOP|wx.EXPAND, border_width)
        sizer.Add(sizer_list, 1, wx.ALL|wx.EXPAND, border_width)
        sizer_buttons.Add(self.button_create, 0, wx.EXPAND, 0)
        sizer_buttons.Add(self.button_plug, 0, wx.TOP|wx.EXPAND, border_width)
        sizer_buttons.Add(self.button_unplug, 0, wx.TOP|wx.EXPAND, border_width)
        sizer_buttons.Add(self.button_select, 0, wx.TOP|wx.EXPAND, border_width)
        sizer.Add(sizer_buttons, 0, wx.LEFT|wx.RIGHT|wx.BOTTOM|wx.EXPAND, border_width)
        self.SetAutoLayout(True)
        self.SetSizer(sizer)
        sizer.Fit(self)
        sizer.SetSizeHints(self)
        self.Layout()
    
    def __bind_events(self):
        self.Bind(wx.EVT_LIST_ITEM_SELECTED, self.on_select_plug, self.list)
        self.Bind(wx.EVT_BUTTON, self.on_create_and_plug_entity, self.button_create)
        self.Bind(wx.EVT_BUTTON, self.on_plug_entity, self.button_plug)
        self.Bind(wx.EVT_BUTTON, self.on_unplug_entity, self.button_unplug)
        self.Bind(wx.EVT_BUTTON, self.on_select_entity, self.button_select)
    
    def __get_selected_list_index(self):
        return self.list.GetNextItem(-1, wx.LIST_NEXT_ALL, wx.LIST_STATE_SELECTED)
    
    def __get_selected_plug_index(self):
        index = self.__get_selected_list_index()
        if index == -1:
            return -1
        else:
            return self.plug_indices[index]
    
    def __get_selected_list_item(self, column):
        index = self.__get_selected_list_index()
        if index == -1:
            return ""
        else:
            return self.list.GetItem(index, column).GetText()
    
    def __update_plug_ctrls(self):
        index = self.__get_selected_plug_index()
        self.button_create.Enable( index != -1 )
        self.button_plug.Enable( index != -1 )
        enable = False
        if index != -1:
            index = self.__get_selected_plug_index()
            enable = self.entity.getpluggedentity(index) != 0
        self.button_unplug.Enable( enable )
        self.button_select.Enable( enable )
    
    def on_create_and_plug_entity(self, event):
        # Show a grimoire browser to let the user choose an entity class
        dlg = grimoire.create_dialog( self )
        ok = dlg.ShowModal() == wx.ID_OK
        class_name = dlg.GetTitle()
        dlg.Destroy()
        if not ok:
            return

        # Create an instance of the selected class
        # position (1, 1, 1) is used just to pass the spatial search cell check
        # it is not important, since the plug is always linked to the entity pos        
        entity = servers.get_world_interface().newentity( str(class_name), 1, 1, 1 )
        if entity is None:
            cjr.show_error_message(
                "Unknown error.\nCouldn't create an instance of" \
                "the entity class '" + class_name + "'."
                )
            return

        # Plug the new entity to the selected slot
        self.__plug_entity_to_current_slot( entity )
    
    def on_select_plug(self, event):
        self.__update_plug_ctrls()
    
    def __plug_entity_to_current_slot(self, entity):
        slot = self.__get_selected_list_item( self.ID_Slot )
        entity_id = entity.getid()
        self.entity.plug( str(slot), entity_id )
        index = self.__get_selected_list_index()
        self.list.SetStringItem( index, self.ID_Entity, str(entity_id) )
        self.__update_plug_ctrls()
    
    def on_plug_entity(self, event):
        state = app.get_object_state()
        count = state.getselectioncount()
        if count == 0:
            cjr.show_error_message(
                "There isn't any selected entity"
                )
        elif count > 1:
            cjr.show_error_message(
                "There's more than one entity selected.\n" \
                "Only one entity can be plugged to the same slot."
                )
        else:
            slot = self.__get_selected_list_item(self.ID_Slot)
            if slot != "":
                entity = state.getselectedentity(0)
                entity_id = entity.getid()
                if entity_id == self.entity.getid():
                    cjr.show_error_message(
                        "So you want to plug the entity to itself, how curious..."
                        )
                else:
                    self.__plug_entity_to_current_slot( entity )
    
    def on_unplug_entity(self, event):
        l_index = self.__get_selected_list_index()
        if l_index != -1:
            p_index = self.plug_indices[l_index]
            entity_id = self.entity.getpluggedentity( p_index )
            if entity_id != 0:
                self.entity.unplug( entity_id )
                self.list.SetStringItem( l_index, self.ID_Entity, "" )
                self.__update_plug_ctrls()
    
    def on_select_entity(self, event):
        index = self.__get_selected_plug_index()
        if index != -1:
            entity_id = self.entity.getpluggedentity(index)
            if entity_id != 0:
                state = app.get_object_state()
                state.addentitytoselection(entity_id)
    
    def set_entity(self, entity):
        self.entity = entity
        
        # Fill plugs list
        self.list.DeleteAllItems()
        for i in range( self.entity.getnumplugs() ):
            index = self.list.InsertStringItem( 0, self.entity.getplugname(i) )
            self.plug_indices.insert(index, i)
            entity_id = str( self.entity.getpluggedentity(i) )
            if entity_id == "0":
                entity_id = ""
            self.list.SetStringItem( index, self.ID_Entity, entity_id )
        
        # Update controls that depends on selected plug
        self.__update_plug_ctrls()
    
    def refresh(self):
        try:
            self.set_entity( self.entity )
        except:
            self.Close()


# create_all_editors function
def create_all_editors(entity, parent):
    editor = PlugsPanel(parent, -1)
    editor.set_entity(entity)
    return [ ('Plugs', editor) ]
