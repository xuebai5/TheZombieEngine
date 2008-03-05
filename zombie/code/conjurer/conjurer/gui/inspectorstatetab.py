#-------------------------------------------------------------------------
# File: inspectorstatetab
#-------------------------------------------------------------------------
# Conjurer Services, S.A. (2005)
#-------------------------------------------------------------------------
# Description: A state inspector for nobject derived objects
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

# Property grid
import wx.propgrid as pg

# Conjurer
import pynebula
import servers
import conjurerconfig as cfg
import conjurerframework as cjr

import nebulagui

import readshared

class StateTab(wx.Panel):
    def __init__(self, parent, object=None, entity = False):
        wx.Panel.__init__(self, parent, -1)     

        self.object = object
        self.entity = entity
        self.xml = readshared.get_xml_dom()

        nebulagui.nebula_object = self.object

        # Expand all button
        btn_expand = wx.Button(self, -1, "Expand All")

        # Collapse all button
        btn_collapse = wx.Button(self, -1, "Collapse All")
        
        # Property grid 
        # N.B. the state property grid is NOT sorted - 
        # see task #4270
        self.cmds_pg = pg.PropertyGrid(
                                self, 
                                -1, 
                                wx.DefaultPosition, 
                                wx.DefaultSize, 
                                pg.PG_BOLD_MODIFIED|
                                    pg.PG_DEFAULT_STYLE|
                                    pg.PG_SPLITTER_AUTO_CENTER
                                )

        # Default button
        btn_default = wx.Button(self, -1, "Default")

        # Save button
        btn_save = wx.Button(self, -1, "Save")

        # Display the state commands
        self.draw_state_commands(self.object)
        
        # Do layout
        border_width = cfg.BORDER_WIDTH
        sizer = wx.BoxSizer(wx.VERTICAL)
        sizer_buttons = wx.BoxSizer(wx.HORIZONTAL)
        sizer_buttons.Add(btn_expand, 0, wx.ADJUST_MINSIZE)
        sizer_buttons.Add(
            btn_collapse, 
            0, 
            wx.ADJUST_MINSIZE|wx.LEFT, 
            border_width
            )
        sizer.Add(sizer_buttons, 0, wx.EXPAND|wx.ALL, border_width)
        sizer_cmds = wx.BoxSizer(wx.HORIZONTAL)
        sizer_cmds.Add(self.cmds_pg, 1, wx.EXPAND)
        sizer.Add(sizer_cmds, 1, wx.EXPAND|wx.LEFT|wx.RIGHT, border_width)
        sizer_more_buttons = wx.BoxSizer(wx.HORIZONTAL)
        sizer_more_buttons.Add(btn_default, 0)
        sizer_more_buttons.Add(
            btn_save, 
            0, 
            wx.ADJUST_MINSIZE|wx.LEFT, 
            border_width
            )
        sizer.Add(sizer_more_buttons, 0, wx.ALIGN_RIGHT|wx.ALL, border_width)
        self.SetSizer(sizer)

        # Set tooltips
        btn_expand.SetToolTip(
            wx.ToolTip("Expand all categories")
            )
        btn_collapse.SetToolTip(
            wx.ToolTip("Collapse all categories")
            )
        btn_default.SetToolTip(
            wx.ToolTip("Return to original state")
            )
        btn_save.SetToolTip(
            wx.ToolTip("Save the new state")
            )

        # Bind events
        self.Bind(wx.EVT_BUTTON, self.on_expand_button, btn_expand)
        self.Bind(wx.EVT_BUTTON, self.on_collpase_button, btn_collapse)
        self.Bind(wx.EVT_BUTTON, self.on_save_button, btn_save)
        self.Bind(wx.EVT_BUTTON, self.on_default_button, btn_default)    

    def __get_undo_command (self):
        # Return the command to execute for undo. The command is a collection
        # of commands separated by ;
        
        command_list = []
        command_list.append("import nebulagui")
        
        for num_cmd in xrange( len(self.undo_list) ):
            command_list.append(
                "nebulagui.nebula_object.%s" % self.undo_list[num_cmd]
                )

        return ';'.join(command_list)

    def __get_redo_command (self):
        # Return the command to execute for redo. The command is a collection
        # of commands separated by ;
        
        cmd_count = len(self.cmd_name_list)
        
        # Start Redo command

        command = "import nebulagui;"
        for num_cmd in xrange(cmd_count):
        
            # Construct the command string to be called.
            # A variable will be use for storage the return value/s

            arg = self.cmd_name_list[num_cmd].rsplit('_')

            command = command + "a = nebulagui.nebula_object."            
            command = command + "%s(" % arg[1]

            i = 0
            for arg_list in self.cmd_list[num_cmd]:
                if arg[2][i] == 's':
                    command = command + "'%s'," % arg_list.GetValueAsString(0)
                elif arg[2][i] == 'o':
                    command = command + "lookup('%s')," % arg_list.GetValueAsString(0)
                else:
                    command = command + "%s," % arg_list.GetValueAsString(0)
                i = i + 1
            
            if i > 0:
                command = command[:-1]

            command = command + ");"

        return command

    def on_save_button (self, evt):
        # Send a enter key event to property for update values

        event =  wx.KeyEvent(wx.wxEVT_CHAR)
        event.m_keyCode = wx.WXK_RETURN  
        self.cmds_pg.GetEventHandler().ProcessEvent(event)        

        # make this object the current
        nebulagui.nebula_object = self.object

        # Get redo command               

        self.redo = self.__get_redo_command()

        # Get undo command

        self.undo = self.__get_undo_command()     

        # new command
        servers.get_command_server().newcommand(str(self.redo), str(self.undo))
        self.draw_state_commands(self.object)

        if self.object.hascommand("beginnewobjectentityclass"):
            servers.get_entity_class_server().setentityclassdirty(
                self.object, 
                True
                )
        elif self.object.hascommand("getid"):
            servers.get_entity_object_server().setentityobjectdirty(
                self.object, 
                True
                )
            
        if self.object.isa("nscenenode"):
            self.object.loadresources()
        
        self.object.setobjectdirty(True)

    def on_default_button(self, evt):
        j = 0
        for num_cmd in xrange(len(self.cmd_name_list)):
            for arg_list in self.cmd_list[num_cmd]:
                arg_list.SetValueFromString(
                    self.prev_state[j], 
                    0
                    )
                j = j + 1

        self.cmds_pg.Refresh()

        # When refresh, the selected property is not refreshed
        # For refresing it, collapse and Expand all items

        scroll_pos =  self.cmds_pg.GetScrollPos(wx.VERTICAL)
        self.cmds_pg.CollapseAll()
        self.cmds_pg.ExpandAll()
        self.cmds_pg.SetScrollPos(wx.VERTICAL, scroll_pos, True)
        self.cmds_pg.ScrollLines(scroll_pos)

    def on_expand_button (self, evt):
        self.cmds_pg.ExpandAll()

    def on_collpase_button (self, evt):
        self.cmds_pg.CollapseAll()

    def draw_state_commands(self, obj):
        self.cmds_pg.Clear()

        if obj is None:
            self.Disable()
            return
        else:
            self.Enable()

        # Get the commands

        inspector = pynebula.new(
                            "nobjinspector",
                            "/editor/inspector"
                            )
        inspector.initcmdsstate(obj)

        # Navigate over the commands and build the interface. We need
        # the command definition to put it in the category. So first create
        # the grid properties  but do not append them until the category
        # is placed with the full description.

        cmds_count = inspector.getcmdscount() 
        self.cmd_list = list()       # Command list
        self.cmd_name_list = list()  # Store the categories
        self.undo_list = list()

        if cmds_count == 0:
            self.cmds_pg.Append(
                pg.PropertyCategory(
                    str("State is EMPTY"), 
                    str("nostate")
                    )
                )

        self.prev_state = list() # Storage state when the inspector is call
        for num_cmd in xrange(cmds_count):
            cmds_name_desc = inspector.getcmdname(num_cmd) + str(" (")
            undo_cmd = inspector.getcmdname(num_cmd) + str("(")
            args_count = inspector.getinargscount(num_cmd)
            self.element = list()   # Start argument list            

            for num_arg in xrange(args_count):                
                label = "label_in" + str(num_cmd) + "-" + str(num_arg+1)
                arg_type = inspector.gettype(num_cmd, num_arg+1)

                # Type is Int

                if arg_type == 1:
                    value = inspector.geti(num_cmd, num_arg+1)
                    self.element.append(
                        pg.IntProperty(
                            'Integer', 
                            label
                            )
                        )
                    value_as_string = str(value)
                    self.element[num_arg].SetValueFromString(
                        value_as_string, 
                        0
                        )
                    self.prev_state.append(value_as_string)
                    cmds_name_desc = cmds_name_desc + value_as_string + ", "
                    undo_cmd = undo_cmd + value_as_string + ","

                # Type is Float

                elif arg_type == 2:
                    value = inspector.getf(num_cmd, num_arg+1)
                    self.element.append(
                        pg.FloatProperty(
                            'Float', 
                            label
                            )
                        )
                    value_as_string = str(value)    
                    self.element[num_arg].SetValueFromString(
                        value_as_string,
                        0
                        )
                    self.prev_state.append(value_as_string)
                    undo_cmd = undo_cmd + value_as_string + ","
                    temp = value_as_string.rsplit('.')
                    value = temp[0] + '.' + temp[1][:2]
                    cmds_name_desc = cmds_name_desc + value_as_string + ", "

                # Type if String

                elif arg_type == 3:
                    value = inspector.gets(num_cmd, num_arg+1)
                    self.element.append(
                        pg.StringProperty(
                            'String',
                            label, 
                            ''
                            )
                        )
                    value_as_string = str(value)
                    self.element[num_arg].SetValueFromString(
                        value_as_string,
                        0
                        )
                    self.prev_state.append(value_as_string)
                    cmds_name_desc = cmds_name_desc +\
                                   '"' + value_as_string + '"' + ", "
                    undo_cmd = undo_cmd + '"' + value_as_string + '"' + ","

                # Type is Boolean

                elif arg_type == 4:
                    value = inspector.getb(num_cmd, num_arg+1)
                    self.element.append(
                        pg.EnumProperty(
                            'Boolean', 
                            label,
                            ['True','False'],
                            [1,0],
                            2 
                            )
                        )
                    if value == 1:
                        self.element[num_arg].SetValueFromString(
                            str("True"), 
                            0
                            )
                        cmds_name_desc = cmds_name_desc + str('True') + ", "
                        self.prev_state.append('True')
                    else:
                        self.element[num_arg].SetValueFromString(
                            str("False"), 
                            0
                            )
                        cmds_name_desc = cmds_name_desc + str('False') + ", "
                        self.prev_state.append('False')
                    undo_cmd = undo_cmd + str(value) + ","

                # Type is Object

                elif arg_type == 5:
                    value = inspector.geto(num_cmd, num_arg+1)             
                    self.element.append(
                        pg.StringProperty(
                            'Object', 
                            label, 
                            ''
                            )
                        )
                    full_name_as_string = str( value.getfullname() )
                    self.element[num_arg].SetValueFromString(
                        full_name_as_string,
                        0
                        )
                    self.prev_state.append(full_name_as_string)
                    cmds_name_desc = cmds_name_desc +\
                                   str(value.getname()) + ", "
                    undo_cmd = undo_cmd + "lookup(" + \
                                            full_name_as_string + "),"

                # Type not implemented

                else:
                    cjr.show_error_message("Unknown type!!!")

            # Write the command definition into the category and attach it

            if args_count > 0:
                cmds_name_desc = cmds_name_desc[:-2] + ")"
                undo_cmd = undo_cmd[:-1] + ")"
            else:
                cmds_name_desc = cmds_name_desc + ")"
                undo_cmd = undo_cmd + ")"

            category = pg.PropertyCategory(
                                cmds_name_desc, 
                                "cmdLabel%d" % num_cmd
                                )
            self.cmds_pg.Append(category)
            self.cmd_name_list.append(
                inspector.getcmdprotoname(num_cmd)
                )

            # Show the arguments (append to grid) recently created 
            # and append them to a command list

            for num_arg in xrange(args_count):
                self.cmds_pg.Append(self.element[num_arg])
            
            self.cmd_list.append(self.element)

            self.undo_list.append(undo_cmd)

        pynebula.delete("/editor/inspector")

        # Make de object the current

        nebulagui.nebula_object = obj
        self.object = obj

        # Refresh control
        self.cmds_pg.Refresh()

    def refresh(self):
        self.draw_state_commands(self.object)
