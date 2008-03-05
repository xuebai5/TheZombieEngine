##\file fsmeditor.py
##\brief FSM editor dialog

import wx

import pynebula

import childdialoggui
import events
import fsmeditpanel
import fsmevents
import fsmpreview
import servers

import conjurerframework as cjr


# FSMEditorDialog class
class FSMEditorDialog(childdialoggui.InstanceTrackingChildDialogGUI):
    """FSM editor dialog"""

    def init(self, parent, fsm_path):
        childdialoggui.InstanceTrackingChildDialogGUI.init(
            self, "FSM editor", parent
            )
        self.fsm_path = fsm_path
        self.panel_edit = fsmeditpanel.EditPanel(self, self.fsm_path)
        self.panel_preview = fsmpreview.PreviewCtrl(self, fsm_path)

        self.__set_properties()
        self.__do_layout()
        self.__bind_events()

    def __set_properties(self):
        # FSM being edited
        self.panel_edit.set_fsm( self.fsm_path )
        self.panel_preview.set_fsm( self.fsm_path )
        self.__update_title()

    def __do_layout(self):
        sizer_layout = wx.BoxSizer(wx.HORIZONTAL)
        sizer_layout.Add(self.panel_edit, 0, wx.EXPAND, 0)
        sizer_layout.Add(self.panel_preview, 1, wx.ALL|wx.EXPAND, 10)
        self.SetAutoLayout(True)
        self.SetSizer(sizer_layout)
        sizer_layout.Fit(self)
        self.Layout()
        self.SetSize((600, 400))
        self.SetMinSize((350, 150))

    def __bind_events(self):
        self.Bind(events.EVT_CHANGED, self.on_change_fsm, self.panel_edit)

    def __update_title(self):
        title = "FSM editor"
        if self.fsm_path != None:
            title = title + " - " + self.get_fsm().getname()
            if self.get_fsm().isdirty():
                title = title + "*"
        self.SetTitle( title )

    def on_change_fsm(self, event):
        dirty = True
        # Handle special change events
        if event.get_value() != None:
            if event.get_value() == fsmevents.ID_FSMNameChanged:
                # Update stored fsm name
                self.fsm_path = event.get_old_value()
                # Preview still needs to be refreshed to update the cmap
                self.panel_preview.set_fsm( self.fsm_path )
                # but dirty flag is unset since renaming implicates saving
                dirty = False
        # Set dirty flag
        self.get_fsm().setdirty( dirty )
        self.__update_title()
        # Refresh preview
        self.panel_preview.on_change_fsm( event )

    def on_app_event(self, event):
        if isinstance(event, events.FSMSaved):
            if event.get_fsm_path() == self.fsm_path:
                self.__update_title()
        elif isinstance(event, (events.ActionAdded, events.ConditionAdded,
            events.FSMSelectorAdded)):
            self.panel_edit.quiet_refresh()

    def select_state(self, state_path):
        self.panel_edit.select_state( state_path )

    def select_transition(self, transition_path, target_index):
        self.panel_edit.select_transition( transition_path, target_index )

    def get_fsm(self):
        return pynebula.lookup( self.fsm_path )

    def quiet_refresh(self):
        self.panel_edit.quiet_refresh()

    def save(self, overwrite=True):
        """Save the fsm being edited by this dialog"""
        if not overwrite:
            cjr.show_information_message(
                "Saving a FSM with another name isn't allowed.\n" \
                "Use the duplicate option in the FSMs library instead."
                )
            return False
        servers.get_fsm_server().savefsm( self.get_fsm() )
        self.get_fsm().setdirty( False )
        self.__update_title()
        return True

    def persist(self):
        return [
            create_window,
            (self.fsm_path,) # trailing comma makes it a tuple
            ]   

    def is_restorable(self, data_list):
        try:
            self.get_fsm()
            return True
        except:
            return False

# create_window function
def create_window(parent, fsm_path=None):
    return FSMEditorDialog(parent, fsm_path)
