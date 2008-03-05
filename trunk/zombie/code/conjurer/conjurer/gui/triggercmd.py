##\file triggercmd.py
##\brief Trigger command operation editor

import wx

import app
import cmdmgrdlg
import events
import propgrid
import servers
import conjurerconfig as cfg
import conjurerframework as cjr


# get_object_name function
def get_object_name( obj ):
    if obj.isa('nentityobject') and not obj.hascomponent('ncGameplay'):
        return str( obj.getid() ) + " (" + obj.getclass() + ")"
    return obj.getname()


# ObjectChooserDialog class
class ObjectChooserDialog(wx.Dialog):
    """Dialog to set an object for a command operation"""
    
    def __init__(self, parent, operation):
        wx.Dialog.__init__(self, parent, -1, "Choose an object/entity",
            style = wx.DEFAULT_DIALOG_STYLE|wx.RESIZE_BORDER|wx.TAB_TRAVERSAL)
        self.operation = operation
        self.current_obj = None
        
        # Global objects
        self.global_objects = [
            ("Game message window", servers.get_game_message_window_proxy()),
            ("Mission handler", servers.get_mission_handler()),
            ("Music handler", servers.get_music_table())
            ]
        
        self.sizer_global_staticbox = wx.StaticBox(self, -1, "Global objects")
        self.sizer_current_staticbox = wx.StaticBox(
                                                    self, 
                                                    -1, 
                                                    "Current selected entity"
                                                    )
        self.label_entity = wx.StaticText(self, -1, "Entity:")
        self.label_name = wx.StaticText(self, -1, "<none>")
        self.button_current = wx.Button(self, -1, "Choose")
        self.list_globals = wx.ListBox(self, -1, choices=[])
        self.button_global = wx.Button(self, -1, "Choose")
        self.button_cancel = wx.Button(self, -1, "Cancel")
        
        self.__set_properties()
        self.__do_layout()
        self.__bind_events()
    
    def __set_properties(self):
        # Get current selected object
        state = app.get_object_state()
        select_count = state.getselectioncount()
        if select_count > 0:
            self.current_obj = state.getselectedentity(0)
            # Show name of selected object
            self.label_name.SetLabel( get_object_name(self.current_obj) )
        
        # Fill global objects list
        for global_obj in self.global_objects:
            self.list_globals.Append( global_obj[0] )
        
        # Disable unaccessible controls at start up
        self.button_current.Enable( self.current_obj is not None )
        self.__update_sel_ctrls()
    
    def __do_layout(self):
        sizer_border = wx.BoxSizer(wx.VERTICAL)
        sizer_layout = wx.BoxSizer(wx.VERTICAL)
        sizer_global = wx.StaticBoxSizer(self.sizer_global_staticbox, wx.VERTICAL)
        sizer_current = wx.StaticBoxSizer(self.sizer_current_staticbox, wx.VERTICAL)
        sizer_name = wx.BoxSizer(wx.HORIZONTAL)
        sizer_name.Add(self.label_entity, 0, wx.ADJUST_MINSIZE)
        sizer_name.Add(self.label_name, 0, wx.LEFT|wx.ADJUST_MINSIZE, cfg.BORDER_WIDTH)
        sizer_current.Add(sizer_name, 0, wx.ALL|wx.EXPAND, cfg.BORDER_WIDTH)
        sizer_current.Add( (cfg.BORDER_WIDTH, cfg.BORDER_WIDTH) )
        sizer_current.Add(self.button_current, 0, wx.LEFT|wx.RIGHT|wx.BOTTOM|wx.EXPAND, cfg.BORDER_WIDTH)
        sizer_layout.Add(sizer_current, 0, wx.EXPAND)
        sizer_global.Add(self.list_globals, 1, wx.ALL|wx.EXPAND, cfg.BORDER_WIDTH)
        sizer_global.Add(self.button_global, 0, wx.LEFT|wx.RIGHT|wx.BOTTOM|wx.EXPAND, cfg.BORDER_WIDTH)
        sizer_layout.Add(sizer_global, 1, wx.TOP|wx.EXPAND, cfg.BORDER_WIDTH * 2)
        sizer_layout.Add(self.button_cancel, 0, wx.TOP|wx.EXPAND, cfg.BORDER_WIDTH * 2)
        sizer_border.Add(sizer_layout, 1, wx.ALL|wx.EXPAND, cfg.BORDER_WIDTH * 2)
        self.SetAutoLayout(True)
        self.SetSizer(sizer_border)
        sizer_border.Fit(self)
        sizer_border.SetSizeHints(self)
        self.Layout()
    
    def __bind_events(self):
        self.Bind(wx.EVT_LISTBOX, self.on_select_global, self.list_globals)
        self.Bind(wx.EVT_LISTBOX_DCLICK, self.on_choose_global, self.list_globals)
        self.Bind(wx.EVT_BUTTON, self.on_choose_current, self.button_current)
        self.Bind(wx.EVT_BUTTON, self.on_choose_global, self.button_global)
        self.Bind(wx.EVT_BUTTON, self.on_cancel, self.button_cancel)
    
    def __update_sel_ctrls(self):
        self.button_global.Enable(
            self.list_globals.GetSelection() != wx.NOT_FOUND
            )
    
    def __fill_operation(self, obj):
        # Check that the object has at least one command to execute
        cmds = obj.getcmds()
        if len( cmds ) == 0:
            cjr.show_warning_message("The object has no commands to execute.")
            return False
        
        # Select the first command as the default one
        cmd = cmds[0].split('_')[1]
        
        # Set the object and command
        self.operation.setobjectcommand( obj, str(cmd) )
        return True
    
    def on_select_global(self, event):
        self.__update_sel_ctrls()
    
    def on_choose_current(self, event):
        if self.__fill_operation( self.current_obj ):
            self.EndModal( wx.ID_OK )
    
    def on_choose_global(self, event):
        index = self.list_globals.GetSelection()
        if index != wx.NOT_FOUND:
            obj = self.global_objects[index][1]
            if self.__fill_operation( obj ):
                self.EndModal( wx.ID_OK )
    
    def on_cancel(self, event):
        self.EndModal( wx.ID_CANCEL )


