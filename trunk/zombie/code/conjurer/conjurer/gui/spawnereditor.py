##\file spawneditor.py
##\brief Specific spawner editor panel

import wx

import app
import editorpanel
import events
import floatctrl
import servers

import conjurerframework as cjr

# BindingsPanel class
class BindingsPanel(editorpanel.EditorPanel):
    """Specific spawner editor panel"""
    
    # Spawners list columns indices
    ID_Spawner = 0
    ID_Probability = 1
    
    def __init__(self, *args, **kwds):
        editorpanel.EditorPanel.__init__(self, *args, **kwds)
        self.entity = None
        
        self.label = wx.StaticText(self, -1, "Bound spawners/spawn points:")
        self.list = wx.ListCtrl(self, -1,
            style=wx.LC_REPORT|wx.LC_SINGLE_SEL|wx.LC_SORT_ASCENDING)
        self.button_bind = wx.Button(self, -1, "Bind selected spawner(s)")
        self.button_unbind = wx.Button(self, -1, "Unbind spawner")
        self.button_select = wx.Button(self, -1, "Select spawner")
        self.label_probability = wx.StaticText(self, -1, "Probability:")
        self.float_probability = floatctrl.FloatCtrl(self, size=wx.Size(50,-1), style=wx.TE_RIGHT)
        
        self.__set_properties()
        self.__do_layout()
        self.__bind_events()
    
    def __set_properties(self):
        self.SetScrollRate(10, 10)
        
        # Spwaners list columns
        self.list.InsertColumn(self.ID_Spawner, "Spawner")
        self.list.InsertColumn(self.ID_Probability, "Probability", wx.LIST_FORMAT_RIGHT)
    
    def __do_layout(self):
        sizer = wx.BoxSizer(wx.VERTICAL)
        sizer_list = wx.BoxSizer(wx.VERTICAL)
        sizer_probability = wx.BoxSizer(wx.HORIZONTAL)
        sizer_buttons = wx.BoxSizer(wx.VERTICAL)
        sizer_list.Add(self.label, 0, wx.FIXED_MINSIZE, 0)
        sizer_list.Add(self.list, 1, wx.TOP|wx.EXPAND, 5)
        sizer.Add(sizer_list, 1, wx.ALL|wx.EXPAND, 10)
        sizer_probability.Add(self.label_probability, 0, wx.ALIGN_CENTER_VERTICAL|wx.FIXED_MINSIZE, 0)
        sizer_probability.Add(self.float_probability, 0, wx.LEFT|wx.FIXED_MINSIZE, 5)
        sizer.Add(sizer_probability, 0, wx.LEFT|wx.RIGHT|wx.BOTTOM|wx.EXPAND, 10)
        sizer_buttons.Add(self.button_bind, 0, wx.EXPAND, 0)
        sizer_buttons.Add(self.button_unbind, 0, wx.TOP|wx.EXPAND, 5)
        sizer_buttons.Add(self.button_select, 0, wx.TOP|wx.EXPAND, 5)
        sizer.Add(sizer_buttons, 0, wx.LEFT|wx.RIGHT|wx.BOTTOM|wx.EXPAND, 10)
        self.SetAutoLayout(True)
        self.SetSizer(sizer)
        sizer.Fit(self)
        sizer.SetSizeHints(self)
        self.Layout()
    
    def __bind_events(self):
        self.Bind(wx.EVT_LIST_ITEM_SELECTED, self.on_select_binding, self.list)
        self.Bind(wx.EVT_BUTTON, self.on_bind_spawner, self.button_bind)
        self.Bind(wx.EVT_BUTTON, self.on_unbind_spawner, self.button_unbind)
        self.Bind(wx.EVT_BUTTON, self.on_select_spawner, self.button_select)
        self.Bind(events.EVT_CHANGING, self.on_change_probability, self.float_probability)
        self.Bind(events.EVT_CHANGED, self.on_change_probability, self.float_probability)
    
    def __get_selected_list_index(self):
        return self.list.GetNextItem(-1, wx.LIST_NEXT_ALL, wx.LIST_STATE_SELECTED)
    
    def __get_selected_list_item(self, column):
        index = self.__get_selected_list_index()
        if index == -1:
            return ""
        else:
            return self.list.GetItem(index, column).GetText()
    
    def __update_binding_ctrls(self):
        name = self.__get_selected_list_item( self.ID_Spawner )
        spawner = servers.get_world_interface().getgameentity( str(name) )
        enable = spawner is not None
        self.button_unbind.Enable( enable )
        self.button_select.Enable( enable )
        self.float_probability.Enable( enable )
        if enable:
            probability = self.entity.getspawnerprobability( spawner )
            self.float_probability.set_value( probability )
    
    def __update_binding_list(self):
        self.list.DeleteAllItems()
        for i in range( self.entity.getspawnersnumber() ):
            spawner = self.entity.getspawnerbyindex( i )
            name = self.__get_spawner_name( spawner )
            prob = self.entity.getspawnerprobability( spawner )
            index = self.list.InsertStringItem( 0, name )
            self.list.SetStringItem( index, self.ID_Probability, str(prob) )
        self.__update_binding_ctrls()
    
    def __get_spawner_name(self, spawner):
        if not spawner.hascomponent('ncGameplay'):
            return '<error>'
        else:
            return spawner.getname()
    
    def on_select_binding(self, event):
        self.__update_binding_ctrls()
    
    def __select_binding(self, spawner):
        for i in range( self.list.GetItemCount() ):
            if self.list.GetItem(i, self.ID_Spawner).GetText() == spawner.getname():
                self.list.SetItemState( i, wx.LIST_STATE_SELECTED, wx.LIST_STATE_SELECTED )
                self.__update_binding_ctrls()
                return
    
    def on_bind_spawner(self, event):
        state = app.get_object_state()
        count = state.getselectioncount()
        if count == 0:
            cjr.show_error_message(
                "There isn't any selected entity"
                )
        else:
            # Add selected spawners
            sel_spawner = None
            for i in range(count):
                entity = state.getselectedentity(i)
                # Skip self and non spawners
                if entity != self.entity and \
                    ( entity.hascomponent('ncSpawner') or \
                    entity.hascomponent('ncSpawnPoint') ):
                    # Skip already bound spawners
                    if not self.entity.existsspawner( entity ):
                        self.entity.addspawner( entity, 1.0 )
                        self.__update_binding_list()
                        sel_spawner = entity
            # Select last added spawner
            if sel_spawner is not None:
                self.__select_binding( sel_spawner )
    
    def on_unbind_spawner(self, event):
        name = self.__get_selected_list_item( self.ID_Spawner )
        spawner = servers.get_world_interface().getgameentity( str(name) )
        if spawner is not None:
            self.entity.removespawner( spawner )
            index = self.__get_selected_list_index()
            self.list.DeleteItem( index )
            self.__update_binding_ctrls()
    
    def on_select_spawner(self, event):
        name = self.__get_selected_list_item( self.ID_Spawner )
        spawner = servers.get_world_interface().getgameentity( str(name) )
        if spawner is None:
            cjr.show_error_message(
                "Spawner '" + name + "'  not found."
                )
        else:
            state = app.get_object_state()
            entity_id = spawner.getid()
            state.addentitytoselection( entity_id )
    
    def on_change_probability(self, event):
        name = self.__get_selected_list_item( self.ID_Spawner )
        spawner = servers.get_world_interface().getgameentity( str(name) )
        if spawner is None:
            cjr.show_error_message(
                "Spawner '" + name + "'  not found."
                )
        else:
            probability = self.float_probability.get_value()
            self.entity.setprobability( spawner, probability )
            index = self.__get_selected_list_index()
            self.list.SetStringItem( index, self.ID_Probability, str(probability) )
    
    def set_entity(self, entity):
        self.entity = entity
        self.__update_binding_list()
    
    def refresh(self):
        try:
            self.set_entity( self.entity )
        except:
            self.Close()


# create_all_editors function
def create_all_editors(entity, parent):
    editor = BindingsPanel(parent, -1)
    editor.set_entity(entity)
    return [ ('Spawners', editor) ]
