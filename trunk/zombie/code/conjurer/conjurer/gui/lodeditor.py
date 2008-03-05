##\file lodditor.py
##\brief Specific LOD editor panel

import wx

import editorpanel
import floatctrl

import conjurerconfig as cfg


# LODPanel class
class LODPanel(editorpanel.EditorPanel):
    
    # LOD lists columns indices
    ID_Level = 0
    ID_Distance = 1
    ID_Geometry = 2
    
    def __init__(self, *args, **kwds):
        editorpanel.EditorPanel.__init__(self, *args, **kwds)
        self.entity = None
        
        self.list_geom_lods = wx.ListCtrl(
                                        self, 
                                        -1,
                                        style=wx.LC_REPORT|wx.LC_SINGLE_SEL
                                        )
        self.label_geom_max = wx.StaticText(
                                            self, 
                                            -1, 
                                            "Maximum distance"
                                            )
        self.float_geom_max = floatctrl.FloatCtrl(
                                            self,
                                            size=wx.Size(50,-1), 
                                            style=wx.TE_RIGHT
                                            )

        self.__set_properties()
        self.__do_layout()
        self.__bind_events()

    def __set_properties(self):
        self.SetScrollRate(10, 10)
        
        # Geometry LOD list columns
        self.list_geom_lods.InsertColumn(
            self.ID_Level, 
            "Level", 
            wx.LIST_FORMAT_RIGHT
            )
        self.list_geom_lods.InsertColumn(
            self.ID_Distance, 
            "Max. distance", 
            wx.LIST_FORMAT_RIGHT
            )
        self.list_geom_lods.InsertColumn(
            self.ID_Geometry, 
            "Geometry"
            )

    def __do_layout(self):
        sizer_layout = wx.BoxSizer(wx.VERTICAL)
        sizer_geom = wx.BoxSizer(wx.VERTICAL)
        sizer_geom_max = wx.BoxSizer(wx.HORIZONTAL)
        sizer_geom.Add(
            self.list_geom_lods, 
            1,
            wx.ALL|wx.EXPAND,
            cfg.BORDER_WIDTH
            )
        sizer_geom_max.Add(
            self.label_geom_max, 
            0,
            wx.ALIGN_CENTER_VERTICAL|wx.FIXED_MINSIZE
            )
        sizer_geom_max.Add(
            self.float_geom_max, 
            0,
            wx.LEFT|wx.FIXED_MINSIZE, 
            cfg.BORDER_WIDTH
            )
        sizer_geom.Add(
            sizer_geom_max,
            0,
            wx.LEFT|wx.RIGHT|wx.BOTTOM|wx.EXPAND, 
            cfg.BORDER_WIDTH
            )
        sizer_layout.Add(
            sizer_geom, 
            1, 
            wx.EXPAND
            )
        self.SetSizerAndFit(sizer_layout)

    def __bind_events(self):
        self.Bind(
            wx.EVT_LIST_ITEM_SELECTED, 
            self.on_select_geom_lod, 
            self.list_geom_lods
            )
        self.Bind(
            floatctrl.EVT_FLOAT, 
            self.on_change_geom_max, 
            self.float_geom_max
            )

    def __get_selected_geom_index(self):
        return self.list_geom_lods.GetNextItem(
            -1, 
            wx.LIST_NEXT_ALL,
            wx.LIST_STATE_SELECTED
            )

    def __update_geom_params_ctrls(self):
        index = self.__get_selected_geom_index()
        if index != -1:
            min_value, max_value = self.entity.getlevelrange( index )
            self.float_geom_max.set_value( max_value )
        self.float_geom_max.Enable( index != -1 )

    def on_select_geom_lod(self, event):
        self.__update_geom_params_ctrls()

    def on_change_geom_max(self, event):
        # Set new max for selected geometry LOD
        index = self.__get_selected_geom_index()
        max_value = self.float_geom_max.get_value()
        min_value, old_max_value = self.entity.getlevelrange( index )
        self.entity.setlevelrange( index, min_value, max_value )
        self.list_geom_lods.SetStringItem(
            index, 
            self.ID_Distance, 
            str(max_value)
            )
        
        # Set new min for next geometry LOD with respect to the selected one
        index = index + 1
        if index < self.entity.getnumlevels():
            min_value = max_value
            old_min_value, max_value = self.entity.getlevelrange( index )
            self.entity.setlevelrange(
                index, 
                min_value,
                max_value
                )

    def set_entity(self, entity):
        self.entity = entity
        
        # Fill geometry LOD list
        self.list_geom_lods.DeleteAllItems()
        for i in range( self.entity.getnumlevels() ):
            min_value, max_value = self.entity.getlevelrange(i)
            geom = self.entity.getlevelclass(i)
            self.list_geom_lods.Append(
                [str(i), str(max_value), str(geom)] 
                )
        
        # Update controls that depends on selected geometry LOD
        self.__update_geom_params_ctrls()

    def refresh(self):
        self.set_entity( self.entity )


# create_all_editors function
def create_all_editors(entity, parent):
    editor = LODPanel(parent, -1)
    editor.set_entity(entity)
    return [ ('LOD', editor) ]
