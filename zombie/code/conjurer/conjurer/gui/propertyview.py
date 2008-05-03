#-------------------------------------------------------------------------
# Conjurer Services, S.A. (2005)
#-------------------------------------------------------------------------
# Description: General property editor. This editor get information from
# an xml file and builds an editor according to it.
#-------------------------------------------------------------------------

#-------------------------------------------------------------------------
# Imports
#-------------------------------------------------------------------------
import wx

# Property grid
import wx.propgrid as pg

# Conjurer
import childdialoggui
import conjurerconfig as cfg

# selectors
import objdlg
import editorpanel

# Holy property grid wrapper
import propgrid
import events
import app

import servers

INHERITED_COLOUR = wx.Colour(251, 247, 181) # Smooth yellow
SUBGROUP_COLOUR =  wx.Colour(220, 251, 181) # Smooth green


propertyEVT_OK_REQUESTED = wx.NewEventType()
EVT_OK_REQUESTED = wx.PyEventBinder(
                                    propertyEVT_OK_REQUESTED,
                                    )
                                    
propertyEVT_CANCEL_REQUESTED = wx.NewEventType()
EVT_CANCEL_REQUESTED = wx.PyEventBinder(
                                    propertyEVT_CANCEL_REQUESTED,
                                    )



class PropertyView(editorpanel.EditorPanel):
    def __init__ (self, parent, model=None):
        """ This class displays the property editor """
        editorpanel.EditorPanel.__init__(self, parent, -1)
        self.model = model
        self.parent = parent        
        self.pg = None
        self.open_editor()  

    def open_editor (self):
        """ This method shows the property editor if the object is 
        an entity. If not it shows an error message """
        if self.model is None:
            self.init_property_win()
            self.bind_signals()
        elif self.model.get_properties_list() != []:
            self.init_property_win()
            self.bind_signals()                
        else:
            self.draw_notavailablemessage()

    def bind_signals (self):  
        """ Bind all signals used in the interface """
        self.Bind(events.EVT_SELECTION, self.on_pg_selected, self.pg)
        self.Bind(events.EVT_CHANGED, self.on_pg_changed, self.pg)
        self.Bind(events.EVT_RIGHT_CLICK, self.on_pg_right_click, self.pg)
        self.Bind(
            wx.EVT_BUTTON,
            self.on_inspect_class_button,
            self.button_inspect_class
            )
        self.Bind(wx.EVT_BUTTON, self.on_ok_button, self.btn_ok)
        self.Bind(wx.EVT_BUTTON, self.on_revert_button, self.btn_revert)
        self.Bind(wx.EVT_BUTTON, self.on_cancel_button, self.btn_cancel)
        self.Bind(wx.EVT_RADIOBUTTON, self.on_check)

    def tidy_up_for_close(self):
        """ This is here to get round a problem with the 
        Property Grid control, where it was reporting in
        error that changes had been lost. Clearing the 
        Property Grid is a simple solution, and it works! """
        if self.pg:
            self.pg.clear()

    def on_check (self, event):
        mode = event.GetEventObject().GetLabel()
        if mode == 'Normal':
            self.model.set_mode(1)
        elif mode == 'Cascade':
            self.model.set_mode(2)
        elif mode == 'Force':
            self.model.set_mode(3)

    def on_pg_right_click (self, event):
        """ Open an inspector if the click property is an object """
        pid = event.get_value()
        prop_type = self.pg.get_property_type(pid)
        if prop_type == propgrid.Type_Object:
            prop_value = self.pg.get_property_value(pid)
            if prop_value is not None:
                obj_type = self.model.get_object_type()
                if obj_type == "nroot" or obj_type == "entityclass":
                    prop_value = prop_value.getfullname()
                win = objdlg.create_window(
                            app.get_top_window(), 
                            prop_value
                            )
                win.display()
        event.Skip()

    def on_pg_changed (self, event):
        """ Set the new property value """
        prop_info = event.get_value()
        data = self.pg.get_property_data(prop_info.get_pid())
        if data is not None:
            prop_name = data['name']
            if self.pg.get_property_type(prop_info.get_pid()) != propgrid.Type_Subgroup:
                if not data['multiparam']:
                    self.model.set_property(prop_name, prop_info.get_value())
                    self.refresh_colours(prop_info.get_pid(), prop_name)
                else:
                    parent = self.pg.get_parent(prop_info.get_pid())
                    self.model.set_property(
                        self.pg.get_property_name(parent), 
                        self.pg.get_property_value(parent)
                        )
                    self.refresh_colours(prop_info.get_pid(), prop_name)
            else:
                if data['multiparam']:
                    self.model.set_property(prop_name, prop_info.get_value())
                self.refresh_colours(prop_info.get_pid(), prop_name)

    def on_pg_selected (self, event):
        """ Update the documentation when a property is selected """
        pid = event.get_selection()
        data = self.pg.get_property_data(pid)
        if data is not None:
            if data.has_key('doc'):
                self.doc_text.SetLabel(data['doc'])
        event.Skip()

    def on_inspect_class_button(self, event):
        class_name = str( self.model.get_object_class() )
        entity_server = servers.get_entity_class_server()
        class_to_inspect = entity_server.getentityclass(
                                    class_name
                                    )
        win = objdlg.create_window(
                    app.get_top_window(), 
                    class_to_inspect.getfullname()
                    )
        win.display()

    def on_revert_button (self, event):
        """ Restore the values to inital values """
        # Update property grid values if the model supports it
        if not self.model.is_for_single_object():
            return True
        for property in self.model.get_dirty_property_list():
            d_prop_name = property['name']
            d_prop_old_value = property['old_value']
            cat = self.pg.get_first_category()
            while cat is not None:
                prop = self.pg.get_first_child_of_category( cat )
                while prop is not None:
                    prop_name = self.pg.get_property_data( prop )["name"]
                    if prop_name == d_prop_name:
                        self.pg.set_property_value(
                            prop, 
                            d_prop_old_value[0][0]
                            )
                    prop = self.pg.get_next_sibling( prop )
                cat = self.pg.get_next_category( cat )

        self.pg.Refresh()

        # Update real values
        self.model.revert()

    def __signal_property_view_event(self, event_type):
        custom_event = wx.PyCommandEvent(
                                event_type,
                                self.GetId()
                                )
        self.GetEventHandler().ProcessEvent(
            custom_event
            )

    def on_ok_button(self, event):
        self.__signal_property_view_event(
            propertyEVT_OK_REQUESTED
            )
        event.Skip()
    
    def on_cancel_button(self, event):
        self.__signal_property_view_event(
            propertyEVT_CANCEL_REQUESTED
            )
        event.Skip()

    def cancel(self):
        """ Restore values to init values and tidy up """
        self.model.revert()
        self.tidy_up_for_close()

    def apply_changes (self):
        """ Add changes to undo stack and tidy up """
        dirty_list = self.model.get_dirty_property_list()
        command = ''
        undo_command = ''
        if len(dirty_list) > 0:
            # Get redo and undo commands
            for prop in dirty_list:
                new_command = self.model.get_string_command(
                                            prop['name'], 
                                            prop['value'], 
                                            self.model.get_object()
                                            )
                init_value = self.model.init_values[prop['name']]['value']
                new_undo_command = self.model.get_string_command(
                                                    prop['name'], 
                                                    init_value[0], 
                                                    self.model.get_object()
                                                    )
                if new_command is None or new_undo_command is None:
                    break;
                command += new_command
                undo_command += new_undo_command

            # Put commands in undo server
            servers.get_command_server().newcommand(
                str(command[:-1]), 
                str(undo_command[:-1])
                )           
        self.tidy_up_for_close()

    def refresh_colours (self, pid, prop_name):
        if self.model.is_inherited(prop_name):
            self.pg.change_property_colour(pid, INHERITED_COLOUR)
        else:
            self.pg.change_property_colour(pid, wx.WHITE)

    def init_property_win(self):
        """ Display the property editor interface 
            (buttons, property grid...) """
        self.normal_exec = wx.RadioButton(self, -1, "Normal")
        self.normal_exec.SetToolTip(
            wx.ToolTip("Changes only affect this class")
            )
        self.normal_exec.SetValue(True)
        self.cascade_exec = wx.RadioButton(self, -1, "Cascade")
        self.cascade_exec.SetToolTip(
            wx.ToolTip(
                "Changes affect only this class and child classes with the same value"
                )
            )
        self.force_exec = wx.RadioButton(self, -1, "Force")
        self.force_exec.SetToolTip(
            wx.ToolTip(
                "Changes affect this class and all its children"
                )
            )
        object_type = self.model.get_object_type()
        if object_type != "entityclass":
            self.normal_exec.Disable()
            self.cascade_exec.Disable()
            self.force_exec.Disable()

        self.button_inspect_class = wx.Button(
                                                self, 
                                                -1, 
                                                "I&nspect Class", 
                                                style=wx.BU_EXACTFIT
                                                )
        if object_type != "entity":
            self.button_inspect_class.Hide()

        # Property grid 
        self.pg = propgrid.PropertyGrid(
                        self, 
                        style = pg.wxPG_BOLD_MODIFIED |
                                    pg.wxPG_DEFAULT_STYLE|
                                    pg.wxPG_SPLITTER_AUTO_CENTER|
                                    pg.wxPG_AUTO_SORT
                        )

        # Draw the property content
        self.fill_property_editor()

        # Doc text
        self.doc_box = wx.StaticBox(self, -1, "Documentation")
        self.doc_text = wx.StaticText(
                                self, 
                                -1, 
                                "Documentation for classes and commands"
                                )

        # Ok button
        self.btn_ok = wx.Button(
                                self, 
                                -1, 
                                "&OK"
                                )
        self.btn_ok.SetToolTip(
            wx.ToolTip(
                "Accept changes and exit"
                )
            )

         # Revert button
        self.btn_revert = wx.Button(
                                    self, 
                                    -1, 
                                    "&Revert", 
                                    )
        self.btn_revert.SetToolTip(
            wx.ToolTip(
                "Revert all changes to the state when this window was opened"
                )
            )
        if not self.model.is_for_single_object():
            self.btn_revert.Hide()

        # Cancel button
        self.btn_cancel = wx.Button(
                                    self, 
                                    wx.ID_CANCEL,
                                    "&Cancel", 
                                    )        
        self.btn_cancel.SetToolTip(
            wx.ToolTip(
                "Revert all changes and exit this window"
                )
            )                                        
        # Do layout
        sizer = wx.BoxSizer(wx.VERTICAL)
        horizontal_sizer = wx.BoxSizer(wx.HORIZONTAL)
        horizontal_sizer.Add(
            self.normal_exec, 
            0, 
            wx.ALIGN_CENTER_VERTICAL|wx.EXPAND
            )
        horizontal_sizer.Add(
            self.cascade_exec, 
            0, 
            wx.ALIGN_CENTER_VERTICAL|wx.LEFT|wx.EXPAND, 
            cfg.BORDER_WIDTH
            )
        horizontal_sizer.Add(
            self.force_exec, 
            0, 
            wx.ALIGN_CENTER_VERTICAL|wx.LEFT|wx.EXPAND, 
            cfg.BORDER_WIDTH
            )
        horizontal_sizer.Add( (1, 1), 1 )
        horizontal_sizer.Add(
            self.button_inspect_class, 
            0, 
            wx.ALIGN_CENTER_VERTICAL,
            cfg.BORDER_WIDTH
            )
        sizer.Add(
            horizontal_sizer, 
            0, 
            wx.ALL|wx.EXPAND, 
            cfg.BORDER_WIDTH
            )
        # propertygrid
        horizontal_sizer = wx.BoxSizer(wx.HORIZONTAL)
        horizontal_sizer.Add(
            self.pg, 
            1, 
            wx.TOP|wx.EXPAND,
            cfg.BORDER_WIDTH
            )
        sizer.Add(
            horizontal_sizer, 
            1, 
            wx.BOTTOM|wx.RIGHT|wx.LEFT|wx.EXPAND, 
            cfg.BORDER_WIDTH
            )
        # documentation
        bsizer = wx.StaticBoxSizer(self.doc_box, wx.VERTICAL)
        bsizer.Add(self.doc_text, 0, wx.ALL, 3)
        sizer.Add(
            bsizer, 
            0, 
            wx.LEFT|wx.RIGHT|wx.EXPAND, 
            cfg.BORDER_WIDTH
            )
        # Action buttons
        horizontal_sizer = wx.BoxSizer(wx.HORIZONTAL)
        horizontal_sizer.Add(self.btn_ok, 0)
        horizontal_sizer.Add(
            self.btn_revert, 
            0, 
            wx.LEFT,
            cfg.BORDER_WIDTH
            )
        horizontal_sizer.Add(
            self.btn_cancel, 
            0, 
            wx.LEFT, 
            cfg.BORDER_WIDTH
            )
        sizer.Add(
            horizontal_sizer, 
            0, 
            wx.ALIGN_RIGHT|wx.ALL,
            cfg.BORDER_WIDTH
            )
        self.SetSizer(sizer)

    def fill_property_editor (self):
        """ Draw all properties with the model information """
        if self.model is None:
            return
        component_list = self.model.get_component_list()
        # Start to draw the properties 
        self.pg.begin()
        # Store group pids
        self.subgroups = {}

        for component in component_list:
            self.pg.append_category(component['group'])
            # Add the properties for this component            
            self.render_properties(
                component['properties']
                )                        
        # All categories drawn
        self.pg.end()

    def set_model ( self, model ):
        """ Sets the model for the property """
        self.model = model
        self.fill_property_editor()

    def draw_notavailablemessage(self):
        """ When an object does not have any properties 
        this message is shown in the panel """
        message = "No component for this entity has property"\
                            " information in the database.\n"\
                            "Please inform the programming team.\n"\
                            "In the meantime you can use the state"\
                            " and commands tab."
        text = wx.StaticText(self, -1, message)
        horizontal_sizer = wx.BoxSizer(wx.HORIZONTAL)
        horizontal_sizer.Add(text, 1, wx.ALL|wx.EXPAND, 10)
        self.SetSizer(horizontal_sizer) 

    def render_properties (self, properties):
        """ Render all properties passed in the property grid """        
        for property in properties:
            property_name = property["name"]
            multiparam = False
            # The property could be hidden for the users
            if self.__is_property_visible(property):
                pid = None
                # Initial value (execute the getter)
                value = self.__get_initial_property_value(property)
                # Iterate for all parameters                 
                if len(value) != len(property['params']):
                    continue

                # Get the subgroup for the property if any
                subgroup = self.__get_property_subgroup(property)

                if subgroup is not None:
                    # Create subgroup if needed
                    if not self.subgroups.has_key(subgroup):
                        # If this is a new group then register it
                        data = {
                                    'doc': "Expand to display all properties for this group",
                                    'name': property_name,
                                    'multiparam': multiparam 
                                        }
                        pid = self.pg.append_group(None, subgroup, None, data)
                        self.subgroups[subgroup] = pid
                        self.pg.change_property_colour( pid, SUBGROUP_COLOUR )
                    else:                    
                        pid = self.subgroups[subgroup]

                # If is a multiparam create subgroup for it
                if len(property['params']) > 1:
                    multiparam = True
                    data = {
                                'doc': "Expand to display all properties for this group",
                                'name': property_name,
                                'multiparam': multiparam 
                                }
                    pid = self.pg.append_group(pid, property_name, None, data)

                for param, value in map(None, property['params'], value):
                    # Extra information for the property
                    data = { 'doc': param['doc'],
                             'name': property_name,
                             'multiparam': multiparam }
                    pid2 = self.pg.append_property(
                                pid, 
                                param['name'], 
                                param, 
                                data
                                )
                    self.pg.set_property_value(pid2, value)
                    self.refresh_colours(pid2, param['name'])

    def __get_initial_property_value (self, property):
        """ Get the initial value (the getter) """
        value_structure = self.model.get_property(
                                    property['name']
                                    )
        value = []
        if value_structure is not None:
            # Could be a multiple object selection with diferent values
            if value_structure != '-':
                if len(value_structure['value']) > 0:
                    # Get the first value, this must be changed
                    # to support multiple getters
                    value = value_structure['value'][0]                    
            else:
                # This is show when is a multipe selection with diferent values
                value = '-'
        return value

    def __is_property_visible (self, property):
        """ Return true if the property should be visible to users """
        visible = True
        for attr in property["attrs"]:
            if attr["type"] == "private":
                if attr["value"].lower() == 'true':
                    visible = False
        return visible

    def __get_property_subgroup (self, property):
        """ Get the property subgroup, None if no have a subgroup """
        subgroup = None
        for attr in property["attrs"]:
            if attr["type"] == "subgroup":
                subgroup = attr["value"]
        return subgroup

    def refresh (self):
        self.pg.clear()
        self.fill_property_editor()


