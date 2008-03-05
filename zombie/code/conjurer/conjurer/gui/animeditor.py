##\file animeditor.py
##\brief Specific animation selector editor panel

import wx

import app
import editorpanel
import events
import floatslider
import grimoire
import servers
import vtextctrl

import conjurerframework as cjr


# DeleteAnimDialog class
class DeleteAnimDialog(wx.Dialog):
    """Dialog to delete an animation"""

    def __init__(self, parent, character):
        wx.Dialog.__init__(self, parent, -1, "Delete an animation",
            style = wx.DEFAULT_DIALOG_STYLE|wx.RESIZE_BORDER|wx.TAB_TRAVERSAL)

        self.label = wx.StaticText(self, -1, "Select a non active animation:")
        self.list = wx.ListBox(self, -1)
        self.button_delete = wx.Button(self, -1, "Delete animation")
        self.button_cancel = wx.Button(self, -1, "Cancel")

        self.deleted_anim_index = (-1,False)
        self.character = character
        self.anim_indices = []

        self.__set_properties()
        self.__do_layout()
        self.__bind_events()

    def __set_properties(self):
        skeleton = self.__get_skeleton()
        # First person animations
        for i in range( self.character.getfirstpersonnumberstates() ):
            index = self.list.Append( self.character.getfirstpersonstatename(i) )
            self.anim_indices.insert(index, (i, True))
        # Third person animations
        for i in range( skeleton.getnumstates() ):
            index = self.list.Append( skeleton.getstatename(i) )
            self.anim_indices.insert(index, (i, False))
        self.__update_button_delete()

    def __do_layout(self):
        sizer = wx.BoxSizer(wx.VERTICAL)
        sizer_list = wx.BoxSizer(wx.VERTICAL)
        sizer_list.Add(self.label, 0, wx.FIXED_MINSIZE, 0)
        sizer_list.Add(self.list, 1, wx.TOP|wx.EXPAND, 5)
        sizer.Add(sizer_list, 1, wx.ALL|wx.EXPAND, 10)
        sizer_buttons = wx.BoxSizer(wx.HORIZONTAL)
        sizer_buttons.Add(self.button_delete, 1, wx.FIXED_MINSIZE, 0)
        sizer_buttons.Add(self.button_cancel, 1, wx.LEFT|wx.FIXED_MINSIZE, 5)
        sizer.Add(sizer_buttons, 0, wx.LEFT|wx.RIGHT|wx.BOTTOM|wx.EXPAND, 10)
        self.SetAutoLayout(True)
        self.SetSizer(sizer)
        sizer.Fit(self)
        sizer.SetSizeHints(self)
        self.Layout()

    def __bind_events(self):
        self.Bind(wx.EVT_LISTBOX, self.on_select_anim, self.list)
        self.Bind(wx.EVT_BUTTON, self.on_delete_anim, self.button_delete)
        self.Bind(wx.EVT_BUTTON, self.on_cancel, self.button_cancel)

    def __get_skeleton(self):
        skeleton = self.character.getentityclass()
        if self.character.isa('necharacter'):
            skeleton = servers.get_entity_class_server().getentityclass(
                skeleton.getskeletonclass() )
        return skeleton

    def __update_button_delete(self):
        self.button_delete.Enable( self.list.GetSelection() != wx.NOT_FOUND )

    def on_select_anim(self, event):
        self.__update_button_delete()

    def on_delete_anim(self, event):
        index = self.list.GetSelection()
        if index != wx.NOT_FOUND:
            name = self.list.GetStringSelection()
            msg = "Deleting an animation cannot be undone.\n\n" \
                "Are you sure you want to delete the animation %s'?" % name
            delete = cjr.confirm_yes_no(None, msg)
            if delete == wx.ID_YES:
                anim_index = self.anim_indices[index]
                deleted = self.character.removeanimstate(
                                anim_index[1], 
                                anim_index[0] 
                                )
                if not deleted:
                    cjr.show_error_message(
                        "Cannot delete the '%s' animation.\n" \
                        "Maybe is it active on some character?" % name
                        )
                else:
                    self.deleted_anim_index = anim_index
                    self.EndModal(wx.ID_OK)

    def on_cancel(self, event):
        self.EndModal(wx.ID_CANCEL)

    def get_deleted_anim_index(self):
        return self.deleted_anim_index[0]


