##\file debugopsdlg.py
##\brief Debug options dialog

import wx
import wx.lib.colourselect

import pynebula
import servers
import format

import childdialoggui

import conjurerconfig as cfg

EnumDebugOptionTypeFlag = 0
EnumDebugOptionTypeColourSetting = 1

class DebugOptionType:
    def __init__(self, option):
        self.option = option
        self.server = option.server

    def is_flag(self):
        return False

    def is_colour_setting(self):
        return False


class DebugOptionTypeFlag(DebugOptionType):
    def get_value(self):
        return self.server.getflagenabled(
                self.option.module_name, 
                self.option.name
                )
                
    def set_value(self, new_value):
        return self.server.setflagenabled(
            self.option.module_name, 
            self.option.name,
            new_value
            )

    def is_flag(self):
        return True


class DebugOptionTypeColourSetting(DebugOptionType):
    def get_value(self):
        colour = self.server.getcoloursettingvalue(
                        self.option.module_name, 
                        self.option.name 
                        )
        colour255 = format.unit_rgb_2_byte_rgb(colour)
        return colour255

    def set_value(self, new_value):
        colour = format.byte_rgb_2_unit_rgba(new_value)
        return self.server.setcoloursettingvalue(
            self.option.module_name, 
            self.option.name,
            colour[0], colour[1], colour[2], colour[3]
            )

    def is_colour_setting(self):
        return True


class DebugOption:
    def __init__(self, module_name, name):
        self.module_name = module_name
        self.name = name
        self.server = servers.get_debug_server()
        self.type = self.__get_type_object()

    def __get_type_object(self):
        option_type = self.__get_option_type()
        if option_type == EnumDebugOptionTypeFlag:
            return DebugOptionTypeFlag(self)
        elif option_type == EnumDebugOptionTypeColourSetting:
            return DebugOptionTypeColourSetting(self)
        else:
            return None

    def get_description(self):
        return self.server.getoptiondescription(self.name)

    def get_description_as_lower_case(self):
        return self.get_description().lower()

    def get_group_name(self):
        return self.server.getoptiongroupname(self.name)

    def get_value(self):
        return self.type.get_value()

    def set_value(self, new_value):
        return self.type.set_value(new_value)

    def __get_option_type(self):
        return self.server.getoptiontype(self.name)

    def is_flag(self):
        return self.type.is_flag()

    def is_colour_setting(self):
        return self.type.is_colour_setting()


class DebugOptionGroup:
    def __init__(self, module_name, group_name):
        self.module_name = module_name
        self.group_name = group_name
        self.debug_options = []

    def get_description(self):
        return self.group_name

    def get_description_as_lower_case(self):
        return self.get_description().lower()

    def has_debug_colour_settings(self):
        return len( self.get_colour_settings() ) > 0

    def has_debug_flags(self):
        return len( self.get_debug_flags() ) > 0

    def get_debug_flags(self):
        def is_debug_flag(debug_option): 
            return debug_option.is_flag()
        return filter(
            is_debug_flag, 
            self.debug_options
            )

    def get_debug_flags_sorted(self):
        return sorted(
            self.get_debug_flags(), 
            lambda x, y: cmp(
                                x.get_description_as_lower_case(), 
                                y.get_description_as_lower_case()
                                )
            )

    def get_colour_settings(self):
        def is_debug_colour_setting(debug_option): 
            return debug_option.is_colour_setting()
        return filter(
            is_debug_colour_setting, 
            self.debug_options
            )

    def get_colour_settings_sorted(self):
        return sorted(
            self.get_colour_settings(), 
            lambda x, y: cmp(
                                x.get_description_as_lower_case(), 
                                y.get_description_as_lower_case()
                                )
            )

    def get_number_of_debug_flags(self):
        return len( self.get_debug_flags() )

    def get_number_of_debug_colour_settings(self):
        return len( self.get_colour_settings() )

    def add_option(self, debug_option):
        self.debug_options.append(debug_option)


