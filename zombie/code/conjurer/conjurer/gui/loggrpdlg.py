##\file loggrpdlg.py
##\brief Log group inspector dialog

import wx

import servers
import childdialoggui
import conjurerconfig as cfg

class LogGroupSelectionDialog(childdialoggui.InstanceTrackingChildDialogGUI):
    """Dialog to view or edit log groups for a given log class"""

    def init(self, parent, log_class_name):
        self.log_class_name = log_class_name
        childdialoggui.InstanceTrackingChildDialogGUI.init(
            self, 
            self.title_string(), 
            parent
            )
        self.group_dictionary = {}
        self.group_check_list = wx.CheckListBox(self, -1, style=wx.LB_SORT)
        self.button_select_all = wx.Button(self, -1, "Select &All") 
        self.button_clear_all = wx.Button(self, -1, "&Clear All")
        self.button_close = wx.Button(self, wx.ID_CANCEL, "&Close")

        self.set_properties()
        self.do_layout()
        self.bind_events()

    def set_properties(self):
        # Retrieve group list from log server
        number_of_groups = self.get_number_of_groups_for_log_class()
        for index in range(number_of_groups):
            group_name = self.get_name_for_group_at_position(index)
            check_box_number = self.group_check_list.Append(group_name)
            # since the list box is sorted we can't assume the position 
            # of any item won't change, so we need to store the id of 
            # each group against its name
            self.group_dictionary[group_name] = index
            is_checked = self.is_group_enabled_at_position(index)
            self.group_check_list.Check(check_box_number, is_checked)

    def on_group_toggled(self, event):
        # Highlight the toggled option because there's a bug in the
        # wx.CheckListBox implementation that makes it scroll to the
        # highlighted group when it's not visible (or to the top if there's
        # no highlighted group).
        selection_index = event.GetSelection()
        self.group_check_list.SetSelection(selection_index)

        # Update log groups state on log server
        group_name = self.group_check_list.GetString(selection_index)
        if self.group_check_list.IsChecked( event.GetSelection() ):
            self.enable_group_by_name(group_name)
        else:
            self.disable_group_by_name(group_name)

    def get_number_of_groups_for_log_class(self):
        log_server = self.get_log_server()
        return log_server.getnumgroups(self.log_class_name)

    def on_select_all_button_clicked(self, event):
        #we could the enable function with the approriate bitmask and
        #then refresh the screen. It would be quicker, but this simple
        #approach works fine.
        number_of_groups = self.get_number_of_groups_for_log_class()
        for index in range(number_of_groups):
            self.group_check_list.Check(index, True)
            self.enable_group_at_position(index)

    def on_clear_all_button_clicked(self, event):
        number_of_groups = self.get_number_of_groups_for_log_class()
        for index in range(number_of_groups):
            self.group_check_list.Check(index, False)
            self.disable_group_at_position(index)

    def on_close_button_clicked(self, event):
        self.Close()

    def persist(self):
        # groups state
        groups = {}
        number_of_groups = self.get_number_of_groups_for_log_class()
        for index in range(number_of_groups):
            group_name = self.get_name_for_group_at_position(index)
            groups[group_name] = self.is_group_enabled_at_position(index)

        data = {
            'groups': groups
            }

        return [
            create_window, 
            (self.log_class_name,), #trailing comma makes it a tuple
            data
            ]

    def get_log_server(self):
        return servers.get_log_server()

    def get_name_for_group_at_position(self, index):
        # N.B. function called uses one-based index
        log_server = self.get_log_server()
        group_name = log_server.getgroupname( self.log_class_name, index + 1)
        return group_name

    def is_group_enabled(self, group_name):
        return self.is_group_enabled_at_position(
            self.get_id_for_group(group_name)
            )

    def is_group_enabled_at_position(self, index):
        log_server = self.get_log_server()
        mask_integer = log_server.getenabledloggroups(self.log_class_name)
        return (mask_integer & (1 << (30 - index)))  != 0

    def enable_group_at_position(self, index):
        # N.B. function called uses one-based index
        log_server = self.get_log_server()
        log_server.enableloggroupbyindex(self.log_class_name, index + 1)

    def disable_group_at_position(self, index):
        # N.B. function called uses one-based index
        log_server = self.get_log_server()
        log_server.disableloggroupbyindex(self.log_class_name, index + 1)

    def get_id_for_group(self, group_name):
        return self.group_dictionary[group_name]

    def enable_group_by_name(self, group_name):
        self.enable_group_at_position(
            self.get_id_for_group(group_name)
            )

    def disable_group_by_name(self, group_name):
        self.disable_group_at_position(
            self.get_id_for_group(group_name)
            )

    def title_string (self):
        log_server = self.get_log_server()
        description = log_server.getdescription(self.log_class_name)
        return "Log Groups - %s" % description

    def restore(self, data_list):
        data = data_list[0]
        group_dictionary = data['groups']
        for group_name in group_dictionary:
            is_enabled = group_dictionary[group_name]
            if is_enabled == True:
                self.enable_group_by_name(group_name)
            else:
                self.disable_group_by_name(group_name)
        self.__refresh_checkbox_list()

    def __refresh_checkbox_list(self):
        # update the checkbox list with the values from Conjurer
        for check_box_index in range( self.group_check_list.GetCount() ):
            group_name = self.group_check_list.GetString(check_box_index)
            is_checked = self.is_group_enabled(group_name)
            self.group_check_list.Check(check_box_index, is_checked)

    def do_layout(self):
        border_width = cfg.BORDER_WIDTH
        outer_sizer = wx.BoxSizer(wx.VERTICAL)
        sizer_top_buttons = wx.BoxSizer(wx.HORIZONTAL)
        sizer_button = wx.BoxSizer(wx.HORIZONTAL)
        horizontal_line = wx.StaticLine(
                                self, 
                                -1, 
                                ( -1, -1 ), 
                                ( -1, -1 ), 
                                wx.LI_HORIZONTAL 
                                )
        sizer_top_buttons.Add(self.button_select_all, 0)
        sizer_top_buttons.Add(self.button_clear_all, 0, wx.LEFT, border_width)
        sizer_button.Add(self.button_close, 0, wx.LEFT, border_width)
        outer_sizer.Add(
            sizer_top_buttons, 
            0, 
            wx.ALIGN_LEFT|wx.LEFT|wx.TOP|wx.RIGHT, 
            border_width
            )
        outer_sizer.Add(
            self.group_check_list, 
            1,
            wx.ALL|wx.EXPAND,
            border_width
            )
        outer_sizer.Add(
            horizontal_line, 
            0, 
            wx.EXPAND|wx.LEFT|wx.RIGHT, 
            border_width
            )
        outer_sizer.Add(sizer_button, 0, wx.ALIGN_RIGHT | wx.ALL, border_width)
        self.SetAutoLayout(True)
        self.SetSizer(outer_sizer)
        outer_sizer.Fit(self)
        outer_sizer.SetSizeHints(self)
        # should not be necessary, but for now required to 
        # make sure dialog is big enough for us to see title
        self.SetSize( (300, 200) )
        self.Layout()

    def bind_events(self):
        self.Bind(
            wx.EVT_CHECKLISTBOX, 
            self.on_group_toggled, 
            self.group_check_list
            )
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
            self.on_close_button_clicked, 
            self.button_close
            )


# create_window function
def create_window(parent, log_class_name=None):
    return LogGroupSelectionDialog(parent, log_class_name)

    
    
