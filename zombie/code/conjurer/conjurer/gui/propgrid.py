##\file propgrid.py
##\brief Wrapper for the horribly interfaced property control

import wx
import wx.propgrid as pg

import events
import servers

import re

# Selectors dialogs for nebula2 objects
import pynebula
import objbrowserwindow
import grimoire
import entitybrowser
import nodelibdlg
import musictabledlg

# Special controls
import intslider
import floatslider


# Property types
Type_Bool = 'b'
Type_Float = 'f'
Type_Int = 'i'
Type_Object = 'o'
Type_String = 's'
Type_Vector3 = 'fff'
Type_Vector4 = 'ffff'
Type_Vector2 = 'ff'
Type_Quaternion = 'q'
Type_Subgroup = 'subgroup' # Used to identify groups

# Format keys
Format_Type = 'type'
Format_Editor = 'editor'
Format_Max = 'max'
Format_Min = 'min'
Format_Step = 'step'
Format_Subtype = 'subtype'
Format_Flags = 'flags'
Format_Enums = 'enums'

# Editor type
Editor_Spinner = 'spinner'
Editor_Slider = 'slider'
Editor_Color = 'color'

# Private types for create controls
Editor_Slider_Int = 'sliderint'
Editor_Slider_Float = 'sliderfloat'

# Speical information 

# _Node class
class _Node:
    """Class that holds information related to a node in the properties tree"""
    def __init__(self):
        # Node references to parent, first child and next sibling
        # With this a full tree can be build and iterated (only forward for siblings)
        self.parent = None
        self.first_child = None
        self.next = None
    
    def get_number_of_children(self):
        """Return how many children this node has, without counting grandchildren"""
        i = 0
        child = self.first_child
        while child is not None:
            i = i + 1
            child = child.next
        return i
    
    def get_last_child(self):
        """Return the last added child, or None if no child has been added"""
        last = self.first_child
        if last is not None:
            while last.next is not None:
                last = last.next
        return last
    
    def append_child(self, child_node):
        """Adds a node to the last position of the children chain"""
        if isinstance(self, _PropertyNode):
            child_node.parent = self
        last = self.get_last_child()
        if last is None:
            self.first_child = child_node
        else:
            last.next = child_node

# _CategoryNode class
class _CategoryNode(_Node):
    """Class that holds information related to a category node"""
    def __init__(self, name):
        _Node.__init__(self)
        # Name of the category (used as an ID too, so names must be unique)
        self.name = name

    def __repr__(self):
        return "Category( name: %s, first_child: %s)" % ( str(self.name), str(self.first_child) )

# _PropertyNode class
class _PropertyNode(_Node):
    """Class that holds information related to a single property field"""
    def __init__(self, property, pid, name, format, data):
        _Node.__init__(self)
        # Property related data
        self.property = property
        self.pid = pid
        self.name = name
        self.format = format
        self.data = data
        # Stores a copy of the current and previous value to generate the
        # 'changed' event.
        # This value is updated by the property grid afterwards.
        self.value = None
        self.old_value = None
    
    def __repr__(self):
        if self.parent is None:
            parent_pid = None
        else:
            parent_pid = self.parent.pid
        return "Property( pid: " + str(self.pid) + \
            ", name: " + str(self.name) + \
            ", value: " + str(self.value) + \
            ", format: " + str(self.format) + \
            ", data: " + str(self.data) + \
            ", parent: " + str(parent_pid) + \
            ", first_child: " + str(self.first_child) + \
            ", next: " + str(self.next) + " )"


# PropertyInfo class
class PropertyInfo:
    """
    Class that holds user usefull information related to a property
    
    The property's value is not updated if the property changes in a future,
    it just mirrors the state of the property at some time.
    """
    def __init__(self, value, pid):
        self.__value = value
        self.__pid = pid
    def get_value(self):
        return self.__value
    def get_pid(self):
        return self.__pid


