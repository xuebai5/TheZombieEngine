##\file warcherdlg.py
##\brief Variable and command watcher dialog

import wx
import wx.lib.intctrl

import pynebula

import intctrl
import servers
import togwin


# Variable to store results of commands executed by the watcher
cmd_result = 0


# WatcherDialog class
class WatcherDialog(togwin.ChildToggableDialog):
    """Dialog to view nebula variable values and script command results"""
    
    def __init__(self, parent):
        togwin.ChildToggableDialog.__init__(
            self, "Variable and command watcher", parent
            )
        
        # controls
        self.cmds_sizer_staticbox = wx.StaticBox(self, -1, "Command watches")
        self.refresh_sizer_staticbox = wx.StaticBox(self, -1, "Refresh settings")
        self.vars_sizer_staticbox = wx.StaticBox(self, -1, "Variable watches")
        self.list_vars = wx.ListCtrl(self, -1,
            style=wx.LC_REPORT|wx.LC_HRULES|wx.LC_VRULES| \
                  wx.LC_SORT_ASCENDING|wx.SUNKEN_BORDER)
        self.label_filter = wx.StaticText(self, -1, "Filter:")
        self.text_filter = wx.TextCtrl(self, -1, "")
        self.list_cmds = wx.ListCtrl(self, -1,
            style=wx.LC_REPORT|wx.LC_EDIT_LABELS|wx.LC_HRULES|wx.LC_VRULES| \
                  wx.SUNKEN_BORDER)
        self.button_add_cmd = wx.Button(self, -1, "Add command watch")
        self.button_del_cmd = wx.Button(self, -1, "Delete command watch")
        self.check_refresh = wx.CheckBox(self, -1, "Refresh all watches each")
        self.int_refresh = intctrl.IntCtrl(
            self, size=(50,-1), style=wx.TE_RIGHT
            )
        self.label_milliseconds = wx.StaticText(self, -1, "milliseconds")
        self.button_refresh = wx.Button(self, -1, "&Refresh all watches now")
        
        # refresh timer
        self.timer_refresh = wx.PyTimer(self.__refresh_values)
        
        self.__set_properties()
        self.__do_layout()
        self.__bind_events()
    
    def __del__(self):
        self.timer_refresh.Stop()
        self.timer_refresh = None
    
    def __set_properties(self):
        # list controls columns
        self.list_vars.InsertColumn(0, 'Name')
        self.list_vars.InsertColumn(1, 'Value')
        self.list_cmds.InsertColumn(0, 'Command')
        self.list_cmds.InsertColumn(1, 'Result')
        
        # default values
        data = {
            'column widths': (150, 50, 150, 50),
            'filter': "",
            'autorefresh enabled': True,
            'autorefresh time': 1000,
            'commands': []
            }
        self.restore([data])
    
    def __do_layout(self):
        main_sizer = wx.BoxSizer(wx.VERTICAL)
        refresh_sizer = wx.StaticBoxSizer(self.refresh_sizer_staticbox, wx.VERTICAL)
        refresh_timer_sizer = wx.BoxSizer(wx.HORIZONTAL)
        cmds_sizer = wx.StaticBoxSizer(self.cmds_sizer_staticbox, wx.VERTICAL)
        cmd_buttons_sizer = wx.BoxSizer(wx.HORIZONTAL)
        vars_sizer = wx.StaticBoxSizer(self.vars_sizer_staticbox, wx.VERTICAL)
        vars_filter_sizer = wx.BoxSizer(wx.HORIZONTAL)
        vars_sizer.Add(self.list_vars, 1, wx.ALL|wx.EXPAND, 5)
        vars_filter_sizer.Add(self.label_filter, 0, wx.RIGHT|wx.ALIGN_CENTER_VERTICAL|wx.FIXED_MINSIZE, 5)
        vars_filter_sizer.Add(self.text_filter, 1, wx.ALIGN_CENTER_VERTICAL|wx.FIXED_MINSIZE, 0)
        vars_sizer.Add(vars_filter_sizer, 0, wx.LEFT|wx.RIGHT|wx.BOTTOM|wx.EXPAND, 5)
        main_sizer.Add(vars_sizer, 1, wx.ALL|wx.EXPAND, 10)
        cmds_sizer.Add(self.list_cmds, 1, wx.ALL|wx.EXPAND, 5)
        cmd_buttons_sizer.Add(self.button_add_cmd, 1, wx.RIGHT|wx.FIXED_MINSIZE, 5)
        cmd_buttons_sizer.Add(self.button_del_cmd, 1, wx.FIXED_MINSIZE, 0)
        cmds_sizer.Add(cmd_buttons_sizer, 0, wx.LEFT|wx.RIGHT|wx.BOTTOM|wx.EXPAND, 5)
        main_sizer.Add(cmds_sizer, 0, wx.LEFT|wx.RIGHT|wx.BOTTOM|wx.EXPAND, 10)
        refresh_timer_sizer.Add(self.check_refresh, 0, wx.ALIGN_CENTER_VERTICAL|wx.FIXED_MINSIZE, 0)
        refresh_timer_sizer.Add(self.int_refresh, 0, wx.LEFT|wx.RIGHT|wx.ALIGN_CENTER_VERTICAL|wx.FIXED_MINSIZE, 5)
        refresh_timer_sizer.Add(self.label_milliseconds, 0, wx.ALIGN_CENTER_VERTICAL|wx.FIXED_MINSIZE, 0)
        refresh_sizer.Add(refresh_timer_sizer, 0, wx.ALL|wx.EXPAND, 5)
        refresh_sizer.Add(self.button_refresh, 0, wx.LEFT|wx.RIGHT|wx.BOTTOM|wx.EXPAND, 5)
        main_sizer.Add(refresh_sizer, 0, wx.LEFT|wx.RIGHT|wx.BOTTOM|wx.EXPAND, 10)
        self.SetAutoLayout(True)
        self.SetSizer(main_sizer)
        main_sizer.Fit(self)
        main_sizer.SetSizeHints(self)
        self.Layout()
    
    def __bind_events(self):
        self.Bind(wx.EVT_TEXT, self.on_filter, self.text_filter)
        self.Bind(wx.EVT_BUTTON, self.on_add_cmd, self.button_add_cmd)
        self.Bind(wx.EVT_BUTTON, self.on_del_cmd, self.button_del_cmd)
        self.Bind(wx.EVT_CHECKBOX, self.on_check_refresh, self.check_refresh)
        self.Bind(wx.lib.intctrl.EVT_INT, self.on_refresh_time, self.int_refresh)
        self.Bind(wx.EVT_BUTTON, self.on_refresh, self.button_refresh)
    
    def on_filter(self, event):
        top_item = self.list_vars.GetTopItem()
        self.__set_vars()
        self.list_vars.EnsureVisible(top_item)
    
    def on_add_cmd(self, event):
        dlg = wx.TextEntryDialog(self, "Enter a python command:", "Conjurer")
        if dlg.ShowModal() == wx.ID_OK:
            i = self.list_cmds.InsertStringItem(
                self.list_cmds.GetItemCount(), dlg.GetValue())
            self.__execute_cmd(i)
        dlg.Destroy()
    
    def on_del_cmd(self, event):
        i = -1
        while True:
            i = self.list_cmds.GetNextItem(i, wx.LIST_NEXT_ALL,
                wx.LIST_STATE_SELECTED)
            if i == -1:
                break
            else:
                self.list_cmds.DeleteItem(i)
                i = i - 1
    
    def on_check_refresh(self, event):
        self.__update_refresh_timer()
    
    def on_refresh_time(self, event):
        self.__update_refresh_timer()
    
    def on_refresh(self, event):
        self.__refresh_values()
    
    def __set_vars(self):
        self.list_vars.DeleteAllItems()
        vars = pynebula.lookup("/sys/var")
        var = vars.gethead()
        while var != None:
            name = var.getname()
            if name.find( self.text_filter.GetValue() ) != -1:
                self.list_vars.InsertStringItem(0, name)
            var = var.getsucc()
        self.__update_var_values()
    
    def __update_var_values(self):
        for i in range(self.list_vars.GetItemCount()):
            var_name = self.list_vars.GetItemText(i)
            var = pynebula.lookup("/sys/var/" + var_name)
            type = var.gettype()
            if type == 'int':
                value = str(var.geti())
            elif type == 'float':
                value = str(round(var.getf(),6))
            elif type == 'string':
                value = str(var.gets())
            elif type == 'bool':
                value = str(var.getb())
            elif type == 'float4':
                v = var.getf4()
                v = ( round(v[0],6), round(v[1],6), round(v[2],6),
                    round(v[3],6) )
                value = str(v)
            else:
                value = 'Unknown type'
            self.list_vars.SetStringItem(i, 1, value)
    
    def __set_cmds(self, cmd_list):
        cmd_list.reverse()
        for cmd in cmd_list:
            i = self.list_cmds.InsertStringItem(0, cmd)
        self.__update_cmd_results()
    
    def __update_cmd_results(self):
        for i in range(self.list_cmds.GetItemCount()):
            self.__execute_cmd(i)
    
    def __execute_cmd(self, cmd_index):
        cmd = "import watcherdlg;"
        cmd = cmd + "watcherdlg.cmd_result = "
        cmd = cmd + self.list_cmds.GetItemText(cmd_index)
        servers.get_python_server().run(str(cmd))
        self.list_cmds.SetStringItem(cmd_index, 1, str(cmd_result))
    
    def __update_refresh_timer(self):
        if self.check_refresh.IsChecked():
            self.timer_refresh.Start( self.int_refresh.get_value() )
        else:
            self.timer_refresh.Stop()
    
    def __refresh_values(self):
        self.__update_var_values()
        self.__update_cmd_results()
    
    def persist(self):
        # commands
        cmd_list = []
        for i in range(self.list_cmds.GetItemCount()):
            cmd_list.append( self.list_cmds.GetItemText(i) )
        
        # persist data
        data = {
            'column widths': (
                self.list_vars.GetColumnWidth(0),
                self.list_vars.GetColumnWidth(1),
                self.list_cmds.GetColumnWidth(0),
                self.list_cmds.GetColumnWidth(1)
                ),
            'filter': self.text_filter.GetValue(),
            'autorefresh enabled': self.check_refresh.GetValue(),
            'autorefresh time': self.int_refresh.get_value(),
            'commands': cmd_list
            }
        
        return [
            create_window, 
            (),  # no parameters for create function
            data]
    
    def restore(self, data_list):
        data = data_list[0]
        
        # all controls but lists
        self.list_vars.SetColumnWidth(0, data['column widths'][0])
        self.list_vars.SetColumnWidth(1, data['column widths'][1])
        self.list_cmds.SetColumnWidth(0, data['column widths'][2])
        self.list_cmds.SetColumnWidth(1, data['column widths'][3])
        self.text_filter.SetValue( data['filter'] )
        self.check_refresh.SetValue( data['autorefresh enabled'] )
        self.int_refresh.set_value( data['autorefresh time'] )
        
        # variables
        self.__set_vars()
        
        # commands
        self.__set_cmds(data['commands'])
        
        # refresh timer
        self.__update_refresh_timer()


# create_window function
def create_window(parent):
    return WatcherDialog(parent)
