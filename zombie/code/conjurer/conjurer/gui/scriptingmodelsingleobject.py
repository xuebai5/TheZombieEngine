##\author Javier Ferrer
##\file scriptinterfacemodel.py
##\brief Overload scriping model interface for a single object.
## Also implements inherentance issues when executing.

import app

# Interface
import scriptingmodel

# Modules for share variables
import readshared
import writeshared

# nebula
import servers

import conjurerframework as cjr


class ScriptingModelSingleObject(scriptingmodel.ScriptingModel):
    def __init__ (self, object):
        scriptingmodel.ScriptingModel.__init__(self, object)
        self.property_list = []
        self.xml = readshared.get_xml_dom()           # xml DOM interface
        self.object_info = self.get_object_info()   # Info extracted from xml
        if self.object_info == None:
            return None
        self.comp_list = self.build_component_list()
        self.command_list = self.build_command_list()
        self.values = {}           # Actual values
        self.init_values = {}     # Values used for retore to init state
        self.refresh( True )       # Initialize values (and init values)
        self.object = object

    def is_for_single_object(self):
        return True

    # Retuen the model object
    def get_object (self):
        return self.object

    # Return the name of the object
    def get_object_name (self):
        pass

    def get_class_info_from_xml(self, type_string):
        class_name = self.get_object_base_class()
        result = self.xml.get_class_info(type_string, class_name)
        return result

    def get_object_base_class(self):
        return ''

    def get_object_class(self):
        return self.object.getclass()

    # Return the values structure
    def get_values(self):
        return self.values

    # Returns a list of property structures
    def get_properties_list (self):
        return self.property_list 

    # Returns a list of command structures
    def get_commands_list (self):
        return self.command_list

    # Returns a string with some information
    def get_info_text (self):
        text = 'Modifying a single object'
        return text

    # Get the value of a specific property
    def get_property (self, name):
        if self.values.has_key(name):
            return self.values[name]
        else:
            return None

    # Get the command 
    def get_command (self, name):
        command = None
        for cmd in self.command_list:
            if cmd['name'].lower() == name.lower():
                command = cmd
        return command

    # Set a value for a specific property, and execute if flag is not unset
    def set_property (self, name, value, execute_now=True, undo=False):
        # Values are always treated as lists
        if not isinstance(value, list):
            value = [value]
        if execute_now:
            found = False
            # Look for property to execute  
            for property in self.property_list:
                if property["name"] == name:
                    found = True
                    self.__execute_inheritance(property, value, undo)
                    break;
            if not found:
                print "Property %s not found" % (name)
                return
        self.values[name]["value"] = value
        self.values[name]["dirty"] = True
        # The inheritance mode will be used to revert changes
        if self.mode == self.inheritance_mode.normal:
            self.values[name]["inheritance"] = self.inheritance_mode.normal
        elif self.mode == self.inheritance_mode.cascade:
            self.values[name]["inheritance"] = self.inheritance_mode.cascade
        elif self.mode == self.inheritance_mode.force:
            self.values[name]["inheritance"] = self.inheritance_mode.force

    def __get_attr_format (self, attrs):
        """ Get the attributes for a property """
        format = { 
                   'editor': None,
                   'min': None,
                   'max': None,
                   'step': None,
                   'subtype': None,
                   'flags': None,
                   'enums': None
                 }

        for attr in attrs:            
            attr_type = attr["type"]
            if "editor" == attr_type:
                format['editor'] = attr["value"]    
            if "min" == attr_type:
                format['min'] = attr["value"]    
            if "max" == attr_type:
                format['max'] = attr["value"]                    
            if "default" == attr_type:
                format['default'] = attr["value"]    
            if "step" == attr_type:
                format['step'] = attr["value"]
            if "subtype" == attr_type:
                format['subtype'] = attr["value"]
            if "flags" == attr_type:
                format['flags'] = attr['value']
            if "enums" == attr_type:
                format['enums'] = attr['value']

        return format

    def find_property (self, prop_name):
        for property in self.property_list:
            if property['name'] == prop_name:
                return property
        return None

    # Execute all the properties
    def execute_all (self):
        for property in self.property_list:
            name = property["name"]
            value = self.values[name]["value"]
            if value != []:
                self.__execute_inheritance(property, value[0])

    # Set all properties to init state (the state when the object was created)
    def revert (self):
        for key, value in self.values.iteritems():
            if value["dirty"] == True:
                init_value = self.init_values[key]["value"]
                value["value"] = init_value
                property = self.find_property(key)
                if property is not None:
                    # Use inheritance mode for revert values
                    mode_back = self.mode
                    self.mode = value["inheritance"]
                    self.__execute_inheritance( property, init_value[0] )
                    self.mode = mode_back
                    value["dirty"] = False
                else:
                    cjr.show_error_message(
                        "Fatal error restoring values\n"\
                            "You can continue but some changes"\
                            " in property %s will not be reverted.\n"\
                            "Please report this error." % str(key)
                        )
        self.refresh()

    def get_dirty_property_list (self):
        """ Return dirty properties (properties that have been modified) """
        dirty_list = []
        for key, value in self.values.iteritems():
            if value["dirty"] == True:
                prop = {}
                prop["name"] = key
                prop["value"] = value["value"]    
                prop["old_value"] = self.init_values[key]["value"]
                dirty_list.append(prop)
        return dirty_list

    def refresh(self, overwrite_init=False):
        # Get the values for all properties, executing getters and
        # storing them in a dictionary
        if self.object_info != None:
            self.my_refresh(overwrite_init)

    def my_refresh(self, overwrite_init):
        # Component properties
        self.property_list = [] # List of all properties in the object
        for component in self.get_component_list():
            prop_list = component["properties"]
            self.update_values(prop_list, self.values, overwrite_init)
            self.property_list.extend(prop_list)

    def is_inherited (self, property_name, object=None):
        # Check if a property is inherited. Default is to return false
        return False

    def get_xml_class_tag(self):
        pass

    def __get_component_structure (self, name):
        comp_info = self.xml.get_class_info(
                            self.get_xml_class_tag(),
                            name.lower()
                            )

        comp = None
        if comp_info:
            prop_list = self.get_property_list(comp_info)
            comp = {
                'group' : comp_info["group"],
                'properties' : prop_list,
                'doc' : comp_info["doc"],
                'cppclass' : comp_info["cppclass"],
                'superclass' : comp_info["superclass"],
                'commands' : comp_info["commands"]
            }

        return comp

    def get_component_list_from_xml (self):
        comp_list = []
        # Return all components
        for component in self.object_info["components"]:
            comp_list.extend(self.get_recursive_components(component))

        # If is a nroot add a component with the class as name
        if self.get_object_type() == "nroot":
            nroot_name = self.get_object_base_class()
            comp_data = self.__get_component_structure( nroot_name )
            if comp_data is not None:
                comp_list.append(comp_data)
            else:
                pass

        return comp_list

    def build_component_list (self):
        pass

    def get_component_list (self):
        return self.comp_list

    def get_recursive_components (self, comp_name):
        """ Get all component including parents """
        comp_list = []
        # Current component
        comp_data = self.__get_component_structure( comp_name )        

        while comp_data is not None:
            comp_list.append(comp_data)
            # Parent components      
            p_comp = comp_data["superclass"]
            comp_data = self.__get_component_structure(p_comp)

        return comp_list

    #--------------------------------------------------------------------------
    #                             PRIVATE MEMBERS
    #--------------------------------------------------------------------------

    # Execute with inheritance
    def __execute_inheritance (self, property, value, undo=False):
        # Execute in the specified inheritance mode
        if self.mode == self.inheritance_mode.normal:
            # execute only for myself
            self.__execute_property(property, value, self.object, undo)
        elif self.mode == self.inheritance_mode.force:
            # execute for all children
            object = self.object
            while object != None:
                self.__execute_property(property, value, object, undo)
                object = object.gethead()
        elif self.mode == self.inheritance_mode.cascade:
            object = self.object.gethead()
            # Excute for all inherited children
            while object != None:                
                if self.is_inherited(property['name'], object):
                    self.__execute_property(property, value, object, undo)
                    #break;
                object = object.gethead()           
            # execute for myself
            self.__execute_property(property, value, self.object, undo)

    # Execute a property giving her name and a value/s
    def __execute_property (self, property, value, object, undo=False):
        writeshared.set_object(object)
        # Execute each setter with his value        
        if value != []:
            command = self.__get_command(property, value, object)
            if undo == False:                
                # Run the command
                servers.get_python_server().run(str(command))
            else:
                init_value = self.init_values[property["name"]]["value"]
                undo_cmd = self.__get_command(property, init_value[0], object)
                servers.get_command_server().newcommand(str(command), 
                                                    str(undo_cmd))

            self.values[property['name']]['value'] = [readshared.get_result()]
            object.setobjectdirty( True )

        return readshared.get_result()

    def get_string_command (self, prop, value, object):
        property = self.find_property( prop )
        if property is not None:
            return self.__get_command( property, value, object )
        else:
            cjr.show_error_message(
                "Sorry, but undo operation will be disabled for this operation"
                )
            return None
        
    def __get_command (self, property, value, object):
        command = ''        
        # Execute each setter with his value        
        for setter in map(None, property["setters"]):
            command_name = setter["name"]            
            command = command + "import writeshared;import readshared;"
            command = command + "object = readshared.get_object();"
            command = command + "result = object." + command_name.lower() + "("
            has_params = False
            p_count = 0
            for param in property["params"]:
                has_params = True
                if param["type"] == 's':
                    # convert '\' to '\\' for correct formating, 
                    # because python server formats it automatically
                    slashes = value[p_count]
                    slashes = slashes.replace('\\','\\\\')
                    command = command + "'" + str(slashes) + "',"
                elif param["type"] == 'o':
                    obj_type = self.get_object_type(value[p_count])
                    if obj_type == "entity":
                        entity_id = value[p_count].getid()
                        get_entity_string = "servers.get_entity_object_server()."\
                                                        "getentityobject(%s)," % str(entity_id)
                        command = command + get_entity_string
                    elif obj_type == "nroot" or "entityclass":
                        full_name = value[p_count].getfullname()
                        command = command + "lookup('" + str(full_name) +"'),"
                    else:
                        # The share module for getting the nobject
                        writeshared.set_object(value[p_count])
                        command += "readshared.get_object(),"
                elif self.is_vector_or_quaternion_type( param["type"] ):
                    if isinstance(value[p_count], list):
                        for each_value in value[p_count]:
                            command = command + str(each_value) + ","
                    else:
                        for each_value in value:
                            command = command + str(each_value) + ","
                else:
                    command = command + str(value[p_count]) + ","
                p_count += 1
            if has_params:
                command = command[:-1]
            command += ");"
            command = command + "writeshared.set_result(result);del object;"

        return command

    def __get_vector_and_quat_types(self):
        return ("ff", "fff", "ffff", "q")

    def is_vector_or_quaternion_type(self, type_string):
        return type_string in self.__get_vector_and_quat_types()

    def get_object_info(self):
        # Return a structure with information about the object class
        pass

    # Update the values for a list of properties
    def update_values (self, property_list, dest_dict, overwrite_init=False ):
        """ overwrite_init is also used to get a dictionary for initial values,
            so that the revert operation can be executed. We have to do this ugly hack because
            the python copy method doesn't work with N2 python server when copying objects.
            So we need a fast way to store this information. This method is very expensive
            because it executes getter commands, but it is better to do this than call the function 
            twice (once with normal values and then with initial values)
        """
        for property in property_list:
            visible = True
            for attr in property["attrs"]:
                if attr["type"] == "private":
                    if attr["value"].lower() == 'true':
                        visible = False

            if not visible:
                continue

            value = {}      # Value dict, format: {value, dirty}
            value_list = [] # List for multiple values
            init_value_list = []
            init_value = {}
            # Add all the values (object with multiple getters) 
            # to a list of values
            for getter in property['getters']:
                getter_name = getter['name']
                getter_value = self.execute_getter( getter_name, self.object )
                if str(getter_value).startswith('('):
                    python_value = self.__get_python_format(
                                            property['params'], 
                                            getter_value 
                                            )
                else:
                    python_value = [getter_value]

                value_list.append(python_value)
                if overwrite_init:
                    init_value_list.append(python_value)

            value['value'] = value_list           # Property value
            value['dirty'] = False                # Must save or not
            value['inheritance'] = self.mode      # Used for revert
    
            if overwrite_init:
                init_value['value'] = init_value_list
                init_value['dirty'] = False   
                init_value['inheritance'] = self.mode
    
            dest_dict[property['name']] = value
    
            if overwrite_init:
                self.init_values[property['name']] = init_value

    def __get_python_format (self, params, value):
        """ This function converts the tuple returned by pynebula in a
            easier format for work in python, it basically transform
            the nebula vector types in python vector types
        """
        total_value = []
        value_count = 0
        for param in params:
            if param['type'] == 'ff':
                v_list = list(value[value_count:value_count+2])
                value_count += 2
            elif param['type'] == 'fff':
                v_list = list(value[value_count:value_count+3])
                value_count += 3
            elif param['type'] == 'ffff' or param['type'] == 'q':
                v_list = list(value[value_count:value_count+4])
                value_count += 4
            else:
                v_list = list(value[value_count:value_count+1])[0]
                value_count += 1

            total_value.append(v_list)

        return total_value

    # Execute a getter command
    def execute_getter (self, getter_name, target_object):
        runner = GetterMethodRunner(
            target_object, 
            getter_name
            )
        return runner.result

    # Return a list with all the properties from xml
    def get_property_list (self, class_info):
        prop_list = []
        for prop in class_info["properties"]:
            property = {
                'name': self.xml.get_prop_name(prop),
                'attrs': self.xml.get_prop_attrs(prop),
                'getters': self.xml.get_prop_getters(prop),
                'setters': self.xml.get_prop_setters(prop),
                'doc': self.xml.get_prop_doc(prop),
            }
            # TODO: When xml.get_prop_params returns the same format as
            # xml.get_cmd_output recode this as in build_command_list
            params = []
            for each in self.xml.get_prop_params(prop):
                param = {
                        'name': each['name'],
                        'doc': each['doc'],
                        'type': each['type'],
                        'subtype': each['subtype'],
                        }

                attrs = each['attrs']
                format = self.__get_attr_format(attrs)
                for key, value in map(None, format.keys(), format.values()):
                    param[key] = value
                params.append(param)
            property['params'] = params

            prop_list.append(property)

        return prop_list

    def build_command_list(self):
        # Return a list with all the commands from xml
        class_name = self.get_object_base_class()
        class_info = self.xml.get_class_info(
                            self.get_xml_class_tag(),
                            class_name.lower()
                            )

        cmd_list = []
        if class_info != None: # Entities return class_info = None
            for cmd in class_info["commands"]:
                command = {
                    'name': self.xml.get_cmd_name(cmd),
                    'doc': self.xml.get_cmd_doc(cmd)
                }
                # Get output params
                params = []
                for parameter in self.xml.get_cmd_output(cmd):
                    param = {
                        'name': self.xml.get_param_name(parameter),
                        'doc': self.xml.get_param_doc(parameter),
                        'type': self.xml.get_param_type(parameter),
                        'subtype': self.xml.get_param_subtype(parameter),
                    }
                    attrs = self.xml.get_param_attrs(parameter)
                    format = self.__get_attr_format(attrs)
                    for key, value in map(None, format.keys(), format.values()):
                        param[key] = value
                    params.append(param)
                command['output'] = params
    
                # Get input params
                params = []
                for parameter in self.xml.get_cmd_input(cmd):
                    param = {
                        'name': self.xml.get_param_name(parameter),
                        'doc': self.xml.get_param_doc(parameter),
                        'type': self.xml.get_param_type(parameter),
                        'subtype': self.xml.get_param_subtype(parameter),
                    }
                    attrs = self.xml.get_param_attrs(parameter)
                    format = self.__get_attr_format(attrs)
                    for key, value in map(None, format.keys(), format.values()):
                        param[key] = value
                    params.append(param)
                command['input'] = params
    
                cmd_list.append(command)

        return cmd_list
                