# AnimationPanel class
class AnimationPanel(editorpanel.EditorPanel):
    """Specific character animation selector editor panel"""
    
    # Animations list columns indices
    ID_Anim = 0
    ID_FadeIn = 1
    ID_Person = 2
    
    def __init__(self, *args, **kwds):
        editorpanel.EditorPanel.__init__(self, *args, **kwds)
        self.character = None
        self.anim_indices = []
        self.filter_name = ""
        
        self.filterlabel = wx.StaticText(self, -1, "Filter:")
        self.filter = wx.TextCtrl(self, -1)
        self.label = wx.StaticText(self, -1, "Animation played:")
        self.list = wx.ListCtrl(self, -1,
            style=wx.LC_REPORT|wx.LC_SINGLE_SEL|wx.LC_SORT_ASCENDING)
        self.text_name = vtextctrl.ValidatedTextCtrl(self, -1, "Name:",
            validator=self.__validate_anim_name)
        self.slider_fade_in = floatslider.FloatSlider(
            self, -1, "Fade in time:", 1.0, 0.0, 2.0, precision=100.0
            )
        self.check_backwards = wx.CheckBox(self, -1, "Play backwards")
        self.check_loop = wx.CheckBox(self, -1, "Loop")
        self.button_person = wx.Button(self, -1, "Switch to first person")
        self.button_del = wx.Button(self, -1, "Delete an animation...")
        
        self.__set_properties()
        self.__do_layout()
        self.__bind_events()
    
    def __set_properties(self):
        self.SetScrollRate(10, 10)
        
        # Animations list columns
        self.list.InsertColumn(self.ID_Anim, "Animation")
        self.list.InsertColumn(self.ID_FadeIn, "Fade in", wx.LIST_FORMAT_RIGHT)
        self.list.InsertColumn(self.ID_Person, "Person")
        
        # Default play options
        self.check_backwards.SetValue(False)
        self.check_loop.SetValue(False)
    
    def __do_layout(self):
        sizer = wx.BoxSizer(wx.VERTICAL)
        sizer_hor = wx.BoxSizer(wx.HORIZONTAL)
        sizer_hor.Add(self.filterlabel, 0, wx.RIGHT | wx.LEFT | wx.EXPAND, 0)
        sizer_hor.Add(self.filter, 0, wx.RIGHT | wx.LEFT | wx.EXPAND, 0)
        sizer.Add(sizer_hor, 0, wx.TOP | wx.BOTTOM | wx.EXPAND, 0)
        sizer_list = wx.BoxSizer(wx.VERTICAL)
        sizer_list.Add(self.label, 0, wx.FIXED_MINSIZE, 0)
        sizer_list.Add(self.list, 1, wx.TOP|wx.EXPAND, 5)
        sizer.Add(sizer_list, 1, wx.ALL|wx.EXPAND, 10)
        sizer.Add(self.text_name, 0, wx.LEFT|wx.RIGHT|wx.EXPAND, 10)
        sizer.Add((5,5))
        sizer.Add(self.slider_fade_in, 0, wx.LEFT|wx.RIGHT|wx.EXPAND, 10)
        sizer.Add((5,5))
        sizer.Add(self.check_backwards, 0, wx.LEFT|wx.RIGHT|wx.BOTTOM|wx.EXPAND, 10)
        sizer.Add((5,5))
        sizer.Add(self.check_loop, 0, wx.LEFT|wx.RIGHT|wx.BOTTOM|wx.EXPAND, 10)
        sizer.Add(self.button_person, 0, wx.LEFT|wx.RIGHT|wx.BOTTOM|wx.EXPAND, 10)
        sizer.Add(self.button_del, 0, wx.LEFT|wx.RIGHT|wx.BOTTOM|wx.EXPAND, 10)
        self.SetAutoLayout(True)
        self.SetSizer(sizer)
        sizer.Fit(self)
        sizer.SetSizeHints(self)
        self.Layout()
    
    def __bind_events(self):
        self.Bind(wx.EVT_TEXT, self.on_filter_changed, self.filter)
        self.Bind(wx.EVT_LIST_ITEM_SELECTED, self.on_select_anim, self.list)
        self.Bind(events.EVT_CHANGED, self.on_change_name, self.text_name)
        self.Bind(events.EVT_CHANGING, self.on_change_fade_in, self.slider_fade_in)
        self.Bind(events.EVT_CHANGED, self.on_change_fade_in, self.slider_fade_in)
        self.Bind(wx.EVT_CHECKBOX, self.on_change_play_options, self.check_backwards)
        self.Bind(wx.EVT_CHECKBOX, self.on_change_play_options, self.check_loop)
        self.Bind(wx.EVT_BUTTON, self.on_switch_person, self.button_person)
        self.Bind(wx.EVT_BUTTON, self.on_delete_anim, self.button_del)
    
    def __get_skeleton(self):
        skeleton = self.character.getentityclass()
        if self.character.isa('necharacter'):
            skeleton = servers.get_entity_class_server().getentityclass(
                skeleton.getskeletonclass() )
        return skeleton
    
    def __get_selected_list_index(self):
        return self.list.GetNextItem(-1, wx.LIST_NEXT_ALL, wx.LIST_STATE_SELECTED)
    
    def __get_selected_anim_index(self):
        index = self.__get_selected_list_index()
        if index == -1:
            return -1
        else:
            return self.anim_indices[index]
    
    def __get_selected_list_item(self, column):
        index = self.__get_selected_list_index()
        if index == -1:
            return ""
        else:
            return self.list.GetItem(index, column).GetText()
    
    def __is_selected_anim_first_person(self):
        return self.__get_selected_list_item( self.ID_Person ) == "First"
    
    def __update_anim_params_ctrls(self):
        # Update fade in time slider for current animation
        index = self.__get_selected_anim_index()
        enable = index != -1
        self.text_name.Enable( enable )
        self.slider_fade_in.Enable( enable )
        self.check_backwards.Enable( enable )
        self.check_loop.Enable( enable )
        self.button_del.Enable( self.list.GetItemCount() > 0 )
        if enable:
            if self.__is_selected_anim_first_person():
                name = self.character.getfirstpersonstatename( index )
                fade_in = self.character.getfadeintime( True, index )
            else:
                skeleton = self.__get_skeleton()
                name = skeleton.getstatename(index)
                fade_in = skeleton.getfadeintime(index)
            self.text_name.set_value( name )
            self.slider_fade_in.set_value( fade_in )
        if self.character.getfirstpersonactive():
            self.button_person.SetLabel("Switch to third person")
        else:
            self.button_person.SetLabel("Switch to first person")
    
    def __validate_anim_name(self, new_name):
        found = False
        skeleton = self.__get_skeleton()
        
        # Look for a repeated name within the first person anims
        for i in range( skeleton.getnumstates() ):
            if new_name == skeleton.getstatename(i):
                msg = "Another animation named '" + new_name + "' already exists."
                cjr.show_error_message(msg)
                return False
        return True

    def on_filter_changed(self, event):
        self.filter_name = self.filter.GetValue()
        self.fill_animations()
        self.__update_anim_params_ctrls()

    def on_select_anim(self, event):
