##\file expentdlg.py
##\brief Export entity dialog

import wx
import wx.lib.filebrowsebutton as filebrowse
import servers
import pynebula
import nebulaguisettings as guisettings

import conjurerframework as cjr
import conjurerconfig as cfg

CJR_NORMAL_ENTITY_TYPE = 0

class ExportEntityDialog(wx.Dialog):
    """Dialog to export entities to file."""
    def __init__(self, parent, entity_list = []):
        wx.Dialog.__init__(
            self, 
            parent, 
            -1, 
            "Export selected entities",
            style = wx.DEFAULT_DIALOG_STYLE|
                        wx.RESIZE_BORDER|
                        wx.TAB_TRAVERSAL|
                        wx.MAXIMIZE_BOX|
                        wx.MINIMIZE_BOX
            )
        self.entity_list = entity_list
        self.button_select_all = wx.Button(self, -1, "Select All") 
        self.button_clear_all = wx.Button(self, -1, "Clear All")
        self.checklist_entity =  wx.CheckListBox(self, -1)
        default_path = guisettings.Repository.getsettingvalue(
                                guisettings.ID_DefaultEntityExportPath
                                )
        self.button_browse_file = filebrowse.FileBrowseButton(
                                            self,
                                            -1,
                                            size = (400, -1),
                                            labelText = "Save to ",
                                            buttonText = "Browse",
                                            startDirectory = default_path,
                                            initialValue = default_path + "\entities.n2",
                                            fileMask = "*.n2"
                                            )
        self.button_ok = wx.Button(self, -1, "&OK")        
        self.button_cancel = wx.Button(self, -1, "&Cancel")
        self.set_properties()
        self.do_layout()
        self.bind_events()

    def set_properties(self):
        for each_entity in self.entity_list:
            self.checklist_entity.Append(
                "%i (%s)" % ( each_entity.getid(), each_entity.getclass() ) 
                )
        self.select_all_entities(True)            

    def select_all_entities(self, boolean_value):
        #either select all or clear all, depending on the boolean value
        for index in range( self.checklist_entity.GetCount() ):
            self.checklist_entity.Check(index, boolean_value)

    def get_selected_entities(self):
        result_list = []
        for index in range( len(self.entity_list) ):
            if self.checklist_entity.IsChecked(index):
                result_list.append( self.entity_list[index] )
        return result_list

    def on_select_all_button_clicked(self, event):
        self.select_all_entities(True)
        
    def on_clear_all_button_clicked(self, event):
        self.select_all_entities(False)
            
    def on_cancel_button_clicked(self, event):
        self.Close()

    def on_ok_button_clicked(self, event):
        result_list = self.get_selected_entities()
        if result_list:
            if self.has_valid_path_name():
                #we need to save the entity id counter to avoid conflicts
                servers.get_entity_object_server().saveconfig()
                object_instancer = pynebula.new('nobjectinstancer')
                object_instancer.init(
                    len(result_list), 
                    len(result_list) 
                    )
                object_instancer.setentityobjecttype(
                    CJR_NORMAL_ENTITY_TYPE
                    )
                for each_entity in result_list:
                    entity_id_as_string = str( each_entity.getid() )
                    object_instancer.appendwithname(
                        each_entity, 
                        entity_id_as_string
                        )
                object_instancer.saveas(
                    self.get_path_name() 
                    )
                pynebula.delete(object_instancer)
                self.Close()
            else: 
                cjr.show_error_message(
                    "Please enter a valid path name"
                    )
        else:
            cjr.show_error_message(
                "Please select at least one entity to export"
                )

    def get_path_name(self):
        return str( self.button_browse_file.GetValue() )

    def has_valid_path_name(self):
        return self.get_path_name()

    def do_layout(self):
        outer_sizer = wx.BoxSizer(wx.VERTICAL)
        sizer_top_buttons = wx.BoxSizer(wx.HORIZONTAL)
        sizer_file_browser = wx.BoxSizer(wx.HORIZONTAL)
        sizer_ok_cancel_buttons = wx.BoxSizer(wx.HORIZONTAL)
        sizer_top_buttons.Add(
            self.button_select_all
            )
        sizer_top_buttons.Add(
            self.button_clear_all,
            0,
            wx.LEFT, 
            cfg.BORDER_WIDTH
            )
        sizer_ok_cancel_buttons.Add(
            self.button_ok
            )
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
            self.checklist_entity, 
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
        sizer_file_browser.Add(
            self.button_browse_file,
            1, 
            wx.EXPAND
            )
        outer_sizer.Add(
            sizer_file_browser, 
            0,
            wx.ALL|wx.EXPAND, 
            cfg.BORDER_WIDTH
            )
        horizontal_line2 = wx.StaticLine(
                                    self, 
                                    -1
                                    )
        outer_sizer.Add(
            horizontal_line2,
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
        self.SetSizerAndFit(outer_sizer)

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


# createWindow function
def create_window(parent, entity_list):
    return ExportEntityDialog(parent, entity_list)