# PropertyGrid class
class PropertyGrid(pg.PropertyGrid):
    """
    Wrapper for the horribly interfaced property control
    
    A tree is built mirroring the properties added in the control. Each node
    stores some useful info for querying and iterating, so a nicer interface
    can be built over the base property grid class.
    
    The nodes also store a copy of the current and previous value, so
    'changing' and 'changed' events can be generated (more info about these
    events at http://wiki.tragnarion.com/tiki-index.php?page=SWP+Nebula+Conjurer+Custom+Controls).
    
    The properties should be added between begin/end calls. The usual is to
    add all the properties in a single strip, so begin is configured by default
    to delete all the previous properties.
    """
    
    def __init__(self, parent, pos=wx.DefaultPosition, size=wx.DefaultSize,
        style=pg.PG_DEFAULT_STYLE):
        pg.PropertyGrid.__init__(self, parent, -1, pos, size, style)
        
        # Properties are stored in several trees, one for each category.
        # A void category is created by default, used as a phantom category.
        # This category will be replaced whenever a category is added.
        self.categories = [ _CategoryNode(None) ]
        self.properties = {}
        self.properties_by_internal_label = {} # Needed because of the ?"&(*/%! property grid interface
        self.flags_properties = []
        self.last_control = None               # Store the last special control used
        self.selected_pid = None               # Control the current selected property
        
        self.Bind( pg.EVT_PG_CHANGED, self.__on_changed, self )
        self.Bind( pg.EVT_PG_CREATECTRL, self.__on_create_ctrl, self )
        self.Bind( pg.EVT_PG_DESTROYCTRL, self.__on_destroy_ctrl, self )
        self.Bind( pg.EVT_PG_SELECTED, self.__on_selected, self )
        self.Bind(pg.EVT_PG_RIGHT_CLICK, self.__on_right_click, self)

    def tidy_up_on_close(self, event):
        """ This is here to get round a problem with the 
        Property Grid control, where it was reporting in
        error that changes had been lost. Clearing the 
        Property Grid is a simple solution, and it works! """
        self.Clear()

    def __is_vector_property(self, property_to_check):
        format_type = property_to_check.format[Format_Type]
        return format_type in self.__get_vector_property_types()

    def __is_vector_or_quaternion_property(self, property_to_check):
        format_type = property_to_check.format[Format_Type]
        return format_type in self.__get_vector_and_quaternion_property_types()

    def __get_vector_property_types(self):
        """ Return a list of vector property types """
        return [Type_Vector2, Type_Vector3, Type_Vector4]

    def __get_vector_and_quaternion_property_types(self):
        """ Return a list of vector and quaternion property types """
        # make a copy of the vector property list
        type_list = self.__get_vector_property_types()[:]
        type_list.append(Type_Quaternion)
        return type_list

    def __on_right_click (self, event):
        """ Send a right click event (also is treated as a double click) """
        prop = event.GetProperty()        
        if prop is not None and self.properties_by_internal_label.has_key( event.GetPropertyName() ):
            p = self.properties_by_internal_label[ event.GetPropertyName() ]
            self.GetEventHandler().ProcessEvent( events.RightClickEvent(
                self.GetId(),p.pid, self ) )

    def __on_selected (self, event):
        """ Send a selected event with the correct pid and update special controls"""
        prop = event.GetProperty()
        if prop is not None:
            if event.GetPropertyName() == '':
                self.selected_pid = None                
                return
            elif self.properties_by_internal_label.has_key( event.GetPropertyName() ):
                p = self.properties_by_internal_label[ event.GetPropertyName() ]
                self.selected_pid = p.pid
                self.GetEventHandler().ProcessEvent( events.SelectionEvent(
                    self.GetId(),p.pid, self ) )
    
                # Update special control values
                if self.last_control is None:
                    event.Skip()
                    return
                else:
                    if p.format[Format_Editor] == Editor_Slider_Float:
                        self.last_control.set_value(
                            float( prop.GetValueAsString(0) )
                            )
                    elif p.format[Format_Editor] == Editor_Slider_Int:
                        self.last_control.set_value( int(prop.GetValueAsString(0)) )
                    else:
                        raise Exception, "Editor not correct for property %s" % p.name
           
        event.Skip()

    def __on_changed(self, event):
        """
        Generates a 'changed' event for the property that has changed
        
        The property may not be necessarily be the one that originated the event,
        but the one that represents an 'append' operation by the user.
        For instance, a change in the component of a vector will cause a
        'changed' event to be thrown for the vector property, not for the
        component.
        """   
        if self.selected_pid != None:
            p = self.properties[ str(self.selected_pid) ]
            if p.format[Format_Editor] == Editor_Slider_Int or p.format[Format_Editor] == Editor_Slider_Float:
                p.property.SetValueFromString(
                    str( self.last_control.get_value() ), 
                    0
                    )
            p.old_value = p.value
            if p.format[Format_Type] == Type_Subgroup:
                # If is a group warn to all children
                child = p.first_child
                while child is not None:
                    self.set_property_value(
                        child.pid, 
                        self.get_property_value(child.pid)
                        )
                    self.GetEventHandler().ProcessEvent( events.ChangedEvent(
                            self.GetId(),
                            PropertyInfo( child.value, child.pid ),
                            PropertyInfo( child.old_value, child.pid ),
                            self
                            ) )
                    # If have more than one param not send signals for all children
                    # Yes...I know that the data structure must not be used here...
                    # If you find a better way to know if the property is a group of
                    # commands or a group of parameters you are free to change it :P
                    if p.data['multiparam']:
                        child = None                    
                    else:                    
                        child = child.next
            else:       
                # Update property copy values
                self.set_property_value( p.pid, self.get_property_value(p.pid) )
            
            # Update parent copy values
            parent = p.parent
            while parent is not None:
                parent.old_value = parent.value
                parent.value = []
                child = parent.first_child
                while child is not None:
                    parent.value.append( self.get_property_value(child.pid) )
                    child = child.next
                parent = parent.parent
            
            # If parent is a vector, make the vector property throw the event
            # instead of its child property
            if p.parent is not None:
                if self.__is_vector_property(p.parent):
                    p = p.parent
    
            if p.format[Format_Type] != Type_Subgroup:
                self.GetEventHandler().ProcessEvent( events.ChangedEvent(
                    self.GetId(),
                    PropertyInfo( p.value, p.pid ),
                    PropertyInfo( p.old_value, p.pid ),
                    self
                    ) )
            self.Refresh()
        else:
            # maybe is a flag property
            for flag in self.flags_properties:
                p = self.properties[ flag ]
                value = self.get_property_value ( flag )
                self.GetEventHandler().ProcessEvent(
                    events.ChangedEvent(
                        self.GetId(),
                        PropertyInfo( value, p.pid ),
                        PropertyInfo( p.value, p.pid),
                        self
                        )
                    )
        event.Skip()
        
    def __on_change_size(self, event):
        """ When resize main window also resize special control """
        if self.last_control is None:
            event.Skip()
            return
        self.last_control.SetSize( self.GetSizeControl() )
        self.last_control.SetPosition( self.GetPositionControl() )
        event.Skip()

    def __on_changing_last_control (self, event):
        """ This function is used if some actions are required when changing
            special controls 
        """
        pass

    def __on_create_ctrl(self, event):
        """ Create the special control for the selected property """
        prop = self.properties_by_internal_label[ event.GetPropertyName() ]

        # Float slider
        if prop.format[Format_Editor] == Editor_Slider_Float:            
            self.last_control = floatslider.FloatSlider(
                                        self, 
                                        -1, 
                                        value=float(0.0),
                                        min_value=float( prop.format[Format_Min] ),
                                        max_value=float(prop.format[Format_Max] ),
                                        float_ctrl_size=(50, -1)
                                        )
        # Integer slider
        elif prop.format[Format_Editor] == Editor_Slider_Int:            
            self.last_control = intslider.IntSlider(
                                        self,
                                        -1,
                                        value=int(0),
                                        min_value=int( prop.format[Format_Min] ),
                                        max_value=int( prop.format[Format_Max] ),
                                        int_ctrl_size=(50, -1)
                                        )
        self.last_control.SetSize( self.GetSizeControl() )
        self.last_control.SetPosition( self.GetPositionControl() )

        self.GetParent().Bind(wx.EVT_PAINT, self.__on_change_size)
        self.Bind(events.EVT_CHANGING, self.__on_changed, self.last_control)
        self.Bind(events.EVT_CHANGED, self.__on_changed, self.last_control)

    def __on_destroy_ctrl(self, event):
        """ Destroy the custom control and unbind any event callbacks """
        prop = self.properties_by_internal_label[ event.GetPropertyName() ]
        prop.property.SetValueFromString(
            str( self.last_control.get_value() ), 
            0 
            )
        self.Unbind(events.EVT_CHANGING, self.last_control)
        self.last_control.Destroy()
        self.last_control = None

    def __get_next_label(self):
        """Return a new unique label"""
        return "property" + str( len(self.properties) )
    
    def __append_property(self, property, parent_pid, name, format, data, label):
        """Add a property to the control and to the tree"""
        # Add property to control
        if parent_pid is None:
            pid = self.Append( property )
            # The only way to add a spinner is after the property is appended
            if format is not None:
                if format[Format_Editor] == Editor_Spinner:
                    self.MakeIntegerSpinControl( label )
        else:
            pid = self.AppendIn( parent_pid, property )
        
        # Store property reference
        p = _PropertyNode( property, pid, name, format, data )
        self.properties[str(pid)] = p

        if format[Format_Flags] != None:
            self.flags_properties.append( str(pid) )

        if parent_pid is None:
            self.categories[-1].append_child(p)
        else:
            self.properties[str(parent_pid)].append_child(p)
        self.properties_by_internal_label[label] = p
        
        ## This comment is left here intentionally to remember that quering
        ## values while adding properties makes the application crash
        # Get default value for the property (must be done after inserting
        # the property node in the tree for the get method to work)
        # TODO: Update parent values
