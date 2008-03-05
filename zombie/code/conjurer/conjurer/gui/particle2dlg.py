##\file particle2dlg.py
##\brief Dialog to create a new particle system

import wx

import app
import particle2
import servers
import nohtree
import nebulaguisettings as guisettings
import conjurerconfig as cfg
import conjurerframework as cjr


# ParticleSystemDialog class
class Particle2SystemDialog(wx.Dialog):
    """Dialog to create a new particle system"""

    def __init__(self, parent):
        wx.Dialog.__init__(
            self, 
            parent, 
            -1, 
            "New particle system class",
            style = cfg.DEFAULT_DIALOG_STYLE
            )
        self.target_library = None
        valid_parent_list = self.get_valid_parent_list()
        self.staticbox_new_details_grp = wx.StaticBox(
                                                        self, 
                                                        -1, 
                                                        "Details of new class"
                                                        )
        self.label_new_name = wx.StaticText(
                                            self, 
                                            -1, 
                                            "Class name", 
                                            style=wx.ALIGN_RIGHT
                                            )
        self.text_new_name = wx.TextCtrl(self, -1, "")
        self.label_parent_name = wx.StaticText(
                                                self, 
                                                -1, 
                                                "Derive from", 
                                                style=wx.ALIGN_RIGHT
                                                )
        self.choice_parent_name = wx.Choice(
                                                self, 
                                                -1, 
                                                (100, 50), 
                                                choices = valid_parent_list
                                                )
        self.staticbox_target_library_grp = wx.StaticBox(
                                                            self, 
                                                            -1, 
                                                            "Target library"
                                                            )
        self.noh = nohtree.NOHTree(
                        self, 
                        -1, 
                        app.get_libraries(), 
                        passive=True
                        )
        # do this here, so initial selection picked up
        self.noh.set_selection_changed_callback(
            self.on_select_target
            )
        self.button_ok = wx.Button(self, -1, "&OK")
        #giving cancel button special ID so it will close when ESC is pressed
        self.button_cancel = wx.Button(self, wx.ID_CANCEL, "&Cancel")
        self.SetDefaultItem(self.button_ok)

        self.__set_properties()
        self.__do_layout()
        self.__bind_events()

    def __set_properties(self):
        #set the max length of the new name text field
        self.text_new_name.SetMaxLength(cfg.MAX_LENGTH_CLASS_NAME)
        #Set the initial selection for the parent class
        self.choice_parent_name.SetStringSelection('nemirage')
        library_path = guisettings.Repository.getsettingvalue(
                                guisettings.ID_DefaultParticleSystemLibraryPath
                                )
        self.expand_tree_to_library_path(library_path)
        # Disable ok button since there's no name entered on start up
        self.update_button_ok()

    def expand_tree_to_library_path(self, library_path):
        adjusted_path = library_path.replace(
                                    '/editor/',
                                    ''
                                    )
        self.noh.expand_tree(adjusted_path)

    def __do_layout(self):
        sizer_layout = wx.BoxSizer(wx.VERTICAL)
        sizer_buttons = wx.BoxSizer(wx.HORIZONTAL)
        sizer_new_details_grp = wx.StaticBoxSizer(
                                            self.staticbox_new_details_grp,
                                            wx.VERTICAL
                                            )
        sizer_target_library_grp = wx.StaticBoxSizer(
                                                self.staticbox_target_library_grp,
                                                wx.VERTICAL
                                                )
        grid_sizer = wx.FlexGridSizer(
                            4, 
                            3, 
                            cfg.BORDER_WIDTH, 
                            cfg.BORDER_WIDTH * 2
                            )
        grid_sizer.AddGrowableCol(1)
        grid_sizer.Add(
            self.label_new_name, 
            0, 
            cfg.LABEL_ALIGN|wx.ALIGN_CENTER_VERTICAL|wx.ADJUST_MINSIZE
            )
        grid_sizer.Add(
            self.text_new_name, 
            0, 
            wx.ADJUST_MINSIZE|wx.EXPAND
            )
        grid_sizer.AddSpacer( (0, 0) )
        grid_sizer.Add(
            self.label_parent_name, 
            0, 
            cfg.LABEL_ALIGN|wx.ALIGN_CENTER_VERTICAL|wx.ADJUST_MINSIZE
            )
        grid_sizer.Add(
            self.choice_parent_name, 
            0, 
            wx.ADJUST_MINSIZE
            )
        grid_sizer.AddSpacer( (0, 0) )
        grid_sizer.AddSpacer( (0, 0) )
        sizer_new_details_grp.Add(
            grid_sizer, 
            0, 
            wx.ALL|wx.EXPAND, 
            cfg.BORDER_WIDTH
            )
        sizer_layout.Add(
            sizer_new_details_grp, 
            0, 
            wx.ALL|wx.EXPAND, 
            cfg.BORDER_WIDTH
            )
        sizer_target_library_grp.Add(
            self.noh, 
            1, 
            wx.ALL|wx.EXPAND, 
            cfg.BORDER_WIDTH
            )
        sizer_layout.Add(
            sizer_target_library_grp, 
            1, 
            wx.ALL|wx.EXPAND, 
            cfg.BORDER_WIDTH
            )
        sizer_buttons.Add(
            self.button_ok, 
            0, 
            wx.ADJUST_MINSIZE
            )
        sizer_buttons.Add(
            self.button_cancel, 
            0, 
            wx.LEFT|wx.ADJUST_MINSIZE, 
            cfg.BORDER_WIDTH
            )
        sizer_layout.Add(
            sizer_buttons, 
            0, 
            wx.ALL|wx.ALIGN_RIGHT, 
            cfg.BORDER_WIDTH
            )
        self.SetAutoLayout(True)
        self.SetSizerAndFit(sizer_layout)
        self.SetSize( (380, 360) )
        self.Layout()

    def __bind_events(self):
        self.Bind(
            wx.EVT_TEXT, 
            self.on_change_name, 
            self.text_new_name
            )
        self.Bind(
            wx.EVT_CHOICE, 
            self.on_change_parent_name, 
            self.choice_parent_name
            )
        self.Bind(
            wx.EVT_BUTTON, 
            self.on_ok, 
            self.button_ok
            )
        self.Bind(
            wx.EVT_BUTTON, 
            self.on_cancel, 
            self.button_cancel
            )

    def get_valid_parent_list(self):
        return valid_parent_class_names()

    def has_valid_class_name(self):
        new_class_name = self.get_new_class_name()
        if new_class_name == "":
            return False
        else:
            ecs = servers.get_entity_class_server()
            return ecs.checkclassname( str( new_class_name.capitalize() ) )

    def has_valid_parent_class_name(self):
        parent_class_name = self.get_parent_class_name()
        if parent_class_name == "":
            return False
        else:
            ecs = servers.get_entity_class_server()
            return ecs.checkclassname( str( parent_class_name.capitalize() ) )

    def update_button_ok(self):
        #I think that doing the checks in reverse order is 
        # more efficient, particularly as checking the class 
        # names involves a call to Nebula
        enable = self.target_library != None and \
            self.has_valid_parent_class_name() and \
            self.has_valid_class_name() 
        self.button_ok.Enable( enable )

    def on_change_name(self, event):
        self.update_button_ok()

    def on_change_parent_name(self, event):
        self.update_button_ok()

    def on_select_target(self, obj):
        if obj.isa('nstringlist'):
            self.target_library = obj.getfullname()
        else:
            self.target_library = None
        self.update_button_ok()

    def get_new_class_name(self):
        return self.text_new_name.GetValue()

    def get_parent_class_name(self):
        return self.choice_parent_name.GetStringSelection()

    def on_ok(self, event):
        try:
            wx.BeginBusyCursor()
            new_class_name = str( self.get_new_class_name() )
            parent_class_name = str( self.get_parent_class_name() )
            entity_class_created = particle2.create_class(
                                            new_class_name, 
                                            parent_class_name, 
                                            self.target_library
                                            )
            if entity_class_created:
                self.EndModal(wx.ID_OK)
            else:
                msg = "Unable to create a particle system "\
                            "class called '%s'"  % new_class_name
                cjr.show_error_message(msg)
        finally:
            wx.EndBusyCursor()

    def on_cancel(self, event):
        self.EndModal(wx.ID_CANCEL)

def valid_parent_class_names():
    """ Return a list of classes that can be used
        as parent classes for particle systems. """
    return ['nemirage', 'nespecialfx']
