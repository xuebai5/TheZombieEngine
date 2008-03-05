##\file trnimportmatsdlg.py
##\brief Dialog to import materials from a terrain class

import wx

import app
import servers
import waitdlg
import conjurerconfig as cfg
import conjurerframework as cjr


# AbstractImportMaterialsDialog class
class AbstractImportMaterialsDialog(wx.Dialog):
    """ Abstract class for dialogs designed to import 
    materials from a terrain class """

    def __init__(self, parent):
        wx.Dialog.__init__(
            self, 
            parent, 
            -1, 
            self.title_text(),
            style=cfg.DEFAULT_DIALOG_STYLE
            )
        self.label = wx.StaticText(
                            self, 
                            -1, 
                            "Select the terrain class to import the materials from  "
                            )
        self.list = wx.ListBox(self, -1, choices=[], style=wx.LB_SORT)
        self.button_ok = wx.Button(self, -1, "&OK")
        self.button_cancel = wx.Button(self, wx.ID_CANCEL, "&Cancel")

        self.__set_properties()
        self.__do_layout()
        self.__bind_events()

    def title_text(self):
        """ Should be implemented by subclass """
        return ""

    def importing_materials_text(self):
        """ Should be implemented by subclass """
        return ""

    def import_materials_to_outdoor(self, outdoor):
        """ Should be implemented by subclass """
        return False

    def __set_properties(self):
        # Terrain classes list
        ec_server = servers.get_entity_class_server()
        outdoors = ec_server.getentityclass("neoutdoor")
         # make sure all subclasses are loaded...
        ec_server.loadentitysubclasses(outdoors)
        outdoor = outdoors.gethead()
        while outdoor is not None:
            self.list.Append( outdoor.getname() )
            outdoor = outdoor.getsucc()
        self.update_ok_button()
        # Default button for return key press
        self.button_ok.SetDefault()

    def __do_layout(self):
        sizer_layout = wx.BoxSizer(wx.VERTICAL)
        sizer_buttons = wx.BoxSizer(wx.HORIZONTAL)
        sizer_layout.Add(
            self.label, 
            0, 
            wx.FIXED_MINSIZE|wx.ALL, 
            cfg.BORDER_WIDTH
            )
        sizer_layout.Add(
            self.list, 
            1, 
            wx.LEFT|wx.RIGHT|wx.EXPAND,
            cfg.BORDER_WIDTH
            )
        horizontal_line = wx.StaticLine(
                                    self, 
                                    -1
                                    )
        sizer_layout.Add(
            horizontal_line, 
            0, 
            wx.EXPAND|wx.LEFT|wx.RIGHT|wx.TOP, 
            cfg.BORDER_WIDTH
            )
        sizer_buttons.Add(self.button_ok, 1, wx.FIXED_MINSIZE)
        sizer_buttons.Add(
            self.button_cancel, 
            1, 
            wx.LEFT|wx.FIXED_MINSIZE, 
            cfg.BORDER_WIDTH
            )
        sizer_layout.Add(
            sizer_buttons, 
            0, 
            wx.ALL|wx.ALIGN_RIGHT, 
            cfg.BORDER_WIDTH
            )
        # Set the sizer and make sure the dialog cannot be
        # made any smaller than its appropriate minimum size
        self.SetSizerAndFit(sizer_layout)
        # Set the initial size so that the title is visible
        self.SetSize( (250, 300) )

    def __bind_events(self):
        self.Bind(wx.EVT_LISTBOX, self.on_select_class, self.list)
        self.Bind(wx.EVT_LISTBOX_DCLICK, self.on_ok, self.list)
        self.Bind(wx.EVT_BUTTON, self.on_ok, self.button_ok)
        self.Bind(wx.EVT_BUTTON, self.on_cancel, self.button_cancel)

    def has_terrain_class_selected(self):
        return self.list.GetSelection() != wx.NOT_FOUND

    def get_selected_terrain_class(self):
        return self.list.GetStringSelection()

    def get_selected_terrain_class_as_string(self):
        return str( self.get_selected_terrain_class() )

    def update_ok_button(self):
        self.button_ok.Enable( self.has_terrain_class_selected() )

    def on_select_class(self, event):
        self.update_ok_button()
        event.Skip()

    def on_ok(self, event):
        outdoor = app.get_outdoor()
        if outdoor is None:
            cjr.show_error_message("The level has no terrain instance.")
            return
        if self.has_terrain_class_selected():
            # Import terrain materials
            try:
                dlg = waitdlg.WaitDialog(
                            self.GetParent(),
                            self.importing_materials_text()
                            )
                imported_ok = self.import_materials_to_outdoor(outdoor)
            finally:
                dlg.Destroy()
            if imported_ok:
                # Closes dialog reporting OK
                self.EndModal(wx.ID_OK)
        else:
            cjr.show_error_message(
                "Please select a terrain class from the list."
                )

    def on_cancel(self, event):
        self.EndModal(wx.ID_CANCEL)


