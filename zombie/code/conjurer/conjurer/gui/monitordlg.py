##\file monitordlg.py
##\brief Dialog to manage the graphs displayed by the monitor server

import wx
import wx.lib.colourselect
import wx.lib.intctrl

import pynebula

import events
import floatslider
import format
import intctrl
import servers
import togwin


# GraphBar class
class GraphBar(wx.PyControl):
    """Control to edit the settings of a graph"""
    
    def __init__(self, parent, var_name, parent_graphs):
        wx.PyControl.__init__(self, parent, style=wx.NO_BORDER|wx.TAB_TRAVERSAL)
        self.var_name = var_name
        self.parent_graphs = parent_graphs
        parent_graphs.append(self)
        
        # controls
        self.checkbox_enable = wx.CheckBox(self, -1, var_name)
        self.color = wx.lib.colourselect.ColourSelect(self, -1)
        self.label_frequency = wx.StaticText(self, -1, "Sampling frequency:")
        self.int_frequency = intctrl.IntCtrl(
            self, value=1000, min=1, max=100000, limited=True,
            size=wx.Size(40,-1), style=wx.TE_RIGHT
            )
        self.label_milliseconds = wx.StaticText(self, -1, "milliseconds")
        self.slider_smooth = floatslider.FloatSlider(
            self, -1, "Smoothness:", 0.5, 0.0, 1.0, precision=1000.0
            )
        self.button_remove = wx.Button(self, -1, "Remove", style=wx.BU_EXACTFIT)
        
        self.__set_properties()
        self.__do_layout()
        self.__bind_events()
    
    def __set_properties(self):
        # Get graph parameters from the monitor server
        graph_index = self.get_graph_index()
        monitor = servers.get_monitor_server()
        frequency, smooth, r,g,b,a = monitor.getgraphparameters( graph_index )
        enabled = monitor.getgraphactive( graph_index )
        # Fill controls with the graph parameters
        self.checkbox_enable.SetValue( enabled )
        self.color.SetValue( format.unit_rgb_2_byte_rgb((r,g,b)) )
        self.int_frequency.set_value( int(frequency*1000) )
        self.slider_smooth.set_value( smooth )
    
    def __do_layout(self):
        sizer_layout = wx.BoxSizer(wx.HORIZONTAL)
        sizer_layout.Add(self.checkbox_enable, 1, wx.ALIGN_CENTER_VERTICAL|wx.FIXED_MINSIZE, 0)
        sizer_layout.Add(self.color, 0, wx.LEFT|wx.ALIGN_CENTER_VERTICAL|wx.FIXED_MINSIZE, 5)
        sizer_layout.Add(self.label_frequency, 0, wx.LEFT|wx.ALIGN_CENTER_VERTICAL|wx.FIXED_MINSIZE, 10)
        sizer_layout.Add(self.int_frequency, 0, wx.LEFT|wx.ALIGN_CENTER_VERTICAL|wx.FIXED_MINSIZE, 5)
        sizer_layout.Add(self.label_milliseconds, 0, wx.LEFT|wx.ALIGN_CENTER_VERTICAL|wx.FIXED_MINSIZE, 5)
        sizer_layout.Add(self.slider_smooth, 0, wx.LEFT|wx.ALIGN_CENTER_VERTICAL|wx.FIXED_MINSIZE, 10)
        sizer_layout.Add(self.button_remove, 0, wx.LEFT|wx.ALIGN_CENTER_VERTICAL|wx.FIXED_MINSIZE, 10)
        self.SetSizerAndFit(sizer_layout)

    def __bind_events(self):
        self.Bind(wx.EVT_SIZE, self.__on_size)
        self.Bind(wx.EVT_CHECKBOX, self.__on_toggle_enable, self.checkbox_enable)
        self.Bind(wx.lib.colourselect.EVT_COLOURSELECT, self.__on_change_params, self.color)
        self.Bind(wx.lib.intctrl.EVT_INT, self.__on_change_params, self.int_frequency)
        self.Bind(events.EVT_CHANGING, self.__on_change_params, self.slider_smooth)
        self.Bind(events.EVT_CHANGED, self.__on_change_params, self.slider_smooth)
    
    def __on_size(self, event):
        # Needed to resize controls along with the window owning this control
        self.Layout()
        event.Skip()
    
    def __on_toggle_enable(self, event):
        graph_index = self.get_graph_index()
        servers.get_monitor_server().setgraphactive( graph_index,
            event.IsChecked() )
    
    def __on_change_params(self, event):
        graph_index = self.get_graph_index()
        color = format.byte_rgb_2_unit_rgba( self.color.GetColour().Get() )
        servers.get_monitor_server().setgraphparameters(
            graph_index,
            float( self.int_frequency.get_value() ) / 1000.0,
            self.slider_smooth.get_value(),
            color[0], color[1], color[2], color[3]
            )
    
    def get_variable_name(self):
        return self.var_name
    
    def get_graph_index(self):
        return self.parent_graphs.index(self)


