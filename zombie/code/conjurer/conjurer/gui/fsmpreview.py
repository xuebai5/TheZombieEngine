##\file fsmpreview.py
##\brief Control to display a FSM preview, allowing some configuration

import wx
import wx.lib.iewin as iewin

import os
import pynebula

import app
import format
import fsmstates
import fsmtransitions
import iehelpwindow
import nebulaguisettings as cfg

fsm_count = 0

# PreviewWindow class
class PreviewWindow(wx.Panel):
    """Scrolled window where a preview of a FSM is shown"""
    
    def __init__(self, parent, fsm_path=None):
        wx.Panel.__init__(self, parent, style=wx.TAB_TRAVERSAL|wx.CLIP_CHILDREN|wx.NO_FULL_REPAINT_ON_RESIZE)
        self.fsm_path = fsm_path
        global fsm_count
        preview_filename = format.mangle_path( str("outgui:images/temp/fsm" + str(fsm_count)) )
        self.dot_path = preview_filename + ".dot"
        self.image_path = preview_filename + ".gif"
        self.map_path = preview_filename + ".map"
        self.html_path = preview_filename + ".html"
        fsm_count += 1
        
        self.ie = iewin.IEHtmlWindow(self, -1, style = wx.NO_FULL_REPAINT_ON_RESIZE)
        
        sizer = wx.BoxSizer(wx.VERTICAL)
        sizer.Add(self.ie, 1, wx.EXPAND)
        self.SetSizer(sizer)
        
        self.Bind(iewin.EVT_BeforeNavigate2, self.__on_before_navigate, self.ie)
    
    def __del__(self):
        self.__clean_up_preview_files()
    
    def __generate_dot_file(self):
        # Begin
        dot_file = open( self.dot_path, 'w' )
        dot_file.write( 'digraph G {\n' )
        
        # States
        fsm = pynebula.lookup( self.fsm_path )
        initial_state = fsm.getinitialstate()
        state = fsm.gethead()
        while state is not None:
            line = '    ' + state.getname()
            label = fsmstates.get_state_gui_name( state.getfullname() )
            line = line + ' [label="' + label + '"'
            line = line + ' , URL="state:' + state.getfullname() + '"'
            if state.isa('nnodestate'):
                line = line + ', style=dashed'
            elif state.isa('nendstate'):
                line = line + ', peripheries=2'
            else:
                line = line + ', style=solid'
            if state == initial_state:
                line = line + ', shape=box'
            line = line + '];\n'
            dot_file.write(line)
            
            # State transitions
            try:
                transition = pynebula.lookup( str(state.getfullname() +
                    "/transitions") ).gethead()
            except:
                transition = None
            while transition is not None:
                condition = transition.getcondition()
                for i in range( transition.gettargetsnumber() ):
                    line = '    ' + state.getname() + ' -> '
                    line = line + transition.gettargetstatebyindex(i).getname()
                    probability = transition.gettargetprobabilitybyindex(i)
                    label = fsmtransitions.get_transition_gui_name( transition.getfullname() ).split(' ',1)[1]
                    if probability != 100:
                        label = label + '\\n(' + str(probability) + '%)'
                    line = line + ' [label="' + label + '"'
                    line = line + ' URL="transition:' + transition.getfullname()
                    line = line + '/' + str(i) + '"'
                    if condition.isa('nscriptcondition'):
                        line = line + ', arrowhead=onormal'
                    if probability != 100:
                        line = line + ', style=dotted'
                    line = line + '];\n'
                    dot_file.write(line)
                transition = transition.getsucc()
            
            state = state.getsucc()
