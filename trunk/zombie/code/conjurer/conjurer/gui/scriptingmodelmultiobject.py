##\author Javier Ferrer
##\file scriptinterfacemodel.py
##\brief Overload scriping model interface for a single object.
## Also implements inherentance issues when executing.

# Interface
import scriptingmodelsingleobject as scriptingmodel

class ScriptingModelMultiObject(scriptingmodel.ScriptingModelSingleObject):
    def __init__(self, objects):
        self.model_list = []       
        for each_object in objects:
            self.model_list.append(
                scriptingmodel.new_scripting_model_for_object(each_object)
                )

        self.properties = self.get_properties_list()
        self.commands = self.build_commands_list()

    # Return the name of the object
    def get_object_name (self):
        names = []
        for model in self.model_list:
            names.append( model.get_object_name() )
        return "; ".join(names)

    # Build a list of intersecting properties
    def get_properties_list (self):
        properties_check = {}
        # build a dictionary with the sum of the properties from each model
        for each_model in self.model_list:
            for each_property in each_model.get_properties_list():
                # attempt to increment the property count
                try:
                    properties_check[each_property['name']][0] += 1
                # if it failed, there was no existing entry, so create it
                except:
                    properties_check[each_property['name']] = [1, each_property]
        # now add to the list of intersecting properties
        # any properties present in all of the models
        # in the model list
        number_of_models = len(self.model_list)
        prop_intersect = []
        for each_value in properties_check.values():
            if each_value[0] == number_of_models:
                prop_intersect.append(each_value[1])
        self.properties_check = properties_check
        return prop_intersect

    def get_dirty_property_list(self):
        return []

    # Make a list with the intersection of commands
    def build_commands_list (self):
        commands_check = {}
        cmd_intersect = []

        for model in self.model_list:
            for command in model.get_commands_list():
                if command['name'] not in commands_check.keys():
                    commands_check[command['name']] = [1, command]
                else:
                    commands_check[command['name']][0] += 1

        value = 1
        for command, check in map(
            None, commands_check.keys(), commands_check.values()
            ):
            if check[0] > value:
                value = check[0]

        for command, check in map(
            None, commands_check.keys(), commands_check.values()
            ):
            if check[0] == value:
                cmd_intersect.append(check[1])

        self.commands_check = commands_check
        return cmd_intersect

    def get_component_list (self):
        return self.model_list[0].get_component_list()

    def get_property (self, prop_name):
        value = None
        for each_property in self.properties:
            if prop_name == each_property['name']:
                check_value = self.check_value(prop_name)
                if check_value:
                    value = self.model_list[0].get_property(prop_name)
                else:
                    value = '-'
        return value

    def get_command (self, cmd_name):
        value = None
        for command in self.commands:
            if cmd_name == command['name']:
                check_value = self.check_value(cmd_name)
                if check_value:
                    value = self.model_list[0].get_command(cmd_name)
                else:
                    value = '-'            
        return value

    def set_property (self, prop_name, value):
        for model in self.model_list:
            model.set_property(prop_name, value)

    def get_object_type(self):
        return None

    def get_object_class(self):
        return None

    def get_info_text (self):
        text = "You are editing multiple objects. Be careful, "\
                    "any changes commited will be made to all"\
                    "selected objects.\n\nAffected objects:\n\n"
        return text                                  

    def is_inherited (self, prop_name):
        inherited = True
        if self.check_value(prop_name):
            for model in self.model_list:
                if not model.is_inherited(prop_name):
                    inherited = False
        return inherited
        
    def revert (self):
        for model in self.model_list:
            model.revert()

    def refresh (self):
        for model in self.model_list:
            model.refresh()

    # Check if value is the same for all objects
    def check_value (self, prop_name):
        same_value = None
        if prop_name in self.properties_check.keys():
            same_value = True
            check_value = self.model_list[0].get_property(prop_name)
    
            # Check if the value is the same in all objects
            for model in self.model_list:            
                value = model.get_property(prop_name)
                if value != check_value:
                    same_value = False

        return same_value

    def is_for_single_object(self):
        return False