# ImportGrassMaterialsDialog class
class ImportGrassMaterialsDialog(AbstractImportMaterialsDialog):
    """Dialog to import grass materials from a terrain class"""
    
    def title_text(self):
        return "Import grass materials"

    def importing_materials_text(self):
        return "Importing grass materials..." 

    def import_materials_to_outdoor(self, outdoor):
        terrain_class_name = self.get_selected_terrain_class_as_string() 
        ec_server = servers.get_entity_class_server()
        source_outdoor =  ec_server.getentityclass(
                                    terrain_class_name
                                    )
        number_of_materials = source_outdoor.getnumberofgrowthmaterials()
        material_list = []
        for each_index in range(number_of_materials):
            material = source_outdoor.getgrowthmaterialbyid(each_index) 
            # do NOT add the default grass material since it cannot be imported
            if material.getname() != "Default":
                material_list.append(material)
        if len(material_list) == 0:
            cjr.show_error_message(
                "'%s' outdoor has no grass materials suitable for import." %
                    terrain_class_name
                )
            result = False
        else:
            dlg = MaterialsForImportSelectionDialog(
                self,
                outdoor, 
                material_list
                )
            result = ( dlg.ShowModal() == wx.ID_OK )
            dlg.Destroy()
        return result

# ImportTerrainMaterialsDialog class
class ImportTerrainMaterialsDialog(AbstractImportMaterialsDialog):
    """Dialog to import terrain materials from a terrain class"""

    def title_text(self):
        return "Import terrain materials"

    def importing_materials_text(self):
        return "Importing terrain materials..." 

    def import_materials_to_outdoor(self, outdoor):
        ec_server = servers.get_entity_class_server()
        entity_class =  ec_server.getentityclass(
                                self.get_selected_terrain_class_as_string() 
                                )
        return outdoor.importmaterials(entity_class)