##        dot_file.write( '    B [label="Attack", style=dashed];\n' )
##        dot_file.write( '    A -> B [label="True", arrowhead=onormal];\n' )
##        dot_file.write( '    B -> A [label="Done"];\n' )
##        dot_file.write( '    \n' )
##        dot_file.write( '    C [label="Shoot", style=bold]\n' );
##        dot_file.write( '    D [label="End shooting", peripheries=2];\n' )
##        dot_file.write( '    C -> D [label="True", arrowhead=onormal];\n' )
        
        # End
        dot_file.write( '}\n' )
        dot_file.close()
    
    def __generate_graph_image(self):
        # Image
        dot_path = cfg.Repository.getsettingvalue( cfg.ID_DotPath )
        cmd = '"\"' + dot_path + '\"'
        cmd = cmd + ' -Tgif ' + '\"' + self.dot_path + '\"'
        cmd = cmd + ' -o ' + '\"' + self.image_path + '\""'
        stdin, stdout, stderr = os.popen3( cmd )
        error = stderr.read()
        if len(error) > 0:
            print "Error while generating the FSM preview image:"
            print error.replace('\n', ' ')
            return False
        stdin.close()
        stdout.close()
        stderr.close()
        
        # Map
        cmd = '"\"' + dot_path + '\"'
        cmd = cmd + ' -Tcmapx ' + '\"' + self.dot_path + '\"'
        cmd = cmd + ' -o ' + '\"' + self.map_path + '\""'
        stdin, stdout, stderr = os.popen3( cmd )
        error = stderr.read()
        if len(error) > 0:
            print "Error while generating the FSM preview client map:"
            print error.replace('\n', ' ')
            return False
        stdin.close()
        stdout.close()
        stderr.close()
        
        # HTML
        map_file = open( self.map_path, 'r' )
        map_tag = map_file.read()
        map_file.close()
        html_file = open( self.html_path, 'w' )
        html_file.write( "<img src='" + format.get_name(self.image_path) + "' usemap=#G border=0>\n" )
        html_file.write( map_tag )
        html_file.close()
        
        return True
    
    def __load_graph_image(self):
        self.ie.LoadUrl( self.html_path )
    
    def __clean_up_preview_files(self):
        if os.path.exists( self.dot_path ):
            os.remove( self.dot_path )
        if os.path.exists( self.image_path ):
            os.remove( self.image_path )
        if os.path.exists( self.map_path ):
            os.remove( self.map_path )
        if os.path.exists( self.html_path ):
            os.remove( self.html_path )
    
    def __on_before_navigate(self, event):
        url = event.URL
        if url.startswith('state:'):
            path = url[ len('state:') : ]
            self.GetGrandParent().select_state( path )
            event.Cancel = True
        elif url.startswith('transition:'):
            path, target = url[ len('transition:') : ].rsplit('/',1)
            self.GetGrandParent().select_transition( path, int(target) )
            event.Cancel = True
    
    def update_preview_image(self):
        if self.fsm_path != None:
            self.__generate_dot_file()
            if self.__generate_graph_image():
                self.__load_graph_image()
    
    def set_fsm(self, fsm_path):
        self.fsm_path = fsm_path


# PreviewCtrl class
class PreviewCtrl(wx.PyControl):
    """Control to display a FSM preview, allowing some configuration"""
    
    def __init__(self, parent, fsm_path=None):
        wx.PyControl.__init__(self, parent, style=wx.NO_BORDER|wx.TAB_TRAVERSAL)
        self.autorefresh_enabled = True
        
        self.win_preview = PreviewWindow(self, fsm_path)
        self.button_refresh = wx.Button(self, -1, "Refresh")
        self.button_options = wx.Button(self, -1, "Preview options...")
        self.button_help = wx.Button(self, -1, "Help...")
        
        self.button_options.Disable()
        self.__do_layout()
        self.__bind_events()
    
    def __do_layout(self):
        sizer_layout = wx.BoxSizer(wx.VERTICAL)
        sizer_buttons = wx.BoxSizer(wx.HORIZONTAL)
        sizer_layout.Add(self.win_preview, 1, wx.EXPAND, 0)
        sizer_buttons.Add(self.button_refresh, 1, wx.FIXED_MINSIZE, 0)
        sizer_buttons.Add(self.button_options, 1, wx.LEFT|wx.FIXED_MINSIZE, 5)
        sizer_buttons.Add(self.button_help, 1, wx.LEFT|wx.FIXED_MINSIZE, 5)
        sizer_layout.Add(sizer_buttons, 0, wx.TOP|wx.EXPAND, 5)
        self.SetAutoLayout(True)
        self.SetSizer(sizer_layout)
        sizer_layout.Fit(self)
        sizer_layout.SetSizeHints(self)
        self.Layout()
    
    def __bind_events(self):
        self.Bind(wx.EVT_SIZE, self.__on_size)
        self.Bind(wx.EVT_BUTTON, self.on_refresh_preview, self.button_refresh)
        self.Bind(wx.EVT_BUTTON, self.on_help, self.button_help)
    
    def __on_size(self, event):
        # Needed to resize controls along with the window owning this control
        self.Layout()
        event.Skip()
    
    def on_refresh_preview(self, event):
        self.win_preview.update_preview_image()
    
    def on_help(self, event):
        iehelpwindow.CreateWindow( app.get_top_window(self),
            "http://wiki.tragnarion.com/tiki-index.php?page=SWD+Nebula+Conjurer+FSM+editor" )
    
    def on_change_fsm(self, event):
        if self.autorefresh_enabled:
            self.on_refresh_preview(event)
    
    def set_fsm(self, fsm_path):
        self.win_preview.set_fsm( fsm_path )