# MonitorDialog class
class MonitorDialog(togwin.ChildToggableDialog):
    """Dialog to manage the graphs displayed by the graph server"""
    
    def __init__(self, parent):
        togwin.ChildToggableDialog.__init__(
            self, "Variable monitor", parent
            )
        self.stats_enabled = False
        
        self.win_graphs = wx.ScrolledWindow(self, -1, style=wx.SUNKEN_BORDER|wx.TAB_TRAVERSAL)
        self.label_current = wx.StaticText(self, -1, "Current graph stats:")
        self.combo_current = wx.ComboBox(self, -1, style=wx.CB_READONLY)
        self.label_min = wx.StaticText(self, -1, "Min:")
        self.float_min = wx.StaticText(self, -1, "", size=(50,-1))
        self.label_max = wx.StaticText(self, -1, "Max:")
        self.float_max = wx.StaticText(self, -1, "", size=(50,-1))
        self.label_mean = wx.StaticText(self, -1, "Mean:")
        self.float_mean = wx.StaticText(self, -1, "", size=(50,-1))
        self.label_last = wx.StaticText(self, -1, "Last:")
        self.float_last = wx.StaticText(self, -1, "", size=(50,-1))
        self.line1 = wx.StaticLine(self, -1, style=wx.LI_HORIZONTAL)
        self.label_var = wx.StaticText(self, -1, "Variable to monitorize:")
        self.choice_var = wx.ComboBox(self, -1, style=wx.CB_READONLY|wx.CB_SORT)
        self.button_add = wx.Button(self, -1, "Add variable")
        self.line2 = wx.StaticLine(self, -1, style=wx.LI_HORIZONTAL)
        self.label_period = wx.StaticText(self, -1, "Global history period:")
        self.int_period = intctrl.IntCtrl(
            self, value=10, min=1, max=99, limited=True,
            size=wx.Size(40,-1), style=wx.TE_RIGHT
            )
        self.label_seconds = wx.StaticText(self, -1, "seconds")
        self.line3 = wx.StaticLine(self, -1, style=wx.LI_VERTICAL)
        self.button_show = wx.ToggleButton(self, -1, "Show graphs")
        self.button_fullscreen = wx.ToggleButton(self, -1, "Fullscreen")
        self.graphs = []
        
        self.__set_properties()
        self.__do_layout()
        self.__bind_events()
        
        # Add current graph found in the monitor server
        monitor_server = servers.get_monitor_server()
        for i in range( monitor_server.getnumgraphs() ):
            self.__add_graph_bar( monitor_server.getgraphvarname(i) )
        
        # Timer to update the current graph stats
        self.stats_timer = wx.PyTimer(self.update_current_stats)
        self.stats_timer.Start(50)
    
    def __set_properties(self):
        self.SetSize((700, 300))
        self.win_graphs.SetScrollRate(10, 10)
        
        # Enable sampling
        servers.get_monitor_server().setsamplingactivated(True)
        servers.get_monitor_server().setdisplayactivated(True)
        
        # Fill the variable choice control with all variables
        var = pynebula.lookup('/sys/var').gethead()
        while var != None:
            name = var.getname()
            self.choice_var.Append(name)
            var = var.getsucc()
        
        # Global history period
        monitor_server = servers.get_monitor_server()
        self.int_period.set_value( int(monitor_server.gethistoryperiod()) )
        
        # Check/uncheck display buttons
        self.button_show.SetValue( monitor_server.getdisplayactivated() )
        self.button_fullscreen.SetValue( monitor_server.getfullscreen() )
    
    def __do_layout(self):
        # Scrolled graph bars window layout
        sizer_graphs_border = wx.BoxSizer(wx.VERTICAL)
        self.sizer_graphs = wx.BoxSizer(wx.VERTICAL)
        sizer_graphs_border.Add(self.sizer_graphs, 1, wx.ALL|wx.EXPAND, 10)
        self.win_graphs.SetSizer(sizer_graphs_border)
        
        # This windows layout
        sizer_layout = wx.BoxSizer(wx.VERTICAL)
        sizer_global = wx.BoxSizer(wx.HORIZONTAL)
        sizer_current = wx.BoxSizer(wx.HORIZONTAL)
        sizer_var = wx.BoxSizer(wx.HORIZONTAL)
        sizer_layout.Add(self.win_graphs, 1, wx.ALL|wx.EXPAND, 10)
        sizer_var.Add(self.label_var, 0, wx.ALIGN_CENTER_VERTICAL|wx.FIXED_MINSIZE, 0)
        sizer_var.Add(self.choice_var, 1, wx.LEFT|wx.ALIGN_CENTER_VERTICAL|wx.FIXED_MINSIZE, 5)
        sizer_var.Add(self.button_add, 0, wx.LEFT|wx.ALIGN_CENTER_VERTICAL|wx.FIXED_MINSIZE, 5)
        sizer_layout.Add(sizer_var, 0, wx.LEFT|wx.RIGHT|wx.BOTTOM|wx.EXPAND, 10)
        sizer_layout.Add(self.line1, 0, wx.LEFT|wx.RIGHT|wx.BOTTOM|wx.EXPAND, 10)
        sizer_current.Add(self.label_current, 0, wx.ALIGN_CENTER_VERTICAL|wx.FIXED_MINSIZE, 0)
        sizer_current.Add(self.combo_current, 1, wx.LEFT|wx.ALIGN_CENTER_VERTICAL|wx.FIXED_MINSIZE, 5)
        sizer_current.Add(self.label_min, 0, wx.LEFT|wx.ALIGN_CENTER_VERTICAL|wx.FIXED_MINSIZE, 10)
        sizer_current.Add(self.float_min, 0, wx.LEFT|wx.ALIGN_CENTER_VERTICAL|wx.FIXED_MINSIZE, 5)
        sizer_current.Add(self.label_max, 0, wx.LEFT|wx.ALIGN_CENTER_VERTICAL|wx.FIXED_MINSIZE, 10)
        sizer_current.Add(self.float_max, 0, wx.LEFT|wx.ALIGN_CENTER_VERTICAL|wx.FIXED_MINSIZE, 5)
        sizer_current.Add(self.label_mean, 0, wx.LEFT|wx.ALIGN_CENTER_VERTICAL|wx.FIXED_MINSIZE, 10)
        sizer_current.Add(self.float_mean, 0, wx.LEFT|wx.ALIGN_CENTER_VERTICAL|wx.FIXED_MINSIZE, 5)
        sizer_current.Add(self.label_last, 0, wx.LEFT|wx.ALIGN_CENTER_VERTICAL|wx.FIXED_MINSIZE, 10)
        sizer_current.Add(self.float_last, 0, wx.LEFT|wx.ALIGN_CENTER_VERTICAL|wx.FIXED_MINSIZE, 5)
        sizer_layout.Add(sizer_current, 0, wx.LEFT|wx.RIGHT|wx.BOTTOM|wx.EXPAND, 10)
        sizer_layout.Add(self.line2, 0, wx.LEFT|wx.RIGHT|wx.BOTTOM|wx.EXPAND, 10)
        sizer_global.Add(self.label_period, 0, wx.ALIGN_CENTER_VERTICAL|wx.FIXED_MINSIZE, 0)
        sizer_global.Add(self.int_period, 0, wx.LEFT|wx.ALIGN_CENTER_VERTICAL|wx.FIXED_MINSIZE, 5)
        sizer_global.Add(self.label_seconds, 0, wx.LEFT|wx.ALIGN_CENTER_VERTICAL|wx.FIXED_MINSIZE, 5)
        sizer_global.Add(self.line3, 0, wx.LEFT|wx.RIGHT|wx.ALIGN_CENTER_VERTICAL|wx.EXPAND, 10)
        sizer_global.Add(self.button_show, 1, wx.ALIGN_CENTER_VERTICAL|wx.FIXED_MINSIZE, 0)
        sizer_global.Add(self.button_fullscreen, 1, wx.LEFT|wx.ALIGN_CENTER_VERTICAL|wx.FIXED_MINSIZE, 5)
        sizer_layout.Add(sizer_global, 0, wx.LEFT|wx.RIGHT|wx.BOTTOM|wx.EXPAND, 10)
        self.SetSizer(sizer_layout)
    
    def __bind_events(self):
        self.Bind(wx.EVT_BUTTON, self.on_add_variable, self.button_add)
        self.Bind(wx.EVT_COMBOBOX, self.on_choose_graph, self.combo_current)
        self.Bind(events.EVT_CHANGING, self.on_change_period, self.int_period)
        self.Bind(events.EVT_CHANGED, self.on_change_period, self.int_period)
        self.Bind(wx.EVT_TOGGLEBUTTON, self.on_toggle_show, self.button_show)
        self.Bind(wx.EVT_TOGGLEBUTTON, self.on_toggle_fullscreen, self.button_fullscreen)
    
    def __del__(self):
        # Disable sampling
        servers.get_monitor_server().setdisplayactivated(False)
        servers.get_monitor_server().setsamplingactivated(False)
        # Release refresh stats timer
        if self.stats_timer is not None:
            self.stats_timer.Stop()
            self.stats_timer = None
    
    def __add_graph_bar(self, var_name):
        # Add graph to the stats selector combo control
        self.combo_current.Append( var_name )
            
        # Add a graph bar control
        self.Freeze()
        graphbar = GraphBar(self.win_graphs, var_name, self.graphs)
        self.sizer_graphs.Add(graphbar, 0, wx.EXPAND, 0)
        self.win_graphs.GetSizer().Fit(self.win_graphs)
        self.win_graphs.GetSizer().SetVirtualSizeHints(self.win_graphs)
        self.Layout()
        self.Thaw()
        
        # Catch remove graph event for the new graph bar
        self.Bind(wx.EVT_BUTTON, self.on_remove_variable, graphbar.button_remove)
    
    def on_add_variable(self, event):
        var_name = self.choice_var.GetStringSelection()
        if var_name != "":
            # Add a graph with default values to the monitor server
            servers.get_monitor_server().addgraph(
                str(var_name), 0.1, 0.0, 1.0,0.0,0.0,1.0 )
            # Add graph bar for the new graph
            self.__add_graph_bar(var_name)
    
    def on_remove_variable(self, event):
        # Remove graph from the stats selector combo control
        graphbar = event.GetEventObject().GetParent()
        graph_index = graphbar.get_graph_index()
        self.combo_current.Delete( graph_index )
        if self.combo_current.GetCount() == 0:
            self.stats_enabled = False
        
        # Remove graph from the monitor server
        servers.get_monitor_server().removegraph( graph_index )
        
        # Stop catching the remove graph event for the removed graph
        self.Unbind(wx.EVT_BUTTON, graphbar)
        
        # Remove graph bar control
        self.Freeze()
        self.graphs.remove(graphbar)
        self.sizer_graphs.Detach(graphbar)
        graphbar.Destroy()
        graphbar = None
        self.win_graphs.GetSizer().Fit(self.win_graphs)
        self.win_graphs.GetSizer().SetVirtualSizeHints(self.win_graphs)
        self.Layout()
        self.Thaw()

    def on_choose_graph(self, event):
        if self.combo_current.GetSelection() != wx.NOT_FOUND:
            servers.get_monitor_server().setcurrentgraph(
                self.combo_current.GetSelection() )
            self.stats_enabled = True

    def on_change_period(self, event):
        servers.get_monitor_server().sethistoryperiod(
            float( self.int_period.get_value() ) )

    def on_toggle_show(self, event):
        servers.get_monitor_server().setdisplayactivated(
            self.button_show.GetValue() )

    def on_toggle_fullscreen(self, event):
        servers.get_monitor_server().setfullscreen(
            self.button_fullscreen.GetValue() )

    def update_current_stats(self):
        monitor_server = servers.get_monitor_server()
        if monitor_server.getnumgraphs() > 0 and self.stats_enabled:
            min, max, mean, last = monitor_server.getcurrentgraphstats()
            self.float_min.SetLabel( str(round(min, 4)) )
            self.float_max.SetLabel( str(round(max, 4)) )
            self.float_mean.SetLabel( str(round(mean, 4)) )
            self.float_last.SetLabel( str(round(last, 4)) )

    def persist(self):
        return [
            create_window,
            ()  #no parameters for create function
            ]


# create_window function
def create_window(parent):
    return MonitorDialog(parent)
