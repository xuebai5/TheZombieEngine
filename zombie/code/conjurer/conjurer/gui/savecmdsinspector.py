#-------------------------------------------------------------------------
# Imports
#-------------------------------------------------------------------------
import wx
import re
import nebulagui

# Property grid
import wx.propgrid as pg

# Conjurer
import pynebula
import servers

# Browser window used for select object
import objbrowserwindow as browser


ret = 0.0

# The user has selected a class from the combo box
# Get all commands of the class and add it to the combobox

def class_select(inspector_win, class_name):
    try:
        wx.BeginBusyCursor()
        # Temporary object for get the commands, must be deleted!!
        inspector = pynebula.new(
                            "nobjinspector",
                            "/editor/inspector"
                            )
    
        # I can call the init function multipletimes to get the 
        # commands for all classes I want
        if class_name == "All":
            for name in inspector_win.object.getclasses():
                inspector.initcmdsclass( str(name) )
        else:
            inspector.initcmdsclass( str(class_name) )
            doc_string = "Class %s not found in xml database.\n"\
                                "Doc and custom controls deactivated" % class_name
            inspector_win.doc_text.SetLabel(doc_string)
            inspector_win.Layout()

        # Add the commands to the combox list
        num_cmds = inspector.getcmdscount()
        inspector_win.cmds_protos = list()
        inspector_win.get_dict = dict()
        for index in xrange(num_cmds):            
            inspector_win.cmds.Append(
                inspector.getcmdname(index) 
                )
            if re.compile("get.*").match( inspector.getcmdname(index) ):
                inspector_win.get_dict[inspector.getcmdname(index)] = \
                                                inspector.getcmdprotoname(index)

        # Delete the inspector
        pynebula.delete("/editor/inspector")
    
        inspector_win.cmds.SetValue("Choose command")
        inspector_win.btn_exec.Disable()
    finally:
        wx.EndBusyCursor()

# When select a command, fill the propertygrid with the correct properties
def cmds_select(inspector_win):
    cmd_name = inspector_win.cmds.GetStringSelection()

    inspector_win.cmds_pg.Clear()        
    class_name = str( inspector_win.classes.GetStringSelection() )

    # Temporary object to get the commands: must be deleted after use!!
    inspector = pynebula.new(
                        "nobjinspector",
                        "/editor/inspector"
                        )

    if class_name == "All":
        for name in inspector_win.object.getclasses():
            inspector.initcmdsclass( str(name) )
    else:
        inspector.initcmdsclass(class_name)

    # Build a dictionary with the cmd name and its
    # position in the array: this way I can list the cmds
    # alphabetically
    inspector_win.cmd_dict = dict()
    for index in xrange( inspector.getcmdscount() ):
        inspector_win.cmd_dict[inspector.getcmdname(index)] = index

    num_cmd = inspector_win.cmd_dict[cmd_name]
    num_args = inspector.getinargscount(num_cmd)        

    # Enable the get button if the command has a get counterpart
    if re.compile("set.*").match(cmd_name):
        name = cmd_name[3:]
        try:
            num_getcmd = inspector_win.cmd_dict[str("get"+name)]
            num_get_out_args = inspector.getoutargscount(num_getcmd)
            if num_args != num_get_out_args:
                inspector_win.btn_get.Disable()
            else:
                inspector_win.btn_get.Enable()                
        except:           
            inspector_win.btn_get.Disable()
    else:
        inspector_win.btn_get.Disable()
    
    # Build the properties depending on the argument type
    inspector_win.cmd_proto_name = inspector.getcmdprotoname(num_cmd)

    arg = inspector_win.cmd_proto_name.rsplit('_')              

    # Draw In properties
    inspector_win.in_elements = list()  
    if num_args > 0:
        inspector_win.cmds_pg.Append(
            pg.PropertyCategory(
                "In Args", 
                "InValues" 
                )
            )
        draw_input_fields(
            inspector_win, 
            arg[2], 
            inspector_win.in_elements, 
            "in"
            )
        
    # Draw Out properties
    inspector_win.out_elements = list()
    if inspector.getoutargscount(num_cmd) > 0:
        inspector_win.cmds_pg.Append(
            pg.PropertyCategory(
                "Out Args", 
                "OutValues"
                )
            )
        draw_input_fields(
            inspector_win, 
            arg[0], 
            inspector_win.out_elements, 
            "out"
            )

    # Delete the inspector
    pynebula.delete("/editor/inspector")

    inspector_win.btn_exec.Enable()
    inspector_win.cmds_pg.Refresh()

