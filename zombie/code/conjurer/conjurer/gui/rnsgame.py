import wx

import servers
import app
import pynebula
import freedialoggui
import floatslider
import events

import conjurerconfig as cfg

class RnsGamePanel(wx.Panel):
    def __init__(self, parent):
        wx.Panel.__init__(self, parent, -1)
        
        self.gameport = "Orochimaru"
        self.number_connections = 3
        
        self.multiplayer = False
        self.server = False
        self.searching = False
        self.network = None
        
        self.singlecheck = wx.RadioButton(
                                    self, 
                                    -1, 
                                    "Singleplayer",
                                    style = wx.RB_GROUP 
                                    )
        self.servercheck = wx.RadioButton( 
                                    self,
                                    -1, 
                                    "Multiplayer Server" 
                                    )
        self.clientcheck = wx.RadioButton( 
                                    self, 
                                    -1, 
                                    "Multiplayer Client" 
                                    )
        
        self.button_search = wx.Button( self, -1, "Start Search" )
        self.button_search.Disable()
        self.button_connect_to = wx.Button(
                                        self, 
                                        -1, 
                                        "Connect to -> ", 
                                        style = wx.BU_RIGHT
                                        )
        self.button_connect_to.Disable()
        self.hostedit = wx.TextCtrl( self, -1, "Host" )
        self.hostedit.SetValue( "127.0.0.1" )
        self.hostedit.Disable()

        self.connectbtn = []
        for i in range( self.number_connections ):
            self.connectbtn.append(
                wx.Button(
                    self,
                    -1, 
                    "Connect to < empty >"  
                    )
                )
            self.connectbtn[ i ].Disable()
            
        self.button_cancel = wx.Button( self, -1, "Cancel Search" )
        self.button_cancel.Disable()
        self.button_start = wx.Button( self, -1, "Start" )
        
        self.serverbox = wx.StaticBox( self, -1, "Normal Options" )
        self.clientbox = wx.StaticBox( self, -1, "Client options" )
        
        self.mousebox = wx.StaticBox( self, -1, "Mouse options" )
        state = app.get_state( "game" )

        self.invertbtn = wx.CheckBox( self, -1, "Invert mouse vertical" )
        
        self.invertbtn.SetValue( state.getmouseyinvert() )
        
        self.mousexslider = floatslider.FloatSlider(
            self, -1, "Sensitivity X", 1.0, 0.0, 5.0, precision=10.0
            )
        self.mousexslider.set_value( state.getmousexfactor() )
        self.mouseyslider = floatslider.FloatSlider(
            self, -1, "Sensitivity Y", 1.0, 0.0, 5.0, precision=10.0
            )
        self.mouseyslider.set_value( state.getmouseyfactor() )
        
        self.fullreloadslider = floatslider.FloatSlider(
            self, -1, "Full Reload Time", 1.0, 0.0, 5.0, precision=10.0
            )
        self.fullreloadslider.set_value( state.getfullreloadtime() )
        
        self.crossmodelabel = wx.StaticText( self, -1, "Cross Mode" )
        self.crossmodelist = wx.ComboBox( 
                                        self,
                                        -1, 
                                        "Cross Mode", 
                                        style=wx.CB_READONLY, 
                                        choices=["Never","Normal","Always"]
                                        )
        
        self.crossmodelist.Select( state.getcrossmode() )

        self.timer = wx.Timer(self)
        self.timer.Start( 1000 )

        self.__do_layout()
        self.__do_binding()
        
        self.parent = parent
        
        if pynebula.exists( "/sys/servers/network" ):
            self.singlecheck.Disable()
            self.servercheck.Disable()
            self.clientcheck.Disable()
            self.button_start.Disable()
    
    def __del__( self ):
        if ( self.network is not None ) and self.searching :
            pynebula.delete( "/sys/servers/network" )
            self.network = None

    def __do_layout( self ):
        sizer = wx.BoxSizer(wx.VERTICAL)
        group_normal_options = wx.StaticBoxSizer(
                                            self.serverbox, 
                                            wx.VERTICAL
                                            )
        horizontal_sizer = wx.BoxSizer(wx.HORIZONTAL)
        horizontal_sizer.Add(
            self.singlecheck, 
            0,
            wx.EXPAND|wx.ALL,
            cfg.BORDER_WIDTH
            )
        horizontal_sizer.Add(
            self.servercheck, 
            0,
            wx.EXPAND|wx.ALL, 
            cfg.BORDER_WIDTH
            )
        group_normal_options.Add(
            horizontal_sizer, 
            0, 
            wx.FIXED_MINSIZE 
            )
        sizer_start = wx.BoxSizer(wx.HORIZONTAL)
        sizer_start.Add( 
            self.button_start,
            1, 
            wx.EXPAND|wx.ALL, 
            cfg.BORDER_WIDTH
            )
        group_normal_options.Add(
            sizer_start,
            0, 
            wx.EXPAND 
            )
        sizer.Add(
            group_normal_options, 
            0, 
            wx.EXPAND|wx.ALL,
            cfg.BORDER_WIDTH 
            )
        
        group_client_options = wx.StaticBoxSizer(
                                            self.clientbox, 
                                            wx.VERTICAL
                                            )
        horizontal_sizer = wx.BoxSizer(wx.HORIZONTAL)
        horizontal_sizer.Add(
            self.clientcheck, 
            0, 
            wx.FIXED_MINSIZE|wx.ALL, 
            cfg.BORDER_WIDTH 
            )
        group_client_options.Add(
            horizontal_sizer,
            0, 
            wx.FIXED_MINSIZE 
            )
        horizontal_sizer = wx.BoxSizer(wx.HORIZONTAL)
        horizontal_sizer.Add(
            self.button_connect_to, 
            1, 
            wx.EXPAND
            )
        horizontal_sizer.Add(
            self.hostedit, 
            1, 
            wx.EXPAND|wx.LEFT,
            cfg.BORDER_WIDTH
            )
        group_client_options.Add(
            horizontal_sizer, 
            0, 
            wx.EXPAND|wx.ALL,
            cfg.BORDER_WIDTH
            )
        sizer_search_and_cancel = wx.BoxSizer(wx.HORIZONTAL)
        sizer_search_and_cancel.Add(
            self.button_search, 
            1,
            wx.EXPAND
            )
        sizer_search_and_cancel.Add(
            self.button_cancel, 
            1, 
            wx.EXPAND|wx.LEFT, 
            cfg.BORDER_WIDTH
            )
        group_client_options.Add(
            sizer_search_and_cancel, 
            0, 
            wx.EXPAND|wx.LEFT|wx.RIGHT,
            cfg.BORDER_WIDTH
            )
        for each_button in self.connectbtn:
            horizontal_sizer = wx.BoxSizer(wx.HORIZONTAL)
            horizontal_sizer.Add(
                each_button, 
                1,
                wx.EXPAND
                )
            group_client_options.Add(
                horizontal_sizer,
                0,
                wx.EXPAND|wx.TOP|wx.LEFT|wx.RIGHT,
                cfg.BORDER_WIDTH
                )
        group_client_options.Add(
            (0, cfg.BORDER_WIDTH),
            0, 
            wx.EXPAND
            )
        sizer.Add(
            group_client_options,
            0, 
            wx.EXPAND|wx.ALL, 
            cfg.BORDER_WIDTH
            )
        
        group_mouse_options = wx.StaticBoxSizer(
                                            self.mousebox, 
                                            wx.VERTICAL
                                            )
        sizer_invertbtn = wx.BoxSizer(wx.HORIZONTAL)
        sizer_invertbtn.Add(
            self.invertbtn, 
            0,
            wx.EXPAND
            )
        group_mouse_options.Add(
            sizer_invertbtn,
            0, 
            wx.FIXED_MINSIZE|wx.ALL,
            cfg.BORDER_WIDTH
            )
        sizer_mousexslider = wx.BoxSizer(wx.HORIZONTAL)
        sizer_mousexslider.Add(
            self.mousexslider, 
            0, 
            wx.EXPAND
            )
        group_mouse_options.Add( 
            sizer_mousexslider,
            0, 
            wx.EXPAND|wx.LEFT|wx.RIGHT,
            cfg.BORDER_WIDTH
            )
        sizer_mouseyslider = wx.BoxSizer(wx.HORIZONTAL)
        sizer_mouseyslider.Add(
            self.mouseyslider,
            0,
            wx.EXPAND
            )
        group_mouse_options.Add(
            sizer_mouseyslider,
            0, 
            wx.EXPAND|wx.LEFT|wx.RIGHT,
            cfg.BORDER_WIDTH
            )
        sizer.Add(
            group_mouse_options, 
            0, 
            wx.EXPAND|wx.ALL,
            cfg.BORDER_WIDTH
            )
        sizer_fullreloadslider = wx.BoxSizer(wx.HORIZONTAL)
        sizer_fullreloadslider.Add(
            self.fullreloadslider,
            0, 
            wx.EXPAND
            )
        sizer.Add( 
            sizer_fullreloadslider, 
            0, 
            wx.EXPAND|wx.ALL,
            cfg.BORDER_WIDTH
            )
        sizer_crossmodelabel = wx.BoxSizer(wx.HORIZONTAL)
        sizer_crossmodelabel.Add(
            self.crossmodelabel, 
            0, 
            cfg.LABEL_ALIGN|wx.ALIGN_CENTRE_VERTICAL
            )
        sizer_crossmodelabel.Add(
            self.crossmodelist,
            0,
            wx.LEFT|wx.ALIGN_CENTRE_VERTICAL, 
            cfg.BORDER_WIDTH
            )
        sizer.Add(
            sizer_crossmodelabel,
            0, 
            wx.EXPAND|wx.LEFT|wx.RIGHT|wx.BOTTOM,
            cfg.BORDER_WIDTH
            )
        self.SetSizerAndFit(sizer)

    def __do_binding( self ):
        self.Bind( 
            wx.EVT_RADIOBUTTON, 
            self.on_single, 
            self.singlecheck 
            )
        self.Bind( 
            wx.EVT_RADIOBUTTON, 
            self.on_server, 
            self.servercheck 
            )
        self.Bind( 
            wx.EVT_RADIOBUTTON, 
            self.on_client,
            self.clientcheck 
            )
        self.Bind( 
            wx.EVT_BUTTON, 
            self.on_search,
            self.button_search 
            )
        self.Bind( 
            wx.EVT_BUTTON, 
            self.on_connectto, 
            self.button_connect_to 
            )
        for each_button in self.connectbtn:
            self.Bind(
                wx.EVT_BUTTON, 
                self.on_connect,
                each_button 
                )
        self.Bind(
            wx.EVT_BUTTON, 
            self.on_cancel, 
            self.button_cancel 
            )
        self.Bind(
            wx.EVT_BUTTON, 
            self.on_start,
            self.button_start 
            )
        self.Bind( 
            wx.EVT_CHECKBOX, 
            self.on_mouseinvert, 
            self.invertbtn 
            )
        self.Bind( 
            events.EVT_CHANGED, 
            self.on_mousexslider,
            self.mousexslider 
            )
        self.Bind( 
            events.EVT_CHANGED, 
            self.on_mouseyslider,
            self.mouseyslider 
            )
        self.Bind( 
            events.EVT_CHANGED, 
            self.on_fullreloadslider, 
            self.fullreloadslider 
            )
        self.Bind(
            wx.EVT_COMBOBOX, 
            self.on_crossmode, 
            self.crossmodelist 
            )
        self.Bind( 
            wx.EVT_TIMER, 
            self.on_update_timer, 
            self.timer 
            )

    def on_single( self, evt ):
        self.button_search.Disable()
        self.multiplayer = False
        self.server = False
        self.button_start.Enable()
        self.button_connect_to.Disable()
        self.hostedit.Disable()
        
    def on_server( self, evt ):
        self.button_search.Disable()
        self.multiplayer = True
        self.server = True
        self.button_start.Enable()
        self.button_connect_to.Disable()
        self.hostedit.Disable()
        
    def on_client( self, evt ):
        self.button_search.Enable()
        self.multiplayer = True
        self.server = False
        self.button_start.Disable()
        self.button_connect_to.Enable()
        self.hostedit.Enable()

    def on_search( self, evt ):
        self.singlecheck.Disable()
        self.servercheck.Disable()
        self.button_search.Disable()
        self.button_cancel.Enable()
        
        self.network = pynebula.new(
                                "nnetworkmanagermulti", 
                                "/sys/servers/network" 
                                )
        self.network.startsearching( self.gameport )

        self.searching = True
    
    def on_connectto( self, evt ):
        if self.network is None:
            self.network = pynebula.new(
                                    "nnetworkmanagermulti", 
                                    "/sys/servers/network" 
                                    )
            
        if self.network is not None:
            self.network.connecthost(
                str( self.hostedit.GetValue() ), 
                self.gameport 
                )
            self.searching = False
            
            state = app.get_state( "load" )
            state.setnextstate( "game" )
            
            state = app.get_state( "game" )
            state.setnextstate( "editor" )
            
            servers.get_conjurer().setstate( "load" )
            self.parent.Close()

    def on_connect( self, evt ):
        if self.network is not None:
            number = -1
            for index in range( self.number_connections ):
                if self.connectbtn[index] == evt.GetEventObject():
                    number = index
                    break

            if number >= 0 and self.network.getnumservers() > number:
                self.network.connect( number )
                self.searching = False

                state = app.get_state( "load" )
                state.setnextstate( "game" )
                
                state = app.get_state( "game" )
                state.setnextstate( "editor" )
                
                servers.get_conjurer().setstate( "load" )
                self.parent.Close()

    def on_cancel( self, evt ):
        self.singlecheck.Enable()
        self.servercheck.Enable()
        self.button_search.Enable()
        for each_button in self.connectbtn:
            each_button.Disable()
            each_button.SetLabel( "Connect to < empty >" )
            
        self.button_cancel.Disable()
        
        self.searching = False
        
        if self.network is not None:
            pynebula.delete( "/sys/servers/network" )
            self.network = None
        
    def on_start( self, evt ):
        if self.network is None:
            if self.multiplayer:
                self.network = pynebula.new( 
                                        "nnetworkmanagermulti",
                                        "/sys/servers/network" 
                                        )
            else:
                self.network = pynebula.new(
                                        "nnetworkmanager", 
                                        "/sys/servers/network" 
                                        )
            
            if self.network is not None:
                if self.server:
                    self.network.setserverport( self.gameport )
                    self.network.startserver()
        
                self.searching = False
                
                state = app.get_state( "load" )
                state.setnextstate( "game" )
                
                state = app.get_state( "game" )
                state.setnextstate( "editor" )
                
                servers.get_conjurer().setstate( "load" )
                self.parent.Close()
        
    def on_mouseinvert( self, evt ):
        state = app.get_state( "game" )
        state.setmouseyinvert( self.invertbtn.GetValue() )
        
    def on_mousexslider( self, evt ):
        state = app.get_state( "game" )
        state.setmousexfactor( evt.get_value() )

    def on_mouseyslider( self, evt ):
        state = app.get_state( "game" )
        state.setmouseyfactor( evt.get_value() )
        
    def on_fullreloadslider( self, evt ):
        state = app.get_state( "game" )
        state.setfullreloadtime( evt.get_value() )
        
    def on_crossmode( self, evt ):
        state = app.get_state( "game" )
        state.setcrossmode( self.crossmodelist.GetSelection() )
        
    def on_update_timer( self, evt ):
        if self.network is not None:
            if self.searching:
                number = self.network.getnumservers()
                n_servers = self.network.getservers()
                for index in range( 0, number ):
                    self.connectbtn[index].SetLabel(
                        "Connect to %s" % n_servers[index]
                        )
                    self.connectbtn[index].Enable()
                
                for index in range( number, self.number_connections ):
                    self.connectbtn[index].SetLabel( "Connect to < empty >" )
                    self.connectbtn[index].Disable()



class RnsGameWindow(freedialoggui.freeDialogGUI):
    def __init__(self, parent):
        freedialoggui.freeDialogGUI.__init__(self, "Renaissance Game", parent)

        self.rnsgame = RnsGamePanel(self)

        # Settings
        self.parent = parent

        sizer = wx.BoxSizer(wx.VERTICAL)
        sizer.Add(
            self.rnsgame,
            0,
            wx.EXPAND
            )
        self.SetSizerAndFit(sizer)


def create_window(frame):
    return RnsGameWindow( frame)
