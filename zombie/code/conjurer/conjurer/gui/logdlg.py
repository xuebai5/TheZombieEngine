##\file logdlg.py
##\brief Log inspector dialog

import wx
import wx.lib.intctrl as intctrl
import wx.lib.scrolledpanel as scrolledpanel

import app
import servers
import togwin
import loggrpdlg
import conjurerconfig as cfg

Num_Heading_Elements = 2

# ModEnablePanel class
class ModEnablePanel(scrolledpanel.ScrolledPanel):
    """Panel with module log enable controls"""
    
    def __init__(self, parent, mod_list, style):
        scrolledpanel.ScrolledPanel.__init__(self, parent, style=style)
        self.mod_list = mod_list
        self.log_server = servers.get_log_server()
        # module enable controls
        self.check_box_mod_list = []
        self.int_ctrl_mod_list = []
        self.log_group_mod_list = []
        for mod in mod_list:
            check_box = wx.CheckBox(
                                self, 
                                -1, 
                                self.log_server.getdescription(mod), 
                                name=mod
                                )
            colour = get_colour_for_log_with_index(
                            self.mod_list.index(mod)
                            )
            check_box.SetForegroundColour(colour)
            self.check_box_mod_list.append(check_box)
            int_ctrl = intctrl.IntCtrl(
                            parent=self, limited=True, value=0, min=0, max=99,
                            size=(25, -1), style=wx.TE_RIGHT, name=mod
                            )
            self.int_ctrl_mod_list.append(int_ctrl)
            log_group_button = wx.Button(
                                            self, 
                                            -1, 
                                            "Edit...", 
                                            style=wx.BU_EXACTFIT, 
                                            name = mod
                                            )
            self.log_group_mod_list.append(log_group_button)

        self.set_properties()
        self.__do_layout()
        self.__bind_events()

    def set_properties(self):
        self.SetScrollRate(10, 10)
        for check_box in self.check_box_mod_list:
            mod_name = str( check_box.GetName() )
            check_box.SetValue( self.log_server.getlogenable(mod_name) )
        for int_ctrl in self.int_ctrl_mod_list:
            mod_name = str( int_ctrl.GetName() )
            int_ctrl.SetValue( self.log_server.getwarninglevel(mod_name) )
        for log_group in self.log_group_mod_list:
            mod_name = str( log_group.GetName() )
            num_groups = self.log_server.getnumgroups(mod_name)
            # enable if this module has groups
            log_group.Enable(num_groups)

    def __do_layout(self):
        outer_sizer = wx.BoxSizer(wx.HORIZONTAL)
        grid_sizer_mods = wx.FlexGridSizer(
                                    (len(self.mod_list) + Num_Heading_Elements),
                                    3, 
                                    cfg.BORDER_WIDTH, 
                                    cfg.BORDER_WIDTH * 2 
                                    )
        log_class_heading = wx.StaticText(self, -1, 'Log Class') 
        font = log_class_heading.GetFont()
        font.SetWeight(wx.FONTWEIGHT_BOLD)
        log_class_heading.SetFont(font)
        level_heading = wx.StaticText(self, -1, 'Level') 
        level_heading.SetFont(font)
        groups_heading = wx.StaticText(self, -1, 'Groups') 
        groups_heading.SetFont(font)
        grid_sizer_mods.Add(log_class_heading)
        grid_sizer_mods.Add(level_heading)
        grid_sizer_mods.Add(groups_heading)
        grid_sizer_mods.Add(
            wx.StaticLine(self, -1, ( -1, -1 ), ( -1, -1 ), wx.LI_HORIZONTAL ),
            0, 
            wx.EXPAND 
            )
        grid_sizer_mods.Add(
            wx.StaticLine( self, -1, ( -1, -1 ), ( -1, -1 ), wx.LI_HORIZONTAL ),
            0, 
            wx.EXPAND 
            )
        grid_sizer_mods.Add(
            wx.StaticLine( self, -1, ( -1, -1 ), ( -1, -1 ), wx.LI_HORIZONTAL ),
            0, 
            wx.EXPAND
            )

        for i in range( len(self.mod_list) ):
            grid_sizer_mods.Add(
                self.check_box_mod_list[i],
                flag=wx.ALIGN_CENTER_VERTICAL
                )
            grid_sizer_mods.Add(
                self.int_ctrl_mod_list[i], 
                flag=wx.ALIGN_CENTER_VERTICAL
                )
            grid_sizer_mods.Add(
                self.log_group_mod_list[i], 
                flag=wx.ALIGN_CENTER_VERTICAL
                )
        outer_sizer.Add(grid_sizer_mods, 0, wx.ALL|wx.EXPAND, cfg.BORDER_WIDTH)
        self.SetSizer(outer_sizer)
        outer_sizer.SetSizeHints(self)
        self.Layout()
    
    def __bind_events(self):
        for check_box in self.check_box_mod_list:
            self.Bind(wx.EVT_CHECKBOX, self.__on_check_box, check_box)
        for int_ctrl in self.int_ctrl_mod_list:
            self.Bind(intctrl.EVT_INT, self.__on_int_ctrl, int_ctrl)
        for log_group_button in self.log_group_mod_list:
            self.Bind(
                wx.EVT_BUTTON, 
                self.__on_click_log_group_button, 
                log_group_button
                )

    # When toggling log enable flag of some module
    def __on_check_box(self, event):
        check_box = event.GetEventObject()
        mod_name = str( check_box.GetName() )
        self.log_server.setlogenable( mod_name, check_box.IsChecked() )

    # When changing the warning level of some module's logs
    def __on_int_ctrl(self, event):
        int_ctrl = event.GetEventObject()
        mod_name = str( int_ctrl.GetName() )
        self.log_server.setwarninglevel( mod_name, int_ctrl.GetValue() )

    def __on_click_log_group_button (self, event):
        log_group_button = event.GetEventObject()
        log_class_name = str( log_group_button.GetName() )
        if log_class_name:
            self.open_log_group_editor(log_class_name)

    def persist(self):
        data = {}
        for mod in self.mod_list:
            data[mod] = (
                                self.log_server.getlogenable(mod),
                                self.log_server.getwarninglevel(mod) 
                                )
        return data

    def restore(self, data):
        # Restore modules
        for index in range( len(self.mod_list) ):
            mod = self.mod_list[index]
            if data.has_key(mod):
                self.log_server.setlogenable( mod, data[mod][0] )
                self.log_server.setwarninglevel( mod, data[mod][1] )
                self.check_box_mod_list[index].SetValue( data[mod][0] )
                self.int_ctrl_mod_list[index].SetValue( data[mod][1] )

    def open_log_group_editor(self, log_class_name):
        win = loggrpdlg.create_window(
                    app.get_top_window(), 
                    log_class_name
                    )
        win.display()


