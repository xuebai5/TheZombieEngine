
#-------------------------------------------------------------------------
# Imports
#-------------------------------------------------------------------------
import wx

import re
import nebulagui

# Property grid
import wx.propgrid as pg

# Conjurer
import servers

# Browser window used for select object
import objbrowserwindow as browser

# selectors
import grimoire
import entitybrowser

ret = 0.0

def execute (inspector):
    # make this object the current
    nebulagui.nebula_object = inspector.object

    # Construct the command string to be called.
    # A variable will be use for storage the return value/s

    command = "import nebulagui;"
    command = command + "result = nebulagui.nebula_object.%s" % inspector.cmds.GetValue()+ "("

    i = 0
    for param in inspector.in_params:            
        field = inspector.in_elements[i]
        param_type = inspector.xml.get_param_type(param)
        if param_type == 's':
            if "nroot" == inspector.xml.get_param_subtype(param):
                command = command + "'%s'," % field.GetValueAsString(0)
            elif "nobjectclass" == inspector.xml.get_param_subtype(param):
                string = field.GetValueAsString(0)
                # get the nobjectclass name and use it as parameter
                object_id = re.compile("(\w*)\(").search(string)
                getentity_string = str( object_id.group(1) )
                command = command + getentity_string + ","
            else:
                command = command + "'%s'," % field.GetValueAsString(0)

        elif param_type == 'o':
            if "nroot" == inspector.xml.get_param_subtype(param):
                command = command + "lookup('%s')," % field.GetValueAsString(0)                
            elif "nobjectclass" == inspector.xml.get_param_subtype(param):
                string = field.GetValueAsString(0)
                # get the nobjectclass id for get the entity object and use it as parameter
                object_id = re.compile("id=(\d*)").search(string)
                getentity_string = "servers.get_entity_object_server().getentityobject("+ str(object_id.group(1)) + ")"
                command = command + getentity_string + ","
            else:
                command = command + "lookup('%s')," % field.GetValueAsString(0)

        else:
            command = command + "%s," % field.GetValueAsString(0)
        i = i + 1

    if i > 0:
        command = command[:-1]

    command = command + ")"

    # Run the command
    servers.get_python_server().run(str(command)) 

    # Look for return values. If it have, extract it from the result
    # and fill the propertygrid fields.

    if len(inspector.out_params) > 0:
        if len(inspector.out_params) > 1:
            for i in xrange(len(inspector.out_params)):                    
                servers.get_python_server().\
                    run("import xmlinspector;"\
                            "xmlinspector.ret = "\
                            "result[" + str(i) +"]")
                __draw_return_values(
                    inspector, 
                    ret, 
                    inspector.out_params[i], 
                    inspector.out_elements[i]
                    )
        else:
            servers.get_python_server().\
                run("import xmlinspector;"\
                        "xmlinspector.ret = "\
                        "result")
            __draw_return_values(
                inspector, 
                ret, 
                inspector.out_params[0], 
                inspector.out_elements[0]
                )

    # delete temp variable
    servers.get_python_server().run( str("del result") )

    # if is a entity dirt the object.
    # TODO: Ugly hack! Find a better way to do this
    if inspector.object.hascommand("beginnewobjectentityclass"):
        servers.get_entity_class_server().setentityclassdirty(
            inspector.object, 
            True
            )
    elif inspector.object.hascommand("getid"):
        servers.get_entity_object_server().setentityobjectdirty(
            inspector.object, 
            True
            )

    # Refresh control
    inspector.cmds_pg.CollapseAll()
    inspector.cmds_pg.ExpandAll()
    inspector.executed = True