class ScriptingModelEntity(ScriptingModelSingleObject):

    def get_object_name (self):
        object_id = self.object.getid()
        object_name = app.get_level().getentityname( object_id )
        if object_name == ":null:":
            return str(object_id)
        else:
            return object_name

    def get_object_base_class(self):
        entity_class = self.object.getentityclass()
        class_object = entity_class
        while class_object != None:
            parent = class_object.getparent()
            if parent.getclass() == "nentityclass":
                name = class_object.getclass()
                class_object = None
            else:
                class_object = class_object.getparent()
        return name.replace("class", "")

    def get_object_info(self):
        # Return a structure with information about the object class
        return self.get_class_info_from_xml("entity")

    def get_xml_class_tag(self):
        return "component"

    def build_component_list (self):
        # Return all components
        comp_list = []
        for comp_index in xrange(self.object.getnumcomponents()):
            comp_name = self.object.getcomponentidbyindex(comp_index)
            comp_list.extend(
                self.get_recursive_components(comp_name)
                )
        return comp_list


class ScriptingModelEntityClass(ScriptingModelSingleObject):

    def get_object_name (self):
        return self.object.getname()

    def get_object_base_class(self):
        parent = self.object.getparent()
        name = self.object.getclass()
        while parent != None and parent.getclass() != "nentityclass":
            current_object = parent
            parent = parent.getparent()
            name = current_object.getclass()
        return name

    def get_object_info (self):
        # Return a structure with information about the object class
        return self.get_class_info_from_xml("entityclass")

    def get_xml_class_tag(self):
        return "componentclass"

    def is_inherited (self, property_name, object=None):
        # Check if a property is inherited. 
        # If no object specfied search for the base class
        inherited = False
        property = self.find_property(property_name)
        if property is not None:
            if len(property['getters']) > 0:
                getter_name = property['getters'][0]["name"]
                if object == None or object == self.object:
                    temp_object = self.object
                    target_class = self.get_object_base_class()
                    while temp_object.getclass() != target_class:
                        temp_object = temp_object.getparent()
                        value_parent = self.execute_getter(
                                                getter_name, 
                                                temp_object
                                                )
                        value_self = self.init_values[property_name]["value"]
                        value_parent = [value_parent]
                        value_self = value_self[0]
                        if value_parent == value_self:
                            inherited = True
                else:
                    # Search for the same value in parents
                    inherited = False
                    value_parent = self.execute_getter(getter_name, object)
                    value_self = self.init_values[property_name]["value"]
                    value_parent = [value_parent]
                    value_self = value_self[0]
                    if value_parent == value_self:
                        inherited = True
    
        return inherited

    def build_component_list (self):
        # Return all components
        comp_list = []
        for comp_index in xrange(self.object.getnumcomponents()):
            comp_name = self.object.getcomponentidbyindex(comp_index)
            comp_list.extend(self.get_recursive_components(comp_name))
        return comp_list


