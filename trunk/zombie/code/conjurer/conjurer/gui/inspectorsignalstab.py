
#-------------------------------------------------------------------------
# Conjurer Services, S.A. (2005)
#-------------------------------------------------------------------------
# Description: A signal emitter for nobject derived objects
#-------------------------------------------------------------------------
# Log:
#    - 21-02-2005: Javier Ferrer:: Created File
#-------------------------------------------------------------------------

#-------------------------------------------------------------------------
# TODO:
#    - Special controls for some types (spinners, textures selection...)
#    - Variable selection for some know types
#-------------------------------------------------------------------------
#-------------------------------------------------------------------------
# Imports
#-------------------------------------------------------------------------
import wx
import re

# Property grid
import wx.propgrid as pg

# Conjurer
import pynebula
import servers
import floatctrl
import conjurerconfig as cfg

# Help system
import nebulahelpevent as nh

# Browser window used for select object
import objbrowserwindow as browser
import nebulagui

# Global variable that comunicate the result of commands with other modules
ret = 0.0

class SignalEmitterTab(wx.Panel):
    def __init__(self, parent, object=None):
        wx.Panel.__init__(self, parent, -1)

        self.object = object
        self.parent = parent
        self.executed = False

        # Create controls...

        # Text label
        text_class = wx.StaticText(self, -1, "Class")

        # Classes combobox. We have to get the list of classes from object
        class_list = list()
        if object is not None:
            for class_name in object.getclasses():
                class_list.append(class_name)
        class_list.append("All")

        self.classes = wx.ComboBox(
                                self, 
                                -1, 
                                "Choose class",
                                size=wx.DefaultSize,
                                choices=class_list,
                                style=wx.CB_DROPDOWN
                                )

        # Text label
        text_cmd = wx.StaticText(self, -1, "Command")

        # Commands combobox
        self.cmds = wx.ComboBox(
                            self, 
                            -1, 
                            "Choose command",
                            size=wx.DefaultSize,
                            style=wx.CB_DROPDOWN|wx.CB_SORT
                            )

        # Property grid
        self.cmds_pg = pg.PropertyGrid(
                                self, 
                                -1, 
                                wx.DefaultPosition, 
                                wx.DefaultSize, 
                                pg.PG_BOLD_MODIFIED
                                    |pg.PG_DEFAULT_STYLE
                                    |pg.PG_SPLITTER_AUTO_CENTER
                                    |pg.PG_FLOAT_PRECISION
                                )

        # Send button
        self.btn_send = wx.Button(self, -1, "Send")
        self.btn_send.Disable()

        # Label
        self.lbl_post = wx.StaticText(self, -1, "Delay (secs)")

        # Float entry
        self.ctrl_delay = floatctrl.FloatCtrl(self, size=wx.Size(50, -1))

        # Do layout
        sizer = wx.BoxSizer(wx.VERTICAL)
        grid_sizer = wx.FlexGridSizer(
                            2, 
                            2, 
                            cfg.BORDER_WIDTH, 
                            cfg.BORDER_WIDTH * 2
                            )
        grid_sizer.AddGrowableCol(1)
        grid_sizer.Add(
            text_class, 
            0, 
            cfg.LABEL_ALIGN|wx.ALIGN_CENTER_VERTICAL|wx.ADJUST_MINSIZE
            )
        grid_sizer.Add(
            self.classes, 
            0, 
            wx.ADJUST_MINSIZE|wx.EXPAND
            )
        grid_sizer.Add(
            text_cmd, 
            0, 
            cfg.LABEL_ALIGN|wx.ALIGN_CENTER_VERTICAL|wx.ADJUST_MINSIZE 
            )
        grid_sizer.Add(
            self.cmds, 
            0, 
            wx.ADJUST_MINSIZE|wx.EXPAND
            )
        sizer.Add(
            grid_sizer, 
            0, 
            wx.ALL|wx.EXPAND,     
            cfg.BORDER_WIDTH
            )
        sizer_property_grid = wx.BoxSizer(wx.HORIZONTAL)
        sizer_property_grid.Add(
            self.cmds_pg, 
            1, 
            wx.EXPAND
            )
        sizer.Add(
            sizer_property_grid, 
            1, 
            wx.EXPAND|wx.ALL, 
            cfg.BORDER_WIDTH
            )
        grid_sizer_delay = wx.FlexGridSizer(
                                    2, 
                                    2, 
                                    cfg.BORDER_WIDTH, 
                                    cfg.BORDER_WIDTH * 2
                                    )
        grid_sizer_delay.Add(
            self.lbl_post, 
            0, 
            cfg.LABEL_ALIGN|wx.ALIGN_CENTER_VERTICAL|wx.ADJUST_MINSIZE, 
            0
            )
        grid_sizer_delay.Add(
            self.ctrl_delay, 
            0, 
            wx.ADJUST_MINSIZE|wx.EXPAND
            )
        sizer.Add(
            grid_sizer_delay, 
            0, 
            wx.ALL, 
            cfg.BORDER_WIDTH
            )
        horizontal_line = wx.StaticLine(
                                    self, 
                                    -1, 
                                    ( -1, -1 ), 
                                    ( -1, -1 ), 
                                    wx.LI_HORIZONTAL 
                                    )
        sizer.Add(
            horizontal_line, 
            0, 
            wx.EXPAND|wx.LEFT|wx.RIGHT, 
            cfg.BORDER_WIDTH
            )

        sizer_buttons = wx.BoxSizer(wx.HORIZONTAL)
        sizer_buttons.Add(self.btn_send, 0)
        sizer.Add(
            sizer_buttons, 
            0, 
            wx.ALL|wx.ALIGN_RIGHT, 
            cfg.BORDER_WIDTH
            )

        self.SetSizer(sizer)

        # Set Tooltips
        self.lbl_post.SetToolTip(
            wx.ToolTip("Delay in seconds before firing the signal")
            )
        self.btn_send.SetToolTip(
            wx.ToolTip("Click to execute the command")
            )
        
        ## Bind events

        self.cmds_pg.Bind(wx.EVT_KEY_DOWN, self.on_key_down)
        self.classes.Bind(wx.EVT_COMBOBOX, self.on_class_select)
        self.classes.Bind(wx.EVT_CHAR, self.on_combo_char)
        self.classes.Bind(wx.EVT_KEY_DOWN, self.on_key_down)
        self.cmds.Bind(wx.EVT_COMBOBOX, self.on_signal_select)
        self.cmds.Bind(wx.EVT_CHAR, self.on_combo_char)
        self.Bind(wx.EVT_BUTTON, self.on_send_button, self.btn_send)
        self.Bind(pg.EVT_PG_CHANGED, self.on_pg, self.cmds_pg)        
        self.Bind(wx.EVT_SIZE, self.OnSize)

    def refresh(self):
        pass

    def on_key_down(self, evt):
        if evt.KeyCode() == nh.KEY_HELP:
            value = self.classes.GetValue()
            if value != "Choose classes":
                value = value + "|nebula"
                self.parent.GetEventHandler().ProcessEvent(
                    nh.HelpEvent(
                        self.GetId(),
                        value, 
                        self
                        )
                    )
        else:
            evt.Skip()
    
    def on_combo_char(self, evt):
        evt.StopPropagation()

    def __get_signal_command (self, time=0.0):
        # construct the command string to be called.
        # a variable will be use to store the return value/s

        arg = self.cmd_proto_name.rsplit('_')
        command = "emitter = pynebula.lookup('" + \
                                str(self.object.getfullname()) + "');"

        # two way for send commands: with and without delay
        
        if time > 0.0:
            command = command + "result = emitter.post(%f, \'%s\'" \
                                          % (time, self.cmds.GetValue())+ ","
        else:
            command = command + "result = emitter.emit(\'%s\'" \
                                          % self.cmds.GetValue()+ ","

        # add parameters

        i = 0
        for field in self.in_elements:
            if arg[2][i] == 's':
                command = command + "'%s'," % field.GetValueAsString(0)
            elif arg[2][i] == 'o':
                command = command + "lookup('%s')," % field.GetValueAsString(0)
            else:
                command = command + "%s," % field.GetValueAsString(0)
            i = i + 1

        command = command[:-1]

        # free memory

        command = command + ")"

        return command

    def on_send_button(self, evt):
        # Send a enter key event to property for update values

        event =  wx.KeyEvent(wx.wxEVT_CHAR)
        event.m_keyCode = wx.WXK_RETURN
        self.cmds_pg.GetEventHandler().ProcessEvent(event)

        # make this object the current
        nebulagui.nebula_object = self.object

        command = self.__get_signal_command(float(self.ctrl_delay.GetValue()))

        # Run the command

        servers.get_python_server().run(str(command))

        # Look for return values. If it have, extract it from the result
        # and fill the propertygrid fields.

        arg = self.cmd_proto_name.rsplit('_')

        if arg[0][0] != 'v':
            if len(arg[0]) > 1:
                for i in xrange(len(arg[0])):
                    servers.get_python_server().run(
                        "import inspectorsignalstab;"\
                        "inspectorsignalstab.ret = "\
                        "result[" + str(i) +"]"
                        )
                    self.__draw_return_values(ret, arg[0], self.out_elements[i])
            else:
                servers.get_python_server().run(
                    "import inspectorsignalstab;"\
                    "inspectorsignalstab.ret = "\
                        "result"
                    )
                self.__draw_return_values(ret, arg[0], self.out_elements[0])

            # Delete the temp var
            servers.get_python_server().run(str("del inspectorsignalstab.ret"))

        # delete temp variable
        servers.get_python_server().run(str("del result"))

        self.cmds_pg.Refresh()        
        self.executed = True

    # Display the return valued in the given propertygrid field
    # The way the value is displayed depends of the type

    def __draw_return_values(self, ret, arg, field):
        for num_args in xrange(len(arg)):
            if arg[num_args] == 'o':
                if ret is not None:
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

    def __draw_input_fields(self, arg, element, type):        
        for i in xrange(len(arg)):
            if type == "in":                            
                label = "label_in%d" % i
            else:
                label = "label_out%d" % i
                
            if arg[i] == 'i':
                element.append(pg.IntProperty('Integer', label))
                self.cmds_pg.Append(element[i])
            elif arg[i] == 'f':
                element.append(pg.FloatProperty('Float', label))
                self.cmds_pg.Append(element[i])
            elif arg[i] == 's':
                if type == "in":
                    element.append(pg.StringCallBackProperty('String', 
                                    label, '' ))
                    self.cmds_pg.Append(element[i])
                    dlg = browser.create_dialog(self, True)
                    element[i].SetCallBackDialog(dlg)
                else:                    
                    element.append(pg.StringProperty('String', label, ''))
                    self.cmds_pg.Append(element[i])
            elif arg[i] == 'b':
                element.append(pg.EnumProperty('Boolean', 
                                   label,['True','False'],[1,0],2,0 ))
                self.cmds_pg.Append(element[i])
            elif arg[i] == 'o':
                if type == "in":
                    element.append(pg.StringCallBackProperty( 'Object', 
                                        label, '' ))
                    self.cmds_pg.Append(element[i])
                    dlg = browser.create_dialog(self, True)        
                    element[i].SetCallBackDialog(dlg)  
                else:
                    element.append(pg.StringProperty('Object', label, ''))
                    self.cmds_pg.Append(element[i])
            elif arg[i] == 'l':
                if type == "in":
                    element.append(pg.StringProperty('List', label, ''))
                    self.cmds_pg.Append(element[i])
                else:
                    element.append(pg.ArrayStringProperty('List', label, ''))
                    self.cmds_pg.Append(element[i])
                
            else:
                element.append(pg.StringProperty('String', label,
                                                       'Unknow type!!!'))
                self.cmds_pg.Append(element[i])
        
    def on_pg(self, evt):
        evt.Skip()
        
    # The user has selected a class from the combo box
    # Get all commands for the class and add them to the combobox

    def on_class_select(self, evt):
        try:
            wx.BeginBusyCursor()
            class_name = evt.GetString()
            # Clear GUI
            self.cmds.Clear()
            self.cmds_pg.Clear()

            # Temporary object to get the commands, must be deleted!!
            inspector = pynebula.new(
                                "nobjinspector",
                                "/editor/inspector"
                                )

            # I can call the init function several times to get the 
            # signals for all the  classes I want
            if class_name == "All":
                for name in self.object.getclasses():
                    inspector.initsignalsclass( str(name) )
            else:
                inspector.initsignalsclass( str(class_name) )

            # Add the commands to the combox list
            num_cmds = inspector.getcmdscount()
            self.cmds_protos = list()
            self.get_dict = dict()
            for index in xrange(num_cmds):            
                self.cmds.Append(
                    inspector.getcmdname(index) 
                    )
                if re.compile("get.*").match( inspector.getcmdname(index) ):
                    self.get_dict[inspector.getcmdname(index)] = \
                                                    inspector.getcmdprotoname(index)

            # Delete the inspector
            pynebula.delete("/editor/inspector")

            self.cmds.SetValue("Choose command")
            self.btn_send.Disable()
        finally:
            wx.EndBusyCursor()

    # On selecting a signal, fill the propertygrid with the correct properties
    def on_signal_select(self, evt):
        cmd_name = self.cmds.GetStringSelection()

        self.cmds_pg.Clear()        
        class_name = self.classes.GetStringSelection()

        # Temporary object to get the commands: must be deleted after use!!
        inspector = pynebula.new(
                            "nobjinspector",
                            "/editor/inspector"
                            )
        if class_name == "All":
            for name in self.object.getclasses():
                inspector.initsignalsclass( str(name) )
        else:
            inspector.initsignalsclass( str(class_name) )

        # Build a dictionary with the cmd name and its
        # position in the array: this way I can list the cmds
        # alphabetically
        self.cmd_dict = dict()
        for index in xrange( inspector.getcmdscount() ):
            self.cmd_dict[inspector.getcmdname(index)] = index

        num_cmd = self.cmd_dict[cmd_name]
        num_args = inspector.getinargscount(num_cmd)

        # Build the properties depending of the argument type
        self.cmd_proto_name = inspector.getcmdprotoname(num_cmd)

        arg = self.cmd_proto_name.rsplit('_')

        # Draw In properties
        self.in_elements = list()
        if num_args > 0:
            self.cmds_pg.Append(
                pg.PropertyCategory(
                    "In Args", 
                    "InValues" 
                    )
                )
            self.__draw_input_fields(
                arg[2], 
                self.in_elements, 
                "in"
                )

        # Draw Out properties
        self.out_elements = list()
        if inspector.getoutargscount(num_cmd) > 0:
            self.cmds_pg.Append(
                pg.PropertyCategory(
                    "Out Args", 
                    "OutValues"
                    )
                )
            self.__draw_input_fields(
                arg[0], 
                self.out_elements, 
                "out"
                )

        # Delete the inspector
        pynebula.delete("/editor/inspector")

        self.btn_send.Enable()
    
    def OnSize(self, evt):
        self.Layout()


