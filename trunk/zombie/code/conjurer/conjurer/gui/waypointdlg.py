##\file waypointdlg.py
##\brief Waypoint editor dialog

import wx

import pynebula

import app
import servers
import togwin

import conjurerconfig as cfg


# WaypointEditorDialog class
class WaypointEditorDialog(togwin.ChildToggableDialog):
    """Dialog that allows to enable/disable waypoint attributes"""
    
    def __init__(self, parent):
        togwin.ChildToggableDialog.__init__(
            self, "Waypoint editor", parent
            )
        self.clbox = wx.CheckListBox(self, -1)
        
        self.__set_properties()
        self.__do_layout()
        self.__bind_events()

    def __set_properties(self):
        self.__set_attributes()

    def __do_layout(self):
        sizer = wx.BoxSizer(wx.HORIZONTAL)
        sizer.Add(
            self.clbox, 
            1, 
            wx.ALL|wx.EXPAND, 
            cfg.BORDER_WIDTH
            )
        self.SetSizerAndFit(sizer)

    def __bind_events(self):
        self.Bind(
            wx.EVT_CHECKLISTBOX,
            self.on_attribute_toggled,
            self.clbox
            )
        pynebula.pyBindSignal(
            app.get_object_state(), 
            'selectionchanged',
            self, 
            'onselectionchanged', 
            0
            )

    def __get_selected_waypoints(self):
        waypoints = []
        state = app.get_object_state()
        for index in range( state.getselectioncount() ):
            entity = state.getselectedentity(index)
            if entity.hascomponent( 'ncRnsWaypoint' ):
                waypoints.append( entity )
        return waypoints

    def __fill_attribute_list(self, waypoints):
        if len(waypoints) > 0:
            waypoint = waypoints[0]
            num_attribs = waypoint.getnumattributes()
            for index in range(num_attribs):
                self.clbox.Append( waypoint.getattributelabel(index) )

    def __update_attribute_list(self, waypoints):
        for index in range( self.clbox.GetCount() ):
            self.clbox.Check(index, False)
            for each_waypoint in waypoints:
                if each_waypoint.getattributestate(index):
                    self.clbox.Check(index)
                    break
            self.__update_label(index, waypoints)

    def __update_label(self, index, waypoints):
        # Count the number of waypoints with the attribute enabled
        num_enables = 0
        for each_waypoint in waypoints:
            if each_waypoint.getattributestate(index):
                num_enables = num_enables + 1
        
        # Update the attribute label with the name and enable count
        if len(waypoints) > 0:
            label = waypoints[0].getattributelabel(index)
            if num_enables > 0 and num_enables < len(waypoints):
                label = label + " (" + str(num_enables) + "/" + str(len(waypoints)) + ")"
            scroll_pos = self.clbox.GetScrollPos(wx.VERTICAL)
            self.clbox.SetString(index, label)
            # Needed because changing a string in the list makes it scroll up
            # to the top element
            self.clbox.SetFirstItem(scroll_pos)

    def __set_attributes(self):
        waypoints = self.__get_selected_waypoints()
        if len(waypoints) > 0:
            if self.clbox.GetCount() == 0:
                self.__fill_attribute_list(waypoints)
            self.__update_attribute_list(waypoints)
        self.clbox.Enable( len(waypoints) > 0 )

    def __append_set_attribute_cmd(self, waypoint, index, value, cmd):
        cmd_init = "noreport = pynebula.lookup('/sys/servers/entityobject')" \
            ".getentityobject(" + str(waypoint.getid()) + ")" \
            ".setattributestate(" + str(index) + ","
        cmd[0] = cmd[0] + cmd_init + str(value) + ");"
        cmd[1] = cmd[1] + cmd_init + str(waypoint.getattributestate(index)) + ");"

    def on_attribute_toggled(self, event):
        # Update the toggled attribute for all the selected waypoints
        waypoints = self.__get_selected_waypoints()
        if len(waypoints) > 0:
            index = event.GetSelection()
            value = self.clbox.IsChecked(index)
            cmd = ["", ""]
            for each_waypoint in waypoints:
                self.__append_set_attribute_cmd(
                    each_waypoint,
                    index, 
                    value, 
                    cmd
                    )
            servers.get_command_server().newcommand(
                cmd[0],
                cmd[1]
                )
            
            # Update the attribute label
            self.__update_label(index, waypoints)

    def onselectionchanged(self):
        self.__set_attributes()

    def refresh(self):
        self.__set_attributes()

    def persist(self):
        return [
            create_window,
            () # no parameters for create function
            ]


# create_window function
def create_window(parent):
    return WaypointEditorDialog(parent)