class MaterialsForImportSelectionDialog(wx.Dialog):
    """Dialog to select materials for import."""
    def __init__(self, parent, target_outdoor, material_list):
        wx.Dialog.__init__(
            self, 
            parent, 
            -1, 
            "Import selected materials to %s" % target_outdoor.getname(),
            style = cfg.DEFAULT_DIALOG_STYLE
            )
        self.target_outdoor = target_outdoor
        self.material_list = material_list
        self.button_select_all = wx.Button(self, -1, "Select &All") 
        self.button_clear_all = wx.Button(self, -1, "Clea&r All")
        self.check_list_materials =  wx.CheckListBox(self, -1)
        self.button_ok = wx.Button(self, -1, "&OK")
        self.button_cancel = wx.Button(self, -1, "&Cancel")
        self.set_properties()
        self.do_layout()
        self.bind_events()

    def set_properties(self):
        for each_material in self.material_list:
            self.check_list_materials.Append(
                each_material.getname() 
                ) 
        self.select_all_materials(True)

    def select_all_materials(self, boolean_value):
        #either select all or clear all, depending on the boolean value
        for index in range( self.check_list_materials.GetCount() ):
            self.check_list_materials.Check(index, boolean_value)

    def get_selected_materials(self):
        list_of_selected_materials = []
        for index in range( len(self.material_list) ):
            if self.check_list_materials.IsChecked(index):
                list_of_selected_materials.append( self.material_list[index] )
        return list_of_selected_materials

    def on_select_all_button_clicked(self, event):
        self.select_all_materials(True)

    def on_clear_all_button_clicked(self, event):
        self.select_all_materials(False)

    def on_cancel_button_clicked(self, event):
        self.Close()

    def on_ok_button_clicked(self, event):
        selected_materials = self.get_selected_materials()
        if selected_materials:
            overwrite_all = False
            for each_material in selected_materials:
                overwrite_this = False
                material_name = each_material.getname()
                has_match = self.target_outdoor.hasgrowthmaterialwithname(
                                        material_name
                                        )
                if has_match:
                    if overwrite_all:
                        overwrite_this = True
                    else:
                        dlg = ConfirmReplaceMaterialDialog(
                                    self,
                                    self.target_outdoor.getname(),
                                    material_name
                                    )
                        result = dlg.ShowModal()
                        dlg.Destroy()
                        if result == wx.ID_YESTOALL:
                            overwrite_all = True
                            overwrite_this = True
                        if result == wx.ID_YES:
                            overwrite_this = True
                        if result == wx.ID_CANCEL:
                            break
                else:
                    overwrite_this = True
                if overwrite_this:
                    result = self.target_outdoor.addcopyofgrowthmaterial(
                                    each_material,
                                    material_name
                                    )
                    if not result:
                        cjr.show_error_message(
                                "Failed to import grass material %s" % 
                                material_name
                            )
            self.EndModal(wx.ID_OK)
        else:
            cjr.show_error_message(
                "Please select at least one material to import"
                )

    def do_layout(self):
        outer_sizer = wx.BoxSizer(wx.VERTICAL)
        sizer_top_buttons = wx.BoxSizer(wx.HORIZONTAL)
        sizer_ok_cancel_buttons = wx.BoxSizer(wx.HORIZONTAL)
        sizer_top_buttons.Add(self.button_select_all)
        sizer_top_buttons.Add(
            self.button_clear_all, 
            0,
            wx.LEFT, 
            cfg.BORDER_WIDTH
            )
        sizer_ok_cancel_buttons.Add(self.button_ok)
        sizer_ok_cancel_buttons.Add(
            self.button_cancel, 
            0, 
            wx.LEFT, 
            cfg.BORDER_WIDTH
            )
        outer_sizer.Add(
            sizer_top_buttons, 
            0, 
            wx.ALIGN_LEFT|wx.LEFT|wx.TOP|wx.RIGHT, 
            cfg.BORDER_WIDTH
            )
        outer_sizer.Add(
            self.check_list_materials, 
            1, 
            wx.ALL|wx.EXPAND,
            cfg.BORDER_WIDTH
            )
        horizontal_line = wx.StaticLine(
                                    self, 
                                    -1
                                    )
        outer_sizer.Add(
            horizontal_line, 
            0, 
            wx.EXPAND|wx.LEFT|wx.RIGHT, 
            cfg.BORDER_WIDTH
            )
        outer_sizer.Add(
            sizer_ok_cancel_buttons, 
            0, 
            wx.ALIGN_RIGHT|wx.ALL, 
            cfg.BORDER_WIDTH
            )
        # Set the sizer and make sure the dialog cannot be
        # made any smaller than its appropriate minimum size
        self.SetSizerAndFit(outer_sizer)
        # Set the initial size so that the title is visible
        self.SetSize( (350, 300) )

    def bind_events(self):
        self.Bind(
            wx.EVT_BUTTON, 
            self.on_select_all_button_clicked, 
            self.button_select_all
            )
        self.Bind(
            wx.EVT_BUTTON, 
            self.on_clear_all_button_clicked, 
            self.button_clear_all
            )
        self.Bind(
            wx.EVT_BUTTON, 
            self.on_ok_button_clicked, 
            self.button_ok
            )
        self.Bind(
            wx.EVT_BUTTON, 
            self.on_cancel_button_clicked, 
            self.button_cancel
            )