class LogOutputPanel(wx.Panel):
    def __init__(self, parent, log_class_list, style):
        wx.Panel.__init__(self, parent, style=style)
        self.log_class_list = log_class_list
        self.log_server = servers.get_log_server()
        # log update timer
        self.timer_log = wx.Timer(self)
        # controls
        self.list_ctrl_logs = wx.ListCtrl(
                                    self, 
                                    -1, 
                                    style = wx.LC_REPORT | wx.LC_NO_HEADER
                                    )
        self.list_ctrl_logs.InsertColumn(0, 'Log')
        self.button_pause = wx.ToggleButton(self, -1, "&Pause")
        self.button_clear = wx.Button(self, -1, "Clea&r")

        self.__do_layout()
        self.__bind_events()

        # Start the log update timer
        self.timer_log.Start(250)

    def __do_layout(self):
        sizer_log_and_buttons = wx.BoxSizer (wx.VERTICAL)
        sizer_log = wx.BoxSizer(wx.HORIZONTAL)
        sizer_buttons = wx.BoxSizer(wx.HORIZONTAL)
        sizer_log.Add(
            self.list_ctrl_logs, 
            1, 
            wx.EXPAND|wx.ALL, 
            cfg.BORDER_WIDTH
            )
        sizer_buttons.Add(self.button_pause, 0)
        sizer_buttons.Add(self.button_clear, 0, wx.LEFT, cfg.BORDER_WIDTH)
        sizer_log_and_buttons.Add(sizer_log, 1, wx.EXPAND)
        horizontal_line = wx.StaticLine(
                                self, 
                                -1, 
                                ( -1, -1 ), 
                                ( -1, -1 ), 
                                wx.LI_HORIZONTAL
                                )
        sizer_log_and_buttons.Add(
            horizontal_line, 
            0, 
            wx.EXPAND|wx.LEFT|wx.RIGHT, 
            cfg.BORDER_WIDTH
            )
        sizer_log_and_buttons.Add(
            sizer_buttons, 
            0, 
            wx.ALL|wx.ALIGN_RIGHT, 
            cfg.BORDER_WIDTH
            )
        self.SetSizer(sizer_log_and_buttons)

    def __bind_events(self):
        self.Bind(wx.EVT_TIMER, self.__on_update_log, self.timer_log)
        self.Bind(wx.EVT_TOGGLEBUTTON, self.__on_pause, self.button_pause)
        self.Bind(wx.EVT_BUTTON, self.__on_clear, self.button_clear)

    # When the log update timer triggers gather the latest log records
    def __on_update_log(self, event):
        update_scroll = True
        if self.list_ctrl_logs.GetItemCount() > 0:
            id_top = self.list_ctrl_logs.GetTopItem()
            id_bottom_max = id_top + self.list_ctrl_logs.GetCountPerPage()
            if id_bottom_max < self.list_ctrl_logs.GetItemCount():
                update_scroll = False

        loglist = self.log_server.getlastlogs()
        for log in loglist:
            item = wx.ListItem()
            item.SetId( self.list_ctrl_logs.GetItemCount() )
            item.SetText(log)
            log_class = log.split(':')[0]
            item.SetTextColour(
                get_colour_for_log_with_index(
                    self.log_class_list.index(log_class) 
                    )
                )
            self.list_ctrl_logs.InsertItem(item)

        if len(loglist) > 0 and update_scroll:
            self.list_ctrl_logs.SetColumnWidth(0, wx.LIST_AUTOSIZE)
            self.list_ctrl_logs.EnsureVisible(
                self.list_ctrl_logs.GetItemCount() - 1
                )

    # When stopping/restarting the log gathering
    def __on_pause(self, event):
        if self.button_pause.GetValue():
            self.timer_log.Stop()
        else:
            self.timer_log.Start()

    # When erasing all gathered log entries
    def __on_clear(self, event):
        self.list_ctrl_logs.DeleteAllItems()

    # When destroying the window
    def on_close(self, event):
        # Stop the timer or otherwise it will try to access a non existent
        # function
        self.timer_log.Stop()
        self.timer_log = None
        self.Destroy()
        return True


