##\author Javier Ferrer
##\file scriptinterfacemodel.py
##\brief This defines an interface for get all data information about nebula commands, 
## properties and signals. Trought this interface you can get all commands and his values.
## This is powerfull interface for builf GUI interfaces or any thing you want.

class ScriptingModel:
    # Enum type for mode attribute
    class inheritance_mode:
        normal = 1
        cascade = 2
        force = 3

    # Initialize values
    def __init__(self, object):
        self.values = {}
        self.init_values = {}
        self.object = object
        self.type = self.__get_object_type(object)
        self.mode = self.inheritance_mode.normal

    # Return a list of command structures
    def get_commands_list (self):
        pass

    # Return the command value scpecified by name
    def get_command (self, name):
        pass

    # Returns a list of property structures
    def get_properties_list (self):
        pass

    # Returns a list of component structures
    def get_components_list (self):
        pass

    # Returns a string with some information
    def get_info_text (self):
        pass

    # Get the value of a specific property
    def get_properties (self, name):
        pass

    # Set a value for a specific property, and execute if flag is not unset
    def set_property (self, name, execute_now=True):
        pass

    # Execute all the properties
    def execute_all (self):
        pass

    # Refresh values from the getters
    def refresh (self):
        pass

    # Set all properties to init state (the state when the object was created)
    def revert ():
        pass

    # Set the inheritance mode (unique, cascade or force)
    def set_mode (self, mode):
        self.mode = mode

    # Return the object type (entityclass, entity, nroot or nobject)
    # This information is used by the client for know 
    def get_object_type (self, object=None):
        if object is not None:
            return self.__get_object_type(object)
        else:
            return self.type

    def get_object_name (self):
        pass

    # Private function to get the object type
    def __get_object_type (self, object):
        if object.isa("nentityclass"):
            return "entityclass"
        elif object.isa("nroot"):
            return "nroot"
        elif object.isa("nentityobject"):
            return "entity"
        elif object.isa("nobject"):
            return "nobject"