class ScriptingModelNRoot(ScriptingModelSingleObject):

    def get_object_name (self):
        return self.object.getname()

    def get_object_base_class(self):
        return self.object.getclass()

    def get_object_info (self):
        # Return a structure with information about the object class    
        return self.get_class_info_from_xml("scriptclass")
        
    def get_xml_class_tag(self):
        return "scriptclass"

    def build_component_list (self):
        # Return all components  - add a component with the class as name
        comp_list = []
        nroot_name = self.get_object_base_class()
        comp_list.extend(self.get_recursive_components(nroot_name))
        return comp_list

    def my_refresh(self, overwrite_init):
        # Common properties - we do a little extra here
        nroot_properties = self.get_property_list(self.object_info)
        self.property_list.extend(nroot_properties)
        self.update_values(self.property_list, self.values, overwrite_init)

class ScriptingModelNObject(ScriptingModelSingleObject):

    def get_object_name (self):
        return "nObject"

    def get_object_base_class(self):
        return self.object.getclass()

    def get_object_info (self):
        # Return a structure with information about the object class    
        return self.get_class_info_from_xml("scriptclass")

    def get_xml_class_tag(self):
        return "scriptclass"

    def build_component_list (self):
        #No components - return empty list        
        return []

    def refresh(self, overwrite_init=False):
        #For NObject we do nothing
        pass