##        self.character.setactivestate( str(self.__get_selected_list_item(self.ID_Anim)),
##            self.check_backwards.GetValue(), self.check_loop.GetValue(), True )
        self.on_change_play_options(None)
        self.__update_anim_params_ctrls()
    
    def on_change_name(self, event):
        index = self.__get_selected_anim_index()
        if index != -1:
            first = self.__is_selected_anim_first_person()
            name = self.text_name.get_value()
            self.character.changeanimstatename( first, index, str(name) )
            index = self.__get_selected_list_index()
            self.list.SetStringItem( index, self.ID_Anim, name )
    
    def on_change_fade_in(self, event):
        index = self.__get_selected_anim_index()
        if index != -1:
            first = self.__is_selected_anim_first_person()
            self.character.setfadeintime( first, index, event.get_value() )
            index = self.__get_selected_list_index()
            self.list.SetStringItem( index, self.ID_FadeIn, str(self.slider_fade_in.get_value()) )
    
    def on_change_play_options(self, event):
        index = self.__get_selected_anim_index()
        if index != -1:
            backwards = self.check_backwards.GetValue()
            loop = self.check_loop.GetValue()
            if self.__is_selected_anim_first_person():
                self.character.setfirstpersonactivestateindex( index, backwards, loop, True )
            else:
                self.character.setactivestateindex( index, backwards, loop, True )
    
    def on_switch_person(self, event):
        to_first = not self.character.getfirstpersonactive()
        self.character.setfirstpersonactive( to_first )
        self.__update_anim_params_ctrls()
    
    def on_delete_anim(self, event):
        dlg = DeleteAnimDialog(self, self.character)
        if dlg.ShowModal() == wx.ID_OK:
            anim_index = dlg.get_deleted_anim_index()
            list_index = self.anim_indices.index( anim_index )
            self.list.DeleteItem( list_index )
            self.__update_anim_params_ctrls()
        dlg.Destroy()
    
    def set_character(self, character):
        self.character = character

        self.fill_animations()
        
        # Update controls that depends on selected animation
        self.__update_anim_params_ctrls()
    
    def fill_animations(self):
        self.list.DeleteAllItems()
        skeleton = self.__get_skeleton()
        # First person
        for i in range( self.character.getfirstpersonnumberstates() ):
            name = self.character.getfirstpersonstatename(i)
            if name.find( self.filter_name ) != -1:
                index = self.list.InsertStringItem( 0, name )
                self.anim_indices.insert(index, i)
                self.list.SetStringItem( index, self.ID_FadeIn, str(round(self.character.getfadeintime(True,i),3)) )
                self.list.SetStringItem( index, self.ID_Person, "First" )
        # Third person
        for i in range( skeleton.getnumstates() ):
            name = skeleton.getstatename(i)
            if name.find( self.filter_name ) != -1:
                index = self.list.InsertStringItem( 0, name )
                self.anim_indices.insert(index, i)
                self.list.SetStringItem( index, self.ID_FadeIn, str(round(skeleton.getfadeintime(i),3)) )
                self.list.SetStringItem( index, self.ID_Person, "Third" )
        
    def refresh(self):
        try:
            self.set_character( self.character )
        except:
            self.Close()


