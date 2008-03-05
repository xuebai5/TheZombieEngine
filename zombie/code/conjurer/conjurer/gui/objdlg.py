##\file objdlg.py
##\brief Object inspector dialog

import wx

import pynebula

import app
import childdialoggui
import editorpanel
import servers

# Specialized editors tabs
import agenteditor
import areatriggereditor
import animeditor
import cameraeditor
import emittedeventseditor
import lodeditor
import missioneditor
import plugeditor
import scripttriggereditor
import soundeditor
import soundtriggereditor
import spawnereditor
import stdlighteditor

# Generic editors tabs
import inspectorstatetab
import inspectorcmdstab
import inspectorsignalstab

import propertyview
import scriptingmodelsingleobject

import nutils


# EditorFactory class
class EditorFactory:
    """
    Create editors for each type of nRoot object
    
    An entry must be added to the creation_funcs member attribute for any type
    of object that have specific editors. The creation_funcs is a dictionary
    whose entries associate the name of a nRoot object type with a function
    that returns its editors. This function must return the editors in a
    sequence of tuples, with each tuple in the format (<editor name string>,
    <editor instance>). The editors will be inserted as new notebook pages by
    the ObjectInspectorDialog which have called the create_editors function.
    These pages will be inserted at the beginning and in reverse order as given
    by the editors sequence. Each editor must be a wx.Panel class (although it
    hasn't been tested with other classes and maybe any wx.Window inherited
    class can be accepted).
    """
    
    def __init__(self):
        self.creation_funcs = {
            'nappviewport': cameraeditor.create_all_editors,
            'ncAgentTrigger': agenteditor.create_all_editors,
            'ncAreaEvent': emittedeventseditor.create_all_editors,
            'ncAreaEventClass': emittedeventseditor.create_class_editors,
            'ncScene': plugeditor.create_all_editors,
            'ncSpawner': spawnereditor.create_all_editors,
            'ncSoundClass': soundeditor.create_all_editors,
            'ncTriggerClass': agenteditor.create_all_editors,
            'neareatrigger': areatriggereditor.create_all_editors,
            'necharacter': animeditor.create_all_editors,
            'neskeleton': animeditor.create_all_editors,
            'nesceneclass': lodeditor.create_all_editors,
            'nescripttrigger': scripttriggereditor.create_all_editors,
            'nesoundsource': soundtriggereditor.create_all_editors,
            'newheelclass': soundeditor.create_all_editors,
            'nmissionhandler': missioneditor.create_all_editors,
            'Stdlight': stdlighteditor.create_all_editors
            }
    
    def create_editors(self, object, parent):
        """Create and return all specific editors for a nRoot object"""
        # Find the available editors for the object's class and components
        editors = []
        for each_type in self.creation_funcs.keys():
            if object.isa(each_type):
                editors.extend(
                    self.creation_funcs[each_type](object, parent)
                    )
            elif object.hascommand('hascomponent'):
                if object.hascomponent(each_type):
                    editors.extend(
                        self.creation_funcs[each_type](object, parent)
                        )
        # Return all the editors found for the given object
        return editors