def draw_return_values(inspector_win, ret, arg, field):
    for num_args in xrange(len(arg)):
        if arg[num_args] == 'o':
            if ret != None:
                field.SetValueFromString(str(ret.getfullname()) , 0)
            else:
                field.SetValueFromString(str("None") , 0)
        elif arg[num_args] == 'b':
            if ret == 1:
                field.SetValueFromString(str("True"), 0)
            else:
                field.SetValueFromString(str("False"), 0)
        elif arg[num_args] == 'l':
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

# Display the entry fields of the propertygrid depending of the type
# The field type id diferent depending of the type of parameter(out or in)

def draw_input_fields(inspector_win, arg, element, type):        
    for i in xrange(len(arg)):
        if type == "in":                            
            label = "label_in%d" % i
        else:
            label = "label_out%d" % i
            
        if arg[i] == 'i':
            element.append(pg.IntProperty('Integer', label))
            inspector_win.cmds_pg.Append(element[i])
        elif arg[i] == 'f':
            element.append(pg.FloatProperty('Float', label))
            inspector_win.cmds_pg.Append(element[i])
        elif arg[i] == 's':
            if type == "in":
                element.append(pg.StringCallBackProperty('String', 
                                label, '' ))
                inspector_win.cmds_pg.Append(element[i])
                dlg = browser.create_dialog(inspector_win, True)
                element[i].SetCallBackDialog(dlg)                                               
            else:                    
                element.append(pg.StringProperty('String', label, ''))
                inspector_win.cmds_pg.Append(element[i])
        elif arg[i] == 'b':
            element.append(pg.EnumProperty('Boolean', 
                               label,['True','False'],[1,0],2 ))
            inspector_win.cmds_pg.Append(element[i])
        elif arg[i] == 'o':
            if type == "in":
                element.append(pg.StringCallBackProperty( 'Object', 
                                    label, '' ))
                inspector_win.cmds_pg.Append(element[i])
                dlg = browser.create_dialog(inspector_win, True)        
                element[i].SetCallBackDialog(dlg)  
            else:
                element.append(pg.StringProperty('Object', label, ''))
                inspector_win.cmds_pg.Append(element[i])
        elif arg[i] == 'l':
            if type == "in":
                element.append(pg.StringProperty('List', label, ''))
                inspector_win.cmds_pg.Append(element[i])
            else:
                element.append(pg.ArrayStringProperty('List', label, ''))
                inspector_win.cmds_pg.Append(element[i])
            
        else:
            element.append(pg.StringProperty('String', label,
                                                   'Unknow type!!!'))
            inspector_win.cmds_pg.Append(element[i])

