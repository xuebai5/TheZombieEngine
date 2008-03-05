##\file preloaddlg.py
##\brief Dialog to set resources to preload for current level

import wx

import servers

import conjurerframework as cjr
import conjurerconfig as cfg

class PreloadDialog(wx.Dialog):
    """Dialog to set resources to preload for current level"""
    
    # Indexes on choice control to select each resource list 
    ClassResources = 0
    SoundResources = 1
    
    # Some GUI labels especific for each resource type
    ResourceLabel = ['class', 'sound']
    
    def __init__(self, parent):
        wx.Dialog.__init__(
            self,
            parent, 
            -1, 
            "Preload manager",
            style=wx.DEFAULT_DIALOG_STYLE|
                        wx.RESIZE_BORDER|
                        wx.TAB_TRAVERSAL
            )
        
        # Copy of the resource lists contained in the preload manager
        # The user manages these ones and only on accepting changes they're
        # copied back to the preload manager
        self.resource_lists = {
            self.ClassResources: self.__get_class_resources(),
            self.SoundResources: self.__get_sound_resources()
            }
        
        # Controls
        self.label = wx.StaticText(self, -1, "Resources to preload")
        self.choice_type = wx.Choice(
                                    self, 
                                    -1, 
                                    choices=['Classes', 'Sounds']
                                    )
        self.listbox_resources = wx.ListBox(self, -1)
        self.button_add = wx.Button(self, -1, "&Add")
        self.button_remove = wx.Button(self, -1, "&Remove")
        self.static_line = wx.StaticLine(self, -1)
        self.button_ok = wx.Button(self, -1, "&OK")
        self.button_cancel = wx.Button(self, wx.ID_CANCEL, "&Cancel")
        
        self.__set_properties()
        self.__do_layout()
        self.__bind_events()
    
    def __set_properties(self):
        # Show class resources by default
        self.choice_type.SetSelection( self.ClassResources )
        self.__fill_resources_listbox( self.ClassResources )
    
    def __do_layout(self):
        sizer_main = wx.BoxSizer(wx.VERTICAL)
        sizer_ok_cancel = wx.BoxSizer(wx.HORIZONTAL)
        sizer_resources = wx.BoxSizer(wx.VERTICAL)
        sizer_list_and_buttons = wx.BoxSizer(wx.HORIZONTAL)
        sizer_add_remove = wx.BoxSizer(wx.VERTICAL)
        sizer_resources.Add(
            self.label, 
            0, 
            wx.ADJUST_MINSIZE,
            0
            )
        sizer_resources.Add(
            self.choice_type, 
            0, 
            wx.TOP|wx.EXPAND|wx.ADJUST_MINSIZE, 
            cfg.BORDER_WIDTH
            )
        sizer_list_and_buttons.Add(
            self.listbox_resources,
            1, 
            wx.TOP|wx.EXPAND|wx.ADJUST_MINSIZE, 
            cfg.BORDER_WIDTH
            )
        sizer_add_remove.Add(
            self.button_add,
            0, 
            wx.ADJUST_MINSIZE
            )
        sizer_add_remove.Add(
            self.button_remove, 
            0,
            wx.TOP|wx.ADJUST_MINSIZE,
            cfg.BORDER_WIDTH
            )
        sizer_list_and_buttons.Add(
            sizer_add_remove, 
            0, 
            wx.TOP|wx.LEFT, 
            cfg.BORDER_WIDTH
            )
        sizer_resources.Add(
            sizer_list_and_buttons, 
            1, 
            wx.EXPAND
            )
        sizer_main.Add(
            sizer_resources,
            1, 
            wx.ALL|wx.EXPAND, 
            cfg.BORDER_WIDTH
            )
        sizer_main.Add(
            self.static_line,
            0,
            wx.LEFT|wx.RIGHT|wx.BOTTOM|wx.EXPAND, 
            cfg.BORDER_WIDTH
            )
        sizer_ok_cancel.Add(
            self.button_ok,
            0, 
            wx.ADJUST_MINSIZE
            )
        sizer_ok_cancel.Add(
            self.button_cancel,
            0, 
            wx.LEFT|wx.ADJUST_MINSIZE,
            cfg.BORDER_WIDTH
            )
        sizer_main.Add(
            sizer_ok_cancel,
            0, 
            wx.LEFT|wx.RIGHT|wx.BOTTOM|wx.ALIGN_RIGHT, 
            cfg.BORDER_WIDTH
            )
        self.SetSizerAndFit(sizer_main)

    def __bind_events(self):
        self.Bind(wx.EVT_CHOICE, self.on_choice_resource_type, self.choice_type)
        self.Bind(wx.EVT_BUTTON, self.on_add_resource, self.button_add)
        self.Bind(wx.EVT_BUTTON, self.on_remove_resource, self.button_remove)
        self.Bind(wx.EVT_BUTTON, self.on_ok, self.button_ok)
        self.Bind(wx.EVT_BUTTON, self.on_cancel, self.button_cancel)

    def __get_class_resources(self):
        """Get from the preload manager the list of class resources"""
        resources = []
        preload_manager = servers.get_preload_manager()
        for i in range( preload_manager.getnumclassestopreload() ):
            resources.append( preload_manager.getclasstopreload(i) )
        return resources

    def __get_sound_resources(self):
        """ Get from the preload manager the list of sound resources. """
        resources = []
        preload_manager = servers.get_preload_manager()
        for i in range( preload_manager.getnumsoundstopreload() ):
            resources.append( preload_manager.getsoundtopreload(i) )
        return resources

    def __set_resources(self, resource_type):
        """ Update the preload manager with the resources
            stored in this dialog. """
        if resource_type == self.ClassResources:
            self.__set_class_resources()
        elif resource_type == self.SoundResources:
            self.__set_sound_resources()

    def __set_class_resources(self):
        """ Update the preload manager with the class 
            resources stored in this dialog. """
        resources = self.resource_lists[self.ClassResources]
        preload_manager = servers.get_preload_manager()
        preload_manager.beginclassestopreload()
        for i in range( len(resources) ):
            preload_manager.addclasstopreload( str(resources[i]) )
        preload_manager.endclassestopreload()

    def __set_sound_resources(self):
        """ Update the preload manager with the sound 
            resources stored in this dialog. """
        resources = self.resource_lists[self.SoundResources]
        preload_manager = servers.get_preload_manager()
        preload_manager.beginsoundstopreload()
        for i in range( len(resources) ):
            preload_manager.addsoundtopreload( str(resources[i]) )
        preload_manager.endsoundstopreload()

    def __fill_resources_listbox(self, resource_type):
        """Show in the resources listbox the resources of the specified type"""
        self.listbox_resources.Clear()
        self.listbox_resources.AppendItems( self.resource_lists[resource_type] )

    def on_choice_resource_type(self, event):
        """ Show in the resources listbox the resources 
            of the type chosen by the user."""
        resource_type = self.choice_type.GetSelection()
        self.__fill_resources_listbox( resource_type )

    def on_add_resource(self, event):
        """Ask the user for a new resource to add in the preload list"""
        resource_type = self.choice_type.GetSelection()
        resource_label = self.ResourceLabel[ resource_type ]
        dlg = wx.TextEntryDialog(
                    self, 
                    "Enter %s to preload" % resource_label,
                    "Add %s" % resource_label 
                    )
        if dlg.ShowModal() == wx.ID_OK:
            resource = dlg.GetValue()
            if resource != "":
                if self.resource_lists[resource_type].count(resource) == 0:
                    self.resource_lists[ resource_type ].append( resource )
                    self.listbox_resources.Append( resource )
                else:
                    cjr.show_error_message(
                        resource_label.capitalize() + \
                        " already exist"
                        )
        dlg.Destroy()

    def on_remove_resource(self, event):
        """Remove from the preload list the resource chosen by the user"""
        resource_index = self.listbox_resources.GetSelection()
        if resource_index != wx.NOT_FOUND:
            resource_type = self.choice_type.GetSelection()
            self.resource_lists[ resource_type ].pop( resource_index )
            self.listbox_resources.Delete( resource_index )

    def on_ok(self, event):
        """ Set the new resources in the preload manager 
            and close this dialog reporting ok. """
        for resource_type in self.resource_lists.keys():
            self.__set_resources( resource_type )
        self.EndModal(wx.ID_OK)

    def on_cancel(self, event):
        """Close this dialog reporting cancel"""
        self.EndModal(wx.ID_CANCEL)