class PropertyViewWindow(childdialoggui.childDialogGUI):
    # If the object is instantiated from this class, this will be a child dialog
    def __init__(self, parent, model = None):
        childdialoggui.childDialogGUI.__init__(
            self, 
            "Property Editor", 
            parent
            )           
        self.model = model
        self.is_applying_changes = False
        self.editor = PropertyView(self, self.model)
        # Settings
        self.parent = parent
        self.SetSize( (400, 400) )
        self.SetTitle( self.model.get_object_name() )
        self.editor.Bind(EVT_CANCEL_REQUESTED, self.__cancel_requested)
        self.editor.Bind(EVT_OK_REQUESTED, self.__ok_requested)
        self.Bind(wx.EVT_CLOSE, self.__on_close)
        return self.editor

    def __cancel_requested(self, event):
        self.Close()

    def __ok_requested(self, event):
        self.is_applying_changes = True
        self.Close()

    def __on_close(self, event):
        if self.is_applying_changes:
            self.editor.apply_changes()
        else:
            self.editor.cancel()
        self.is_applying_changes = False
        event.Skip()

    def persist(self):
        pass

    def restore(self, data_list):
        pass

    def refresh(self): 
        pass
   
def create_window(frame, model):
    return PropertyViewWindow(frame, model)


# Use this class to show the property view as a modal dialog
class PropertyViewDialog(wx.Dialog):
    def __init__(self, parent, title, model):
        wx.Dialog.__init__(
            self, 
            parent, 
            -1, 
            title,
            style = wx.DEFAULT_DIALOG_STYLE |
                        wx.RESIZE_BORDER |
                        wx.TAB_TRAVERSAL
            )
        self.model = model
        self.editor = PropertyView(self, self.model)
        self.parent = parent
        self.editor.Bind(EVT_CANCEL_REQUESTED, self.__cancel_requested)
        self.editor.Bind(EVT_OK_REQUESTED, self.__ok_requested)
        self.Bind(wx.EVT_CLOSE, self.__on_close)
        self.SetSize( (400, 400) )

    def __cancel_requested(self, event):
        self.editor.cancel()
        self.EndModal(wx.ID_CANCEL)

    def __ok_requested(self, event):
        self.editor.apply_changes()
        self.EndModal(wx.ID_OK)

    def __on_close(self, event):
        self.__cancel_requested(event)

