##\file grassmaterials.py
##\brief Terrain grass materials editor

import wx

import app
import format
import grassbrushes
import grasscolor
import togwin
import trn

import conjurerconfig as cfg
import conjurerframework as cjr

# Event used to notify selection of a material control
EVT_SELECT_TYPE = wx.NewEventType()
EVT_SELECT = wx.PyEventBinder(EVT_SELECT_TYPE, 1)


# MaterialSelectedEvent class
class MaterialSelectedEvent(wx.PyCommandEvent):
    def __init__(self, id, object):
        wx.PyCommandEvent.__init__(self, EVT_SELECT_TYPE, id)
        self.SetEventObject(object)


# GrassMaterialCtrl class
class GrassMaterialCtrl(wx.PyControl):
    """Control used to handle a single grass material"""

    def __init__(self, parent, id, material_index, material):
        wx.PyControl.__init__(self, parent, id, style=wx.NO_BORDER|wx.TAB_TRAVERSAL)
        self.material_index = material_index
        self.material = material
        self.is_selected = False
        # controls
        self.button_select_colour = wx.Button(
                                                self, 
                                                -1, 
                                                size=( (25,25) ),
                                                style=wx.NO_BORDER
                                                )
        self.label_name = wx.StaticText(
                                    self, 
                                    -1, 
                                    material.getname()
                                    )
        self.__set_properties()
        self.__do_layout()
        self.__bind_events()

    def __set_properties(self):
        if self.is_for_default_material():
            self.button_select_colour.Disable()
        else:
            self.set_button_colour_from_material()

    def __do_layout(self):
        sizer_main = wx.BoxSizer(wx.VERTICAL)
        sizer = wx.BoxSizer(wx.HORIZONTAL)
        sizer.Add(
            self.button_select_colour,
            0,
            wx.ALL|wx.ALIGN_CENTER_VERTICAL|wx.FIXED_MINSIZE,
            cfg.BORDER_WIDTH 
            )
        sizer.Add(
            self.label_name, 
            1, 
            wx.LEFT|wx.ALIGN_CENTER_VERTICAL|wx.FIXED_MINSIZE, 
            cfg.BORDER_WIDTH
            )
        sizer_main.Add(
            sizer
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
            wx.EXPAND
            )
        self.SetSizerAndFit(sizer_main)

    def __bind_events(self):
        wx.EVT_SIZE(self, self.on_size)
        self.Bind(wx.EVT_LEFT_DOWN, self.on_select_material)
        self.label_name.Bind(wx.EVT_LEFT_DOWN, self.on_select_material)
        self.button_select_colour.Bind(
            wx.EVT_LEFT_DOWN, 
            self.on_select_material
            )
        self.Bind(
            wx.EVT_BUTTON,
            self.on_button_select_colour_clicked, 
            self.button_select_colour
            )

    def on_size(self, event):
        self.Layout()
        event.Skip()

    def __update_colours(self):
        if self.is_selected:
            bgcolor = wx.SystemSettings.GetColour(wx.SYS_COLOUR_INACTIVECAPTION)
        else:
            bgcolor = wx.SystemSettings.GetColour(wx.SYS_COLOUR_BTNFACE)
        self.SetBackgroundColour(bgcolor)
        self.label_name.SetBackgroundColour(bgcolor)
        self.Refresh()

    def on_select_material(self, event):
        self.select()
        self.GetEventHandler().ProcessEvent(
            MaterialSelectedEvent(
                self.GetId(), 
                self 
                )
            )
        event.Skip()

    def on_button_select_colour_clicked(self, event):
        self.on_select_colour()

    def on_select_colour(self):
        # cannot change colour of default material
        if not self.is_for_default_material():
            dlg = grasscolor.MaterialColorDialog(
                        app.get_top_window(), 
                        self.material
                        )
            if dlg.ShowModal() == wx.ID_OK:
                self.set_button_colour_from_material()
            dlg.Destroy()

    def set_button_colour_from_material(self):
        bg_colour = format.unit_rgb_2_byte_rgb(
                            self.material.getcolor() 
                            )
        self.button_select_colour.SetBackgroundColour(bg_colour)

    def edit_brushes(self):
        # cannot edit brushes for default material
        if not self.is_for_default_material():
            dlg = grassbrushes.MaterialBrushesDialog(
                        app.get_top_window(),
                        self.material
                        )
            dlg.ShowModal()
            dlg.Destroy()

    def select(self):
        self.is_selected = True
        self.__update_colours()

    def deselect(self):
        self.is_selected = False
        self.__update_colours()

    def get_material_name(self):
        return self.label_name.GetLabel()

    def get_material_id(self):
        return self.material_index

    def is_for_default_material(self):
        # The default material always has index of 0 
        return self.material_index == 0


