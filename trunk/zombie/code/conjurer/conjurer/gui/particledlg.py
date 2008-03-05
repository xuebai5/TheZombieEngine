##\file particledlg.py
##\brief Dialog to create a new particle system

import wx

import app
import format
import nebulaguisettings as cfg
import objbrowserwindow
import objdlg
import particle
import servers
import nohtree


# ParticleSystemDialog class
class ParticleSystemDialog(wx.Dialog):
    """Dialog to create a new particle system"""
    
    def __init__(self, parent):
        wx.Dialog.__init__(self, parent, -1, "New particle system class",
            style = wx.DEFAULT_DIALOG_STYLE|wx.RESIZE_BORDER|wx.TAB_TRAVERSAL)
        self.target_library = None
        
        self.label_name = wx.StaticText(self, -1, "Class name:", style=wx.ALIGN_RIGHT)
        self.text_name = wx.TextCtrl(self, -1, "")
        self.label_texture = wx.StaticText(self, -1, "Texture:", style=wx.ALIGN_RIGHT)
        self.button_texture = wx.Button(self, -1, "<default>")
        self.label_target = wx.StaticText(self, -1, "Target library:")
        self.noh = nohtree.NOHTree(self, -1, app.get_libraries(), passive=True)
        self.button_ok = wx.Button(self, -1, "OK")
        self.button_cancel = wx.Button(self, -1, "Cancel")
        
        self.__set_properties()
        self.__do_layout()
        self.__bind_events()
    
    def __set_properties(self):
        # Disable button ok since there's no name entered on start up
        self.__update_button_ok()
        
        # TODO: Set the default texture
        pass
    
    def __do_layout(self):
        sizer_border = wx.BoxSizer(wx.VERTICAL)
        sizer_layout = wx.BoxSizer(wx.VERTICAL)
        sizer_buttons = wx.BoxSizer(wx.HORIZONTAL)
        sizer_texture = wx.BoxSizer(wx.HORIZONTAL)
        sizer_name = wx.BoxSizer(wx.HORIZONTAL)
        sizer_name.Add(self.label_name, 0, wx.ALIGN_CENTER_VERTICAL|wx.ADJUST_MINSIZE, 0)
        sizer_name.Add(self.text_name, 1, wx.LEFT|wx.ADJUST_MINSIZE, 5)
        sizer_layout.Add(sizer_name, 0, wx.EXPAND, 10)
        sizer_texture.Add(self.label_texture, 0, wx.ALIGN_CENTER_VERTICAL|wx.ADJUST_MINSIZE, 0)
        sizer_texture.Add(self.button_texture, 1, wx.LEFT|wx.ADJUST_MINSIZE, 5)
        sizer_layout.Add(sizer_texture, 0, wx.TOP|wx.EXPAND, 5)
        sizer_layout.Add(self.label_target, 0, wx.TOP|wx.EXPAND, 5)
        sizer_layout.Add(self.noh, 1, wx.TOP|wx.EXPAND, 5)
        sizer_buttons.Add(self.button_ok, 0, wx.ADJUST_MINSIZE, 0)
        sizer_buttons.Add((5, 5), 1, wx.ADJUST_MINSIZE, 0)
        sizer_buttons.Add(self.button_cancel, 0, wx.ADJUST_MINSIZE, 0)
        sizer_layout.Add(sizer_buttons, 0, wx.TOP|wx.EXPAND, 10)
        sizer_border.Add(sizer_layout, 1, wx.ALL|wx.EXPAND, 10)
        self.SetAutoLayout(True)
        self.SetSizer(sizer_border)
        sizer_border.Fit(self)
        sizer_border.SetSizeHints(self)
        self.Layout()
        self.SetSize( (250,300) )
    
    def __bind_events(self):
        self.Bind(wx.EVT_TEXT, self.on_change_name, self.text_name)
        self.Bind(wx.EVT_BUTTON, self.on_texture, self.button_texture)
        self.noh.set_selection_changed_callback( self.on_select_target )
        self.Bind(wx.EVT_BUTTON, self.on_ok, self.button_ok)
        self.Bind(wx.EVT_BUTTON, self.on_cancel, self.button_cancel)
    
    def __update_button_ok(self):
        if self.text_name.GetValue() == "":
            enable = False
        else:
            enable = servers.get_entity_class_server().checkclassname(
                str(self.text_name.GetValue().capitalize()) )
        enable = enable and self.target_library != None
        self.button_ok.Enable( enable )
    
    def on_change_name(self, event):
        self.__update_button_ok()
    
    def on_texture(self, event):
        # Let the user choose a file among the shared textures
        mangled_shared_dir = format.mangle_path( "wc:export/textures" )
        dir = cfg.Repository.getsettingvalue( cfg.ID_BrowserPath_SharedMaterial )
        if dir == "":
            dir = mangled_shared_dir
        dlg = wx.FileDialog(
            self, message="Choose an image file",
            defaultDir = dir,
            wildcard="Image files (*.dds)|*.dds",
            style=wx.OPEN
            )
        
        if dlg.ShowModal() == wx.ID_OK:
            mangled_path = format.mangle_path( dlg.GetPath() )
            if not mangled_path.startswith( mangled_shared_dir ):
                wx.MessageBox("You should choose a texture file from the" \
                    " directory 'wc:export/textures' or below.",
                    "Conjurer", style=wx.ICON_ERROR)
            else:
                self.button_texture.SetLabel(
                    format.get_relative_path( mangled_shared_dir, mangled_path )
                    )
                # Record last directory
                dir = format.get_directory( mangled_path )
                cfg.Repository.setsettingvalue( cfg.ID_BrowserPath_SharedMaterial, dir )
        
        dlg.Destroy()
    
    def on_select_target(self, obj):
        if obj.isa('nstringlist'):
            self.target_library = obj.getfullname()
        else:
            self.target_library = None
        self.__update_button_ok()
    
    def on_ok(self, event):
        if self.button_texture.GetLabel() == "<default>":
            entity_class = particle.CreateClass( str(self.text_name.GetValue()), self.target_library )
        else:
            texture = format.append_to_path( "wc:export/textures", self.button_texture.GetLabel() )
            entity_class = particle.CreateClass( str(self.text_name.GetValue()), self.target_library, str(texture) )
        if entity_class is None:
            wx.MessageBox( "Couldn't create the particle system class named '" \
                + self.text_name.GetValue() + "'", "Conjurer",
                style=wx.ICON_ERROR )
        else:
            objdlg.create_window( app.get_top_window(), entity_class.getfullname() )
            self.EndModal(wx.ID_OK)
    
    def on_cancel(self, event):
        self.EndModal(wx.ID_CANCEL)
