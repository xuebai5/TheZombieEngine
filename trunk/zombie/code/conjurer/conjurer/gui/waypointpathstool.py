##\file waypointpathstool.py
##\brief Waypoint Paths Tool dialog

import wx

import app
import servers
import togwin
import pynebula

import conjurerframework as cjr
import conjurerconfig as cfg

# MultiPlaceDialog class
class WaypointsPathsDialog(togwin.ChildToggableDialog):
    """Waypoint Paths Tool"""
    
    def __init__(self, parent):
        togwin.ChildToggableDialog.__init__(
            self, "Waypoint Paths Tool", parent
            )

        # Getting waypoint server
        self.server = servers.get_waypoint_server()
        if self.server is None:
            cjr.show_error_message(
                "Cannot find waypoint server"
                )
            self.Close()

        # Creating controls
        
        self.path_list = wx.ListBox( self, style = wx.LB_SORT )
        self.newpath = wx.Button( self, label = "&New Path" )
        self.addwp = wx.Button( self, label = "&Add Waypoint" )
        self.removewp = wx.Button( self, label = "&Remove Waypoint" )
        self.renamepath = wx.Button( self, label = "Rena&me Path" )
        self.destroypath = wx.Button( self, label = "&Destroy Path" )
        self.selectwaypoints = wx.Button( self, label = "&Select Waypoints" )
        self.inbetween = wx.Button( self, label = "Add in &Between" )

        self.attachentity = wx.Button( self, label = "Attach selected& Entity" )
        
        self.refresh = wx.Button( self, label = "&Refresh" )
               
        self.__set_properties()
        self.__do_layout()
        self.__bind_events()

    def __set_properties(self):
        self.restore([None])

    def __refresh_listpaths(self):
        # empties the list
        self.path_list.Clear()
        
        # getting server
        server = servers.get_waypoint_server()
        
        if server is None:
            return
        
        num = server.getnumpaths()
        
        for i in range(num):
            self.path_list.Append( self.server.getpathname( i ) )

    def __do_layout(self):
        sizer_border = wx.BoxSizer(wx.VERTICAL)
        sizer_layout = wx.BoxSizer(wx.HORIZONTAL)
        sizer_buttons = wx.BoxSizer(wx.VERTICAL)
        
        sizer_buttons.Add(
            self.newpath, 
            0, 
            wx.EXPAND
            )
        sizer_buttons.Add(
            self.addwp, 
            0, 
            wx.EXPAND|wx.TOP, 
            cfg.BORDER_WIDTH
            )
        sizer_buttons.Add(
            self.inbetween, 
            0, 
            wx.EXPAND|wx.TOP, 
            cfg.BORDER_WIDTH
            )
        sizer_buttons.Add(
            self.removewp, 
            0,
            wx.EXPAND|wx.TOP, 
            cfg.BORDER_WIDTH
            )
        sizer_buttons.Add(
            self.renamepath, 
            0, 
            wx.EXPAND|wx.TOP, 
            cfg.BORDER_WIDTH
            )
        sizer_buttons.Add(
            self.destroypath, 
            0, 
            wx.EXPAND|wx.TOP, 
            cfg.BORDER_WIDTH
            )
        sizer_buttons.Add(
            self.selectwaypoints,
            0, 
            wx.EXPAND|wx.TOP, 
            cfg.BORDER_WIDTH
            )
        sizer_buttons.Add(
            self.refresh,
            0,
            wx.EXPAND|wx.TOP,
            cfg.BORDER_WIDTH
            )
        sizer_buttons.Add(
            self.attachentity, 
            0, 
            wx.EXPAND|wx.TOP,
            cfg.BORDER_WIDTH
            )
        sizer_layout.Add(
            self.path_list, 
            1, 
            wx.EXPAND
            )
        sizer_layout.Add(   
            sizer_buttons, 
            0, 
            wx.FIXED_MINSIZE|wx.LEFT, 
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
        self.Bind(wx.EVT_BUTTON, self.on_refresh_pathlist, self.refresh)
        self.Bind(wx.EVT_BUTTON, self.on_remove_waypoint, self.removewp)
        self.Bind(wx.EVT_BUTTON, self.on_add_waypoint, self.addwp)
        self.Bind(wx.EVT_BUTTON, self.on_selectwaypoints, self.selectwaypoints)
        self.Bind(wx.EVT_BUTTON, self.on_newpath, self.newpath)
        self.Bind(wx.EVT_BUTTON, self.on_renamepath, self.renamepath)
        self.Bind(wx.EVT_BUTTON, self.on_destroypath, self.destroypath)
        self.Bind(wx.EVT_BUTTON, self.on_inbetween, self.inbetween)
        self.Bind(wx.EVT_BUTTON, self.on_attachentity, self.attachentity)

    def on_attachentity (self, event):
        """ Attach all selected living entities selected in
            summoner to the path selected in list """
        path_name = self.path_list.GetStringSelection()
        
        if path_name == '':
            return

        # add all living entities 
        entities = self.__get_selected_living_entities()
        if ( entities != [] ):
            for entity in entities:
                entity.setpathid(
                    self.server.getpathid( str(path_name) )
                    )

    def __get_selected_living_entities ( self ):
        """ Return all living entities selected in summoner """
        entities = []
        num_entities = app.get_object_state().getselectioncount()

        if num_entities == 0:
            cjr.show_error_message(
                "You must select at least one entity in summoner"
                )
            return entities

        for i in xrange(num_entities):
            entity = app.get_object_state().getselectedentity(i)
            if entity.hascomponent("ncGameplayLiving"):
                entities.append(entity)            

        return entities

    def on_inbetween(self, event):
        
        state = app.get_object_state()
        
        server = servers.get_waypoint_server()
                
        if server is None:
            return

        list_boundary = []
        list_to_be_in_between = []
        
        for i in range( state.getselectioncount() ):
            entity = state.getselectedentity(i)
            if entity.isa( 'newaypoint' ):
                if entity.getpathid() != 0:
                    list_boundary.append( entity )
                else:
                    list_to_be_in_between.append( entity )
                    
        if len(list_boundary) != 2:
            return
                    
        prev = list_boundary[0]
        last = list_boundary[1]
         
        for i in range(len(list_to_be_in_between)):
            server.insertwaypointinpath(
                list_to_be_in_between[i].getwaypointid(), 
                prev.getwaypointid(), 
                last.getwaypointid() 
                )
            prev = list_to_be_in_between[i]

    def on_destroypath(self, event):
        if not self.has_path_selected():
            cjr.show_error_message(
                "Please select a path"
                )
            return
        server = servers.get_waypoint_server()
        if server is None:
            return
        server.destroyspath(
            self.get_selected_path_name_as_string()
            )
        self.__refresh_listpaths()

    def on_newpath(self, event):
        dialog = wx.TextEntryDialog(
                        self, 
                        "Pick a new path name", 
                        "New Path", 
                        "SomeName"
                        )

        if dialog.ShowModal() == wx.ID_OK:
            server = servers.get_waypoint_server()
            if server is None:
                return
            server.createnewpath(
                str( dialog.GetValue() ) 
                )
        
        dialog.Destroy()
        self.__refresh_listpaths()

    def on_renamepath(self, event):
        if not self.has_path_selected():
            cjr.show_error_message(
                "Please select a path"
                )
            return
        server = servers.get_waypoint_server()
        if server is None:
            return
        path_old_name = self.get_selected_path_name_as_string()
        dialog = wx.TextEntryDialog(
                        self, 
                        "New name for path", 
                        "Path rename", 
                        path_old_name 
                        )
        if dialog.ShowModal() == wx.ID_OK:
            server.renamepath(
                str(path_old_name), 
                str( dialog.GetValue() ) 
                )
        dialog.Destroy()
        self.__refresh_listpaths()

    def on_refresh_pathlist(self, event):
        self.__refresh_listpaths()

    def on_selectwaypoints(self, event):
        if not self.has_path_selected():
            cjr.show_error_message(
                "Please select a path"
                )
            return
        server = servers.get_waypoint_server()
        if server is None:
            return
        path_name = self.get_selected_path_name()
        state = app.get_object_state()
        state.resetselection()
        path_id = server.getpathid(
                        self.get_selected_path_name_as_string()
                        )
        for i in range( server.getnumwaypoints( path_id ) ):
            entity = server.getwaypointfrompath(
                            path_id, 
                            i 
                            )
            if entity is not None:
                state.addentitytoselection(
                    entity.getid()
                    )

    def on_remove_waypoint(self, event):
        server = servers.get_waypoint_server()
        if server is None:
            return
        
        state = app.get_object_state()

        for i in range( state.getselectioncount() ):
            entity = state.getselectedentity(i)
            if entity.isa( 'newaypoint' ):
                server.removewaypointfromitspath( entity.getwaypointid() )

    def on_add_waypoint(self, event):
        if not self.has_path_selected():
            cjr.show_error_message(
                "Please select a path"
                )
            return
        server = servers.get_waypoint_server()
        if server is None:
            return
        state = app.get_object_state()
        path_name = self.get_selected_path_name_as_string()
        for i in range( state.getselectioncount() ):
            entity = state.getselectedentity(i)
            if entity.isa( 'newaypoint' ):
                if entity.getpathid() != 0:
                    self.on_remove_waypoint( event )
                server.insertwpatthebackofpath(
                    entity.getwaypointid(), 
                    path_name
                    )

    def get_selected_path_name(self):
        return self.path_list.GetStringSelection()
        
    def get_selected_path_name_as_string(self):
        return str( self.get_selected_path_name() )

    def has_path_selected(self):
        return self.path_list.GetSelection() != wx.NOT_FOUND

    def persist(self):
        return [
            create_window, 
            (), # no parameters for create function
            None
            ]

    def restore(self, data_list):
        self.__refresh_listpaths()

# create_window function
def create_window(parent):
    return WaypointsPathsDialog(parent)
