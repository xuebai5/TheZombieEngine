##\file particlelistdlg.py
##\brief Particle system list dialog

import wx

import servers
import app

import togwin
import objdlg
import particle2dlg
import particleemitdlg

import conjurerconfig as cfg
import conjurerframework as cjr


class ParticleSystemListDialog(togwin.ChildToggableDialog):
    def __init__(self, parent):
        togwin.ChildToggableDialog.__init__(
            self,
            "Particle Systems",
            parent
            )

        self.list_particles = wx.ListBox(
                        self, 
                        -1, 
                        choices=[], 
                        style=wx.LB_SORT
                        )
        self.button_new = wx.Button(self, -1, "&New")
        self.button_edit = wx.Button(self, -1, "&Edit")
        self.button_delete = wx.Button(self, -1, "&Delete")
        self.button_emitters = wx.Button(self, -1, "E&mitters...")
        self.button_close = wx.Button(self, wx.ID_CANCEL, "&Close")

        self.__set_properties()
        self.__do_layout()
        self.__bind_events()
    
    def __set_properties(self):
        self.__build_particle_list()
        self.__update_buttons()

    def __update_buttons(self):
        should_enable = self.has_item_selected()
        self.button_edit.Enable(should_enable)
        self.button_delete.Enable(should_enable)
        self.button_emitters.Enable(should_enable)

    def __build_particle_list(self):
        self.list_particles.Clear()
        ec_server = servers.get_entity_class_server()
        particle_list = []
        try:
            wx.BeginBusyCursor()
            for each in particle2dlg.valid_parent_class_names():
                parent_class = ec_server.getentityclass(each)
                ec_server.loadentitysubclasses(parent_class)
                next_class = parent_class.gethead()
                while next_class is not None:
                    if next_class.getclasskeyint("ParticleSystem"):
                        particle_list.append(next_class)
                    next_class = next_class.getsucc()
            for each_item in particle_list:
                self.list_particles.Append( each_item.getname() )
        finally:
            wx.EndBusyCursor()

    def __do_layout(self):
        sizer_main = wx.BoxSizer(wx.VERTICAL)
        sizer_list_and_buttons = wx.BoxSizer(wx.HORIZONTAL)
        sizer_list_and_buttons.Add(
            self.list_particles, 1, wx.EXPAND|wx.ALL, cfg.BORDER_WIDTH
            )
        vertical_line = wx.StaticLine(
                                self, 
                                -1, 
                                ( -1, -1 ), 
                                ( -1, -1 ), 
                                wx.LI_VERTICAL
                                )
        sizer_list_and_buttons.Add(
            vertical_line, 
            0, 
            wx.EXPAND|wx.TOP|wx.BOTTOM, 
            cfg.BORDER_WIDTH
            )        
        sizer_buttons = wx.BoxSizer(wx.VERTICAL)        
        sizer_buttons.Add(
            self.button_new,
            0,
            wx.FIXED_MINSIZE
            )
        sizer_buttons.Add(
            self.button_edit, 
            0, 
            wx.TOP|wx.FIXED_MINSIZE, 
            cfg.BORDER_WIDTH
            )
        sizer_buttons.Add(
            self.button_delete, 
            0, 
            wx.TOP|wx.FIXED_MINSIZE, 
            cfg.BORDER_WIDTH
            )
        sizer_buttons.Add(
            self.button_emitters, 
            0, 
            wx.TOP|wx.FIXED_MINSIZE, 
            cfg.BORDER_WIDTH
            )
        sizer_list_and_buttons.Add(
            sizer_buttons, 0, wx.ALIGN_RIGHT|wx.ALL, cfg.BORDER_WIDTH
            )
        sizer_main.Add(
            sizer_list_and_buttons,
            1,
            wx.EXPAND
            )
        horizontal_line = wx.StaticLine(
                                self, 
                                -1, 
                                ( -1, -1 ), 
                                ( -1, -1 ), 
                                wx.LI_HORIZONTAL
                                )
        sizer_main.Add(
            horizontal_line, 
            0, 
            wx.EXPAND|wx.LEFT|wx.RIGHT, 
            cfg.BORDER_WIDTH
            )
        sizer_main.Add(
            self.button_close, 
            0, 
            wx.FIXED_MINSIZE|wx.ALL|wx.ALIGN_RIGHT, 
            cfg.BORDER_WIDTH
            )
        self.SetAutoLayout(True)
        self.SetSizerAndFit(sizer_main)
        self.SetSize( (270, 300) )
        self.Layout()

    def __bind_events(self):
        self.Bind(
            wx.EVT_BUTTON, 
            self.__on_new, 
            self.button_new
            )
        self.Bind(
            wx.EVT_BUTTON, 
            self.__on_edit, 
            self.button_edit
            )
        self.Bind(
            wx.EVT_LISTBOX_DCLICK, 
            self.__on_edit, 
            self.list_particles
            )
        self.Bind(
            wx.EVT_BUTTON, 
            self.__on_delete, 
            self.button_delete
            )
        self.Bind(
            wx.EVT_BUTTON, 
            self.__on_emitters, 
            self.button_emitters
            )
        self.Bind(
            wx.EVT_BUTTON, 
            self.__on_close_button_clicked,
            self.button_close
            )
        self.Bind(
            wx.EVT_LISTBOX, 
            self.__on_select_class, 
            self.list_particles
            )

    def get_selected_class_name(self):
        return self.list_particles.GetStringSelection()

    def get_selected_class(self):
        """ Return the actual class, if one is selected. """
        if self.has_item_selected():
            ec_server = servers.get_entity_class_server()
            return ec_server.getentityclass(
                str( self.get_selected_class_name() )
                )
        else:
            return None

    def has_item_selected(self):
        return self.list_particles.GetSelection() != wx.NOT_FOUND

    def update_list(self):
        self.__build_particle_list()

    def __on_new(self, event):
        dlg = particle2dlg.Particle2SystemDialog(self)
        result = dlg.ShowModal()
        dlg.Destroy()
        if result == wx.ID_OK:
            self.update_list()

    def __on_edit(self, event):
        if self.has_item_selected():
            particle_system = self.get_selected_class()
            try:
                wx.BeginBusyCursor()
                win = objdlg.create_window(
                            app.get_top_window(), 
                            particle_system.getfullname()
                            )
            finally:
                wx.EndBusyCursor()
            win.display()
        else:
            cjr.show_error_message(
                "Please select a Particle System to edit."
                )

    def delete_class ( self, entity_name ):
        msg = "This functionality is not yet available.\n\n"\
                    "Please use the Grimoire to delete the class if required."
        result = cjr.show_error_message(msg)

    def __on_delete(self, event):
        if self.has_item_selected():
            self.delete_class(
                self.get_selected_class_name()
                )
        else:
            cjr.show_error_message(
                "Please select a Particle System to delete."
                )

    def __on_emitters(self, event):
        if self.has_item_selected():
            particle_system_name = self.get_selected_class_name()
            win = particleemitdlg.create_window(
                        app.get_top_window(), 
                        str(particle_system_name)
                        )
            win.display()
        else:
            cjr.show_error_message(
                "Please select a Particle System."
                )

    def __on_close_button_clicked(self, event):
        self.Close()

    def __on_select_class(self, event):
        self.__update_buttons()

    def persist(self):
        return [
            create_window,
            (),  # no parameters for create function
            None
            ]

def create_window(parent):
    return ParticleSystemListDialog(parent)