# GrassMaterialsDialog class
class GrassMaterialsDialog(togwin.ChildToggableDialog):
    """Dialog to manage terrain grass materials"""

    def __init__(self, parent):
        togwin.ChildToggableDialog.__init__(self, "Grass materials", parent)
        self.material_selected = None
        # all controls except materials
        self.scroll_materials = wx.ScrolledWindow(
                                        self, 
                                        -1, 
                                        style=wx.NO_BORDER|wx.TAB_TRAVERSAL
                                        )
        self.button_new = wx.Button(self, -1, "&New")
        self.button_copy = wx.Button(self, -1, "&Copy")
        self.button_delete = wx.Button(self, -1, "Delete")
        self.button_brushes = wx.Button(self, -1, "&Brushes...")
        self.button_colour = wx.Button(self, -1, "Colou&r...")        
        self.button_close = wx.Button(self, wx.ID_CANCEL, "&Close")
        self.material_ctrls = []

        self.__set_properties()
        self.__do_layout()
        self.__bind_events()

    def __set_properties(self):
        self.SetSize( (400, 300) )
        self.scroll_materials.SetScrollRate(10, 10)
        
        if app.get_outdoor() is None:
            cjr.show_warning_message(
                "The current level has no terrain"
                )
            return

        self.__build_material_list()

    def __do_layout(self):
        main_sizer = wx.BoxSizer(wx.VERTICAL)
        sizer_list_and_buttons = wx.BoxSizer(wx.HORIZONTAL)
        sizer_list_and_buttons.Add(
            self.scroll_materials, 1, wx.EXPAND|wx.ALL, cfg.BORDER_WIDTH
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
            self.button_new, 0, wx.FIXED_MINSIZE
            )
        sizer_buttons.Add(
            self.button_copy, 0, wx.FIXED_MINSIZE|wx.TOP, cfg.BORDER_WIDTH
            )
        sizer_buttons.Add(
            self.button_delete, 0, wx.FIXED_MINSIZE|wx.TOP, cfg.BORDER_WIDTH
            )
        sizer_buttons.Add(
            self.button_brushes, 0, wx.FIXED_MINSIZE|wx.TOP, cfg.BORDER_WIDTH
            )
        sizer_buttons.Add(
            self.button_colour, 0, wx.FIXED_MINSIZE|wx.TOP, cfg.BORDER_WIDTH
            )
        sizer_list_and_buttons.Add(
            sizer_buttons, 0, wx.ALIGN_RIGHT|wx.ALL, cfg.BORDER_WIDTH
            )
        main_sizer.Add(
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
        main_sizer.Add(
            horizontal_line, 
            0, 
            wx.EXPAND|wx.LEFT|wx.RIGHT, 
            cfg.BORDER_WIDTH
            )
        main_sizer.Add(
            self.button_close, 
            0, 
            wx.FIXED_MINSIZE|wx.ALL|wx.ALIGN_RIGHT, 
            cfg.BORDER_WIDTH
            )
        self.SetSizerAndFit(main_sizer)
        self.SetSize( (290, 360) )

    def __bind_events(self):
        self.Bind(wx.EVT_BUTTON, self.on_new_material, self.button_new)
        self.Bind(wx.EVT_BUTTON, self.on_copy_material, self.button_copy)
        self.Bind(wx.EVT_BUTTON, self.on_delete_material, self.button_delete)
        self.Bind(
            wx.EVT_BUTTON, 
            self.on_material_brushes, 
            self.button_brushes
            )
        self.Bind(
            wx.EVT_BUTTON, 
            self.on_material_colour, 
            self.button_colour
            )
        self.Bind(wx.EVT_BUTTON, self.on_click_close_button, self.button_close)

    def on_new_material(self, event):
        # Ask for material name
        dlg = wx.TextEntryDialog(
                    self, 
                    "Enter a material name", 
                    "New grass material" 
                    )
        if dlg.ShowModal() == wx.ID_OK:
            name = str( dlg.GetValue() )
            if name.strip() == "":
                cjr.show_error_message(
                    "Please enter a name for the new grass material."
                    )
            else:
                # Validate name
                terrain = app.get_outdoor()
                if terrain.hasgrowthmaterialwithname(name):
                    cjr.show_error_message(
                        "There is already a grass material called '%s'." % name
                        )
                else:
                    # Add new material
                    terrain.creategrowthmaterial(name)
                    self.__build_material_list()
        dlg.Destroy()

    def on_copy_material(self, event):
        material = self.material_selected.material
        # Ask for material name
        dlg = wx.TextEntryDialog(
                    self, 
                    "This will create a copy of the '%s' material.\n\n" \
                    "Enter a name for the new material." % material.getname(), 
                    "Create a copy of an existing grass material" 
                    )
        if dlg.ShowModal() == wx.ID_OK:
            name = str( dlg.GetValue() )
            if name.strip() == "":
                cjr.show_error_message(
                    "Please enter a name for the new grass material."
                    )
            else:
                # Validate name
                terrain = app.get_outdoor()
                if terrain.hasgrowthmaterialwithname(name):
                    cjr.show_error_message(
                        "There is already a grass material called '%s'." % name
                        )
                else:
                    # Add copy of material
                    terrain.addcopyofgrowthmaterial(
                        material, 
                        name
                        )
                    self.__build_material_list()
        dlg.Destroy()

    def on_delete_material(self, event):
        material_id = self.__get_id_for_selected_material()
        # don't allow the user to delete the default material
        if material_id > 0: 
            # Ask for confirmation
            name = self.__get_name_of_selected_material()
            msg = "Are you sure you want to delete the '%s' grass material?" \
                        % name
            result = cjr.confirm_yes_no(self, msg)
            if result == wx.ID_YES:
                # Delete material
                terrain = app.get_outdoor()
                terrain.deletegrowthmaterial( material_id )
                self.__build_material_list()

    def on_click_close_button(self, event):
        self.Close()

    def on_select_material(self, event):
        if self.material_selected != event.GetEventObject():
            if self.material_selected is not None:
                self.material_selected.deselect()
            self.material_selected = event.GetEventObject()
            material_id = self.__get_id_for_selected_material()
            tool = trn.get_terrain_tool('toolGrass')
            tool.setgrassid( material_id )
            self.__update_buttons()

    def __add_material_ctrl(self, material_id, material):
        material_ctrl = GrassMaterialCtrl(
                                self.scroll_materials, 
                                -1, 
                                material_id, 
                                material
                                )
        self.material_ctrls.append(material_ctrl)
        self.Bind(EVT_SELECT, self.on_select_material, material_ctrl)
        return material_ctrl

    def __delete_material_ctrl(self, material_ctrl):
        self.Unbind(EVT_SELECT, material_ctrl)
        if self.material_selected == material_ctrl:
            self.material_selected = None
        material_ctrl.Destroy()

    def __get_material_ctrls_sorted_by_name(self):
        """sort alphabetically, ignoring case, but with Default always first"""
        def material_sort(x, y): 
            x_name = x.get_material_name().lower()
            y_name = y.get_material_name().lower()
            if x_name == "default":
                return -1
            else:
                if y_name == "default":
                    return 1
                else:
                    return cmp(x_name, y_name)
        return sorted(
            self.material_ctrls,
            material_sort
            )

    def __build_material_list(self):
        self.Freeze()
        self.__update_material_ctrls()
        self.__refresh_material_list()
        self.Thaw()

    def __update_material_ctrls(self):
        """Build the list of controls for display"""
        for each_control in self.material_ctrls:
            self.__delete_material_ctrl(each_control)
        self.material_ctrls = []
        outdoor = app.get_outdoor()
        if outdoor is not None:
            for material_index in range( outdoor.getnumberofgrowthmaterials() ):
                material = outdoor.getgrowthmaterialbyid(material_index)
                self.__add_material_ctrl(material_index, material)

    def __refresh_material_list(self):
        # clean up materials subwindow
        sizer_materials = wx.BoxSizer(wx.VERTICAL)
        # rebuild materials subwindow
        for each_material in self.__get_material_ctrls_sorted_by_name():
            sizer_materials.Add(
                each_material, 
                0, 
                wx.EXPAND
                )
            self.scroll_materials.SetSizerAndFit(sizer_materials)
        # refresh materials subwindow
        self.Layout()
        self.__update_buttons()

    def on_material_colour(self, event):
        material_id = self.__get_id_for_selected_material()
        # don't allow the user to change the colour of the default material
        if material_id > 0:
            # ok, pass this on to the control
            self.material_selected.on_select_colour()

    def on_material_brushes(self, event):
        material_id = self.__get_id_for_selected_material()
        # don't allow the user to associate brushes with the default material
        if material_id > 0:
            # ok, pass this on to the control
            self.material_selected.edit_brushes()

    def persist(self):
        return [
            create_window, 
            (),  # no parameters for create function
            None
            ]

    def __get_id_for_selected_material(self):
        if self.material_selected is None:
            return None
        else:
            return self.material_selected.get_material_id()

    def __get_name_of_selected_material(self):
        if self.material_selected is None:
            return ''
        else:
            return self.material_selected.get_material_name()

    def __update_buttons(self):
        material_id = self.__get_id_for_selected_material()
        enable = (material_id is not None and material_id != 0)
        self.button_copy.Enable( enable )        
        self.button_colour.Enable( enable )
        self.button_brushes.Enable( enable )
        self.button_delete.Enable( enable )


# create_window function
def create_window(parent):
    return GrassMaterialsDialog(parent)