class ConfirmReplaceMaterialDialog(wx.Dialog):
    """ Dialog to ask user if should replace existing material. """
    def __init__(self, parent, target_outdoor_name, material_name):
        wx.Dialog.__init__(
            self, 
            parent, 
            -1, 
            "Confirm Material Replace"
            )
        bitmap = wx.ArtProvider_GetBitmap(
                        wx.ART_WARNING, 
                        wx.ART_MESSAGE_BOX,
                        (32,32)
                        )
        self.icon_warning = wx.StaticBitmap(
                                        self, 
                                        -1,
                                        bitmap
                                        )
        label_text = "The '%s' outdoor already has a grass material "\
                            "called '%s'.\n\n"\
                            "Would you like to replace the existing material?" \
                                % (target_outdoor_name, material_name)
        self.label = wx.StaticText(
                            self, 
                            -1, 
                            label_text
                            )
        self.button_yes = wx.Button(self, -1, "&Yes") 
        self.button_yes_to_all = wx.Button(self, -1, "Yes to &All")
        self.button_no = wx.Button(self, -1, "&No")
        self.button_cancel = wx.Button(self, -1, "&Cancel")

        self.do_layout()
        self.bind_events()

    def on_yes_button_clicked(self, event):
        self.EndModal(wx.ID_YES)

    def on_yes_to_all_button_clicked(self, event):
        self.EndModal(wx.ID_YESTOALL)

    def on_no_button_clicked(self, event):
        self.EndModal(wx.ID_NO)

    def on_cancel_button_clicked(self, event):
        self.EndModal(wx.ID_CANCEL)
    
    def do_layout(self):
        outer_sizer = wx.BoxSizer(wx.VERTICAL)
        sizer_warning = wx.BoxSizer(wx.HORIZONTAL)
        sizer_buttons = wx.BoxSizer(wx.HORIZONTAL)
        sizer_warning.Add(
            self.icon_warning, 
            0,
            wx.ALL, 
            cfg.BORDER_WIDTH
            )
        sizer_warning.Add(
            self.label,
            0,
            wx.ALL, 
            cfg.BORDER_WIDTH
            )
        outer_sizer.Add(
            sizer_warning, 
            1, 
            wx.ALL, 
            cfg.BORDER_WIDTH
            )
        sizer_buttons.Add(
            self.button_yes
            )
        sizer_buttons.Add(
            self.button_yes_to_all, 
            0, 
            wx.LEFT, 
            cfg.BORDER_WIDTH
            )
        sizer_buttons.Add(
            self.button_no, 
            0, 
            wx.LEFT, 
            cfg.BORDER_WIDTH
            )
        sizer_buttons.Add(
            self.button_cancel, 
            0, 
            wx.LEFT, 
            cfg.BORDER_WIDTH
            )
        outer_sizer.Add(
            sizer_buttons, 
            0, 
            wx.ALIGN_CENTRE|wx.ALL, 
            cfg.BORDER_WIDTH * 2
            )
        # Set the sizer and make sure the dialog cannot be
        # made any smaller than its appropriate minimum size
        self.SetSizerAndFit(outer_sizer)

    def bind_events(self):
        self.Bind(
            wx.EVT_BUTTON, 
            self.on_yes_button_clicked, 
            self.button_yes
            )
        self.Bind(
            wx.EVT_BUTTON, 
            self.on_yes_to_all_button_clicked, 
            self.button_yes_to_all
            )
        self.Bind(
            wx.EVT_BUTTON, 
            self.on_no_button_clicked, 
            self.button_no
            )
        self.Bind(
            wx.EVT_BUTTON, 
            self.on_cancel_button_clicked, 
            self.button_cancel
            )


    