def execute(inspector_win):
    # Send a enter key event to property for update values

    event =  wx.KeyEvent(wx.wxEVT_CHAR)
    event.m_keyCode = wx.WXK_RETURN  
    inspector_win.cmds_pg.GetEventHandler().ProcessEvent(event)

    # make this object the current
    nebulagui.nebula_object = inspector_win.object

    # Construct the command string to be called.
    # A variable will be use for storage the return value/s

    arg = inspector_win.cmd_proto_name.rsplit('_')
    command = "import nebulagui;"
    command = command + "result = nebulagui.nebula_object.%s" % inspector_win.cmds.GetValue()+ "("

    i = 0
    for field in inspector_win.in_elements:            
        if arg[2][i] == 's':
            command = command + "'%s'," % field.GetValueAsString(0)
        elif arg[2][i] == 'o':
            command = command + "lookup('%s')," % field.GetValueAsString(0)                
        else:
            command = command + "%s," % field.GetValueAsString(0)                
        i = i + 1

    if i > 0:
        command = command[:-1]

    command = command + ")"
    
    # Make undo command if possible
    normal_execute = True

    if re.compile("set.*").match(inspector_win.cmds.GetValue()):
        name = inspector_win.cmds.GetValue()[3:]
        num_getcmd = inspector_win.cmds.FindString(str("get"+name))

        if num_getcmd != wx.NOT_FOUND:
            proto_cmd = inspector_win.get_dict[str("get"+name)]
            get_arg = proto_cmd.rsplit('_')

            if get_arg[2] == 'v':

                # Execute the get for get undo values

                get_cmd = "import nebulagui;import savecmdsinspector;"
                get_cmd = get_cmd + "savecmdsinspector.ret = nebulagui.nebula_object.%s" \
                                                      % str("get"+name) + "()"
                pynebula.lookup('/sys/servers/persist/npythonserver').\
                                    run(str(get_cmd))
            
                # Create undo command

                i = 0
                undo_cmd = "import nebulagui;"
                undo_cmd = undo_cmd + "nebulagui.nebula_object.%s" % str("set"+name) + "("
                for i in xrange(len(get_arg[0])):       
                    if len(get_arg[0]) > 1:
                        element = ret[i]
                    else:
                        element = ret
                    if get_arg[0][i] == 's':
                        undo_cmd = undo_cmd + "'%s'," % str(element)
                    elif get_arg[0][i] == 'o':
                        undo_cmd = undo_cmd + "lookup('%s')," % str(element)                
                    else:
                        undo_cmd = undo_cmd + "%s," % str(element)                
                    i = i + 1

                undo_cmd = undo_cmd[:-1] + ")"

                # New command
                servers.get_command_server().newcommand(str(command), 
                                                        str(undo_cmd))
                normal_execute = False

    if normal_execute:   
        # Run the command
        servers.get_python_server().run(str(command))       
    
    # Look for return values. If it have, extract it from the result
    # and fill the propertygrid fields.

    if arg[0][0] != 'v':
        if len(arg[0]) > 1:
            for i in xrange(len(arg[0])):                    
                value = servers.get_python_server().\
                         run("import savecmdsinspector;"\
                             "savecmdsinspector.ret = "\
                             "result[" + str(i) +"]")
                draw_return_values(inspector_win, ret, arg[0], inspector_win.out_elements[i]) 
        else:
            value = servers.get_python_server().\
                         run("import savecmdsinspector;"\
                             "savecmdsinspector.ret = "\
                             "result")
            draw_return_values(inspector_win, ret, arg[0], inspector_win.out_elements[0])
            
                
    # delete temp variable
    servers.get_python_server().run(str("del result"))

    # if is a entity dirt the object.
    # TODO: Ugly hack! Find a better way to do this

    if inspector_win.object.hascommand("beginnewobjectentityclass"):
        servers.get_entity_class_server().setentityclassdirty(inspector_win.object, True)
    elif inspector_win.object.hascommand("getid"):
        servers.get_entity_object_server().setentityobjectdirty(inspector_win.object, True)

    # Refresh control
    inspector_win.cmds_pg.CollapseAll()
    inspector_win.cmds_pg.ExpandAll()
    inspector_win.executed = True

def get_button(inspector_win):
    # make this object the current
    nebulagui.nebula_object = inspector_win.object

    # Construct the get command        

    cmd_name = inspector_win.cmds.GetValue()
    get_cmd_name = "get" + cmd_name[3:]

    # Execute the get command

    command = "import nebulagui;import savecmdsinspector;"
    command = command + "savecmdsinspector.result = nebulagui.nebula_object.%s" % get_cmd_name + "()"
    servers.get_python_server().run(str(command))

    # The in arguments of set commands are always the out arguments
    # of set commands, then I can use them.

    arg = inspector_win.cmd_proto_name.rsplit('_')
    if arg[2][0] != 'v':
        if len(arg[2]) > 1:
            for i in xrange(len(arg[2])):
                draw_return_values(inspector_win, result[i], arg[2],
                                        inspector_win.in_elements[i])
        else:
            draw_return_values(inspector_win, result, arg[2], inspector_win.in_elements[0])

    inspector_win.cmds_pg.CollapseAll()
    inspector_win.cmds_pg.ExpandAll()