#        p.value = self.get_property_value(pid)
        
        # Return property ID
        return pid
    
    def __append_vector2(self, parent_pid, name, format, data):
        """Add to the end a group of properties for a 2D vector"""
        pid = self.append_group( parent_pid, name, format, data )        
        self.append_property( pid, 'x', Type_Float, data )        
        self.append_property( pid, 'y', Type_Float, data )
        return pid

    def __append_vector3(self, parent_pid, name, format, data):
        """Add to the end a group of properties for a 3D vector"""
        pid = self.append_group( parent_pid, name, format, data )
        self.append_property( pid, 'x', Type_Float, data )
        self.append_property( pid, 'y', Type_Float, data )
        self.append_property( pid, 'z', Type_Float, data )
        return pid

    def __append_vector4(self, parent_pid, name, format, data):
        """Add to the end a group of properties for a 4D vector"""
        pid = self.append_group( parent_pid, name, format, data )
        self.append_property( pid, 'x', Type_Float, data )
        self.append_property( pid, 'y', Type_Float, data )
        self.append_property( pid, 'z', Type_Float, data )
        self.append_property( pid, 'w', Type_Float, data )
        return pid

    def __append_int_property ( self, parent_pid, name, label, format ):
        """ Append an integer property to the grid, taken into 
        account all special editors """
        editor = format[Format_Editor]
        if editor == Editor_Spinner:
            # Editor spinner
            prop = pg.IntProperty( name, label )
        elif editor == Editor_Slider:
            # Set to slider for integers
            format[Format_Editor] = Editor_Slider_Int
            prop = pg.IntProperty( name, label )
            self.MakeAnyControlById( prop.GetId() )
        elif format[Format_Enums] != None:
            # Enum editor
            enums = format[Format_Enums]
            enums_list = self.__get_enum_list(enums)
            prop = pg.EnumProperty(
                        name, 
                        label, 
                        enums_list[0], 
                        enums_list[1], 
                        1
                        )
        elif format[Format_Flags] != None:
            # Flags editor
            flags = format[Format_Flags] 
            #pid = self.append_group(parent_pid, name, format)
            labels_list = []
            values_list = []
            for flag in flags.split(','):
                label = self.__get_next_label()
                flag_string = flag.split(':')[0]
                flag_values = int(flag.split(':')[1])
                labels_list.append(flag_string)
                values_list.append(flag_values)
            pid = pg.FlagsProperty ( name, label, labels_list, values_list )
            return pid
        else:
            prop = pg.IntProperty( name, label )
        return prop
    
    def __append_float_property (self, name, label, format):
        """ Append a integer property to the grid, taking into
        account all special editors """
        editor = format[Format_Editor]
        if editor == Editor_Slider:
            # Set to slider for integers
            format[Format_Editor] = Editor_Slider_Float
            prop = pg.FloatProperty( name, label )
            self.MakeAnyControlById( prop.GetId() );
        else:
            prop = pg.FloatProperty( name, label )
        return prop

    def change_property_colour (self, pid, color):
        self.SetPropertyColour(pid, color)

    def append_category(self, name):
        """Add a new category """
        label = "category_" + name
        prop = pg.PropertyCategory(name, label)
        pid = self.Append( prop )
        # If only
        if self.categories[0].name == None:
            self.categories[0] = _CategoryNode(name)
        return pid

    def append_group(self, parent_pid, name, format=None, data=None):
        """Add to the end a parent property"""
        # A format is needed because could used for change 
        # childrens (see __on_change method)
        if format is None:
            format = { Format_Type: Type_Subgroup,
                       Format_Editor: None,
                       Format_Max: None,
                       Format_Min: None,
                       Format_Step: None,
                       Format_Subtype: None,
                       Format_Flags: None,
                       Format_Enums: None
                      }
        label = self.__get_next_label()
        prop = pg.ParentProperty( name, label )
        pid = self.__append_property(
                    prop, 
                    parent_pid, 
                    name, 
                    format, 
                    data, 
                    label
                    )
        return pid

    def append_property(self, parent_pid, name, format, data=None):
        """Add to the end a child property"""
        # Create a basic dictionary if format given is just the type
        if isinstance(format, str):
            format = { Format_Type: format,
                       Format_Editor: None,
                       Format_Max: None,
                       Format_Min: None,
                       Format_Step: None,
                       Format_Subtype: None,
                       Format_Flags: None,
                       Format_Enums: None
                      }

        # Create a property for the correct type
        label = self.__get_next_label()
        type = format[Format_Type]
        if type == Type_Bool:
            prop = pg.BoolProperty( name, label )
        elif type == Type_Float:
            prop = self.__append_float_property( name, label, format )
        elif type == Type_Int:
            prop = self.__append_int_property( parent_pid, name, label, format )
            if not isinstance(prop, pg.PGProperty):
                return prop                
        elif type == Type_Object:
            if format[Format_Subtype] != None:
                prop = self.__get_subtype_property(
                            format[Format_Subtype],
                            name, 
                            label
                            )
            else:
                prop = pg.StringProperty( name, label, '' )
        elif type == Type_String:
            if format[Format_Subtype] != None:
                prop = self.__get_subtype_property(
                            format[Format_Subtype], 
                            name, 
                            label
                            )
            else:
                prop = pg.StringProperty( name, label, '' )
        elif type == Type_Vector2:
            pid = self.__append_vector2( parent_pid, name, format, data )
            return pid
        elif type == Type_Vector3:
            if format[Format_Editor] == Editor_Color:
                # Color editor
                prop = pg.ColourProperty( name, label, wx.BLACK)
            else:
                pid = self.__append_vector3( parent_pid, name, format, data )
                return pid
        elif type == Type_Vector4 or type == Type_Quaternion:
            pid = self.__append_vector4( parent_pid, name, format, data )
            return pid
        else:
            raise TypeError, "Type '%s' not supported" % type
        
        # Add the property to the control for simple types
        pid = self.__append_property(
                    prop, 
                    parent_pid, 
                    name, 
                    format, 
                    data, 
                    label
                    )
        return pid
    
    def get_first_category(self):
        """
        Return the first category added (its name)
        
        Return None if there isn't any category
        """
        return self.categories[0].name
    
    def get_next_category(self, category_name):
        """
        Return the next category (its name) of the given category
        
        Return None if the category is the last one
        """
        for i in range( len(self.categories) - 1 ):
            if self.categories[i].name == category_name:
                return self.categories[i+1].name
        return None
    
    def get_first_child_of_category(self, category_name):
        """
        Return the first child property (its ID) of the given category
        
        Return None if the category hasn't any child
        """
        for i in range( len(self.categories) ):
            c = self.categories[i]
            if c.name == category_name:
                if c.first_child is None:
                    return None
                else:
                    return c.first_child.pid
        return None
    
    def get_next_sibling(self, pid):
        """
        Return the next property (its ID) of the given property
        
        Return None if the property is the last one
        """
        p = self.properties[str(pid)].next
        if p is None:
            return None
        else:
            return p.pid
    
    def get_first_child(self, pid):
        """
        Return the first child property (its ID) of the given property
        
        Return None if the property hasn't any child
        """
        p = self.properties[str(pid)].first_child
        if p is None:
            return None
        else:
            return p.pid
    
    def get_parent(self, pid):
        """
        Return the parent property (its ID) of the given property
        
        Return None if the property hasn't any parent (categories don't count
        as parent of properties)
        """
        p = self.properties[str(pid)].parent
        if p is None:
            return None
        else:
            return p.pid
    
    def get_top_parent(self, pid):
        """
        Return the top parent property (its ID) of the given property
        
        Return the given property ID if it's already the top parent
        """
        p = self.properties[str(pid)]
        while p.parent is not None:
            p = p.parent
        return p.pid
    
    def get_property_name(self, pid):
        """Return the name of a property"""
        p = self.properties[str(pid)]
        return p.name
    
    def __get_object_value(self, pid):
        """Return the object of an object property"""        
        p = self.properties[str(pid)]
        value = p.property.GetValueAsString(0)
        if value == "None":
            return None
        # Regular expresion for match the object type
        ENTITY_OBJECT = re.compile(r'\w[\w\d]*\(id=(.*)\)')
        NROOT = re.compile(r'(/.*)')
        NOBJECT = re.compile(r'nObject\((.*)\)')
        # Get the real object        
        g = ENTITY_OBJECT.search(value)
        if (g):
            return servers.get_entity_object_server().getentityobject( int(g.group(1)) )
        else:
            g = NROOT.search(value)
            if (g):
                return pynebula.lookup(value)
            else:
                g = NOBJECT.search(value)
                if (g):
                    return None
                else:
                    return None
        
    def __get_vector_value(self, pid):
        """Return the value of a vector property group, as a sequence"""
        v = []
        p = self.properties[str(pid)].first_child
        while p is not None:
            v.append( float( p.property.GetValueAsString(0) ) )
            p = p.next
        return v

    def __get_color_value (self, pid):
        v = []
        p = self.properties[str(pid)]
        color = p.property.GetValueAsString(0)
        color = color[1:-1]
        for component in color.split(','):
            v.append(float(float(int(component)) / 255.0))
        return v

    def __get_enum_list (self, flags):

        list_values = []
        list_strings = []
        for flag in flags.split(','):            
            flag_value = int(flag.split(':')[1])
            flag_string = str(flag.split(':')[0])
            list_values.append(flag_value)
            list_strings.append(flag_string)

        return [list_strings, list_values]

    def __get_group_value (self, pid):
        """Return a list with all values of al properties in a group"""
        v = []
        p = self.properties[str(pid)].first_child
        while p is not None:
            v.append( self.get_property_value(p.pid) )
            p = p.next
        return v
    
    def get_property_value(self, pid):
        """
        Return the value of a property
        
        Nested sequences of children values are returned for parent properties
        """
        p = self.properties[str(pid)]
        value = p.property.GetValueAsString(0)
        return_value = None
        
        # Handle group properties
        #if p.format is None:
            # Is a parent node
            # TODO: Build a tree from children values instead of returning
            #       the string displayed in the GUI
            #return value
        
        # Handle basic types
        type = p.format[Format_Type]
        if type == Type_Bool:
            return_value = {'True':True, 'False':False}[value]
        elif type == Type_Float:
            return_value = float(value)
        elif type == Type_Int:
            if p.format[Format_Flags] != None or p.format[Format_Enums] != None:
                return_value = p.property.GetValueAsLong()                
            else:
                return_value = int(value)
        elif type == Type_Object:
            return_value = self.__get_object_value( pid )
        elif type == Type_String:
            return value
        elif type in self.__get_vector_and_quaternion_property_types():
            if p.format[Format_Editor] == Editor_Color:
                #p.property.GetValueAsString(0)
                return_value = self.__get_color_value( pid )
            else:
                return_value = self.__get_vector_value( pid )        
        elif type == Type_Subgroup:
            return_value = self.__get_group_value( pid )
        else:
            raise TypeError, "Unsupported type for property '%s'" % p.name
        return return_value
    
    def get_property_type(self, pid):
        """Return the type of a property"""
        p = self.properties[str(pid)]
        return p.format[Format_Type]
    
    def get_property_data(self, pid):
        """Return the user data attached to a property"""
        p = self.properties[str(pid)]
        return p.data
    
    def __get_object_type (self, object):
        """ Get the nebula2 object type """
        if object.isa("nentityclass"):
            return "entityclass"
        elif object.isa("nroot"):
            return "nroot"
        elif object.isa("nentityobject"):
            return "entity"
        elif object.isa("nobject"):
            return "nobject"

    def __set_object_value(self, pid, object):
        """Set a format for an object to a property"""
        value = None       
        if str(object).startswith('-'):
            object = None
        if object != None and not isinstance(object, int):
            object_type = self.__get_object_type(object)
            if object_type == 'nroot'or object_type == 'entityclass':
                value = object.getfullname()
            elif object_type == 'entity':
                id = object.getid()            
                value = object.getclass() + "(id=" + str(id) + ")"
            else:
                value = default.getclass()
                value = "nObject(" + value + ")"
    
        p = self.properties[str(pid)]
        p.property.SetValueFromString( str(value), 0 )
    
    def __set_vector_value(self, pid, value):
        """Set the values of the properties in a vector property group"""
        if value == None:
            return 
        p = self.properties[str(pid)]
        if p.get_number_of_children() != len(value):
            return  # Better not put anything that make the control crash
            #raise Exception, "Vector property '" + str(p.name) + "' has " \
            #    + str(p.get_number_of_children()) + " dimensions and the given " \
            #    "vector '" + str(value) + "' has " + str(len(value))
        p = p.first_child
        for i in range(len(value)):
            self.set_property_value( p.pid, value[i] )
            p = p.next    

    def __set_color_value ( self, pid, value):
        """Set the values of a color property"""        
        p = self.properties[str(pid)]
        color = '('
        for v in value:
            color = color + str(int(v * 255)) + ','
        color = color[:-1] + ')'
        p.property.SetValueFromString(color, 0)

    def __set_group_value(self, pid, value):
        pass

    def __make_list_from_flags(self, value, flags):
        string = ''
        for flag in flags.split(','):
            flag_value = int(flag.split(':')[1])
            if (bool(value & flag_value) == True):
                string = string + flag.split(':')[0] + ','
        string = string[:-1]
        return string

    def __get_string_from_enum (self, value, enums):        
        for enum in enums.split(','):
            if value == int(enum.split(':')[1]):
                return enum.split(':')[0]
        return enums.split(',')[0].split(':')[0]

    def __get_subtype_property(self, subtype, name, label):
        if "asset" == subtype:
            directory = servers.get_file_server().manglepath("wc:export/assets") + "\\"
            prop = pg.FileProperty(name, label, directory)
        elif "texture" == subtype:
            directory = servers.get_file_server().manglepath("wc:export/textures") + "\\"
            prop = pg.FileProperty(name, label, directory)
        elif "image" == subtype:
            directory = servers.get_file_server().manglepath("wc:export/textures") + "\\"
            prop = pg.AdvImageFileProperty(name, label)
        elif "shader" == subtype:
            directory = servers.get_file_server().manglepath("wc:export/shader") + "\\"
            prop = pg.FileProperty(name, label, directory)
        elif "file" == subtype:
            directory = servers.get_file_server().manglepath("wc:")
            prop = pg.FileProperty(name, label, directory)
        elif "material" == subtype:
            prop = pg.StringCallBackProperty(name, label, '')
            dlg = nodelibdlg.NodeLibDialog( self, nodelibdlg.SELECT,
                'material', 'Material', "/usr/materials")
            prop.SetCallBackDialog(dlg)
        elif "directory" == subtype:
            directory = servers.get_file_server().manglepath("home:")
            prop = pg.DirProperty(name, label, directory)
        elif "entityobjectclass" == subtype:
            prop = pg.StringCallBackProperty(name, label, '' )
            dlg = objbrowserwindow.create_dialog(
                        self, 
                        True, 
                        "/sys/nobject/nentityobject"
                        )
            prop.SetCallBackDialog(dlg)
            prop = pg.StringProperty(name, label, '')
        elif "nroot" == subtype:
            prop = pg.StringCallBackProperty(name, label, '' )
            dlg = objbrowserwindow.create_dialog(
                        self, 
                        True, 
                        "/"
                        )
            prop.SetCallBackDialog(dlg)
        elif "entityclass" == subtype:
            prop = pg.StringCallBackProperty(name, label, '' )
            dlg = grimoire.create_dialog(self, True)
            prop.SetCallBackDialog(dlg)
        elif "entityobject" == subtype:
            prop = pg.StringCallBackProperty(name, label, '' )
            dlg = entitybrowser.create_dialog(self)
            prop.SetCallBackDialog(dlg)
        elif "musicsample" == subtype:
            prop = pg.StringCallBackProperty(name, label, '' )
            dlg = musictabledlg.dialog_for_music_sample_selection(self)
            prop.SetCallBackDialog(dlg)
        elif "musicmood" == subtype:
            prop = pg.StringCallBackProperty(name, label, '' )
            dlg = musictabledlg.dialog_for_music_mood_selection(self)
            prop.SetCallBackDialog(dlg)
        elif "musicstyle" == subtype:
            prop = pg.StringCallBackProperty(name, label, '' )
            dlg = musictabledlg.dialog_for_music_style_selection(self)
            prop.SetCallBackDialog(dlg)
        else:
            print "Bad subtype"
            prop = pg.StringProperty(name, label, '')
    
        return prop

    def set_property_value(self, pid, value, throw_event=False):
        """Set the value of a property"""
        p = self.properties[str(pid)]
        p.value = value   
        if value is not None:
            type = p.format[Format_Type]
            # Floats
            if  type == Type_Float:
                p.property.SetValueFromString( str(round(value, 5)), 0 )        
            # Strings
            elif type == Type_String:                
                p.property.SetValueFromString( str(value), 0 )           
            #Bools
            elif type == Type_Bool:
                if value == 1:
                    value = "True"
                else:
                    value = "False"
                p.property.SetValueFromString( str(value), 0 )           
            # Integers
            elif type == Type_Int:
                # Flags
                if p.format[Format_Flags] is not None:
                    string_value = self.__make_list_from_flags(
                                            value, 
                                            p.format[Format_Flags]
                                            )
                    p.property.SetValueFromString( string_value, 0 )
                # Enums
                elif p.format[Format_Enums] is not None:
                    string_value = self.__get_string_from_enum(
                                            value, 
                                            p.format[Format_Enums]
                                            )
                    p.property.SetValueFromString( str(string_value), 0 ) 
                # Normal integer
                else:
                    p.property.SetValueFromString( str(value), 0 )
            # Objects
            elif type == Type_Object:
                self.__set_object_value( pid, value )
            # Vectors
            elif type in self.__get_vector_and_quaternion_property_types():
                if p.format[Format_Editor] == Editor_Color:
                    self.__set_color_value( pid, value)
                else:
                    self.__set_vector_value( pid, value )
            # Subgroups and others
            elif type == Type_Subgroup:
                pass

            else:
                raise TypeError, "Unsupported type %s for property '%s'" % ( str(type), p.name )
        else:
            pass

    def clear(self):
        """Delete all the categories and properties"""
        self.categories = [ _CategoryNode(None) ]
        self.properties = {}
        self.selected_pid = None
        pg.PropertyGrid.Clear(self)
    
    def Clear(self):
        """Same as clear, just overwritten from the base class to avoid errors"""
        self.clear()
    
    def begin(self, clear=True):
        """Prepare to start adding properties"""
        if clear:
            self.clear()
        self.Freeze()
    
    def end(self):
        """Finish adding properties"""
        self.__fill_values_of_all_nodes()
        self.Thaw()
        self.Refresh()
    
    def __fill_values_of_all_nodes(self):
        """Store a copy of all the property values"""
        for each in self.categories:
            self.__fill_values_of_branch_nodes( each.first_child )
    
    def __fill_values_of_branch_nodes(self, property):
        """Store a copy of the properties values for a branch"""
        while property is not None:
            property.value = self.get_property_value( property.pid )
            self.__fill_values_of_branch_nodes( property.first_child )
            property = property.next