# AttachmentsPanel class
class AttachmentsPanel(editorpanel.EditorPanel):
    """Specific character attachment setting editor panel"""
    
    # Attachments list columns indices
    ID_Slot = 0
    ID_Entity = 1
    
    def __init__(self, *args, **kwds):
        editorpanel.EditorPanel.__init__(self, *args, **kwds)
        self.character = None
        self.attach_indices = []
        
        self.label = wx.StaticText(self, -1, "Attached entities:")
        self.list = wx.ListCtrl(self, -1,
            style=wx.LC_REPORT|wx.LC_SINGLE_SEL|wx.LC_SORT_ASCENDING)
        self.button_create = wx.Button(self, -1, "Create and attach entity")
        self.button_attach = wx.Button(self, -1, "Attach selected entity")
        self.button_dettach = wx.Button(self, -1, "Dettach attached entity")
        self.button_select = wx.Button(self, -1, "Select attached entity")
        self.button_delete = wx.Button(self, -1, "Delete attached entity")
        
        self.__set_properties()
        self.__do_layout()
        self.__bind_events()
    
    def __set_properties(self):
        self.SetScrollRate(10, 10)
        
        # Animations list columns
        self.list.InsertColumn(self.ID_Slot, "Slot")
        self.list.InsertColumn(self.ID_Entity, "Entity", wx.LIST_FORMAT_RIGHT)
    
    def __do_layout(self):
        sizer = wx.BoxSizer(wx.VERTICAL)
        sizer_list = wx.BoxSizer(wx.VERTICAL)
        sizer_buttons = wx.BoxSizer(wx.VERTICAL)
        sizer_list.Add(self.label, 0, wx.FIXED_MINSIZE, 0)
        sizer_list.Add(self.list, 1, wx.TOP|wx.EXPAND, 5)
        sizer.Add(sizer_list, 1, wx.ALL|wx.EXPAND, 10)
        sizer_buttons.Add(self.button_create, 0, wx.EXPAND, 0)
        sizer_buttons.Add(self.button_attach, 0, wx.TOP|wx.EXPAND, 5)
        sizer_buttons.Add(self.button_dettach, 0, wx.TOP|wx.EXPAND, 5)
        sizer_buttons.Add(self.button_select, 0, wx.TOP|wx.EXPAND, 5)
        sizer_buttons.Add(self.button_delete, 0, wx.TOP|wx.EXPAND, 5)
        sizer.Add(sizer_buttons, 0, wx.LEFT|wx.RIGHT|wx.BOTTOM|wx.EXPAND, 10)
        self.SetAutoLayout(True)
        self.SetSizer(sizer)
        sizer.Fit(self)
        sizer.SetSizeHints(self)
        self.Layout()
    
    def __bind_events(self):
        self.Bind(wx.EVT_LIST_ITEM_SELECTED, self.on_select_attach, self.list)
        self.Bind(wx.EVT_BUTTON, self.on_create_and_attach_entity, self.button_create)
        self.Bind(wx.EVT_BUTTON, self.on_attach_entity, self.button_attach)
        self.Bind(wx.EVT_BUTTON, self.on_dettach_entity, self.button_dettach)
        self.Bind(wx.EVT_BUTTON, self.on_select_entity, self.button_select)
        self.Bind(wx.EVT_BUTTON, self.on_delete_entity, self.button_delete)
    
    def __get_selected_list_index(self):
        return self.list.GetNextItem(-1, wx.LIST_NEXT_ALL, wx.LIST_STATE_SELECTED)
    
    def __get_selected_attach_index(self):
        index = self.__get_selected_list_index()
        if index == -1:
            return -1
        else:
            return self.attach_indices[index]
    
    def __get_selected_list_item(self, column):
        index = self.__get_selected_list_index()
        if index == -1:
            return ""
        else:
            return self.list.GetItem(index, column).GetText()
    
    def __update_attach_params_ctrls(self):
        index = self.__get_selected_attach_index()
        self.button_create.Enable( index != -1 )
        self.button_attach.Enable( index != -1 )
        enable = False
        if index != -1:
            index = self.__get_selected_attach_index()
            enable = self.character.getattachedentity(index) != 0
        self.button_dettach.Enable( enable )
        self.button_select.Enable( enable )
        self.button_delete.Enable( enable )
    
    def on_create_and_attach_entity(self, event):
        # Show a grimoire browser to let the user choose an entity class
        dlg = grimoire.create_dialog( self )
        ok = dlg.ShowModal() == wx.ID_OK
        class_name = dlg.GetTitle()
        dlg.Destroy()
        if not ok:
            return
        
        # Create an instance of the selected class
        # position (1, 1, 1) is used just to pass the spatial search cell check
        # it is not important, since the attachment is always linked to the entity pos
        entity = servers.get_world_interface().newentity( str(class_name), 1, 1, 1 )
        if entity is None:
            cjr.show_error_message(
                "Unknown error.\nCouldn't create an instance of" \
                "the entity class '" + class_name + "'."
                )
            return
        
        # Attach the new entity to the selected slot
        self.__attach_entity_to_current_slot( entity )
    
    def on_select_attach(self, event):
        self.__update_attach_params_ctrls()
    
    def __attach_entity_to_current_slot(self, entity):
        slot = self.__get_selected_list_item( self.ID_Slot )
        entity_id = entity.getid()
        self.character.attach( str(slot), entity_id )
        index = self.__get_selected_list_index()
        self.list.SetStringItem( index, self.ID_Entity, str(entity_id) )
        self.__update_attach_params_ctrls()
    
    def on_attach_entity(self, event):
        state = app.get_object_state()
        count = state.getselectioncount()
        if count == 0:
            cjr.show_error_message(
                "There isn't any selected entity"
                )
        elif count > 1:
            cjr.show_error_message(
                "There's more than one entity selected.\n" \
                "Only one entity can be attached to the same slot."
                )
        else:
            slot = self.__get_selected_list_item(self.ID_Slot)
            if slot != "":
                entity = state.getselectedentity(0)
                entity_id = entity.getid()
                if entity_id == self.character.getid():
                    cjr.show_error_message(
                        "So you want to attach the character to itself, how curious..."
                        )
                else:
                    self.__attach_entity_to_current_slot( entity )

    def on_dettach_entity(self, event):
        l_index = self.__get_selected_attach_index()
        if l_index != -1:
            p_index = self.attach_indices[l_index]
            entity_id = self.character.getattachedentity( p_index )
            if entity_id != 0:
                self.character.dettach( entity_id )
                self.list.SetStringItem( l_index, self.ID_Entity, "" )
                self.__update_attach_params_ctrls()

    def on_select_entity(self, event):
        index = self.__get_selected_attach_index()
        if index != -1:
            entity_id = self.character.getattachedentity(index)
            if entity_id != 0:
                state = app.get_object_state()
                state.addentitytoselection(entity_id)

    def on_delete_entity(self, event):
        index = self.__get_selected_attach_index()
        if index == -1:
            return

        # Ask for confirmation
        slot = self.__get_selected_list_item(self.ID_Slot)
        if slot == "":
            return
        msg = "Are you sure that you want to delete the " \
                    "the entity attached to the '%s' slot?" % slot
        delete = cjr.confirm_yes_no(self, msg)
        if delete != wx.ID_YES:
            return
        
        # Dettach the entity from the selected slot
        p_index = self.attach_indices[index]
        entity_id = self.character.getattachedentity( p_index )
        if entity_id == 0:
            return
        self.on_dettach_entity(None)
        
        # Store selected entities
        obj_state = app.get_object_state()
        selection_ids = []
        for i in range( obj_state.getselectioncount() ):
            id = obj_state.getselectedentity(i).getid()
            if entity_id != id:
                selection_ids.append( id )
        
        # Delete the entity
        obj_state.resetselection()
        obj_state.addentitytoselection( entity_id )
        obj_state.delentities()
        
        # Restore the selection
        for id in selection_ids:
            obj_state.addentitytoselection( id )
        
        self.__update_attach_params_ctrls()
    
    def set_character(self, character):
        self.character = character
        
        # Fill attachments list
        self.list.DeleteAllItems()
        for i in range( character.getnumberattachments() ):
            index = self.list.InsertStringItem( 0, character.getattachmentname(i) )
            self.attach_indices.insert(index, i)
            entity = str( character.getattachedentity(i) )
            if entity == "0":
                entity = ""
            self.list.SetStringItem( index, self.ID_Entity, entity )
        
        # Update controls that depends on selected attachment
        self.__update_attach_params_ctrls()
    
    def refresh(self):
        try:
            self.set_character( self.character )
        except:
            self.Close()


# create_all_editors function
def create_all_editors(character, parent):
    editor1 = AnimationPanel(parent, -1)
    editor1.set_character(character)
    editor2 = AttachmentsPanel(parent, -1)
    editor2.set_character(character)
    return [ ('Attachments', editor2), ('Animation', editor1) ]