class GetterMethodRunner:
    def __init__(self, target_object, getter_string):
        self.target_object = target_object
        self.method_name = str( getter_string.lower() )
        self.statement_list = []
        self.result = None
        if self.target_object.hascommand(self.method_name):
            self.run()

    def run(self):
        writeshared.set_object(self.target_object)
        self.add_import_statements()
        self.add_assign_object_statement()
        self.add_getter_statement()
        self.add_delete_statement()
        self.run_command()
        self.result = readshared.get_result()

    def add_statement(self, statement_string):
        self.statement_list.append(statement_string)

    def add_import_statements(self):
        self.add_statement("import writeshared")
        self.add_statement("import readshared")
        self.add_statement("import copy")

    def add_assign_object_statement(self):
        self.add_statement("target_object = readshared.get_object()")

    def add_getter_statement(self):
        self.add_statement(
            "writeshared.set_result( target_object.%s() )" % self.method_name
            )
        
    def add_delete_statement(self):
        self.add_statement("del target_object")

    def run_command(self):
        # build a single string with each of the commands 
        # in the list separated by semi-colons
        command = ';'.join(self.statement_list)
        # Run the command
        servers.get_python_server().run(command)


def new_scripting_model_for_object(target_object):
    if target_object.isa("nentityclass"):
        return ScriptingModelEntityClass(target_object)
    elif target_object.isa("nroot"):
        return ScriptingModelNRoot(target_object)
    elif target_object.isa("nentityobject"):
        return ScriptingModelEntity(target_object)
    elif target_object.isa("nobject"):
        return ScriptingModelNObject(target_object)
    return None




        
        