# LogInspector class
class LogInspector(togwin.ChildToggableDialog):
    """Dialog that shows the nebula log history"""
    def __init__(self, parent):
        togwin.ChildToggableDialog.__init__(
            self, 
            "Log inspector", 
            parent
            )
        self.log_server = servers.get_log_server()
        log_class_list = list ( self.log_server.getlogclasses() )
        class_and_description_list = []
        for each_class in log_class_list:
            class_and_description_list.append(
                ( each_class, self.log_server.getdescription(each_class) )
                )
        # ignore case when sorting on description
        class_and_description_list.sort(
            lambda x, y: cmp( x[1].lower(), y[1].lower() ) 
            )
        sorted_log_class_list = []
        for each_tuple in class_and_description_list:
            sorted_log_class_list.append(each_tuple[0])
        self.log_output_panel = LogOutputPanel(
                                            self, 
                                            sorted_log_class_list,
                                            style = wx.BORDER_SUNKEN
                                            )
        win = wx.SashLayoutWindow(
                    self, 
                    -1 
                    )
        self.log_class_panel = ModEnablePanel(
                                        win,
                                        sorted_log_class_list,
                                        style=wx.BORDER_SUNKEN|wx.TAB_TRAVERSAL
                                        )
        self.log_class_sash = win

        self.__do_layout()
        self.__bind_events()

    def __do_layout(self):
        # set the ideal width, allowing a bit extra for the scroll bar
        ideal_width = self.log_class_panel.GetBestSize()[0] + 20
        self.log_class_sash.SetDefaultSize( (ideal_width, -1) )
        self.log_class_sash.SetMaxSize( (ideal_width, -1) )
        self.log_class_sash.SetMinimumSizeX(50)
        self.log_class_sash.SetOrientation(wx.LAYOUT_VERTICAL)
        self.log_class_sash.SetAlignment(wx.LAYOUT_RIGHT)
        self.log_class_sash.SetSashVisible(wx.SASH_LEFT, True)
        self.SetAutoLayout(True)
        self.SetSize((700, 400))
        self.Layout()

    def __bind_events(self):
        self.Bind(
            wx.EVT_SASH_DRAGGED_RANGE, 
            self.__on_sash_drag, 
            self.log_class_sash
            )
        self.Bind(
            wx.EVT_SIZE, 
            self.__on_size
            )

    def __on_sash_drag(self, event):
        if event.GetDragStatus() == wx.SASH_STATUS_OUT_OF_RANGE:
            return
        new_width = event.GetDragRect().width
        max_width = self.log_class_sash.GetMaxSize()[0]
        self.log_class_sash.SetDefaultSize(
            (
                min(new_width, max_width),
                -1
                )
            )
        self.__layout_window()

    def __on_size(self, event):
        self.__layout_window()
        event.Skip()

    def __layout_window(self):
        wx.LayoutAlgorithm().LayoutWindow(self, self.log_output_panel)

    def persist(self):
        data = {
            'modules': self.log_class_panel.persist()
            }
        return [
            create_window,
            (), #no creation parameters
            data
            ]

    def restore(self, data_list):
        data = data_list[0]
        if data.has_key('modules'):
            self.log_class_panel.restore( data['modules'] )


# Return the colour assigned to the module of the given log
def get_colour_for_log_with_index(index):
    return Colour_List[
        index % len(Colour_List)
        ]

# color list to assign colors to modules
Colour_List = [
                    (0,0,0), (127,0,0), (0,127,0), (0,0,159),
                    (127,95,0), (0,127,95), (95,0,127),
                    (63,63,63),
                    (63,0,0), (0,63,0), (0,0,79)
                    ]


# create_window function
def create_window(parent):
    return LogInspector(parent)