# ObjectInspectorDialog class
class ObjectInspectorDialog(childdialoggui.InstanceTrackingChildDialogGUI):
    """Dialog that allows to inspect and change the state of an object"""

    def init(self, parent, object_id):
        childdialoggui.InstanceTrackingChildDialogGUI.init(
            self, "Object inspector", parent
            )
        self.object_id = object_id
        self.is_applying_changes = False
        # editor factory
        self.factory = EditorFactory()
        # notebook control
        self.notebook = wx.Notebook(self, -1)
        self.property_tab = None
        self.__set_properties()
        self.__do_layout()
        self.__bind_events()

    def on_page_changed (self, evt):
        page = evt.GetEventObject().GetSelection()
        if evt.GetEventObject().GetPageText(page) == "State":
            if self.class_tab.executed:
                self.state_tab.draw_state_commands( self.__get_object() )
                self.class_tab.executed = False

    def __set_properties(self):
        self.SetSize( (380, 400) )
        # Inspected object
        object = self.__get_object()
        # check object is valid before doing 
        # anything else - it may have been deleted
        if object is None:
            return False
        # General editors
        model = scriptingmodelsingleobject.new_scripting_model_for_object(
                        object
                        )
        self.class_tab = inspectorcmdstab.ClassCommandsTab(
                                self.notebook, 
                                object
                                )
        self.property_tab = propertyview.PropertyView(
                                    self.notebook, 
                                    model
                                    )
        self.state_tab = inspectorstatetab.StateTab(
                                self.notebook, 
                                object
                                )
        self.signals_tab = inspectorsignalstab.SignalEmitterTab(
                                    self.notebook, 
                                    object
                                    )
        self.notebook.AddPage(self.property_tab, "Properties" )
        self.notebook.AddPage(self.state_tab, "State" )
        self.notebook.AddPage(self.class_tab, "Commands")
        self.notebook.AddPage(self.signals_tab, "Signals")
        # Specific editors
        self.editors = self.factory.create_editors(
                        object, 
                        self.notebook
                        )
        for editor in self.editors:
            self.notebook.InsertPage(
                0, 
                editor[1], 
                editor[0] 
                )
        # Select first page
        self.notebook.SetSelection(0)
        # Append object name to the dialog title
        name = nutils.get_detailed_object_description(
                    object, 
                    self.object_id
                    )
        self.SetTitle( name )

    def __do_layout(self):
        sizer = wx.BoxSizer(wx.HORIZONTAL)
        sizer.Add(self.notebook, 1, wx.EXPAND, 0)
        self.SetAutoLayout(True)
        self.SetSizerAndFit(sizer)
        self.SetSize( (350, 380) )
        self.Layout()
    
    def __bind_events(self):
        self.Bind(
            wx.EVT_NOTEBOOK_PAGE_CHANGED, 
            self.on_page_changed
            )
        pynebula.pyBindSignal(
            servers.get_kernel_server(), 
            'objectdeleted',
            self, 
            'onobjectdeleted', 
            0 
            )
        #bind to events on property tab if I have one
        if self.__has_property_tab():
            self.property_tab.Bind(
                propertyview.EVT_OK_REQUESTED, 
                self.__handle_ok_on_property_tab
                )
            self.property_tab.Bind(
                propertyview.EVT_CANCEL_REQUESTED, 
                self.__handle_cancel_on_property_tab
                )

    def __has_property_tab(self):
        return self.property_tab is not None

    def __handle_cancel_on_property_tab(self, event):
        self.Close()

    def __handle_ok_on_property_tab(self, event):
        self.is_applying_changes = True
        self.Close()

    def __del__(self):
        pynebula.pyUnbindTargetObject(
            servers.get_kernel_server(), 
            'objectdeleted', 
            self
            )

    def on_close(self, event):
        if self.__get_object() is not None:
            if self.__has_property_tab():
                if self.is_applying_changes:
                    self.property_tab.apply_changes()
                else:
                    self.property_tab.cancel()
        self.is_applying_changes = False
        childdialoggui.InstanceTrackingChildDialogGUI.on_close(
            self, 
            event
            )

    def __get_object(self):
        """
        Return a reference to the object for my object id.

        If no object is found None will be returned.
        For objects in NOH use absolute paths as their object id, 
        for entities use the id given by the entity server.
        """
        if self.object_id is None:
            return None
        if isinstance(self.object_id, int):
            # integer -> nEntity -> Get object from entity server
            return self.__get_entity_object_with_id(
                self.object_id 
                )
        elif self.object_id.startswith('/'):
            # NOH path -> nRoot -> Get object from NOH
            try:
                return pynebula.lookup( self.object_id )
            except:
                return None
        else:
            # name -> nEntity -> Get object from entity server
            entity_id = app.get_level().findentity( self.object_id )
            if entity_id == 0:
                return None
            else:
                return self.__get_entity_object_with_id(
                    entity_id
                    )

    def __get_entity_object_with_id(self, object_id):
        object_server = servers.get_entity_object_server()
        return object_server.getentityobject(object_id)

    def onobjectdeleted(self):
        if self.__get_object() is None:
            self.Close()

    def persist(self):
        data = {
            'selected page': self.notebook.GetSelection(),
            'specific editors': []
            }
        obj = self.__get_object()
        if obj is not None:
            data['object type'] = obj.getclass()
        for i in range(self.notebook.GetPageCount()-3):
            editor = self.notebook.GetPage(i)
            data['specific editors'].append( editor.persist() )
        return [
            create_window,
            (self.object_id,),   # trailing comma makes it a tuple
            data
            ]

    def restore(self, data_list):
        data = data_list[0]
        for editor in self.editors:
            if data.has_key('specific editors'):
                editor[1].restore( data['specific editors'].pop() )
        # TODO: Restore general editors
        if data.has_key('general editors'):
            print 'general editors'
        # Select visible page
        self.notebook.SetSelection( data['selected page'] )

    def is_restorable(self, data_list):
        obj = self.__get_object()
        if obj is not None:
            data = data_list[0]
            if data.has_key('object type'):
                return obj.getclass() == data['object type']
        return False

    def refresh(self):
        if self.__get_object() is None:
            self.Close()
        else:
            for each_page in self.__get_notebook_pages():
                each_page.refresh()

    def on_app_event(self, event):
        for each_page in self.__get_notebook_pages():
            if isinstance(each_page, editorpanel.EditorPanel):
                each_page.on_app_event(event)

    def __get_notebook_pages(self):
        list_of_pages = []
        for page_index in range( self.notebook.GetPageCount() ):
            list_of_pages.append( self.notebook.GetPage(page_index) )
        return list_of_pages


# create_window function
def create_window(parent, object_id=None):
    try:
        try:
            wx.BeginBusyCursor()
            win = ObjectInspectorDialog(
                        parent, 
                        object_id
                        )
        finally:
            wx.EndBusyCursor()
    except:
        # make sure any errors are not hidden
        raise
    return win
