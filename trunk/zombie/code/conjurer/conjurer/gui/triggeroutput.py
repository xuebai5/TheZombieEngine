##\file triggeroutput.py
##\brief Control used to set output operations to a trigger

import wx

import events
import floatctrl
import servers
import triggercmd
import triggerlib
import conjurerconfig as cfg


# TriggerOutputCtrl class
class TriggerOutputCtrl(wx.PyControl):
    """Control used to set output operations to a trigger"""
    
    def __init__(self, parent, event, entity):
        wx.PyControl.__init__(self, parent, style=wx.NO_BORDER|wx.TAB_TRAVERSAL)
        self.entity = entity
        self.event = str(event)
        event_id = servers.get_trigger_server().geteventtransientid( event )
        event_label = servers.get_trigger_server().geteventlabel( event_id )
        event_label = "On " + event_label.lower()
        self.staticbox_operations = wx.StaticBox(self, -1, event_label)
        self.list_operations = wx.ListBox(self, -1)
        self.button_add_cmd = wx.Button(self, -1, "Add cmd")
        self.button_add_script = wx.Button(self, -1, "Add script")
        self.button_remove = wx.Button(self, -1, "Remove")
        self.button_edit = wx.Button(self, -1, "Edit")
        self.label_execute = wx.StaticText(self, -1, "Execute")
        self.choice_execute = wx.Choice(
                                        self, 
                                        -1, 
                                        choices=['All', 'One', 'Some']
                                        )
        self.label_min_delay = wx.StaticText(self, -1, "Minimum delay")
        self.float_min_delay = floatctrl.FloatCtrl(
                                        self, 
                                        size=wx.Size(50,-1), 
                                        style=wx.TE_RIGHT 
                                        )
        self.label_max_delay = wx.StaticText(self, -1, "Maximum delay")
        self.float_max_delay = floatctrl.FloatCtrl(
                                        self, 
                                        size=wx.Size(50,-1), 
                                        style=wx.TE_RIGHT 
                                        )

        self.__set_properties()
        self.__do_layout()
        self.__bind_events()
    
    def __set_properties(self):
        self.__update_sel_ctrls()
        # Build operation type popup menu
        self.ID_Command = wx.NewId()
        self.ID_Script = wx.NewId()
        self.operation_type_menu = wx.Menu()
        self.operation_type_menu.Append( self.ID_Command, "Command" )
        self.operation_type_menu.Append( self.ID_Script, "Script" )
    
    def __do_layout(self):
        border_width = cfg.BORDER_WIDTH
        sizer_border = wx.StaticBoxSizer(self.staticbox_operations, wx.VERTICAL)
        sizer_layout = wx.BoxSizer(wx.VERTICAL)
        sizer_layout.Add(self.list_operations, 1, wx.EXPAND)
        sizer_buttons = wx.BoxSizer(wx.HORIZONTAL)
        sizer_buttons.Add(self.button_add_cmd, 0, wx.ADJUST_MINSIZE)
        sizer_buttons.Add(
            self.button_add_script, 
            0, 
            wx.ADJUST_MINSIZE|wx.LEFT, 
            border_width
            )
        sizer_buttons.Add(
            self.button_remove, 
            0, 
            wx.ADJUST_MINSIZE|wx.LEFT, 
            border_width
            )
        sizer_buttons.Add(
            self.button_edit, 
            0, 
            wx.ADJUST_MINSIZE|wx.LEFT, 
            border_width
            )
        sizer_layout.Add(
            sizer_buttons, 
            0, 
            wx.TOP|wx.BOTTOM|wx.ALIGN_RIGHT, 
            border_width
            )
        grid_sizer = wx.FlexGridSizer(
                            3, 
                            2, 
                            cfg.BORDER_WIDTH, 
                            cfg.BORDER_WIDTH * 2
                            )
        grid_sizer.Add(
            self.label_execute, 
            0, 
            cfg.LABEL_ALIGN | wx.ALIGN_CENTER_VERTICAL | wx.ADJUST_MINSIZE
            )
        grid_sizer.Add(
            self.choice_execute, 
            0, 
            wx.ADJUST_MINSIZE | wx.EXPAND
            )
        grid_sizer.Add(
            self.label_min_delay, 
            0, 
            cfg.LABEL_ALIGN | wx.ALIGN_CENTER_VERTICAL | wx.ADJUST_MINSIZE
            )
        grid_sizer.Add(
            self.float_min_delay, 
            0, 
            wx.ADJUST_MINSIZE | wx.EXPAND
            )
        grid_sizer.Add(
            self.label_max_delay, 
            0, 
            cfg.LABEL_ALIGN | wx.ALIGN_CENTER_VERTICAL | wx.ADJUST_MINSIZE
            )
        grid_sizer.Add(
            self.float_max_delay, 
            0, 
            wx.ADJUST_MINSIZE | wx.EXPAND
            )
        sizer_layout.Add(
            grid_sizer, 
            0, 
            wx.LEFT | wx.RIGHT | wx.TOP | wx.EXPAND, 
            cfg.BORDER_WIDTH
            )
        sizer_border.Add(sizer_layout, 1, wx.ALL|wx.EXPAND, cfg.BORDER_WIDTH)
        self.SetAutoLayout(True)
        self.SetSizer(sizer_border)
        sizer_border.Fit(self)
        sizer_border.SetSizeHints(self)
        self.Layout()
    
    def __bind_events(self):
        self.Bind(wx.EVT_SIZE, self.__on_size)
        self.Bind(wx.EVT_LISTBOX, self.__on_select, self.list_operations)
        self.Bind(wx.EVT_LISTBOX_DCLICK, self.__on_edit, self.list_operations)
        self.Bind(wx.EVT_BUTTON, self.__on_add_cmd, self.button_add_cmd)
        self.Bind(wx.EVT_BUTTON, self.__on_add_script, self.button_add_script)
        self.Bind(wx.EVT_BUTTON, self.__on_remove, self.button_remove)
        self.Bind(wx.EVT_BUTTON, self.__on_edit, self.button_edit)
        self.Bind(wx.EVT_CHOICE, self.__on_change_config, self.choice_execute)
        self.Bind(
            events.EVT_CHANGING, 
            self.__on_change_config, 
            self.float_min_delay
            )
        self.Bind(
            events.EVT_CHANGED, 
            self.__on_change_config, 
            self.float_min_delay
            )
        self.Bind(
            events.EVT_CHANGING, 
            self.__on_change_config, 
            self.float_max_delay
            )
        self.Bind(
            events.EVT_CHANGED, 
            self.__on_change_config, 
            self.float_max_delay
            )
    
    def __on_size(self, event):
        # Needed to resize controls along with the window owning this control
        self.Layout()
        event.Skip()
    
    def __on_select(self, event):
        self.__update_sel_ctrls()
    
    def __on_add_cmd(self, event):
        # Add an empty command operation
        operation = self.entity.newcommandoperation( self.event )
        index = self.entity.getoperationsnumber( self.event ) - 1
        
        # Choose the target object and default command
        dlg = triggercmd.ObjectChooserDialog( self, operation )
        result_ok = dlg.ShowModal() == wx.ID_OK
        dlg.Destroy()
        if not result_ok:
            self.entity.deleteoperation( self.event, index )
            return
        
        # Let the user adjust the default command
        dlg = triggercmd.CommandOperationDialog( self, operation )
        dlg.ShowModal()
        dlg.Destroy()
        
        self.__mark_entity_as_dirty()
        
        # Show operation in GUI
        self.list_operations.Append( self.__get_op_brief(index) )
    
    def __mark_entity_as_dirty(self):
        servers.get_entity_object_server().setentityobjectdirty(
            self.entity, 
            True 
            )
    
    def __on_add_script(self, event):
        # Add script operation
        dlg = ScriptDialog( self, None )
        if dlg.ShowModal() == wx.ID_OK:
            script = dlg.get_script()
            if script != "":
                self.entity.newscriptoperation( self.event, str(script) )
                self.__mark_entity_as_dirty()
                # Show operation in GUI
                index = self.entity.getoperationsnumber( self.event ) - 1
                self.list_operations.Append( self.__get_op_brief(index) )
        dlg.Destroy()
    
    def __on_remove(self, event):
        index = self.list_operations.GetSelection()
        if index != wx.NOT_FOUND:
            self.entity.deleteoperation( self.event, index )
            self.__mark_entity_as_dirty()
            self.__update_operations_list()
    
    def __on_edit(self, event):
        index = self.list_operations.GetSelection()
        if index != wx.NOT_FOUND:
            operation = self.entity.getoperation( self.event, index )
            op_type = operation.gettypelabel()
            if op_type == 'Command':
                self.__edit_cmd( index, operation )
            elif op_type == 'Script':
                self.__edit_script( index, operation )
    
    def __edit_cmd(self, index, operation):
        dlg = triggercmd.CommandOperationDialog( self, operation )
        dlg.ShowModal()
        self.__mark_entity_as_dirty()
        self.list_operations.SetString( index, self.__get_op_brief(index) )
        dlg.Destroy()
    
    def __edit_script(self, index, operation):
        script = operation.getname()
        dlg = ScriptDialog( self, script )
        if dlg.ShowModal() == wx.ID_OK:
            script = dlg.get_script()
            if script != "":
                self.entity.setscriptoperation( self.event, index, str(script) )
                self.__mark_entity_as_dirty()
                self.list_operations.SetString( 
                    index, 
                    self.__get_op_brief(index)
                    )
        dlg.Destroy()
    
    def __on_change_config(self, event):
        if not self.getting_values:
            mode = self.choice_execute.GetSelection()
            min_value = self.float_min_delay.get_value()
            max_value = self.float_max_delay.get_value()
            self.entity.setoutputsetconfig(
                self.event, 
                min_value, 
                max_value, 
                mode 
                )
            self.__mark_entity_as_dirty()
    
    def __get_op_brief(self, op_index):
        """
        Generate a brief descriptor of an output operation.
        
        Format for each type:
        
          - Command: object_id/entity_name.cmd_name
          - Script: script_name
          - Error: Unknown
        """
        oper = self.entity.getoperation( self.event, op_index )
        op_type = oper.gettypelabel()
        if op_type == 'Command':
            brief = ".".join(
                        [
                        triggercmd.get_object_name( oper.gettargetobject() ), 
                        oper.gettargetcommand() 
                        ]
                        )
        elif op_type == 'Script':
            brief = oper.getname()
        else:
            brief = "Unknown"
        return brief
    
    def __update_sel_ctrls(self):
        enable = self.list_operations.GetSelection() != wx.NOT_FOUND
        self.button_remove.Enable( enable )
        self.button_edit.Enable( enable )
    
    def __update_operations_list(self):
        self.list_operations.Clear()
        self.__update_sel_ctrls()
        if self.entity is None:
            return
        
        # Output operations
        for i in range( self.entity.getoperationsnumber(self.event) ):
            self.list_operations.Append( self.__get_op_brief(i) )
    
    def set_entity(self, entity):
        self.entity = entity
        self.getting_values = True
        
        # Output operations
        self.__update_operations_list()
        
        # Execution config
        min_value, max_value, mode = self.entity.getoutputsetconfig(
                                                    self.event 
                                                    )
        self.choice_execute.SetSelection( mode )
        self.float_min_delay.set_value( min_value )
        self.float_max_delay.set_value( max_value )
        
        self.getting_values = False


# ScriptDialog class
class ScriptDialog(wx.SingleChoiceDialog):
    """Dialog to define a script operation"""
    def __init__(self, parent, default_script):
        scripts = triggerlib.get_script_operations()
        scripts.sort()
        wx.SingleChoiceDialog.__init__(self, parent,
            "Select a trigger script", "Script operation", scripts)
        if default_script is not None:
            try:
                self.SetSelection( scripts.index(default_script) )
            except:
                pass
    
    def get_script(self):
        return self.GetStringSelection()