# CommandOperationDialog class
class CommandOperationDialog(wx.Dialog):
    """Trigger command operation editor"""
    
    def __init__(self, parent, operation):
        wx.Dialog.__init__(self, parent, -1, "Choose a command",
            style = wx.DEFAULT_DIALOG_STYLE|wx.RESIZE_BORDER|wx.TAB_TRAVERSAL)
        self.operation = operation
        
        self.gameplay_cmds = {
            'ncAgentTrigger': ['setbehavior'],
            'ncSound': ['playsoundevent', 'stoplastsound'],
            'ncSpawner': ['spawnentity'],
            'ncSpawnPoint': ['spawnentity'],
            'ncTransform': ['setposition']
            }
        self.globals_cmds = {
            'nmusictable': [
                'sendmoodtoplay', 
                'sendstopmusic', 
                'sendplaymusicpart', 
                'sendplaymusicstinger', 
                'sendsetcurrentstyle'
                ]
            }
        self.music_table_cmd_inputs = self.__build_music_table_cmd_inputs()
        
        self.sizer_obj_staticbox = wx.StaticBox(self, -1, "Object/Entity")
        self.sizer_args_staticbox = wx.StaticBox(self, -1, "Command arguments")
        target_name = get_object_name( operation.gettargetobject() )
        self.label_obj_id = wx.StaticText(self, -1, target_name)
        self.check_filter = wx.CheckBox(self, -1, "Show only gameplay commands")
        self.label_cmd = wx.StaticText(self, -1, "Command")
        self.choice_cmd = wx.Choice(self, -1, choices=[])
        self.prop_args = propgrid.PropertyGrid(self)
        self.button_ok = wx.Button(self, -1, "&OK")
        
        self.__set_properties()
        self.__do_layout()
        self.__bind_events()
    
    def __set_properties(self):
        # Set the OK button as the default one
        self.button_ok.SetDefault()
        
        # Set correct object type label
        obj = self.operation.gettargetobject()
        if obj.isa('nentityobject'):
            self.sizer_obj_staticbox.SetLabel("Entity")
        elif obj.isa('nentityclass'):
            self.sizer_obj_staticbox.SetLabel("Class")
        else:
            self.sizer_obj_staticbox.SetLabel("Object")
        
        # Enable command filter by default
        self.check_filter.SetValue( True )
        
        # Fill the command list
        self.__update_cmd_list()
        
        # Fill the command arguments
        self.__update_cmd_args()
    
    def __do_layout(self):
        sizer_border = wx.BoxSizer(wx.VERTICAL)
        sizer_layout = wx.BoxSizer(wx.VERTICAL)
        sizer_buttons = wx.BoxSizer(wx.HORIZONTAL)
        sizer_args = wx.StaticBoxSizer(self.sizer_args_staticbox, wx.VERTICAL)
        sizer_cmd = wx.BoxSizer(wx.HORIZONTAL)
        sizer_obj = wx.StaticBoxSizer(self.sizer_obj_staticbox, wx.HORIZONTAL)
        sizer_obj.Add(
            self.label_obj_id, 
            0, 
            wx.LEFT|wx.BOTTOM|wx.ADJUST_MINSIZE, 
            cfg.BORDER_WIDTH
            )
        sizer_cmd.Add(
            self.label_cmd, 
            0, 
            wx.ALIGN_CENTER_VERTICAL | wx.ADJUST_MINSIZE
            )
        sizer_cmd.Add(
            self.choice_cmd, 
            0, 
            wx.LEFT|wx.ADJUST_MINSIZE, 
            cfg.BORDER_WIDTH
            )
        sizer_layout.Add(
            sizer_obj, 
            0, 
            wx.EXPAND| wx.BOTTOM, 
            cfg.BORDER_WIDTH
            )
        sizer_layout.Add(
            self.check_filter, 
            0, 
            wx.EXPAND| wx.TOP | wx.BOTTOM, 
            cfg.BORDER_WIDTH
            )
        sizer_layout.Add(sizer_cmd, 0, wx.EXPAND | wx.ALL, cfg.BORDER_WIDTH)
        sizer_args.Add(self.prop_args, 1, wx.ALL|wx.EXPAND, cfg.BORDER_WIDTH)
        sizer_layout.Add(sizer_args, 1, wx.TOP|wx.EXPAND, cfg.BORDER_WIDTH)
        sizer_buttons.Add(self.button_ok, 0, wx.ADJUST_MINSIZE)
        sizer_layout.Add(
            sizer_buttons, 
            0, 
            wx.TOP | wx.ALIGN_RIGHT, 
            cfg.BORDER_WIDTH
            )
        sizer_border.Add(
            sizer_layout, 
            1, 
            wx.ALL|wx.EXPAND, 
            cfg.BORDER_WIDTH
            )
        self.SetAutoLayout(True)
        self.SetSizer(sizer_border)
        sizer_border.Fit(self)
        sizer_border.SetSizeHints(self)
        self.Layout()
        self.SetSize((350, -1))
    
    def __bind_events(self):
        self.Bind(wx.EVT_CHOICE, self.on_select_cmd, self.choice_cmd)
        self.Bind(wx.EVT_CHECKBOX, self.on_toggle_filter, self.check_filter)
        self.Bind(events.EVT_CHANGED, self.on_change_args, self.prop_args)
        self.Bind(wx.EVT_BUTTON, self.on_ok, self.button_ok)
    
    def __build_music_table_cmd_inputs(self):
        # custom
        input_dictionary = dict()
        # sendmoodtoplay
        sendmoodtoplay_input_list = []
        sendmoodtoplay_input_list.append({
            'name': "Mood name",
            propgrid.Format_Type: propgrid.Type_String,
            propgrid.Format_Editor: None,
            propgrid.Format_Max: None,
            propgrid.Format_Min: None,
            propgrid.Format_Step: None,
            propgrid.Format_Subtype: 'musicmood',
            propgrid.Format_Flags: None,
            propgrid.Format_Enums: None
            })
        sendmoodtoplay_input_list.append({
           'name': "Loop?",
            propgrid.Format_Type: propgrid.Type_Bool,
            propgrid.Format_Editor: None,
            propgrid.Format_Max: None,
            propgrid.Format_Min: None,
            propgrid.Format_Step: None,
            propgrid.Format_Subtype: None,
            propgrid.Format_Flags: None,
            propgrid.Format_Enums: None
            })
        input_dictionary['sendmoodtoplay'] = sendmoodtoplay_input_list
        # sendplaymusicpart
        sendplaymusicpart_input_list = []
        sendplaymusicpart_input_list.append({
            'name': "Sample name",
            propgrid.Format_Type: propgrid.Type_String,
            propgrid.Format_Editor: None,
            propgrid.Format_Max: None,
            propgrid.Format_Min: None,
            propgrid.Format_Step: None,
            propgrid.Format_Subtype: 'musicsample',
            propgrid.Format_Flags: None,
            propgrid.Format_Enums: None
            })
        sendplaymusicpart_input_list.append({
           'name': "Player",
            propgrid.Format_Type: propgrid.Type_Object,
            propgrid.Format_Editor: None,
            propgrid.Format_Max: None,
            propgrid.Format_Min: None,
            propgrid.Format_Step: None,
            propgrid.Format_Subtype: None,
            propgrid.Format_Flags: None,
            propgrid.Format_Enums: None
            })
        input_dictionary['sendplaymusicpart'] = sendplaymusicpart_input_list
        # sendplaymusicstinger
        sendplaymusicstinger_input_list = []
        sendplaymusicstinger_input_list.append({
            'name': "Sample name",
            propgrid.Format_Type: propgrid.Type_String,
            propgrid.Format_Editor: None,
            propgrid.Format_Max: None,
            propgrid.Format_Min: None,
            propgrid.Format_Step: None,
            propgrid.Format_Subtype: 'musicsample',
            propgrid.Format_Flags: None,
            propgrid.Format_Enums: None
            })
        sendplaymusicstinger_input_list.append({
            'name': "Volume",
            propgrid.Format_Type: propgrid.Type_Float,
            propgrid.Format_Editor: None,
            propgrid.Format_Max: None,
            propgrid.Format_Min: None,
            propgrid.Format_Step: None,
            propgrid.Format_Subtype: None,
            propgrid.Format_Flags: None,
            propgrid.Format_Enums: None
            })
        sendplaymusicstinger_input_list.append({
           'name': "Player",
            propgrid.Format_Type: propgrid.Type_Object,
            propgrid.Format_Editor: None,
            propgrid.Format_Max: None,
            propgrid.Format_Min: None,
            propgrid.Format_Step: None,
            propgrid.Format_Subtype: None,
            propgrid.Format_Flags: None,
            propgrid.Format_Enums: None
            })
        input_dictionary['sendplaymusicstinger'] = sendplaymusicstinger_input_list
        # sendsetcurrentstyle
        sendsetcurrentstyle_input_list = []
        sendsetcurrentstyle_input_list.append({
            'name': "Style name",
            propgrid.Format_Type: propgrid.Type_String,
            propgrid.Format_Editor: None,
            propgrid.Format_Max: None,
            propgrid.Format_Min: None,
            propgrid.Format_Step: None,
            propgrid.Format_Subtype: 'musicstyle',
            propgrid.Format_Flags: None,
            propgrid.Format_Enums: None
            })
        sendsetcurrentstyle_input_list.append({
            'name': "Intensity",
            propgrid.Format_Type: propgrid.Type_Int,
            propgrid.Format_Editor: None,
            propgrid.Format_Max: None,
            propgrid.Format_Min: None,
            propgrid.Format_Step: None,
            propgrid.Format_Subtype: None,
            propgrid.Format_Flags: None,
            propgrid.Format_Enums: None
            })
        sendsetcurrentstyle_input_list.append({
           'name': "Player",
            propgrid.Format_Type: propgrid.Type_Object,
            propgrid.Format_Editor: None,
            propgrid.Format_Max: None,
            propgrid.Format_Min: None,
            propgrid.Format_Step: None,
            propgrid.Format_Subtype: None,
            propgrid.Format_Flags: None,
            propgrid.Format_Enums: None
            })
        sendsetcurrentstyle_input_list.append({
           'name': "Loop?",
            propgrid.Format_Type: propgrid.Type_Bool,
            propgrid.Format_Editor: None,
            propgrid.Format_Max: None,
            propgrid.Format_Min: None,
            propgrid.Format_Step: None,
            propgrid.Format_Subtype: None,
            propgrid.Format_Flags: None,
            propgrid.Format_Enums: None
            })
        input_dictionary['sendsetcurrentstyle'] = sendsetcurrentstyle_input_list
        # sendsetstopmusic
        sendsetstopmusic_input_list = []
        sendsetstopmusic_input_list.append({
           'name': "Player",
            propgrid.Format_Type: propgrid.Type_Object,
            propgrid.Format_Editor: None,
            propgrid.Format_Max: None,
            propgrid.Format_Min: None,
            propgrid.Format_Step: None,
            propgrid.Format_Subtype: None,
            propgrid.Format_Flags: None,
            propgrid.Format_Enums: None
            })
        input_dictionary['sendsetstopmusic'] = sendsetstopmusic_input_list
        # return the dictionary we've built
        return input_dictionary
    
    def __update_cmd_list(self):
        # Get commands list
        obj = self.operation.gettargetobject()
        if self.check_filter.IsChecked():
            clazz = obj.getclass()
            cmd_protos = cmdmgrdlg.get_class_commands( clazz, True )
            if obj.isa('nentityobject'):
                for component in self.gameplay_cmds.keys():
                    if obj.hascomponent( component ):
                        cmd_protos.extend( self.gameplay_cmds[component] )
            else:
                for object in self.globals_cmds.keys():
                    if obj.isa( object ):
                        cmd_protos.extend( self.globals_cmds[object] )
        else:
            cmd_protos = obj.getcmds()
        cmds = []
        for proto in cmd_protos:
            if proto.find('_') == -1:
                cmds.append( proto )
            else:
                cmds.append( proto.split('_')[1] )

        # Fill commands choice list, discarding filtered commands
        cmds.sort()
        self.choice_cmd.Clear()
        self.choice_cmd.AppendItems( cmds )

        # Select current command, if not filtered out
        cmd = self.operation.gettargetcommand()
        index = self.choice_cmd.FindString( cmd )
        if index != wx.NOT_FOUND:
            self.choice_cmd.SetSelection( index )
    
    def __update_cmd_args(self):
        # Fill arguments
        self.prop_args.clear()
        in_format = self.__get_inputs_format()
        i = 0
        self.prop_args.begin()
        for format in in_format:
            name = format['name']
            pid = self.prop_args.append_property( None, name, format )
            type = format['type']
            if type == propgrid.Type_Bool:
                value = bool( self.operation.getboolarg(i) )
            elif type == propgrid.Type_Float:
                value = self.operation.getfloatarg(i)
            elif type == propgrid.Type_Int:
                value = self.operation.getintarg(i)
            elif type == propgrid.Type_Object:
                value = None
            elif type == propgrid.Type_String:
                value = self.operation.getstringarg(i)
            else:
                raise TypeError, "Unsupported type '%s' for property '%s'"  % (type, name)
            self.prop_args.set_property_value( pid, value )
            i = i + len(type)
        self.prop_args.end()
    
    def __get_inputs_format(self):
        # TODO: Replace by using the xml format when available for all commands
        
        # Get the input types prototype
        obj = self.operation.gettargetobject()
        cmd = self.operation.gettargetcommand()
        obj_class_name = obj.getclass()
        (found_ok, custom_list) = self.__check_for_custom_inputs(obj_class_name, cmd)
        if found_ok:
            return custom_list
        in_types = 'v'
        cmd_protos = cmdmgrdlg.get_class_commands(obj_class_name, True)
        cmd_protos.extend( obj.getcmds() )
        # look through the list of class and object commands for one matching
        # the target command name - if you find it, break out of the loop, get 
        # the inputs and return them as a list of format objects
        for proto in cmd_protos:
            if proto.split('_')[1] == cmd:
                in_types = proto.split('_')[2]
                break
        # Create basic format for input types, like the one used for xml format
        list_of_format_objects = []
        if in_types != 'v':
            for type in in_types:
                if type == propgrid.Type_Bool:
                    name = 'Boolean'
                elif type == propgrid.Type_Float:
                    name = 'Float'
                elif type == propgrid.Type_Int:
                    name = 'Integer'
                elif type == propgrid.Type_Object:
                    name = 'Object'
                elif type == propgrid.Type_String:
                    name = 'String'
                else:
                    name = 'Unknown'
                list_of_format_objects.append( {
                    'name': name,
                    propgrid.Format_Type: type,
                    propgrid.Format_Editor: None,
                    propgrid.Format_Max: None,
                    propgrid.Format_Min: None,
                    propgrid.Format_Step: None,
                    propgrid.Format_Subtype: None,
                    propgrid.Format_Flags: None,
                    propgrid.Format_Enums: None
                    } )
        return list_of_format_objects

    def __check_for_custom_inputs(self, class_name, cmd_name):
        if class_name == "nmusictable":
            try:
                input_list = self.music_table_cmd_inputs[cmd_name]
                return (True, input_list)
            except:
                return (False, [] )
        else:
            return ( False, [] )

    def on_select_cmd(self, event):
        cmd = self.choice_cmd.GetStringSelection()
        if cmd != "":
            obj = self.operation.gettargetobject()
            self.operation.setobjectcommand( obj, str(cmd) )
            self.__update_cmd_args()
    
    def on_toggle_filter(self, event):
        self.__update_cmd_list()
        self.__update_cmd_args()
    
    def on_change_args(self, event):
        i = 0
        pid = self.prop_args.get_first_child_of_category(None)
        while pid is not None:
            value = self.prop_args.get_property_value(pid)
            type = self.prop_args.get_property_type(pid)
            if type == propgrid.Type_Bool:
                self.operation.setboolarg( i, value )
            elif type == propgrid.Type_Float:
                self.operation.setfloatarg( i, value )
            elif type == propgrid.Type_Int:
                self.operation.setintarg( i, value )
            elif type == propgrid.Type_Object:
                pass
            elif type == propgrid.Type_String:
                self.operation.setstringarg( i, str(value) )
            else:
                name = self.prop_args.get_property_name(pid)
                raise TypeError, "Unsupported type '%s' for property '%s'" % (type, name)
            i = i + len(type)
            pid = self.prop_args.get_next_sibling(pid)
    
    def on_ok(self, event):
        self.EndModal( wx.ID_OK )
 