def cmds_select(inspector, cmd_name):
    inspector.cmds_pg.Clear()        
    
    # get command
    inspector.cmd = inspector.xml.get_cmd_by_name(
                            inspector.class_info[0]["commands"], 
                            cmd_name
                            )

    # get and update doc
    doc = inspector.xml.get_cmd_doc(inspector.cmd)
    inspector.doc_text.SetLabel(doc)
    inspector.Layout()

    # Draw In properties
    inspector.in_elements = list()
    inspector.out_elements = list()

    inspector.in_params = inspector.xml.get_cmd_input(inspector.cmd)

    if len(inspector.in_params) > 0:
        inspector.cmds_pg.Append(
            pg.PropertyCategory( 
                "In Args", 
                "InValues" 
                )
            )
        __draw_input_fields(
            inspector, 
            inspector.in_params, 
            "in"
            )

    # Draw Out properties
    inspector.out_params = inspector.xml.get_cmd_output(inspector.cmd)

    if len(inspector.out_params) > 0:
        inspector.cmds_pg.Append(
            pg.PropertyCategory(
                "Out Args", 
                "OutValues")
                )
        __draw_input_fields(
            inspector, 
            inspector.out_params, 
            "out"
            )

    inspector.btn_exec.Enable()
    inspector.cmds_pg.Refresh()

# Display the return valued in the given propertygrid field
# The value format displayed depends of the type
def __draw_return_values(inspector, ret, arg, field):
    arg_type = inspector.xml.get_param_type(arg)

    if arg_type == 'o':
        if ret != None:
            field.SetValueFromString(str(ret.getfullname()) , 0)
        else:
            field.SetValueFromString(str("None") , 0)
    elif arg_type == 'b':
        if ret == 1:
            field.SetValueFromString(str("True"), 0)
        else:
            field.SetValueFromString(str("False"), 0)
    elif arg_type == 'l':
        total = "\""
        for i in xrange(len(ret)):
            if re.compile("<Nebula object at .*>").match(
                                            str(ret[i].__class__)):
                total = total + str(ret[i].getfullname()) + "\"\""
            else:
                total = total + str(ret[i]) + "\"\""
        
        total = total[0:-1]                
        field.SetValueFromString(str(total), 0)
    else:
        field.SetValueFromString(str(ret), 0)

# The user has selected a class from the combo box
# Get all commands of the class and adds it to the combobox
def class_select(inspector, class_name):

    # I can call the init function multipletimes to get
    # the commands of all classes I want

    inspector.class_info = []
    if class_name == "All":
        for name in inspector.object.getclasses():
            class_info = inspector.check_class_info(name)
            inspector.class_info.append(class_info)
    else:
        class_info = inspector.check_class_info(class_name)
        inspector.class_info.append(class_info)
        doc = inspector.class_info[0]["doc"]            
        inspector.doc_text.SetLabel(doc)
        inspector.Layout()
    
    # Add the commands to the combox list

    for info in inspector.class_info:
        for cmd in info["commands"]:                        
            inspector.cmds.Append(inspector.xml.get_cmd_name(cmd))
        
    inspector.cmds.SetValue("Choose command")
    inspector.btn_exec.Disable()

# When select a command, fill the propertygrid with the correct properties

# Display the entry fields of the propertygrid depending of the type
# The field type id diferent depending of the type of parameter(out or in)