class DebugOptionGroupPanel(wx.Panel):
    def __init__(self, parent, module_name, option_group):
        wx.Panel.__init__(self, parent, -1)
        self.option_group = option_group
        self.panel_flags = DebugOptionFlagPanel(
                                    self,
                                    module_name,
                                    option_group
                                    )
        self.panel_colour_settings = DebugOptionColourSettingPanel(
                                                    self,
                                                    module_name,
                                                    option_group
                                                    )
        self.__set_properties()
        self.__do_layout()

    def __set_properties(self):
        self.panel_flags.Show(
            self.option_group.has_debug_flags()
            )
        self.panel_colour_settings.Show(
            self.option_group.has_debug_colour_settings()
            )

    def refresh_on_option_change(self, option_name):
        self.panel_flags.refresh_on_option_change(
            option_name
            )

    def __do_layout(self):
        sizer_layout = wx.BoxSizer(wx.VERTICAL)
        sizer_layout.Add(
            self.panel_flags,
            2,
            wx.EXPAND
            )
        sizer_layout.Add(
            self.panel_colour_settings,
            1, 
            wx.EXPAND
            )
        self.SetSizerAndFit(sizer_layout)
        
    def persist(self):
        options = {}
        for each_option in self.option_group.debug_options:
            options[each_option.name] = each_option.get_value()
        return options
        
    def restore(self, data):
        for option_name, option_value in data['options'].iteritems():
            for each_option in self.option_group.debug_options:
                if each_option.name == option_name:
                    each_option.set_value(option_value)
                    continue
        self.refresh()

    def refresh(self):
        self.panel_flags.refresh()
        self.panel_colour_settings.refresh()


