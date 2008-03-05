##\file objtrackdlg.py
##\brief Object tracker that lists the entities currently selected in Summoner

import wx

import pynebula
import app

import togwin
import objdlg

import conjurerconfig as cfg


# SelectedEntityCtrl class
class SelectedEntityCtrl(wx.PyControl):
    """ Control used to handle a single selected entity. """

    def __init__(self, parent, entity):
        wx.PyControl.__init__(
            self, 
            parent, 
            -1, 
            style=wx.NO_BORDER|wx.TAB_TRAVERSAL
            )
        self.entity = entity
        # controls
        self.button_inspect = wx.Button(
                                                self, 
                                                -1, 
                                                "Inspect",
                                                style=wx.BU_EXACTFIT
                                                )
        self.label_description = wx.StaticText(
                                    self, 
                                    -1, 
                                    ""
                                    )
        self.__set_properties()
        self.__do_layout()
        self.__bind_events()

    def __set_properties(self):
        self.refresh()

    def refresh(self):
        self.set_label_from_entity()

    def set_label_from_entity(self):
        self.label_description.SetLabel(
            self.get_description_for_entity()
            )

    def set_entity(self, entity):
        if self.entity != entity:
            self.entity = entity
            self.refresh()

    def __do_layout(self):
        sizer_main = wx.BoxSizer(wx.VERTICAL)
        sizer = wx.BoxSizer(wx.HORIZONTAL)
        sizer.Add(
            self.button_inspect,
            0,
            wx.BOTTOM|wx.TOP|wx.ALIGN_CENTER_VERTICAL|wx.FIXED_MINSIZE,
            cfg.BORDER_WIDTH 
            )
        sizer.Add(
            self.label_description, 
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
                                (-1, -1), 
                                (-1, -1), 
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
        self.Bind(
            wx.EVT_BUTTON,
            self.on_button_inspect_clicked, 
            self.button_inspect
            )

    def on_size(self, event):
        self.Layout()
        event.Skip()

    def on_button_inspect_clicked(self, event):
        win = objdlg.create_window(
                    app.get_top_window(), 
                    self.entity.getid()
                    )
        win.display()

    def get_description_for_entity(self):
        return "%s (%s)" % (
            str( self.entity.getid() ), 
            self.entity.getclass() 
            )


# SelectedEntityDialog class
class SelectedEntityDialog(togwin.ChildToggableDialog):
    """ Object inspector that always inspects the entities 
        currently selected in Summoner. """

    def __init__(self, parent):
        togwin.ChildToggableDialog.__init__(
            self, 
            "Selected Entities",
            parent
            )
        self.entities = []
        self.entity_ctrl_list = []
        self.scroll_entity_list = wx.ScrolledWindow(
                                            self, 
                                            -1, 
                                            style=wx.NO_BORDER|wx.TAB_TRAVERSAL
                                            )
        self.scroll_entity_list.SetSizer(
            wx.BoxSizer(wx.VERTICAL) 
            )
        self.button_close = wx.Button(
                                        self, 
                                        wx.ID_CANCEL, 
                                        "&Close"
                                        )
        self.__set_properties()
        self.__do_layout()
        self.__bind_events()

    def persist(self):
        return [
            create_window,
            () #no creation parameters
            ]

    def __set_properties(self):
        self.scroll_entity_list.SetScrollRate(10, 10)
        self.onselectionchanged()

    def show_details(self):
        self.Freeze()
        self.update_title()
        self.update_entity_list()
        self.Thaw()

    def update_title(self):
        self.SetTitle(
            "Selected Entities (%s)" % self.get_number_of_entities()
            )

    def get_number_of_entities(self):
        return len(self.entities)

    def get_number_of_entity_ctrls(self):
        return len(self.entity_ctrl_list)

    def __do_layout(self):
        sizer_main = wx.BoxSizer(wx.VERTICAL)
        sizer_main.Add(
            self.scroll_entity_list, 
            1, 
            wx.EXPAND|wx.ALL, 
            cfg.BORDER_WIDTH
            )
        horizontal_line = wx.StaticLine(
                                self, 
                                -1, 
                                (-1, -1), 
                                (-1, -1), 
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
        self.SetSizerAndFit(sizer_main)
        self.SetSize( (230, 210 ) )

    def __bind_events(self):
        pynebula.pyBindSignal(
            app.get_object_state(),
            'selectionchanged',
            self,
            'onselectionchanged',
            0
            )
        self.Bind(
            wx.EVT_BUTTON, 
            self.on_click_close_button, 
            self.button_close
            )

    def onselectionchanged(self):
        state = app.get_object_state()
        select_count = state.getselectioncount()
        entity_list = []
        for index in range(select_count):
            entity = state.getselectedentity(index)
            entity_list.append(entity)
        self.entities = entity_list
        self.show_details()

    def on_click_close_button(self, event):
        self.Close()

    def __del__ (self):
        pynebula.pyUnbindTargetObject(
            app.get_object_state(), 
            'selectionchanged', 
            self
            )

    def add_entity_ctrl(self, entity):
        entity_ctrl = SelectedEntityCtrl(
                                self.scroll_entity_list,
                                entity
                                )
        self.entity_ctrl_list.append(entity_ctrl)
        list_sizer = self.scroll_entity_list.GetSizer()
        list_sizer.Add(
                entity_ctrl,
                0,
                wx.EXPAND
                )
        return entity_ctrl

    def update_entity_list(self):
        """ Update the entity list - to make things quicker
            reuse entity ctrls if possible.
            First, if there are more controls currently in the 
            list than are needed, remove those that are 
            redundant.
            Then work throught the list of entities, grabbing
            an existing control and updating it if possible. 
            Create new controls only as necessary. """

        list_sizer = self.scroll_entity_list.GetSizer()
        number_of_ctrls = self.get_number_of_entity_ctrls()
        number_of_entities = self.get_number_of_entities()
        difference = number_of_ctrls - number_of_entities
        if difference > 0:
            for unused_temp in range(difference):
                redundant_ctrl = self.entity_ctrl_list.pop()
                redundant_ctrl.Destroy()
        for entity_index in range(number_of_entities):
            entity = self.entities[entity_index]
            if entity_index + 1 > number_of_ctrls:
                self.add_entity_ctrl(entity)
            else:
                # set the entity on the existing control
                self.entity_ctrl_list[entity_index].set_entity(
                    entity
                    )
        list_sizer.Layout()

# create_window function
def create_window(parent):
    return SelectedEntityDialog(parent)