def __draw_input_fields(inspector, params, type):        
    i = 0        

    for param in params:
        if type == "in":                            
            label = "label_in%d" % i
        else:
            label = "label_out%d" % i

        param_type = inspector.xml.get_param_type(param)
        param_name = inspector.xml.get_param_name(param)

        # Get posible attributes

        editor = ""
        min_value = ""
        max_value = ""
        default_value = ""
        step = ""
        subtype = ""

        for attr in inspector.xml.get_param_attrs(param):
            attr_type = inspector.xml.get_attr_type(attr)

            if "editor" == attr_type:
                editor = inspector.xml.get_attr_value(attr)

            if "min" == attr_type:
                min_value = inspector.xml.get_attr_value(attr)

            if "max" == attr_type:
                max_value = inspector.xml.get_attr_value(attr)

            if "default" == attr_type:
                default_value = inspector.xml.get_attr_value(attr)

            if "step" == attr_type:
                step = inspector.xml.get_attr_value(attr)

            if "subtype" == attr_type:
                subtype = inspector.xml.get_attr_value(attr)

        if param_type == 'i':
            if "spin" == editor:
                element = pg.IntProperty(param_name, label)                

            elif "color" == editor:
                element = pg.ColourProperty(param_name, label, wx.RED)

            elif "slider" == editor:
                element = pg.IntProperty(param_name, label)

            elif "enum" == editor:
                test_labels = ["uno", "dos", "tres"]
                test_values = [1, 2, 3]
                element = pg.EnumProperty(
                                param_name, 
                                label, 
                                test_labels, 
                                test_values)

            elif "flags" == editor:
                element = pg.IntProperty("flags", label)
            else:
                element = pg.IntProperty(param_name, label)
        
        elif param_type == 'f':
            element = pg.FloatProperty('Float', label) 

        elif param_type == 's':
            if "" == editor:
                if type == "in":
                    element = check_string_subtypes(subtype, param_name, label)
                else:
                    element = pg.StringProperty(param_name, label, '')
            elif "font" == editor:
                element = pg.FontProperty(param_name, label, wx.NORMAL_FONT)

        elif param_type == 'b':
            element = pg.EnumProperty('Boolean', 
                               label,['True','False'],[1,0],2)
        elif param_type == 'o':
            if type == "in":
                element = check_string_subtypes(subtype, param_name, label)
            else:
                element = pg.StringProperty(param_name, label, '')
        elif param_type == 'l':
            if type == "in":
                element = pg.StringProperty('List', label, '')
            else:
                element = pg.ArrayStringProperty('List', label, '')
            
        else:
            element = pg.StringProperty('String', label,
                                                   'Unknow type!!!')

        inspector.cmds_pg.Append(element)

        if "spin" == editor or "slider" == editor:
            inspector.cmds_pg.MakeIntegerSpinControl(label)

        if "in" == type:                
            inspector.in_elements.append(element)
        else:
            inspector.out_elements.append(element)

        i = i + 1

def mangle_path(path):
    return servers.get_file_server().manglepath(path)

def check_string_subtypes(subtype, param_name, label):
    if subtype is not "":
        if "asset" == subtype:
            directory = mangle_path("wc:export/assets") + "\\"
            element = pg.FileProperty(param_name, label, directory)
        elif "texture" == subtype:
            directory = mangle_path("wc:export/textures") + "\\"
            element = pg.FileProperty(param_name, label, directory)
        elif "shader" == subtype:
            directory = mangle_path("wc:export/shader") + "\\"
            element = pg.FileProperty(param_name, label, directory)
        elif "file" == subtype:
            directory = mangle_path("wc:")
            element = pg.FileProperty(param_name, label, directory)
        elif "directory" == subtype:
            directory = mangle_path("home:")
            element = pg.DirProperty(param_name, label, directory)
        elif "entityobject" == subtype:
            element = pg.StringCallBackProperty(param_name, label, '' )
            dlg = browser.create_dialog(True, "/sys/nobject/nentityobject")
            element.SetCallBackDialog(dlg)
        elif "nroot" == subtype:
            element = pg.StringCallBackProperty(param_name, label, '' )
            dlg = browser.create_dialog(True, "/")                            
            element.SetCallBackDialog(dlg)
        elif "entityclass" == subtype:
            element = pg.StringCallBackProperty(param_name, label, '' )
            dlg = grimoire.create_dialog(True)                            
            element.SetCallBackDialog(dlg)
        elif "nobjectclass" == subtype:
            element = pg.StringCallBackProperty(param_name, label, '' )
            dlg = entitybrowser.create_dialog(self)
            element.SetCallBackDialog(dlg)
    else:
        # No subtype
        element = pg.StringProperty(param_name, label, '')

    return element