class DebugOptionFlagCtrl(wx.PyControl):
    """ Control used to handle a flag. """

    def __init__(self, parent, flag):
        wx.PyControl.__init__(
            self, 
            parent, 
            -1, 
            style=wx.NO_BORDER|wx.TAB_TRAVERSAL
            )
        self.flag = flag
        # controls
        self.checkbox = wx.CheckBox(
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
        self.set_checkbox_value_from_flag()
        self.set_label_from_flag()

    def set_checkbox_value_from_flag(self):
        self.checkbox.SetValue(
            self.flag.get_value()
            )

    def set_label_from_flag(self):
        self.checkbox.SetLabel(
            self.flag.get_description()
            )

    def get_option_name(self):
        return self.flag.name

    def __do_layout(self):
        sizer_main = wx.BoxSizer(wx.VERTICAL)
        sizer = wx.BoxSizer(wx.HORIZONTAL)
        sizer.Add(
            self.checkbox, 
            0, 
            wx.ALIGN_CENTER_VERTICAL|wx.LEFT|wx.TOP,
            1
            )
        sizer_main.Add(
            sizer, 
            0,
            wx.TOP|wx.LEFT|wx.RIGHT,
            2
            )
        self.SetSizerAndFit(sizer_main)

    def __bind_events(self):
        wx.EVT_SIZE(self, self.on_size)
        self.Bind(
            wx.EVT_CHECKBOX, 
            self.on_checkbox,
            self.checkbox
            )

    def on_size(self, event):
        self.Layout()
        event.Skip()

    def on_checkbox(self, event):
        self.update_value(
            event.GetSelection() 
            )

    def update_value(self, boolean):
        """ Update the Nebula object. """
        self.flag.set_value(
            boolean
            )

    def set_flag(self, flag):
        self.flag = flag
        self.refresh()

    def check(self):
        self.checkbox.SetValue(
            True
            )
        self.update_value(
            True
            )

    def uncheck(self):
        self.checkbox.SetValue(
            False
            )
        self.update_value(
            False
            )


class DebugOptionFlagPanel(wx.Panel):
    def __init__(self, parent, module_name, option_group):
        wx.Panel.__init__(self, parent, -1)
        self.module_name = module_name
        self.option_group = option_group
        
        self.list_ctrls = []
        
        self.scroll_list = wx.ScrolledWindow(
                                self, 
                                -1, 
                                style=wx.SUNKEN_BORDER|wx.TAB_TRAVERSAL
                                )
        self.scroll_list.SetBackgroundColour(
            wx.WHITE
            )
        self.scroll_list.SetSizer(
            wx.BoxSizer(wx.VERTICAL) 
            )

        self.button_select_all = wx.Button(
                                            self, 
                                            -1, 
                                            "Select &All",
                                            style=wx.BU_EXACTFIT
                                            ) 
        self.button_clear_all = wx.Button(
                                        self, 
                                        -1, 
                                        "&Clear All",
                                        style=wx.BU_EXACTFIT
                                        )
        # N.B. need to do layout before calling __set_properties
        self.__do_layout()
        self.__set_properties()
        self.__bind_events()

    def __do_layout(self):
        sizer_main = wx.StaticBoxSizer(
                            wx.StaticBox(self, -1, "Flags"), 
                            wx.VERTICAL
                            )
        sizer_buttons = wx.BoxSizer(wx.HORIZONTAL)
        sizer_buttons.Add(
            self.button_select_all, 
            0
            )
        sizer_buttons.Add(
            self.button_clear_all,
            0, 
            wx.LEFT, 
            cfg.BORDER_WIDTH
            )
        sizer_main.Add(
            sizer_buttons, 
            0, 
            wx.ALIGN_LEFT|wx.LEFT|wx.RIGHT,
            cfg.BORDER_WIDTH
            )
        sizer_main.Add(
            self.scroll_list, 
            1, 
            wx.EXPAND|wx.ALL, 
            cfg.BORDER_WIDTH
            )
        self.SetSizerAndFit(sizer_main)

    def __set_properties(self):
        self.scroll_list.SetScrollRate(10, 10)
        self.update_list()

    def get_number_of_list_ctrls(self):
        return len(self.list_ctrls)

    def update_list(self):
        """ Update the list - to make things quicker
            reuse ctrls if possible.
            First, if there are more controls currently in the 
            list than are needed, remove those that are 
            redundant.
            Then work throught the list of settings, grabbing
            an existing control and updating it if possible. 
            Create new controls only as necessary. """
        list_sizer = self.scroll_list.GetSizer()
        flags_sorted = self.option_group.get_debug_flags_sorted() 
        number_of_ctrls = self.get_number_of_list_ctrls()
        number_of_options = len(flags_sorted)
        difference = number_of_ctrls - number_of_options
        if difference > 0:
            for unused_temp in range(difference):
                redundant_ctrl = self.list_ctrls.pop()
                redundant_ctrl.Destroy()
        for index, option in enumerate(flags_sorted):
            if index + 1 > number_of_ctrls:
                self.add_list_ctrl(option)
            else:
                # set the entity on the existing control
                self.list_ctrls[index].set_flag(
                    option
                    )
        list_sizer.Layout()

    def add_list_ctrl(self, flag):
        list_ctrl = DebugOptionFlagCtrl(
                                self.scroll_list, 
                                flag
                                )
        self.list_ctrls.append(list_ctrl)
        list_sizer = self.scroll_list.GetSizer()
        list_sizer.Add(
                list_ctrl,
                0,
                wx.EXPAND
                )
        return list_ctrl

    def refresh(self):
        self.update_list()

    def __bind_events(self):
        self.Bind(
            wx.EVT_BUTTON, 
            self.__on_select_all, 
            self.button_select_all
            )
        self.Bind(
            wx.EVT_BUTTON, 
            self.__on_clear_all, 
            self.button_clear_all
            )

    def __on_select_all(self, event):
        for each_ctrl in self.list_ctrls:
            each_ctrl.check()
    
    def __on_clear_all(self, event):
        for each_ctrl in self.list_ctrls:
            each_ctrl.uncheck()

    def refresh_on_option_change(self, option_name):
        for each_ctrl in self.list_ctrls:
            if each_ctrl.get_option_name() == option_name:
                each_ctrl.refresh()


class DebugOptionColourSettingCtrl(wx.PyControl):
    """ Control used to handle a colour setting. """

    def __init__(self, parent, colour_setting):
        wx.PyControl.__init__(
            self, 
            parent, 
            -1, 
            style=wx.NO_BORDER|wx.TAB_TRAVERSAL
            )
        self.colour_setting = colour_setting
        # controls
        self.button_colour = wx.lib.colourselect.ColourSelect(
                                        self, 
                                        -1,
                                        size=( (20, 20) ),
                                        style=wx.NO_BORDER
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
        self.set_colour_button_from_colour_setting()
        self.set_label_from_colour_setting()

    def set_colour_button_from_colour_setting(self):
        self.button_colour.SetColour(
            self.colour_setting.get_value()
            )

    def set_label_from_colour_setting(self):
        self.label_description.SetLabel(
            self.colour_setting.get_description()
            )

    def get_description(self):
        return self.label_description.GetLabel()

    def __do_layout(self):
        sizer_main = wx.BoxSizer(wx.VERTICAL)
        sizer = wx.BoxSizer(wx.HORIZONTAL)
        sizer.Add(
            self.button_colour,
            0
            )
        sizer.Add(
            self.label_description, 
            0, 
            wx.LEFT|wx.ALIGN_CENTER_VERTICAL, 
            cfg.BORDER_WIDTH
            )
        sizer_main.Add(
            sizer, 
            0,
            wx.TOP|wx.LEFT|wx.RIGHT,
            2
            )
        self.SetSizerAndFit(sizer_main)

    def __bind_events(self):
        wx.EVT_SIZE(self, self.on_size)
        self.Bind(
            wx.lib.colourselect.EVT_COLOURSELECT, 
            self.on_button_colour, 
            self.button_colour
            )

    def on_size(self, event):
        self.Layout()
        event.Skip()

    def on_button_colour(self, event):
        colour255 = self.button_colour.GetColour().Get()
        self.colour_setting.set_value(colour255)
        
    def set_colour_setting(self, colour_setting):
        self.colour_setting = colour_setting
        self.refresh()
        

class DebugOptionColourSettingPanel(wx.Panel):
    def __init__(self, parent, module_name, option_group):
        wx.Panel.__init__(self, parent, -1)
        self.module_name = module_name
        self.option_group = option_group
        # visual elements
        self.list_ctrls = []
        self.scroll_list = wx.ScrolledWindow(
                                self, 
                                -1, 
                                style=wx.SUNKEN_BORDER|wx.TAB_TRAVERSAL
                                )
        self.scroll_list.SetBackgroundColour(
            wx.WHITE
            )
        self.scroll_list.SetSizer(
            wx.BoxSizer(wx.VERTICAL) 
            )
        # N.B. need to do layout before set properties
        self.__do_layout()
        self.__set_properties()

    def __do_layout(self):
        sizer_main = wx.StaticBoxSizer(
                            wx.StaticBox(self, -1, "Colour settings"), 
                            wx.VERTICAL
                            )
        sizer_main.Add(
            self.scroll_list, 
            1, 
            wx.EXPAND|wx.ALL, 
            cfg.BORDER_WIDTH
            )
        self.SetSizer(sizer_main)

    def __set_properties(self):
        self.scroll_list.SetScrollRate(10, 10)
        self.update_list()

    def get_number_of_list_ctrls(self):
        return len(self.list_ctrls)

    def update_list(self):
        """ Update the list - to make things quicker
            reuse ctrls if possible.
            First, if there are more controls currently in the 
            list than are needed, remove those that are 
            redundant.
            Then work throught the list of settings, grabbing
            an existing control and updating it if possible. 
            Create new controls only as necessary. """

        list_sizer = self.scroll_list.GetSizer()
        colour_settings_sorted = self.option_group.get_colour_settings_sorted() 
        number_of_ctrls = self.get_number_of_list_ctrls()
        number_of_options = len(colour_settings_sorted)
        difference = number_of_ctrls - number_of_options
        if difference > 0:
            for unused_temp in range(difference):
                redundant_ctrl = self.list_ctrls.pop()
                redundant_ctrl.Destroy()
        for index, option in enumerate(colour_settings_sorted):
            if index + 1 > number_of_ctrls:
                self.add_list_ctrl(option)
            else:
                # set the entity on the existing control
                self.list_ctrls[index].set_colour_setting(
                    option
                    )
        list_sizer.Layout()

    def add_list_ctrl(self, colour_setting):
        list_ctrl = DebugOptionColourSettingCtrl(
                                self.scroll_list, 
                                colour_setting
                                )
        self.list_ctrls.append(list_ctrl)
        list_sizer = self.scroll_list.GetSizer()
        list_sizer.Add(
                list_ctrl,
                0,
                wx.EXPAND
                )
        return list_ctrl

    def refresh(self):
        self.update_list()


# DebugOptionsDialog class
class DebugOptionsDialog(childdialoggui.InstanceTrackingChildDialogGUI):
    """Dialog that allows to enable/disable debug options"""
    def init(self, parent, module_name):
        childdialoggui.InstanceTrackingChildDialogGUI.init(
            self, 
            "Debug options", 
            parent
            )
        self.module_name = module_name
        self.debug_options = []
        self.debug_group_dict = {}
        # visual elements
        self.notebook = wx.Notebook(self, -1)
        self.__set_properties()
        self.__do_layout()
        self.__bind_events()

    def __set_properties(self):
        # title
        self.SetTitle("Debug options - %s" % self.module_name)
        self.build_debug_option_list()
        self.build_debug_groups()
        for each_group in self.get_debug_groups_as_sorted_list():
            group_tab = DebugOptionGroupPanel(
                                self.notebook, 
                                self.module_name,
                                each_group
                                )
            self.notebook.AddPage(
                group_tab, 
                each_group.get_description()
                )

    def get_debug_groups_as_sorted_list(self):
        """ Return the debug groups as a list, sorted
        by name. """
        return sorted(
            self.debug_group_dict.values(), 
            lambda group_a, group_b: cmp(
                                group_a.get_description_as_lower_case(), 
                                group_b.get_description_as_lower_case()
                                )
            )

    def build_debug_option_list(self):
        server = servers.get_debug_server()
        number_of_options = server.getnumoptions(
                                        self.module_name
                                        )
        for index in range(number_of_options):
            option_name = server.getoptionat(
                                    self.module_name, 
                                    index
                                    )
            option = DebugOption(
                            self.module_name, 
                            option_name
                            )
            self.debug_options.append(option)

    def build_debug_groups(self):
        self.debug_group_dict = {}
        for each_option in self.debug_options:
            group_name = each_option.get_group_name()
            try:
                group = self.debug_group_dict[group_name]
            except KeyError:
                group = DebugOptionGroup(
                                self.module_name,
                                group_name
                                )
                self.debug_group_dict[group_name] = group
            group.add_option(each_option)

    def __do_layout(self):
        sizer = wx.BoxSizer(wx.VERTICAL)
        sizer.Add(self.notebook, 1, wx.EXPAND)
        self.SetSizerAndFit(sizer)
        self.SetSize( (300, 350) )
        self.Layout()

    def __get_module(self):
        return pynebula.lookup(
            "/sys/servers/debug/modules/%s" % self.module_name
            )

    def __bind_events(self):
        pynebula.pyBindSignal(
            self.__get_module(),
            'debugoptionchanged',
            self, 
            'onsignaldebugoptionchanged', 
            0
            )

    def onsignaldebugoptionchanged(self, option_name):
        for each_page in self.__get_notebook_pages():
            each_page.refresh_on_option_change(option_name)

    def __del__(self):
        pynebula.pyUnbindTargetObject(
            self.__get_module(),
            'debugoptionchanged', 
            self
            )

    def __get_notebook_pages(self):
        page_list = []
        for page_index in range( self.notebook.GetPageCount() ):
            page_list.append(
                self.notebook.GetPage(page_index) 
                )
        return page_list

    def persist(self):
        data = {
            'selected_page': self.notebook.GetSelection(),
            'options': {}
            }
        for each_page in self.__get_notebook_pages():
            data['options'].update(
                each_page.persist() 
                )
        return (
            create_window, 
            (self.module_name,), # comma makes it a tuple
            data
            )

    def restore(self, data_list):
        data = data_list[0]
        for each_page in self.__get_notebook_pages():
            each_page.restore(data)
        # switch to appropriate page
        page_index = data['selected_page'] 
        if page_index != wx.NOT_FOUND:
            self.notebook.SetSelection( page_index )


# create_window function
def create_window(parent, module_name=None):
    try:
        try:
            wx.BeginBusyCursor()
            win = DebugOptionsDialog(
                        parent, 
                        module_name
                        )
        finally:
            wx.EndBusyCursor()
    except:
        # make sure any errors are not hidden
        raise
    return win

